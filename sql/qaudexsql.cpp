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

#include "qaudexsql.h"

void SubmitCDListThread::run() {
  error = !cdlist_model->submitAll();
  //Wait::sleep(10);
  //error = FALSE;
}

void SubmitListThread::run() {
  error = !list_model->submitAll();
  //Wait::sleep(2);
  //error = FALSE;
}

QAudexSQL::QAudexSQL(QObject* parent, QSettings *settings, QProfileModel *profile_model, QCDDAMetaDataModel *cdda_metadata_model) {

  Q_UNUSED(parent);

  this->settings = settings;
  this->profile_model = profile_model;
  this->cdda_metadata_model = cdda_metadata_model;

  cdlist_model = NULL;
  list_model = NULL;

  open = FALSE;
  _error = FALSE;

}

QAudexSQL::~QAudexSQL() {
  if (cdlist_model) delete cdlist_model;
  if (list_model) delete list_model;
  if (db.isOpen()) db.close();
}

int QAudexSQL::checkConnection() {
  if (!settings) return 1;
  if (!open_connection()) return 1;
  if (!open_tables()) return 2;
  return 0;
}

bool QAudexSQL::createTables() {
  if (!open) if (!open_connection()) {
    emit error(trUtf8("Connection to database host failed. Destination host unreachable with your settings."), trUtf8("Please check your settings. Also check, if the database host is available. You can use the \"Check connection\" button in the settings dialog box."));
    _error = TRUE;
    return FALSE;
  }

  if (db.driverName()=="QMYSQL") {

    if ((db.tables().contains("cdlist")) || (db.tables().contains("list"))) {
      emit error(trUtf8("In this database audex tables already exist."), trUtf8("Please check your database. Tables named 'cdlist' and 'list' must not exist."));
      _error = TRUE;
      return FALSE;
    }

    QSqlQuery query;
    if (!query.exec("CREATE TABLE cdlist (id integer NOT NULL AUTO_INCREMENT UNIQUE,"
		"discid bigint,"
		"ripper varchar(255),"
		"added_on date,"
		"artistortitle varchar(255),"
		"albumorsubtitle varchar(255),"
		"cdnum integer,"
		"profile integer,"
		"year varchar(5),"
		"volume_id bigint,"
		"cdnum_id integer,"
		"cover blob"
		");")) {
      emit error(trUtf8("Error while creating tables."), trUtf8("Table named 'cdlist' could not be created: \"%1\"").arg(query.lastError().text()));
      _error = TRUE;
      return FALSE;
    }

    if (!query.exec("CREATE TABLE list ("
		"id integer NOT NULL AUTO_INCREMENT UNIQUE,"
		"title varchar(255),"
		"artist varchar(255),"
		"album varchar(255),"
		"year varchar(5),"
		"discid bigint,"
		"tracknum integer"
		");")) {
      emit error(trUtf8("Error while creating tables."), trUtf8("Table named 'list' could not be created: \"%1\"").arg(query.lastError().text()));
      _error = TRUE;
      return FALSE;
    }


  } else if (db.driverName()=="QPSQL") {

    if ((db.tables().contains("cdlist")) || (db.tables().contains("list"))) {
      emit error(trUtf8("In this database audex tables already exist."), trUtf8("Please check your database. Tables named 'cdlist' and 'list' must not exist."));
      _error = TRUE;
      return FALSE;
    }
    QSqlQuery query;
    if (!query.exec("CREATE TABLE cdlist (id serial NOT NULL,"
		"discid bigint,"
		"ripper character varying,"
		"added_on date,"
		"artistortitle character varying,"
		"albumorsubtitle character varying,"
		"cdnum integer,"
		"profile integer,"
		"\"year\" character(5),"
		"volume_id bigint,"
		"cdnum_id integer,"
		"cover bytea"
		");")) {
      emit error(trUtf8("Error while creating tables."), trUtf8("Table named 'cdlist' could not be created: \"%1\"").arg(query.lastError().text()));
      _error = TRUE;
      return FALSE;
    }
    if (!query.exec("CREATE TABLE list ("
		"id serial NOT NULL,"
		"title character varying,"
		"artist character varying,"
		"album character varying,"
		"\"year\" character(5),"
		"discid bigint,"
		"tracknum integer"
		");")) {
      emit error(trUtf8("Error while creating tables."), trUtf8("Table named 'list' could not be created: \"%1\"").arg(query.lastError().text()));
      _error = TRUE;
      return FALSE;
    }

  } else if (db.driverName()=="QIBASE") {

  } else return FALSE;

  return TRUE;

}

