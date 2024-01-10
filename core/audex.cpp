/* AUDEX CDDA EXTRACTOR
 * Copyright (C) 2007-2009 by Marco Nelles (audex@maniatek.de)
 * http://opensource.maniatek.de/audex
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

#include "audex.h"

/* The heart of audex */


Audex::Audex(QWidget* parent, ProfileModel *profile_model, CDDAModel *cdda_model) : QObject(parent) {

  Q_UNUSED(parent);

  this->profile_model = profile_model;
  this->cdda_model = cdda_model;

  p_profile_name = profile_model->data(profile_model->index(profile_model->currentProfileRow(), PROFILE_MODEL_COLUMN_NAME_INDEX)).toString();
  p_suffix = profile_model->getSelectedEncoderSuffixFromCurrentIndex();

  encoder_wrapper = new EncoderWrapper(this,
			profile_model->getSelectedEncoderPatternFromCurrentIndex(),
			Preferences::deletePartialFiles());

  if (!encoder_wrapper) {
    kDebug() << "PANIC ERROR. Could not load object EncoderWrapper. Low mem?";
    return;
  }

  cdda_extract_thread = new CDDAExtractThread(this, cdda_model->device());
  if (!encoder_wrapper) {
    kDebug() << "PANIC ERROR. Could not load object CDDAExtractThread. Low mem?";
    return;
  }

  jobs = new AudexJobs();
  connect(jobs, SIGNAL(newJobAvailable()), this, SLOT(start_encode()));

  wave_file_writer = new WaveFileWriter();

  last_measuring_point_sector = -1;
  timer_extract = new QTimer(this);
  connect(timer_extract, SIGNAL(timeout()), this, SLOT(calculate_speed_extract()));
  timer_extract->start(2000);

  last_measuring_point_encoder_percent = -1;
  timer_encode = new QTimer(this);
  connect(timer_encode, SIGNAL(timeout()), this, SLOT(calculate_speed_encode()));
  timer_encode->start(1000);

  connect(encoder_wrapper, SIGNAL(progress(int)), this, SLOT(progress_encode(int)));
  connect(encoder_wrapper, SIGNAL(finished()), this, SLOT(finish_encode()));
  connect(encoder_wrapper, SIGNAL(info(const QString&)), this, SLOT(slot_info(const QString&)));
  connect(encoder_wrapper, SIGNAL(warning(const QString&)), this, SLOT(slot_warning(const QString&)));
  connect(encoder_wrapper, SIGNAL(error(const QString&, const QString&)), this, SLOT(slot_error(const QString&, const QString&)));

  connect(cdda_extract_thread, SIGNAL(progress(int, int, int)), this, SLOT(progress_extract(int, int, int)));
  connect(cdda_extract_thread, SIGNAL(output(const QByteArray&)), this, SLOT(write_to_wave(const QByteArray&)));
  connect(cdda_extract_thread, SIGNAL(finished()), this, SLOT(finish_extract()));
  connect(cdda_extract_thread, SIGNAL(terminated()), this, SLOT(finish_extract()));
  connect(cdda_extract_thread, SIGNAL(info(const QString&)), this, SLOT(slot_info(const QString&)));
  connect(cdda_extract_thread, SIGNAL(warning(const QString&)), this, SLOT(slot_warning(const QString&)));
  connect(cdda_extract_thread, SIGNAL(error(const QString&, const QString&)), this, SLOT(slot_error(const QString&, const QString&)));

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

  overall_frames = 0;

}

Audex::~Audex() {

  delete encoder_wrapper;
  delete cdda_extract_thread;
  delete wave_file_writer;
  delete jobs;

  QDir dir(tmp_dir);
  if (dir.exists()) {
    kDebug() << QString("Deleting empty temporary directory \"%1\"").arg(tmp_dir);
    dir.rmdir(tmp_dir);
  }

}

