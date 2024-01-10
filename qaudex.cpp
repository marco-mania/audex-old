/* AUDEX CDDA EXTRACTOR
 * Copyright (C) 2007 by Marco Nelles (marcomaniac@gmx.de)
 * http://www.anyaudio.de/audex
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "qaudex.h"

/* The heart of audex */


QAudex::QAudex(QWidget* parent, QCDDADevice *device, QSettings *settings, QProfileModel *profile_model, QCDDAMetaDataModel *cdda_metadata_model) {

  this->parent = parent;

  this->device = device;
  this->settings = settings;
  this->profile_model = profile_model;
  this->cdda_metadata_model = cdda_metadata_model;

  encoder_wrapper = new QEncoderWrapper(this, settings, profile_model);
  if (!encoder_wrapper) {
    qDebug() << "(QAudex) PANIC ERROR. Could not load QEncoderWrapper. Low mem?";
    return;
  }

  cdda_extract_thread = new QCDDAExtractThread(this, device);
  if (!encoder_wrapper) {
    qDebug() << "(QAudex) PANIC ERROR. Could not load QCDDAExtractThread. Low mem?";
    return;
  }

  jobs = new QJobs();
  connect(jobs, SIGNAL(newJobAvailable()), this, SLOT(start_encode()));

  wave_file_writer = new QWaveFileWriter();

  audex_sql = new QAudexSQL(this, settings, profile_model, cdda_metadata_model);
  connect(audex_sql, SIGNAL(startDBSync()), this, SLOT(start_db_sync()));
  connect(audex_sql, SIGNAL(finishedDBSync()), this, SLOT(finished_db_sync()));
  connect(audex_sql, SIGNAL(longerProgress()), this, SLOT(slot_longer_dbsync()));
  connect(audex_sql, SIGNAL(longerProgressFinished()), this, SLOT(slot_longer_dbsync_finished()));
  connect(audex_sql, SIGNAL(info(const QString&)), this, SLOT(slot_info(const QString&)));
  connect(audex_sql, SIGNAL(warning(const QString&)), this, SLOT(slot_warning(const QString&)));
  connect(audex_sql, SIGNAL(error(const QString&, const QString&)), this, SLOT(slot_error(const QString&, const QString&)));
  connect(audex_sql, SIGNAL(log(const QString&)), this, SLOT(slot_log(const QString&)));

  last_measuring_point_sector = -1;
  timer_extract = new QTimer(this);
  connect(timer_extract, SIGNAL(timeout()), this, SLOT(calculate_speed_extract()));
  timer_extract->start(2000);

  last_measuring_point_encoder_percent = -1;
  timer_encode = new QTimer(this);
  connect(timer_encode, SIGNAL(timeout()), this, SLOT(calculate_speed_encode()));
  timer_encode->start(1000);

  connect(profile_model, SIGNAL(info(const QString&)), this, SLOT(slot_info(const QString&)));
  connect(profile_model, SIGNAL(warning(const QString&)), this, SLOT(slot_warning(const QString&)));
  connect(profile_model, SIGNAL(error(const QString&, const QString&)), this, SLOT(slot_error(const QString&, const QString&)));
  connect(profile_model, SIGNAL(log(const QString&)), this, SLOT(slot_log(const QString&)));

  connect(encoder_wrapper, SIGNAL(progress(int)), this, SLOT(progress_encode(int)));
  connect(encoder_wrapper, SIGNAL(finished()), this, SLOT(finish_encode()));
  connect(encoder_wrapper, SIGNAL(info(const QString&)), this, SLOT(slot_info(const QString&)));
  connect(encoder_wrapper, SIGNAL(warning(const QString&)), this, SLOT(slot_warning(const QString&)));
  connect(encoder_wrapper, SIGNAL(error(const QString&, const QString&)), this, SLOT(slot_error(const QString&, const QString&)));
  connect(encoder_wrapper, SIGNAL(log(const QString&)), this, SLOT(slot_log(const QString&)));

  connect(cdda_extract_thread, SIGNAL(progress(int, int, int)), this, SLOT(progress_extract(int, int, int)));
  connect(cdda_extract_thread, SIGNAL(output(const QByteArray&)), this, SLOT(write_to_wave(const QByteArray&)));
  connect(cdda_extract_thread, SIGNAL(finished()), this, SLOT(finish_extract()));
  connect(cdda_extract_thread, SIGNAL(terminated()), this, SLOT(finish_extract()));
  connect(cdda_extract_thread, SIGNAL(info(const QString&)), this, SLOT(slot_info(const QString&)));
  connect(cdda_extract_thread, SIGNAL(warning(const QString&)), this, SLOT(slot_warning(const QString&)));
  connect(cdda_extract_thread, SIGNAL(error(const QString&, const QString&)), this, SLOT(slot_error(const QString&, const QString&)));
  connect(cdda_extract_thread, SIGNAL(log(const QString&)), this, SLOT(slot_log(const QString&)));

  process_counter = 0;
  timeout_done = FALSE;
  timeout_counter = 0;
  _finished = FALSE;
  _finished_successful = FALSE;

  en_track_index = 0;
  en_track_count = 0;

  ex_track_index = 0;
  ex_track_count = 0;

  current_sector = 0;
  current_encoder_percent = 0;

}