QSqlQuery QAudexSQL::getDisc(quint32 discid, bool fuzzy) {
  if (!open) if (!open_connection()) {
    emit error(trUtf8("Connection to database host failed. Destination host unreachable with your settings."), trUtf8("Please check your settings. Also check, if the database host is available. You can use the \"Check connection\" button in the settings dialog box."));
    _error = TRUE;
    return QSqlQuery();
  }
  QSqlQuery query;
  if (fuzzy) {
    query.exec(QString("SELECT * FROM cdlist WHERE (discid<%1+1000) AND (discid>%1-1000)").arg(discid));
  } else {
    query.exec(QString("SELECT * FROM cdlist WHERE (discid=%1)").arg(discid));
  }
  return query;
}

QSqlQuery QAudexSQL::getDisc(const QString& artist, const QString& album) {
  if (!open) if (!open_connection()) {
    emit error(trUtf8("Connection to database host failed. Destination host unreachable with your settings."), trUtf8("Please check your settings. Also check, if the database host is available. You can use the \"Check connection\" button in the settings dialog box."));
    _error = TRUE;
    return QSqlQuery();
  }
  QSqlQuery query;
  query.exec(QString("SELECT * FROM cdlist WHERE (UPPER(artistortitle) LIKE '%%%1%%') AND (UPPER(albumorsubtitle) LIKE '%%%2%%')").arg(artist.toUpper()).arg(album.toUpper()));
  return query;
}

int QAudexSQL::isMultiCD(quint32 discid) {
  if (!open) if (!open_connection()) {
    emit error(trUtf8("Connection to database host failed. Destination host unreachable with your settings."), trUtf8("Please check your settings. Also check, if the database host is available. You can use the \"Check connection\" button in the settings dialog box."));
    _error = TRUE;
    return 0;
  }
  QSqlQuery query = getDisc(discid, FALSE);
  query.next();
  int cdnum = query.value(6).toInt();
  int cdnum_id = query.value(10).toInt();
  QSqlQuery query2; query2.exec(QString("SELECT * FROM cdlist WHERE (cdnum_id=%1)").arg(cdnum_id));
  if (query2.size()<2) return 0;
  return cdnum;
}

QSqlQuery QAudexSQL::getAllDiscsRipped(int days) {
  if (!open) if (!open_connection()) {
    emit error(trUtf8("Connection to database host failed. Destination host unreachable with your settings."), trUtf8("Please check your settings. Also check, if the database host is available. You can use the \"Check connection\" button in the settings dialog box."));
    _error = TRUE;
    return QSqlQuery();
  }

  QSqlQuery query;
  if (days >= 0) {
    query.exec(QString("SELECT cdnum_id,MAX(added_on),MAX(artistortitle),MAX(albumorsubtitle),MAX(discid)"
	"FROM cdlist WHERE (added_on >= CURRENT_DATE-%1) GROUP BY cdnum_id,added_on ORDER BY added_on DESC").arg(days));
  } else {
    query.exec(QString("SELECT cdnum_id,MAX(added_on),MAX(artistortitle),MAX(albumorsubtitle),MAX(discid)"
	"FROM cdlist GROUP BY cdnum_id,added_on ORDER BY added_on DESC"));
  }
  return query;
}

