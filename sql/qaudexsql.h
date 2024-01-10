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

#ifndef QAUDEXSQL_HEADER
#define QAUDEXSQL_HEADER

#include <QtCore>
#include <QtSql>

#include "../base/qprofilemodel.h"
#include "../base/wait.h"
#include "../metadata/qcddametadatamodel.h"

class SubmitCDListThread : public QThread {
public:
  QSqlTableModel *cdlist_model;
  bool error;
  void run();
};

class SubmitListThread : public QThread {
public:
  QSqlTableModel *list_model;
  bool error;
  void run();
};

class QAudexSQL : public QObject {
  Q_OBJECT
public:
  QAudexSQL(QObject* parent, QSettings *settings, QProfileModel *profile_model, QCDDAMetaDataModel *cdda_metadata_model);
  ~QAudexSQL();

  int checkConnection();
  bool createTables();

  QSqlQuery getDisc(quint32 discid, bool fuzzy = TRUE);
  QSqlQuery getDisc(const QString& artist, const QString& album);

  int isMultiCD(quint32 discid);

  QSqlQuery getAllDiscsRipped(int days);
  QSqlQuery getAllDiscsByCDNumID(const int cdnum_id);
  QSqlQuery getAllTracksByDiscid(quint32 discid);

  QStringList drivers();

  bool wasError();

public slots:
  void sync();

signals:
  void log(const QString& message);
  void error(const QString& description,
	const QString& solution = QString());
  void warning(const QString& description);
  void info(const QString& description);

  void startDBSync();
  void finishedDBSync();

  void longerProgress();
  void longerProgressFinished();

private:
  QSqlDatabase db;
  bool open;
  QSqlTableModel *cdlist_model;
  QSqlTableModel *list_model;

  SubmitCDListThread submit_cdlist_thread;
  SubmitListThread submit_list_thread;

  QSettings *settings;
  QProfileModel *profile_model;
  QCDDAMetaDataModel *cdda_metadata_model;

  bool open_connection();
  bool open_tables();

  bool _error;

};

#endif