QAudex::~QAudex() {

  delete audex_sql;
  delete encoder_wrapper;
  delete cdda_extract_thread;
  delete wave_file_writer;
  delete jobs;

  QDir dir(tmp_dir);
  if (dir.exists()) {
    qDebug() << QString("(QAudex) Deleting empty temporary directory \"%1\"").arg(tmp_dir);
    dir.rmdir(tmp_dir);
  }

}

bool QAudex::prepare() {

  if (profile_model->currentProfileRow()<0) {
    slot_error(trUtf8("No profile selected. Operation abort."));
    return FALSE;
  }

  QPID pid;
  tmp_dir = settings->value("settings/temp_path", "/tmp/").toString();
  if (tmp_dir.right(1) != "/") tmp_dir += "/";
  tmp_dir += "audex."+QString("%1").arg(pid.getPID())+"/";
  qDebug() << "(QAudex) Temporary directory in use:" << tmp_dir;
  QDir *dir = new QDir(tmp_dir);
  if (!dir->exists()) {
    if (!dir->mkpath(tmp_dir)) {
      slot_error(trUtf8("Unable to create temporary directory \"%1\". Please check. Abort operation.").arg(tmp_dir));
      return FALSE;
    }
  }

  return TRUE;

}

void QAudex::start() {

  emit changedEncodeTrack(0, 0, "");
  if (check()) start_extract(); else request_finish(FALSE);

}

void QAudex::cancel() {

  request_finish(FALSE);

}

const QStringList& QAudex::extractProtocol() {
  return cdda_extract_thread->protocol();
}

const QStringList& QAudex::encoderProtocol() {
  return encoder_wrapper->protocol();
}