QSqlQuery QAudexSQL::getAllDiscsByCDNumID(const int cdnum_id) {
  if (!open) if (!open_connection()) {
    emit error(trUtf8("Connection to database host failed. Destination host unreachable with your settings."), trUtf8("Please check your settings. Also check, if the database host is available. You can use the \"Check connection\" button in the settings dialog box."));
    _error = TRUE;
    return QSqlQuery();
  }

  QSqlQuery query;
  query.exec(QString("SELECT * FROM cdlist WHERE (cdnum_id=%1) ORDER BY cdnum").arg(cdnum_id));
  return query;

}

QSqlQuery QAudexSQL::getAllTracksByDiscid(quint32 discid) {
  if (!open) if (!open_connection()) {
    emit error(trUtf8("Connection to database host failed. Destination host unreachable with your settings."), trUtf8("Please check your settings. Also check, if the database host is available. You can use the \"Check connection\" button in the settings dialog box."));
    _error = TRUE;
    return QSqlQuery();
  }
  QSqlQuery query;
  query.exec(QString("SELECT * FROM list WHERE (discid=%1)").arg(discid));
  return query;
}

QStringList QAudexSQL::drivers() {
  return db.drivers();
}

bool QAudexSQL::wasError() {
  return _error;
}

void QAudexSQL::sync() {

  if (!open_connection()) {
    emit log("(QAudexSQL) database error: "+db.lastError().text());
    emit error(trUtf8("Database error: %1").arg(db.lastError().text()), trUtf8("Please check your database settings."));
    _error = TRUE;
    return;
  }
  if (!open_tables()) {
    emit log("(QAudexSQL) database error: "+db.lastError().text());
    emit error(trUtf8("Database error: %1").arg(db.lastError().text()), trUtf8("Please check your database settings."));
    _error = TRUE;
    return;
  }

  int imagesize = 0;

  { int row = cdlist_model->rowCount();
    cdlist_model->insertRow(row);
    QModelIndex index = cdlist_model->index(row, 1); cdlist_model->setData(index, cdda_metadata_model->discid());
    index = cdlist_model->index(row, 2); cdlist_model->setData(index, settings->value("settings/sql_name", "audex").toString());
    index = cdlist_model->index(row, 3); cdlist_model->setData(index, QDateTime::currentDateTime().date());
    index = cdlist_model->index(row, 4); cdlist_model->setData(index, cdda_metadata_model->artist());
    index = cdlist_model->index(row, 5); cdlist_model->setData(index, cdda_metadata_model->album());
    int cdnum = cdda_metadata_model->cdNum(); if (cdnum==0) cdnum=1;
    index = cdlist_model->index(row, 6); cdlist_model->setData(index, cdnum);
    index = cdlist_model->index(row, 7); cdlist_model->setData(index, profile_model->data(profile_model->index(profile_model->currentProfileRow(), Q_PROFILE_MODEL_COLUMN_PROFILEDBINDEX_INDEX)).toInt());
    index = cdlist_model->index(row, 8); cdlist_model->setData(index, cdda_metadata_model->year());
    index = cdlist_model->index(row, 9); cdlist_model->setData(index, 0);

    if (cdda_metadata_model->customData("cdnum_id").toInt()==-1) {
      QSqlQuery query; int cdnum_id = 0;
      query.exec("SELECT MAX(cdnum_id) FROM cdlist");
      if (query.next()) cdnum_id = query.value(0).toInt()+1;
      index = cdlist_model->index(row, 10); cdlist_model->setData(index, cdnum_id);
    } else {
      index = cdlist_model->index(row, 10); cdlist_model->setData(index, cdda_metadata_model->customData("cdnum_id").toInt());
    }

    QImage image(cdda_metadata_model->customData("cover").value<QImage>());
    if ((image!=QImage()) && (image.height()>1) && (image.width()>1)) {
      QByteArray ba;
      QBuffer buffer(&ba);
      buffer.open(QIODevice::WriteOnly);
      image.save(&buffer, "JPG");
      imagesize = ba.size();
      index = cdlist_model->index(row, 11); cdlist_model->setData(index, ba);
    }

  }

  { for (int i=0; i<cdda_metadata_model->rowCount(); i++) {
      if (!cdda_metadata_model->isAudioTrack(i+1)) continue;
      int row = list_model->rowCount();
      list_model->insertRow(row);
      QModelIndex index = list_model->index(row, 1);
      list_model->setData(index, cdda_metadata_model->data(cdda_metadata_model->index(i, Q_CDDA_METADATA_MODEL_COLUMN_TITLE_INDEX)));
      index = list_model->index(row, 2); list_model->setData(index, cdda_metadata_model->data(cdda_metadata_model->index(i, Q_CDDA_METADATA_MODEL_COLUMN_ARTIST_INDEX)));
      index = list_model->index(row, 3); list_model->setData(index, cdda_metadata_model->album());
      index = list_model->index(row, 4); list_model->setData(index, cdda_metadata_model->year());
      index = list_model->index(row, 5); list_model->setData(index, cdda_metadata_model->discid());
      index = list_model->index(row, 6); list_model->setData(index, i+1);
    }
  }

  submit_cdlist_thread.cdlist_model = cdlist_model;
  submit_list_thread.list_model = list_model;

  if (imagesize > 0) {
    emit log(QString("(QAudexSQL) start listing CD in database (cover size %1 KB)").arg((double)(imagesize / 1024)));
    emit info(trUtf8("start listing CD in database (cover size %1 KB)...").arg((double)(imagesize / 1024)) );
  } else {
    emit log("(QAudexSQL) start listing CD in database");
    emit info(trUtf8("Start listing CD in database..."));
  }

  emit startDBSync();
  submit_cdlist_thread.start();
  //works
  int c = 0;
  while (submit_cdlist_thread.isRunning()) {
    Wait::usleep(100);
    QCoreApplication::processEvents();
    if (c<4000) c++; else if (c++==4000) emit longerProgress();
  }
  if (c>4000) emit longerProgressFinished();
  if (submit_cdlist_thread.error) {
    emit finishedDBSync();
    emit log("(QAudexSQL) database error: "+db.lastError().text());
    emit error(trUtf8("Database error: %1").arg(db.lastError().text()), trUtf8("Please check your database settings or make a bug report."));
    _error = TRUE;
    return;
  } else {
    emit log("(QAudexSQL) finished listing CD in database");
    emit info(trUtf8("Finished listing CD in database."));
  }

  emit log(QString("(QAudexSQL) start listing %1 tracks in database").arg(cdda_metadata_model->numOfAudioTracks()));
  emit info(trUtf8("Start listing %1 tracks in database...").arg(cdda_metadata_model->numOfAudioTracks()));

  submit_list_thread.start();
  //works
  while (submit_list_thread.isRunning()) {
    Wait::usleep(100);
    QCoreApplication::processEvents();
  }
  if (submit_list_thread.error) {
    emit finishedDBSync();
    emit log("(QAudexSQL) database error: "+db.lastError().text());
    emit error(trUtf8("Database error: %1").arg(db.lastError().text()), trUtf8("Please check your database settings or make a bug report."));
    _error = TRUE;
    return;
  } else {
    emit finishedDBSync();
    emit log(QString("(QAudexSQL) finished listing %1 tracks in postgreSQL-database").arg(cdda_metadata_model->numOfAudioTracks()));
    emit info(trUtf8("Finished listing %1 tracks in postgreSQL-database.").arg(cdda_metadata_model->numOfAudioTracks()));
  }

}

