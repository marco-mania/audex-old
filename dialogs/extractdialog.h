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

#ifndef EXTRACTDIALOG_HEADER
#define EXTRACTDIALOG_HEADER

#include <QtCore>
#include <QtGui>
#include "ui_extractdialog.h"

#include "../metadata/qcddametadatamodel.h"
#include "../base/qprofilemodel.h"
#include "../qaudex.h"
#include "protocoldialog.h"

class extractdialog : public QDialog, private Ui::ExtractDialog {

  Q_OBJECT

public:
  extractdialog(QWidget *parent, QCDDADevice *device, QSettings *settings, QProfileModel *profile_model, QCDDAMetaDataModel *cdda_metadata_model);
  ~extractdialog();

public slots:
  int exec();

private slots:
  void cancel();

  void show_changed_extract_track(int no, int total, const QString& artist, const QString& title);
  void show_changed_encode_track(int no, int total, const QString& filename);

  void show_progress_extract_track(int percent);
  void show_progress_extract_overall(int percent);
  void show_progress_encode_track(int percent);

  void show_speed_encode(double speed);
  void show_speed_extract(double speed);

  void start_db_sync();
  void finished_db_sync();
  void db_sync_longer_progress();
  void db_sync_longer_progress_finished();

  void conclusion(bool successful);

  void show_info(const QString& description);
  void show_warning(const QString& description);
  void show_error(const QString& description, const QString& solution);

  void ask_timeout();

  void open_encoder_protocol_dialog();
  void open_extract_protocol_dialog();

protected:
  void closeEvent(QCloseEvent *event);

private:
  QAudex *audex;

  QCDDADevice *device;
  QSettings *settings;
  QProfileModel *profile_model;
  QCDDAMetaDataModel *cdda_metadata_model;
  QEncoderWrapper *encoder_wrapper;

  bool finished;
  bool once;

  QString slabel1;
  QString slabel2;
  QString slabel3;

};

#endif