void QAudex::start_extract() {

  if (_finished) return;

  if (ex_track_count>=cdda_metadata_model->numOfAudioTracksInSelection()) {
    if (!jobs->jobInProgress() && !jobs->pendingJobs()) request_finish(TRUE);
    return;
  }

  ex_track_index++;

  bool skip = TRUE;
  if (cdda_metadata_model->selection().count() == 0) skip = FALSE;
  if (skip) {
    for (int i = 0; i < cdda_metadata_model->selection().count(); i++) {
      if (cdda_metadata_model->selection()[i].row() == ex_track_index-1) {
        skip = FALSE;
        break;
      }
    }
  }
  if (!cdda_metadata_model->isAudioTrack(ex_track_index-1)) skip = TRUE;

  if (!skip) {

    QString artist = cdda_metadata_model->artist();
    QString album = cdda_metadata_model->album();
    QString tartist = cdda_metadata_model->data(cdda_metadata_model->index(ex_track_index-1, Q_CDDA_METADATA_MODEL_COLUMN_ARTIST_INDEX)).toString();
    QString ttitle = cdda_metadata_model->data(cdda_metadata_model->index(ex_track_index-1, Q_CDDA_METADATA_MODEL_COLUMN_TITLE_INDEX)).toString();
    QString year = cdda_metadata_model->year();
    QString genre = cdda_metadata_model->genre();
    QString ext = profile_model->data(profile_model->index(profile_model->currentProfileRow(), Q_PROFILE_MODEL_COLUMN_EXTENSION_INDEX)).toString();
    QString basepath = settings->value("settings/base_path", "~/").toString();
    bool fat32_compatible = profile_model->data(profile_model->index(profile_model->currentProfileRow(), Q_PROFILE_MODEL_COLUMN_FAT32COMPATIBLE_INDEX)).toBool();
    int cdnum = cdda_metadata_model->cdNum();
    int trackoffset = cdda_metadata_model->trackOffset();
    bool overwrite = settings->value("settings/overwrite_existing_files", FALSE).toBool();

    QString targetFilename;
    if (!construct_target_filename(targetFilename, ex_track_index, cdnum, trackoffset,
	artist, album, tartist, ttitle, year, genre, ext, basepath, fat32_compatible, overwrite, (ex_track_index==1))) {
      request_finish(FALSE);
      return;
    }
    ex_track_target_filename = targetFilename;

    //if empty (maybe because it already exists) skip
    if (!targetFilename.isEmpty()) {

      //in KDE4 we will make use of squeeze capabilities of klineedit...
      emit changedExtractTrack(ex_track_index,
	cdda_metadata_model->numOfAudioTracks(),
	tartist,
	ttitle);

      QString sourceFilename = tmp_dir+QString("%1").arg(cdda_metadata_model->discid())+"."+QString("%1").arg(ex_track_index)+".wav";
      ex_track_source_filename = sourceFilename;
      wave_file_writer->open(sourceFilename);

      if (settings->value("settings/full_paranoia_mode", TRUE).toBool()) {
        cdda_extract_thread->setParanoiaMode(3);
      } else {
        cdda_extract_thread->setParanoiaMode(0);
      }
      cdda_extract_thread->setNeverSkip(settings->value("settings/never_skip_mode", TRUE).toBool());
      cdda_extract_thread->setTrackToRip(ex_track_index);
      cdda_extract_thread->start(); process_counter++;

      ex_track_count++;

    } else {

      en_track_count++;
      ex_track_count++; //important to check for finish
      cdda_extract_thread->skipTrack(ex_track_index);
      start_extract();

    }

  } else {

    start_extract();

  }

}

void QAudex::finish_extract() {

  process_counter--;

  wave_file_writer->close();
  if (_finished) {
    QFile file(ex_track_source_filename);
    file.remove();
    if (!process_counter) execute_finish();
    return;
  }
  jobs->addNewJob(ex_track_source_filename, ex_track_target_filename, ex_track_index);
  start_extract();

}

void QAudex::start_encode() {

  if (_finished) return;

  if (en_track_count>=cdda_metadata_model->numOfAudioTracksInSelection()) {
    request_finish(TRUE);
    return;
  }

  if (encoder_wrapper->isProcessing()) return;

  QJob* job = jobs->orderJob();
  if (!job) return;

  int cdnum = cdda_metadata_model->cdNum();
  int trackoffset = cdda_metadata_model->trackOffset();
  QString artist = cdda_metadata_model->artist();
  QString album = cdda_metadata_model->album();
  QString tartist = cdda_metadata_model->data(cdda_metadata_model->index(job->trackNo()-1, Q_CDDA_METADATA_MODEL_COLUMN_ARTIST_INDEX)).toString();
  QString ttitle = cdda_metadata_model->data(cdda_metadata_model->index(job->trackNo()-1, Q_CDDA_METADATA_MODEL_COLUMN_TITLE_INDEX)).toString();
  QString year = cdda_metadata_model->year();
  QString genre = cdda_metadata_model->genre();
  QString ext = profile_model->data(profile_model->index(profile_model->currentProfileRow(), Q_PROFILE_MODEL_COLUMN_EXTENSION_INDEX)).toString();
  QString basepath = settings->value("settings/base_path", "~/").toString();
  bool fat32_compatible = profile_model->data(profile_model->index(profile_model->currentProfileRow(), Q_PROFILE_MODEL_COLUMN_FAT32COMPATIBLE_INDEX)).toBool();

  QString targetFilename = job->targetFilename();
  en_track_target_filename = targetFilename;

  emit changedEncodeTrack(job->trackNo(), cdda_metadata_model->numOfAudioTracks(), targetFilename);
  en_track_count++;

  en_track_filename = job->sourceFilename();
  en_track_index = job->trackNo();
  if (!encoder_wrapper->encode(job->trackNo(), cdnum, trackoffset,
	artist, album, tartist, ttitle, genre, year, ext, basepath, fat32_compatible,
	job->sourceFilename(), targetFilename)) {
    request_finish(FALSE);
  }
  process_counter++;

}