bool Audex::prepare() {

  if (profile_model->currentProfileRow()<0) {
    slot_error(i18n("No profile selected. Operation abort."));
    return FALSE;
  }

  PID pid;
  tmp_dir = temp_path();
  if (tmp_dir.right(1) != "/") tmp_dir += "/";
  tmp_dir += "audex."+QString("%1").arg(pid.getPID())+"/";
  kDebug() << "Temporary directory in use:" << tmp_dir;
  QDir *dir = new QDir(tmp_dir);
  if (!dir->exists()) {
    if (!dir->mkpath(tmp_dir)) {
      slot_error(i18n("Unable to create temporary directory \"%1\". Please check. Abort operation.", tmp_dir));
      return FALSE;
    }
  }

  return TRUE;

}

void Audex::start() {

  emit changedEncodeTrack(0, 0, "");
  emit info(i18n("Start ripping and encoding with profile \"%1\"...", p_profile_name));
  if (check()) start_extract(); else request_finish(FALSE);

}

void Audex::cancel() {

  request_finish(FALSE);

}

const QStringList& Audex::extractProtocol() {
  return cdda_extract_thread->protocol();
}

const QStringList& Audex::encoderProtocol() {
  return encoder_wrapper->protocol();
}

void Audex::start_extract() {

  if (_finished) return;

  if (ex_track_count>=cdda_model->numOfAudioTracksInSelection()) {
    if (!jobs->jobInProgress() && !jobs->pendingJobs()) request_finish(TRUE);
    return;
  }

  ex_track_index++;

  bool skip = !cdda_model->isTrackInSelection(ex_track_index);

  if (!cdda_model->isAudioTrack(ex_track_index)) skip = TRUE;

  if (!skip) {

    QString artist = cdda_model->artist();
    QString title = cdda_model->title();
    QString tartist = cdda_model->data(cdda_model->index(ex_track_index-1, CDDA_MODEL_COLUMN_ARTIST_INDEX)).toString();
    QString ttitle = cdda_model->data(cdda_model->index(ex_track_index-1, CDDA_MODEL_COLUMN_TITLE_INDEX)).toString();
    QString year = cdda_model->year();
    QString genre = cdda_model->genre();
    QString suffix = p_suffix;
    QString basepath = Preferences::basePath();
    bool fat32_compatible = profile_model->data(profile_model->index(profile_model->currentProfileRow(), PROFILE_MODEL_COLUMN_FAT32COMPATIBLE_INDEX)).toBool();
    int cdnum;
    if (!cdda_model->isMultiCD()) cdnum = 0; else cdnum = cdda_model->cdNum();
    int trackoffset = cdda_model->trackOffset();
    bool overwrite = Preferences::overwriteExistingFiles();

    QString targetFilename;
    if (!construct_target_filename(targetFilename, ex_track_index, cdnum, trackoffset,
	artist, title, tartist, ttitle, year, genre, suffix, basepath, fat32_compatible, overwrite, (ex_track_index==1))) {
      request_finish(FALSE);
      return;
    }
    ex_track_target_filename = targetFilename;

    //if empty (maybe because it already exists) skip
    if (!targetFilename.isEmpty()) {

      emit changedExtractTrack(ex_track_index,
	cdda_model->numOfAudioTracks(),
	tartist,
	ttitle);

      QString sourceFilename = tmp_dir+QString("%1").arg(cdda_model->discid())+"."+QString("%1").arg(ex_track_index)+".wav";
      ex_track_source_filename = sourceFilename;
      wave_file_writer->open(sourceFilename);

      if (Preferences::paranoiaMode()) {
        cdda_extract_thread->setParanoiaMode(3);
      } else {
        cdda_extract_thread->setParanoiaMode(0);
      }
      cdda_extract_thread->setNeverSkip(Preferences::neverSkip());
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

void Audex::finish_extract() {

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

void Audex::start_encode() {

  if (_finished) return;

  if (en_track_count>=cdda_model->numOfAudioTracksInSelection()) {
    request_finish(TRUE);
    return;
  }

  if (encoder_wrapper->isProcessing()) return;

  AudexJob* job = jobs->orderJob();
  if (!job) return;

  int cdnum = cdda_model->cdNum();
  int trackoffset = cdda_model->trackOffset();
  QString artist = cdda_model->artist();
  QString title = cdda_model->title();
  QString tartist = cdda_model->data(cdda_model->index(job->trackNo()-1, CDDA_MODEL_COLUMN_ARTIST_INDEX)).toString();
  QString ttitle = cdda_model->data(cdda_model->index(job->trackNo()-1, CDDA_MODEL_COLUMN_TITLE_INDEX)).toString();
  QString year = cdda_model->year();
  QString genre = cdda_model->genre();
  QString suffix = p_suffix;
  QImage cover = cdda_model->cover();
  bool fat32_compatible = profile_model->data(profile_model->index(profile_model->currentProfileRow(), PROFILE_MODEL_COLUMN_FAT32COMPATIBLE_INDEX)).toBool();

  QString targetFilename = job->targetFilename();
  en_track_target_filename = targetFilename;

  emit changedEncodeTrack(job->trackNo(), cdda_model->numOfAudioTracks(), targetFilename);
  en_track_count++;

  en_track_filename = job->sourceFilename();
  en_track_index = job->trackNo();
  if (!encoder_wrapper->encode(job->trackNo(), cdnum, trackoffset,
	artist, title, tartist, ttitle, genre, year, suffix, cover, fat32_compatible, temp_path(),
	job->sourceFilename(), targetFilename)) {
    request_finish(FALSE);
  }
  process_counter++;

}

void Audex::finish_encode() {

  process_counter--;
  jobs->reportJobFinished();

  QString basepath = Preferences::basePath();
  QString filename = en_track_target_filename.mid(basepath.length());
  if (filename[0]!='/') filename = "/"+filename;
  cdda_model->setCustomDataPerTrack(en_track_index, "filename", filename);

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

void Audex::calculate_speed_extract() {
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

void Audex::calculate_speed_encode() {
  if ((last_measuring_point_encoder_percent > -1) && (encoder_wrapper->isProcessing()) && (current_encoder_percent > 0)) {
    int song_length = cdda_model->data(cdda_model->index(en_track_index-1, CDDA_MODEL_COLUMN_LENGTH_INDEX), CDDA_MODEL_INTERNAL_ROLE).toInt();
    double new_value = (double)((double)song_length/100.0f)*((double)current_encoder_percent-(double)last_measuring_point_encoder_percent);
    if (new_value < 0.0f) new_value = 0.0f;
    emit speedEncode(new_value);
  } else {
    emit speedEncode(0.0f);
  }
  last_measuring_point_encoder_percent = current_encoder_percent;
}

void Audex::progress_extract(int percent_of_track, int sector, int overall_sectors_read) {

  if (overall_frames==0) {
    QSet<int> sel = cdda_model->selectedTracks();
    QSet<int>::ConstIterator it(sel.begin()), end(sel.end());
    for (; it!=end; ++it) {
      if ((*it < 0) || (*it > cdda_extract_thread->cddaParanoia()->numOfTracks()) ||
           (!cdda_extract_thread->cddaParanoia()->isAudioTrack((*it)))) {
        continue;
      }
      overall_frames += cdda_extract_thread->cddaParanoia()->numOfFramesOfTrack((*it));
    }
  }

  float fraction = 0.0f;
  if (overall_frames>0) fraction = (float)overall_sectors_read / (float)overall_frames;

  emit progressExtractTrack(percent_of_track);
  emit progressExtractOverall((int)(fraction*100.0f));

  current_sector = sector;

}

void Audex::progress_encode(int percent) {
  emit progressEncodeTrack(percent);
  current_encoder_percent = percent;
}

void Audex::write_to_wave(const QByteArray& data) {
  wave_file_writer->write(data);
}

void Audex::start_ftp_transfer(const QString& filename) {
  emit ftpUploadStart(filename);
}

void Audex::progress_ftp_transfer(const int percent, const int overall) {
  emit ftpUploadProgress(percent, overall);
}

void Audex::finished_ftp_transfer(const QString& filename) {
 emit ftpUploadFinished(filename);
}

void Audex::slot_error(const QString& description, const QString& solution) {
  emit error(description, solution);
  request_finish(FALSE);
}

void Audex::slot_warning(const QString& description) {
  emit warning(description);
}

void Audex::slot_info(const QString& description) {
  emit info(description);
}

void Audex::check_if_thread_still_running() {
  if (cdda_extract_thread->isRunning()) {
    // this could happen if the thread is stuck in paranoia_read
    // because of an unreadable cd
    cdda_extract_thread->terminate();
    kDebug() << "Terminate extracting thread.";
  }
}

bool Audex::construct_target_filename(QString& targetFilename,
	int trackno, int cdno, int gindex,
	const QString& artist, const QString& title,
	const QString& tartist, const QString& ttitle,
	const QString& year, const QString& genre,
	const QString& ext, const QString& basepath,
	bool fat_compatible,
	bool overwrite_existing_files, bool is_first_track) {

  PatternParser patternparser;
  targetFilename = ((basepath.right(1)=="/")?basepath:basepath+"/")+patternparser.parseFilenamePattern(profile_model->data(profile_model->index(profile_model->currentProfileRow(), PROFILE_MODEL_COLUMN_PATTERN_INDEX)).toString(),
	trackno, cdno, gindex, artist, title, tartist, ttitle, year, genre, ext, fat_compatible);

  int lastSlash = targetFilename.lastIndexOf("/", -1);
  if (lastSlash == -1) {
    emit error(i18n("Cant find path \"%1\".", targetFilename), i18n("Please check your path (write access?)"));
    return FALSE;
  }
  QString targetPath = targetFilename.mid(0, lastSlash);
  target_dir = targetPath;
  QString targetStrippedFilename = targetFilename.mid(lastSlash+1);
  QDir *dir = new QDir(targetPath);
  if (!dir->exists()) {
    if (!dir->mkpath(targetPath)) {
      emit error(i18n("Unable to create directory \"%1\".", targetPath), i18n("Please check your path (write access?)"));
      return FALSE;
    } else {
      emit info(i18n("Directory \"%1\" successfully created.", targetPath));
    }
  } else {
    if (is_first_track) emit warning(i18n("Directory \"%1\" already exists.", targetPath));
  }
  delete dir;

  DiskFreeSpace diskfreespace;
  quint64 free = diskfreespace.getFreeSpace(targetPath) / 1024;
  if (free < 200*1024) {
    emit warning(i18n("Free space on \"%1\" is less than 200 MB. Problems with low space possible.", targetPath));
  }

  QFile *file = new QFile(targetFilename);
  if (file->exists()) {
    if (overwrite_existing_files) {
      emit warning(i18n("Warning! File \"%1\" already exists. Overwriting.", targetStrippedFilename));
    } else {
      emit warning(i18n("Warning! File \"%1\" already exists. Skipping.", targetStrippedFilename));

      QString basepath = Preferences::basePath();
      QString filename = targetFilename.mid(basepath.length());
      if (filename[0]!='/') filename = "/"+filename;
      cdda_model->setCustomDataPerTrack(trackno, "filename", filename);

      target_filename_list.append(targetFilename);
      targetFilename.clear();
    }
  }
  delete file;

  return TRUE;

}

bool Audex::check() {
  QString tmp = temp_path();

  if (tmp.isEmpty()) {
    slot_error(i18n("Temporary directory name \"%1\" must not be empty.", tmp), i18n("Please set one."));
    return FALSE;
  }

  QDir *dir = new QDir(tmp);
  if (!dir->exists()) {
    slot_error(i18n("Temporary directory \"%1\" does not exists.", tmp), i18n("Please check."));
    return FALSE;
  }
  delete dir;

  DiskFreeSpace diskfreespace;
  quint64 free = diskfreespace.getFreeSpace(tmp) / 1024;
  if (free < 800*1024) {
    slot_warning(i18n("Free space on temporary directory \"%1\" is less than 800 MiB. Problems with low space possible.", tmp));
  } else if (free < 200*1024) {
    slot_error(i18n("Temporary directory \"%1\" needs at least 200 MiB of free memory.", tmp), i18n("Please free space or set another path."));
    return FALSE;
  }

  return TRUE;
}

void Audex::request_finish(bool successful) {

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

void Audex::execute_finish() {

  if (Preferences::ejectCDTray()) {
    emit info(i18n("Eject CD tray"));
    cdda_model->eject();
  }

  bool overwrite = Preferences::overwriteExistingFiles();

  QString co;
  if ((_finished_successful) && (profile_model->data(profile_model->index(profile_model->currentProfileRow(), PROFILE_MODEL_COLUMN_SC_INDEX)).toBool())) {

    //store the cover
    if (!cdda_model->cover().isNull()) {
      QImage image(cdda_model->cover());
      if (profile_model->data(profile_model->index(profile_model->currentProfileRow(), PROFILE_MODEL_COLUMN_SC_SCALE_INDEX)).toBool()) {
        QSize size = profile_model->data(profile_model->index(profile_model->currentProfileRow(), PROFILE_MODEL_COLUMN_SC_SIZE_INDEX)).toSize();
        image = image.scaled(size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        kDebug() << QString("Cover scaled to %1x%2.").arg(size.width()).arg(size.height());
      }
      QString pattern = profile_model->data(profile_model->index(profile_model->currentProfileRow(), PROFILE_MODEL_COLUMN_SC_NAME_INDEX)).toString();
      QString format = profile_model->data(profile_model->index(profile_model->currentProfileRow(), PROFILE_MODEL_COLUMN_SC_FORMAT_INDEX)).toString();

      PatternParser patternparser;
      QString coverName = patternparser.parseSimplePattern(pattern,
	cdda_model->cdNum(), cdda_model->artist(), cdda_model->title(),
	QString("%1").arg(cdda_model->year()), cdda_model->genre(),
	format.toLower(), profile_model->data(profile_model->index(profile_model->currentProfileRow(), PROFILE_MODEL_COLUMN_FAT32COMPATIBLE_INDEX)).toBool());
      QString fileName = target_dir+"/"+coverName;

      bool to_store = TRUE;
      if (!overwrite) {
        QFileInfo info(fileName);
        if (info.exists()) {
	  to_store = FALSE;
	  emit warning(i18n("Warning! File \"%1\" already exists. Skipping.", info.fileName()));
	}
      }
      if (to_store) {
        image.save(fileName, format.toAscii().data());
        emit info(i18n("Cover \"%1\" successfully saved.", coverName));
        co = fileName;
      }
    }

  }

  QString pl;
  if ((_finished_successful) && (profile_model->data(profile_model->index(profile_model->currentProfileRow(), PROFILE_MODEL_COLUMN_PL_INDEX)).toBool()) && (target_filename_list.count()>0)) {
    //create the playlist
    Playlist playlist(target_filename_list, cdda_model);
    QString pattern = profile_model->data(profile_model->index(profile_model->currentProfileRow(), PROFILE_MODEL_COLUMN_PL_NAME_INDEX)).toString();
    QString format = profile_model->data(profile_model->index(profile_model->currentProfileRow(), PROFILE_MODEL_COLUMN_PL_FORMAT_INDEX)).toString();

    PatternParser patternparser;
    QString playlistName = patternparser.parseSimplePattern(pattern,
	cdda_model->cdNum(), cdda_model->artist(), cdda_model->title(),
	QString("%1").arg(cdda_model->year()), cdda_model->genre(),
	format.toLower(), profile_model->data(profile_model->index(profile_model->currentProfileRow(), PROFILE_MODEL_COLUMN_FAT32COMPATIBLE_INDEX)).toBool());
    QString fileName = target_dir+"/"+playlistName;

    bool to_store = TRUE;
    if (((!cdda_model->isMultiCD()) || (cdda_model->cdNum()<1)) && (format!="M3U")) {
      if (!overwrite) {
        QFileInfo info(fileName);
        if (info.exists()) {
          to_store = FALSE;
	  emit warning(i18n("Warning! File \"%1\" already exists. Skipping.", info.fileName()));
        }
      }
    }
    if (to_store) {
      QFile data(fileName);
      bool fexists = data.exists() && cdda_model->isMultiCD() && (cdda_model->cdNum()>0);
      bool success;
      if ((fexists) && (format=="M3U"))
        success = data.open(QFile::WriteOnly | QFile::Append);
      else
        success = data.open(QFile::WriteOnly | QFile::Truncate);
      if (success) {
        QTextStream out(&data);
        if (format=="M3U") {
	  if (fexists) {
	    out << "\n" << playlist.asM3U(FALSE, TRUE).join("\n");
	  } else {
            out << playlist.asM3U().join("\n");
	  }
        } else if (format=="PLS") {
          out << playlist.asPLS(TRUE).join("\n");
        } else if (format=="XSPF") {
          out << playlist.asXSPF().join("\n");
        }
	data.close();
	emit info(i18n("Playlist \"%1\" successfully created.", playlistName));
	pl = fileName;
      }
    }

  }

  QString in;
  if ((_finished_successful) && (profile_model->data(profile_model->index(profile_model->currentProfileRow(), PROFILE_MODEL_COLUMN_INF_INDEX)).toBool())) {

    PatternParser patternparser;
    QString infoName = patternparser.parseSimplePattern(profile_model->data(profile_model->index(profile_model->currentProfileRow(), PROFILE_MODEL_COLUMN_INF_NAME_INDEX)).toString(),
	cdda_model->cdNum(), cdda_model->artist(), cdda_model->title(),
	QString("%1").arg(cdda_model->year()), cdda_model->genre(),
	profile_model->data(profile_model->index(profile_model->currentProfileRow(), PROFILE_MODEL_COLUMN_INF_SUFFIX_INDEX)).toString(), profile_model->data(profile_model->index(profile_model->currentProfileRow(), PROFILE_MODEL_COLUMN_FAT32COMPATIBLE_INDEX)).toBool());
    QString fileName = target_dir+"/"+infoName;

    bool to_store = TRUE;
    if (!overwrite) {
      QFileInfo info(fileName);
      if (info.exists()) {
        to_store = FALSE;
	emit warning(i18n("Warning! File \"%1\" already exists. Skipping.", info.fileName()));
      }
    }
    if (to_store) {
      QFile file(fileName);
      if (file.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&file);
        QStringList text = profile_model->data(profile_model->index(profile_model->currentProfileRow(), PROFILE_MODEL_COLUMN_INF_TEXT_INDEX)).toStringList();
        patternparser.parseInfoText(text, cdda_model->artist(), cdda_model->title(),
	  QString("%1").arg(cdda_model->year()), cdda_model->genre(),
	  cdda_model->discid(), size_of_all_files_in_list(target_filename_list), cdda_model->lengthOfAudioTracksInSelection(), cdda_model->numOfAudioTracksInSelection());
        out << text.join("\n");
	file.close();
	emit info(i18n("Infofile \"%1\" successfully stored.", infoName));
        in = fileName;
      }
    }

  }

  QString hl;
  if ((_finished_successful) && (profile_model->data(profile_model->index(profile_model->currentProfileRow(), PROFILE_MODEL_COLUMN_HL_INDEX)).toBool())) {

    QString pattern = profile_model->data(profile_model->index(profile_model->currentProfileRow(), PROFILE_MODEL_COLUMN_HL_NAME_INDEX)).toString();
    QString format = profile_model->data(profile_model->index(profile_model->currentProfileRow(), PROFILE_MODEL_COLUMN_HL_FORMAT_INDEX)).toString();

    PatternParser patternparser;
    QString infoName = patternparser.parseSimplePattern(pattern,
	cdda_model->cdNum(), cdda_model->artist(), cdda_model->title(),
	QString("%1").arg(cdda_model->year()), cdda_model->genre(),
	format.toLower(), profile_model->data(profile_model->index(profile_model->currentProfileRow(), PROFILE_MODEL_COLUMN_FAT32COMPATIBLE_INDEX)).toBool());
    QString fileName = target_dir+"/"+infoName;

    bool to_store = TRUE;
    if ((!cdda_model->isMultiCD()) || (cdda_model->cdNum()<1)) {
      if (!overwrite) {
        QFileInfo info(fileName);
        if (info.exists()) {
          to_store = FALSE;
	  emit warning(i18n("Warning! File \"%1\" already exists. Skipping.", info.fileName()));
        }
      }
    }
    if (to_store) {
      QFile file(fileName);
      bool fexists = file.exists() && cdda_model->isMultiCD() && (cdda_model->cdNum()>0);
      bool success;
      if (fexists)
        success = file.open(QFile::WriteOnly | QFile::Append);
      else
        success = file.open(QFile::WriteOnly | QFile::Truncate);
      if (success) {
        QTextStream out(&file);
	if (fexists) out << "\n";
	Hashlist hashlist;
	if (format=="SFV") {
	  out << hashlist.getSFV(target_filename_list).join("\n");
	}
	emit info(i18n("Hashlist \"%1\" successfully stored.", infoName));
	hl = fileName;
	file.close();
      }
    }

  }

  QString di;
  if ((_finished_successful) && (profile_model->data(profile_model->index(profile_model->currentProfileRow(), PROFILE_MODEL_COLUMN_DISCID_INDEX)).toBool())) {

    di = target_dir+"/discid";

    bool to_store = TRUE;
    if (!overwrite) {
      QFileInfo info(di);
      if (info.exists()) {
        to_store = FALSE;
	emit warning(i18n("Warning! File \"%1\" already exists. Skipping.", info.fileName()));
      }
    }
    if (to_store) {
      QFile file(di);
      if (file.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&file);
        out << QString("%1").arg(cdda_model->discid());
	file.close();
	emit info(i18n("Discid successfully stored."));
      }
    } else {
      di.clear();
    }

  }

  if ((_finished_successful) && (Preferences::upload()) && (target_filename_list.count()>0)) {

    QString targetpath = QFileInfo(target_filename_list[0]).absolutePath().mid(Preferences::basePath().length());

    QStringList files_to_transfer = target_filename_list;
    if (!co.isEmpty()) files_to_transfer << co;
    if (!pl.isEmpty()) files_to_transfer << pl;
    if (!in.isEmpty()) files_to_transfer << in;
    if (!hl.isEmpty()) files_to_transfer << hl;
    if (!di.isEmpty()) files_to_transfer << di;

    Upload upload(Preferences::url(), this);
    connect(&upload, SIGNAL(info(const QString&)), this, SLOT(slot_info(const QString&)));
    connect(&upload, SIGNAL(error(const QString&, const QString&)), this, SLOT(slot_error(const QString&, const QString&)));
    upload.upload(targetpath, files_to_transfer);

  }

  //flush temporary path
  QDir tmp(tmp_dir);
  QStringList files = tmp.entryList(QStringList() << "*", QDir::Files | QDir::NoDotAndDotDot);
  for (int i = 0; i < files.count(); ++i) {
    QFile::remove(tmp_dir+files[i]);
    kDebug() << "Deleted temporary file" << tmp_dir+files[i];
  }

  emit finished(_finished_successful);

}

qreal Audex::size_of_all_files_in_list(const QStringList& filenames) const {
  qreal size = .0f;
  for (int i = 0; i < filenames.count(); ++i) {
    QFileInfo info(filenames.at(i));
    size += info.size();
  }
  return size;
}