bool QAudexSQL::open_connection() {
  if (!settings) return FALSE;
  QSqlDatabase::removeDatabase("qt_sql_default_connection");
  db = QSqlDatabase::addDatabase(settings->value("settings/sql_driver", "QPSQL").toString());
  db.setHostName(settings->value("settings/sql_host", "localhost").toString());
  db.setPort(settings->value("settings/sql_port", 5432).toInt());
  db.setDatabaseName(settings->value("settings/sql_db", "audex").toString());
  db.setUserName(settings->value("settings/sql_user", "audex").toString());
  db.setPassword(QString(QByteArray::fromBase64(settings->value("settings/sql_password", QByteArray("audex").toBase64()).toByteArray())));
  db.setConnectOptions(settings->value("settings/sql_options", "").toString());
  if (db.open()) {
    open = TRUE;
    return TRUE;
  } else {
    return FALSE;
  }
}

bool QAudexSQL::open_tables() {

  cdlist_model = new QSqlTableModel(this, db);
  cdlist_model->setEditStrategy(QSqlTableModel::OnManualSubmit);
  cdlist_model->setTable("cdlist");
  if ((cdlist_model->headerData(0, Qt::Horizontal).toString()!="id") || (cdlist_model->record().field(0).type()!=QVariant::Int)) return FALSE;
  if ((cdlist_model->headerData(1, Qt::Horizontal).toString()!="discid") || (cdlist_model->record().field(1).type()!=QVariant::LongLong)) return FALSE;
  if ((cdlist_model->headerData(2, Qt::Horizontal).toString()!="ripper") || (cdlist_model->record().field(2).type()!=QVariant::String)) return FALSE;
  if ((cdlist_model->headerData(3, Qt::Horizontal).toString()!="added_on") || (cdlist_model->record().field(3).type()!=QVariant::Date)) return FALSE;
  if ((cdlist_model->headerData(4, Qt::Horizontal).toString()!="artistortitle") || (cdlist_model->record().field(4).type()!=QVariant::String)) return FALSE;
  if ((cdlist_model->headerData(5, Qt::Horizontal).toString()!="albumorsubtitle") || (cdlist_model->record().field(5).type()!=QVariant::String)) return FALSE;
  if ((cdlist_model->headerData(6, Qt::Horizontal).toString()!="cdnum") || (cdlist_model->record().field(6).type()!=QVariant::Int)) return FALSE;
  if ((cdlist_model->headerData(7, Qt::Horizontal).toString()!="profile") || (cdlist_model->record().field(7).type()!=QVariant::Int)) return FALSE;
  if ((cdlist_model->headerData(8, Qt::Horizontal).toString()!="year") || (cdlist_model->record().field(8).type()!=QVariant::String)) return FALSE;
  if ((cdlist_model->headerData(9, Qt::Horizontal).toString()!="volume_id") || (cdlist_model->record().field(9).type()!=QVariant::LongLong)) return FALSE;
  if ((cdlist_model->headerData(10, Qt::Horizontal).toString()!="cdnum_id") || (cdlist_model->record().field(10).type()!=QVariant::Int)) return FALSE;
  if ((cdlist_model->headerData(11, Qt::Horizontal).toString()!="cover") || (cdlist_model->record().field(11).type()!=QVariant::QVariant::ByteArray)) return FALSE;

  list_model = new QSqlTableModel(this, db);
  list_model->setEditStrategy(QSqlTableModel::OnManualSubmit);
  list_model->setTable("list");
  if ((list_model->headerData(0, Qt::Horizontal).toString()!="id") || (list_model->record().field(0).type()!=QVariant::Int)) return FALSE;
  if ((list_model->headerData(1, Qt::Horizontal).toString()!="title") || (list_model->record().field(1).type()!=QVariant::String)) return FALSE;
  if ((list_model->headerData(2, Qt::Horizontal).toString()!="artist") || (list_model->record().field(2).type()!=QVariant::String)) return FALSE;
  if ((list_model->headerData(3, Qt::Horizontal).toString()!="album") || (list_model->record().field(3).type()!=QVariant::String)) return FALSE;
  if ((list_model->headerData(4, Qt::Horizontal).toString()!="year") || (list_model->record().field(4).type()!=QVariant::String)) return FALSE;
  if ((list_model->headerData(5, Qt::Horizontal).toString()!="discid") || (list_model->record().field(5).type()!=QVariant::LongLong)) return FALSE;
  if ((list_model->headerData(6, Qt::Horizontal).toString()!="tracknum") || (list_model->record().field(6).type()!=QVariant::Int)) return FALSE;

  return TRUE;

}