void QAudex::finish_encode() {

  process_counter--;
  jobs->reportJobFinished();

  target_filename_list.append(en_track_target_filename);

  QFile file(en_track_filename);
  file.remove();

  if (_finished) {
    if (!process_counter) execute_finish();
    return;
  }
  emit changedEncodeTrack(0, 0, "");
  progress_encode(0);
  start_encode();

}

void QAudex::calculate_speed_extract() {
  if ((last_measuring_point_sector > -1) && (cdda_extract_thread->isProcessing())) {
    double new_value = (double)(current_sector-last_measuring_point_sector)/(2.0f*75.0f);
    if (new_value < 0.0f) new_value = 0.0f;
    if ((new_value < 0.2f) && (!timeout_done)) {
      timeout_counter += 2;
      if (timeout_counter >= 300) { timeout_done = TRUE; emit timeout(); }
    }
    emit speedExtract(new_value);
  } else {
    emit speedExtract(0.0f);
  }
  last_measuring_point_sector = current_sector;
}

void QAudex::calculate_speed_encode() {
  if ((last_measuring_point_encoder_percent > -1) && (encoder_wrapper->isProcessing()) && (current_encoder_percent > 0)) {
    int song_length = cdda_metadata_model->data(cdda_metadata_model->index(en_track_index-1, Q_CDDA_METADATA_MODEL_COLUMN_LENGTH_INDEX), Q_CDDA_METADATA_MODEL_INTERNAL_ROLE).toInt();
    double new_value = (double)((double)song_length/100.0f)*((double)current_encoder_percent-(double)last_measuring_point_encoder_percent);
    if (new_value < 0.0f) new_value = 0.0f;
    emit speedEncode(new_value);
  } else {
    emit speedEncode(0.0f);
  }
  last_measuring_point_encoder_percent = current_encoder_percent;
}

void QAudex::progress_extract(int percent_of_track, int sector, int overall_sectors_read) {

  emit progressExtractTrack(percent_of_track);
  emit progressExtractOverall((int)(((float)overall_sectors_read/(float)(cdda_metadata_model->numOfFramesOfAudioTracksInSelection()))*100.0f));

  current_sector = sector;

}

void QAudex::progress_encode(int percent) {
  emit progressEncodeTrack(percent);
  current_encoder_percent = percent;
}

void QAudex::busy_encode() {
  emit progressEncodeTrack(-1);
}

void QAudex::write_to_wave(const QByteArray& data) {
  wave_file_writer->write(data);
}

void QAudex::start_db_sync() {
  emit startDBSync();
}

void QAudex::finished_db_sync() {
  emit finishedDBSync();
}

void QAudex::slot_longer_dbsync() {
  emit longerDBSyncProgress();
}

void QAudex::slot_longer_dbsync_finished() {
  emit longerDBSyncProgressFinished();
}

void QAudex::slot_log(const QString& message) {
  emit log(message);
}

void QAudex::slot_error(const QString& description, const QString& solution) {
  emit error(description, solution);
  request_finish(FALSE);
}

void QAudex::slot_warning(const QString& description) {
  emit warning(description);
}

void QAudex::slot_info(const QString& description) {
  emit info(description);
}

void QAudex::check_if_thread_still_running() {
  if (cdda_extract_thread->isRunning()) {
    // this could happen if the thread is stuck in paranoia_read
    // because of an unreadable cd
    cdda_extract_thread->terminate();
    emit log("(QAudex) Terminate extracting thread.");
  }
}

