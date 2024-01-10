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

#ifndef QAUDEX_HEADER
#define QAUDEX_HEADER

#include <QtCore>

#include "base/qprofilemodel.h"
#include "metadata/qcddametadatamodel.h"
#include "device/qcddaextractthread.h"
#include "device/qcddadevice.h"
#include "base/qwavefilewriter.h"
#include "base/qencoderwrapper.h"
#include "base/qdiskfreespace.h"
#include "base/qmaskparser.h"
#include "base/qpid.h"
#include "base/qplaylist.h"
#include "sql/qaudexsql.h"

class QJob : public QObject {
  Q_OBJECT
public:
  QJob() {
    source_filename = "";
    target_filename = "";
    _trackno = 0;
  }
  ~QJob() { }
  void setSourceFilename(const QString& n) { source_filename = n; }
  QString sourceFilename() const { return source_filename; }
  void setTargetFilename(const QString& n) { target_filename = n; }
  QString targetFilename() const { return target_filename; }
  void setTrackNo(const int t) { _trackno = t; }
  int trackNo() const { return _trackno; }
private:
  QString source_filename;
  QString target_filename;
  int _trackno;
};

class QJobs : public QObject {
  Q_OBJECT
public:
  QJobs() {
    job_in_progress = false;
  }
  ~QJobs() {
    for (int i = 0; i < cache.count(); i++) {
      //make really sure all files are away
      QFile file(cache.at(i)->sourceFilename());
      if (file.exists()) file.remove();
      delete cache.at(i);
    }
  }
  QJob* orderJob() {
    if (job_queue.isEmpty()) {
      return NULL;
    } else {
      job_in_progress = true;
      return job_queue.dequeue();
    }
  }
  void reportJobFinished() {
    job_in_progress = false;
  }
  bool jobInProgress() {
    return job_in_progress;
  }
  bool pendingJobs() {
    return (job_queue.count() > 0);
  }
public slots:
  void addNewJob(const QString& sourceFilename, const QString& targetFilename, const int trackno) {
    QJob *j = new QJob();
    j->setSourceFilename(sourceFilename);
    j->setTargetFilename(targetFilename);
    j->setTrackNo(trackno);
    job_queue.enqueue(j);
    cache.append(j);
    emit newJobAvailable();
  }
signals:
  void newJobAvailable();
private:
  QQueue<QJob*> job_queue;
  QList<QJob*> cache;
  bool job_in_progress;
};

class QAudex : public QObject {

  Q_OBJECT

public:
  QAudex(QWidget* parent, QCDDADevice *device, QSettings *settings, QProfileModel *profile_model, QCDDAMetaDataModel *cdda_metadata_model);
  ~QAudex();

  bool prepare();

public slots:
  void start();
  void cancel();

  const QStringList& extractProtocol();
  const QStringList& encoderProtocol();

private slots:
  void start_extract();
  void finish_extract();
  void start_encode();
  void finish_encode();

  void calculate_speed_extract();
  void calculate_speed_encode();

  void progress_extract(int percent_of_track, int sector, int overall_sectors_read);
  void progress_encode(int percent);
  void busy_encode();

  void write_to_wave(const QByteArray& data);

  void start_db_sync();
  void finished_db_sync();
  void slot_longer_dbsync();
  void slot_longer_dbsync_finished();

  void slot_log(const QString& message);
  void slot_error(const QString& description, const QString& solution = QString());
  void slot_warning(const QString& description);
  void slot_info(const QString& description);

  void check_if_thread_still_running();

signals:
  void changedExtractTrack(int no, int total, const QString& artist, const QString& title);
  void changedEncodeTrack(int no, int total, const QString& filename);

  void progressExtractTrack(int percent);
  void progressExtractOverall(int percent);
  void progressEncodeTrack(int percent);

  void speedExtract(double times);
  void speedEncode(double times);

  void startDBSync();
  void finishedDBSync();
  void longerDBSyncProgress();
  void longerDBSyncProgressFinished();

  void finished(bool successful);

  void timeout();

  void log(const QString& message);
  void error(const QString& description,
	const QString& solution = QString());
  void warning(const QString& description);
  void info(const QString& description);

private:
  QWidget *parent;
  QSettings *settings;
  QCDDADevice *device;
  QProfileModel *profile_model;
  QCDDAMetaDataModel *cdda_metadata_model;
  QEncoderWrapper *encoder_wrapper;
  QCDDAExtractThread *cdda_extract_thread;
  QJobs *jobs;
  QWaveFileWriter *wave_file_writer;

  QAudexSQL *audex_sql;

  bool construct_target_filename(QString& targetFilename,
	int trackno, int cdno, int gindex,
	const QString& artist, const QString& album,
	const QString& tartist, const QString& ttitle,
	const QString& date, const QString& genre, const QString& ext, const QString& basepath,
	bool fat_compatible,
	bool overwrite_existing_files, bool is_first_track);

  bool check();
  QString tmp_dir;
  QString target_dir;
  QStringList target_filename_list;

  bool _finished;
  bool _finished_successful;
  void request_finish(bool successful);
  void execute_finish();

  int process_counter;
  bool timeout_done;
  int timeout_counter;

  /*PROCESS 1: EXTRACTING*/
  QString ex_track_source_filename;
  QString ex_track_target_filename;
  int ex_track_index;
  int ex_track_count;
  QTimer *timer_extract;
  int current_sector;
  int last_measuring_point_sector;

  /*PROCESS 2: ENCODING*/
  QString en_track_filename;
  QString en_track_target_filename;
  int en_track_index;
  int en_track_count;
  QTimer *timer_encode;
  int current_encoder_percent;
  int last_measuring_point_encoder_percent;

};

#endif