bool QAudex::construct_target_filename(QString& targetFilename,
	int trackno, int cdno, int gindex,
	const QString& artist, const QString& album,
	const QString& tartist, const QString& ttitle,
	const QString& year, const QString& genre, const QString& ext, const QString& basepath,
	bool fat_compatible,
	bool overwrite_existing_files, bool is_first_track) {

  QMaskParser maskparser;
  targetFilename = maskparser.parseFilenameMask(profile_model->data(profile_model->index(profile_model->currentProfileRow(), Q_PROFILE_MODEL_COLUMN_MASK_INDEX)).toString(),
	trackno, cdno, gindex, artist, album, tartist, ttitle, year, genre, ext, basepath, fat_compatible);

  int lastSlash = targetFilename.lastIndexOf("/", -1);
  if (lastSlash == -1) {
    emit error(trUtf8("Cant find path \"%1\".").arg(targetFilename), trUtf8("Please check your path (write access?)"));
    return FALSE;
  }
  QString targetPath = targetFilename.mid(0, lastSlash);
  target_dir = targetPath;
  QString targetStrippedFilename = targetFilename.mid(lastSlash+1);
  QDir *dir = new QDir(targetPath);
  if (!dir->exists()) {
    if (!dir->mkpath(targetPath)) {
      emit error(trUtf8("Unable to create directory \"%1\".").arg(targetPath), trUtf8("Please check your path (write access?)"));
      return FALSE;
    } else {
      emit info(trUtf8("Directory \"%1\" successfully created.").arg(targetPath));
    }
  } else {
    if (is_first_track) emit warning(trUtf8("Directory \"%1\" already exists.").arg(targetPath));
  }
  delete dir;

  QDiskFreeSpace diskfreespace;
  quint64 free = diskfreespace.getFreeSpace(targetPath) / 1024;
  if (free < 200*1024) {
    emit warning(trUtf8("Free space on \"%1\" is less than 200 MB. Problems with low space possible.").arg(targetPath));
  }

  QFile *file = new QFile(targetFilename);
  if (file->exists()) {
    if (overwrite_existing_files) {
      emit warning(trUtf8("Warning! File \"%1\" already exists. Overwriting.").arg(targetStrippedFilename));
    } else {
      emit warning(trUtf8("Warning! File \"%1\" already exists. Skipping.").arg(targetStrippedFilename));
      target_filename_list.append(targetFilename);
      targetFilename.clear();
    }
  }
  delete file;

  return TRUE;

}

bool QAudex::check() {
  QString tmp = settings->value("settings/temp_path", "/tmp/").toString();

  if (tmp.isEmpty()) {
    slot_error(trUtf8("Temporary directory name \"%1\" must not be empty.").arg(tmp), trUtf8("Please set one."));
    return FALSE;
  }

  QDir *dir = new QDir(tmp);
  if (!dir->exists()) {
    slot_error(trUtf8("Temporary directory \"%1\" does not exists.").arg(tmp), trUtf8("Please check."));
    return FALSE;
  }
  delete dir;

  QDiskFreeSpace diskfreespace;
  quint64 free = diskfreespace.getFreeSpace(tmp) / 1024;
  if (free < 800*1024) {
    slot_warning(trUtf8("Free space on temporary directory \"%1\" is less than 800 MiB. Problems with low space possible.").arg(tmp));
  } else if (free < 200*1024) {
    slot_error(trUtf8("Temporary directory \"%1\" needs at least 200 MB of free memory.").arg(tmp), trUtf8("Please free space or set another path."));
    return FALSE;
  }

  return TRUE;
}

void QAudex::request_finish(bool successful) {

  if (!_finished) {
    _finished = TRUE;
    _finished_successful = successful;
  } else {
    return;
  }

  if (process_counter>0) {

    encoder_wrapper->cancel();
    cdda_extract_thread->cancel();
    QTimer::singleShot(1000, this, SLOT(check_if_thread_still_running()));

  } else {

    execute_finish();

  }

}

void QAudex::execute_finish() {
  if (settings->value("settings/eject_after_extract", FALSE).toBool()) {
    emit info(trUtf8("Eject CD tray"));
    device->eject();
  }
  if ((_finished_successful) && (profile_model->data(profile_model->index(profile_model->currentProfileRow(), Q_PROFILE_MODEL_COLUMN_SC_INDEX)).toBool())) {
    //store the cover
    if (cdda_metadata_model->customData("cover").value<QImage>()!=QImage()) {
      QImage image(cdda_metadata_model->customData("cover").value<QImage>());
      if (profile_model->data(profile_model->index(profile_model->currentProfileRow(), Q_PROFILE_MODEL_COLUMN_SC_SCALE_INDEX)).toBool()) {
        QSize size = profile_model->data(profile_model->index(profile_model->currentProfileRow(), Q_PROFILE_MODEL_COLUMN_SC_SIZE_INDEX)).toSize();
        image = image.scaled(size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        emit log(QString("(QAudex) Cover scaled to %1x%2.").arg(size.width()).arg(size.height()));
      }
      QString mask = profile_model->data(profile_model->index(profile_model->currentProfileRow(), Q_PROFILE_MODEL_COLUMN_SC_NAME_INDEX)).toString();
      QString format = profile_model->data(profile_model->index(profile_model->currentProfileRow(), Q_PROFILE_MODEL_COLUMN_SC_FORMAT_INDEX)).toString();

      QMaskParser maskparser;
      QString coverName = maskparser.parseSimpleMask(mask,
	cdda_metadata_model->cdNum(), cdda_metadata_model->artist(), cdda_metadata_model->album(),
	QString("%1").arg(cdda_metadata_model->year()), cdda_metadata_model->genre(),
	format.toLower(), profile_model->data(profile_model->index(profile_model->currentProfileRow(), Q_PROFILE_MODEL_COLUMN_FAT32COMPATIBLE_INDEX)).toBool());
      QString fileName = target_dir+"/"+coverName;

      image.save(fileName, format.toAscii().data());
      emit info(trUtf8("Cover \"%1\" successfully saved.").arg(coverName));
    }
  }
  if ((_finished_successful) && (profile_model->data(profile_model->index(profile_model->currentProfileRow(), Q_PROFILE_MODEL_COLUMN_PL_INDEX)).toBool())) {
    //create the playlist
    QPlaylist playlist(target_filename_list, cdda_metadata_model->metadata());
    QString mask = profile_model->data(profile_model->index(profile_model->currentProfileRow(), Q_PROFILE_MODEL_COLUMN_PL_NAME_INDEX)).toString();
    QString format = profile_model->data(profile_model->index(profile_model->currentProfileRow(), Q_PROFILE_MODEL_COLUMN_PL_FORMAT_INDEX)).toString();

    QMaskParser maskparser;
    QString playlistName = maskparser.parseSimpleMask(mask,
	cdda_metadata_model->cdNum(), cdda_metadata_model->artist(), cdda_metadata_model->album(),
	QString("%1").arg(cdda_metadata_model->year()), cdda_metadata_model->genre(),
	format.toLower(), profile_model->data(profile_model->index(profile_model->currentProfileRow(), Q_PROFILE_MODEL_COLUMN_FAT32COMPATIBLE_INDEX)).toBool());
    QString fileName = target_dir+"/"+playlistName;

    QFile data(fileName);
    if (data.open(QFile::WriteOnly | QFile::Truncate)) {
      QTextStream out(&data);
      if (format=="M3U") {
        out << playlist.asM3U().join("\n");
      } else if (format=="PLS") {
        out << playlist.asPLS(TRUE).join("\n");
      } else if (format=="XSPF") {
        out << playlist.asXSPF().join("\n");
      }
    }
    data.close();
    emit info(trUtf8("Playlist \"%1\" successfully created.").arg(playlistName));
  }
  if ((_finished_successful) && (settings->value("settings/sql_enabled", FALSE).toBool())) {
    //do some database sync
    audex_sql->sync();
  }
  emit finished(_finished_successful);
}
