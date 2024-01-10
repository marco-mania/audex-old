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

#include "mainwindow.h"

#define GENRE_MAX 148
const char *ID3_GENRES[GENRE_MAX] = {
  "Blues", "Classic Rock", "Country", "Dance", "Disco", "Funk", "Grunge", "Hip-Hop",
  "Jazz", "Metal", "New Age", "Oldies", "Other", "Pop", "R&B", "Rap", "Reggae",
  "Rock", "Techno", "Industrial", "Alternative", "Ska", "Death Metal", "Pranks", "Soundtrack",
  "Euro-Techno", "Ambient", "Trip-Hop", "Vocal", "Jazz+Funk", "Fusion", "Trance", "Classical",
  "Instrumental", "Acid", "House", "Game", "Sound Clip", "Gospel", "Noise", "Alt",
  "Bass", "Soul", "Punk", "Space", "Meditative", "Instrumental Pop",
  "Instrumental Rock", "Ethnic", "Gothic", "Darkwave", "Techno-Industrial", "Electronic",
  "Pop-Folk", "Eurodance", "Dream", "Southern Rock", "Comedy", "Cult",
  "Gangsta Rap", "Top 40", "Christian Rap", "Pop/Funk", "Jungle", "Native American",
  "Cabaret", "New Wave", "Psychedelic", "Rave", "Showtunes", "Trailer", "Lo-Fi", "Tribal",
  "Acid Punk", "Acid Jazz", "Polka", "Retro", "Musical", "Rock & Roll", "Hard Rock", "Folk",
  "Folk/Rock", "National Folk", "Swing", "Fast-Fusion", "Bebob", "Latin", "Revival",
  "Celtic", "Bluegrass", "Avantgarde", "Gothic Rock", "Progressive Rock",
  "Psychedelic Rock", "Symphonic Rock", "Slow Rock", "Big Band", "Chorus", "Easy Listening",
  "Acoustic", "Humour", "Speech", "Chanson", "Opera", "Chamber Music", "Sonata", "Symphony",
  "Booty Bass", "Primus", "Porn Groove", "Satire", "Slow Jam", "Club", "Tango",
  "Samba", "Folklore", "Ballad", "Power Ballad", "Rhythmic Soul", "Freestyle", "Duet",
  "Punk Rock", "Drum Solo", "A Cappella", "Euro-House", "Dance Hall", "Goa",
  "Drum & Bass", "Club-House", "Hardcore", "Terror", "Indie", "BritPop", "Negerpunk",
  "Polsk Punk", "Beat", "Christian Gangsta Rap", "Heavy Metal", "Black Metal", "Crossover",
  "Contemporary Christian", "Christian Rock", "Merengue", "Salsa", "Thrash Metal",
  "Anime", "JPop", "Synthpop"
};

mainwindow::mainwindow(QWidget *parent) : QMainWindow(parent) {

  setupUi(this);

  create_toolbar();
  create_statusbar();

  settings = new QSettings(QString("%1/.%2/%2.conf").arg(QDir::homePath()).arg(MARK), QSettings::IniFormat);

  profile_model = new QProfileModel(this, settings);
  connect(profile_model, SIGNAL(error(const QString&, const QString&)), this, SLOT(show_error(const QString&, const QString&)));
  connect(profile_model, SIGNAL(log(const QString&)), this, SLOT(show_log(const QString&)));
  connect(profileComboBox, SIGNAL(currentIndexChanged(int)), profile_model, SLOT(setCurrentProfileRow(int)));
  connect(profileComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(update_labels(int)));

  void update_title();
  void update_genre();
  void update_cdnum();
  void update_trackoffset();

  //lineEdit_3->setInputMask("9999;_");
  connect(checkBox, SIGNAL(clicked(bool)), this, SLOT(update_various(bool)));
  connect(checkBox, SIGNAL(clicked(bool)), this, SLOT(update_title_correction_tools(bool)));
  connect(lineEdit, SIGNAL(editingFinished()), this, SLOT(update_artist()));
  connect(lineEdit_2, SIGNAL(editingFinished()), this, SLOT(update_title()));
  connect(comboBox_2->lineEdit(), SIGNAL(editingFinished()), this, SLOT(update_genre()));
  connect(lineEdit_3, SIGNAL(editingFinished()), this, SLOT(update_year()));
  connect(textEdit, SIGNAL(textChanged()), this, SLOT(update_extended_data()));
  connect(label_19, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(cover_context_menu(const QPoint&)));

  profileComboBox->setModel(profile_model);
  profileComboBox->setModelColumn(0);
  if (profile_model->rowCount() > 0) {
    int row = settings->value("profiles/std", 0).toInt();
    if ((row < 0) || (row >= profile_model->rowCount())) row = 0;
    profileComboBox->setCurrentIndex(row);
  }

  device = new QCDDADevice(this, settings->value("settings/device", "/dev/cdrom").toString());
  connect(device, SIGNAL(driveStatusChanged(QCDDADrive::DriveStatus)), this, SLOT(update_drive_status(QCDDADrive::DriveStatus)));
  connect(device, SIGNAL(error(const QString&, const QString&)), this, SLOT(show_error(const QString&, const QString&)));
  connect(device, SIGNAL(log(const QString&)), this, SLOT(show_log(const QString&)));

  cddb = new QCDDB(this);
  connect(cddb, SIGNAL(queryDone(int)), this, SLOT(slot_cddb_query_done(int)));
  connect(cddb, SIGNAL(readDone()), this, SLOT(slot_cddb_read_done()));
  connect(cddb, SIGNAL(submitDone()), this, SLOT(slot_cddb_submit_done()));
  connect(cddb, SIGNAL(error(const QString&, const QString&)), this, SLOT(show_error(const QString&, const QString&)));
  connect(cddb, SIGNAL(warning(const QString&)), this, SLOT(show_warn(const QString&)));
  connect(cddb, SIGNAL(log(const QString&)), this, SLOT(show_log(const QString&)));
  connect(cddb, SIGNAL(busy(bool, int)), this, SLOT(slot_cddb_busy(bool, int)));
  connect(cddb, SIGNAL(proxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*)), this, SLOT(slot_cddb_proxy_auth_req(const QNetworkProxy&, QAuthenticator*)));

  cdda_metadata_model = new QCDDAMetaDataModel(this, device);
  connect(cdda_metadata_model, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(update_cddb_actions()));
  connect(cdda_metadata_model, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(editNextTableItem(const QModelIndex&)));
  tableView->setModel(cdda_metadata_model);
  tableView->setAlternatingRowColors(TRUE);
  //tableView->verticalHeader()->hide();
  tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
  tableView->setEditTriggers(QAbstractItemView::EditKeyPressed | QAbstractItemView::DoubleClicked);
  tableView->resizeColumnToContents(2); tableView->resizeColumnToContents(3);

  QStringList genres;
  for (int i=0; i < GENRE_MAX; i++) genres.append(QString().fromAscii(ID3_GENRES[i]));
  comboBox_2->addItems(genres);

  connect(action_Eject, SIGNAL(triggered()), this, SLOT(eject()));
  connect(actionE_xtract, SIGNAL(triggered()), this, SLOT(open_extract_dialog()));
  //connect(actionExtract_w_o_db_sync, SIGNAL(triggered()), this, SLOT(open_extract_dialog_no_db_sync()));
  //connect(actionDB_sync_only, SIGNAL(triggered()), this, SLOT(open_extract_dialog_db_sync_only()));
  connect(action_Fetch, SIGNAL(triggered()), this, SLOT(fetch_cddb_data()));
  connect(action_Submit, SIGNAL(triggered()), this, SLOT(submit_cddb_data()));
  connect(actionSwap, SIGNAL(triggered()), this, SLOT(swap_metadata_artist_and_titles()));
  connect(actionCapitalize, SIGNAL(triggered()), this, SLOT(capitalize_metadata()));
  connect(actionSplit_titles, SIGNAL(triggered()), this, SLOT(split_metadata_titles()));
  connect(actionAutofill_artists, SIGNAL(triggered()), this, SLOT(autofill_artists()));
  connect(action_Settings, SIGNAL(triggered()), this, SLOT(open_settings_dialog()));
  connect(actionEx_it, SIGNAL(triggered()), this, SLOT(close()));
  connect(actionAbout_Qt, SIGNAL(triggered()), this, SLOT(open_about_qt_dialog()));
  connect(action_Info, SIGNAL(triggered()), this, SLOT(open_about_dialog()));

  actionFetchCover = new QAction(trUtf8("Fetch cover from amazon..."), this);
  connect(actionFetchCover, SIGNAL(triggered()), this, SLOT(open_amazon_cover_dialog()));
  actionLoadCover = new QAction(trUtf8("Load own cover..."), this);
  connect(actionLoadCover, SIGNAL(triggered()), this, SLOT(load_cover()));
  actionSaveCover = new QAction(trUtf8("Save cover to file..."), this);
  connect(actionSaveCover, SIGNAL(triggered()), this, SLOT(save_cover()));
  actionShowFullSizeCover = new QAction(trUtf8("Show full size cover..."), this);
  connect(actionShowFullSizeCover, SIGNAL(triggered()), this, SLOT(open_cover_dialog()));
  actionRemoveCover = new QAction(trUtf8("Remove cover"), this);
  connect(actionRemoveCover, SIGNAL(triggered()), this, SLOT(remove_cover()));

  setWindowTitle(trUtf8("Audex BETA VERSION %1").arg(VERSION));

  action_Eject->setToolTip(trUtf8("Eject CD-ROM drive"));
  action_Eject->setStatusTip(trUtf8("Eject CD-ROM drive"));
  actionE_xtract->setToolTip(trUtf8("Start CDDA extraction"));
  actionE_xtract->setStatusTip(trUtf8("Start CDDA extraction"));
  action_Fetch->setToolTip(trUtf8("Fetch CDDB/FreeDB metadata"));
  action_Fetch->setStatusTip(trUtf8("Fetch CDDB/FreeDB metadata"));
  action_Submit->setToolTip(trUtf8("Submit metadata to CDDB/FreeDB"));
  action_Submit->setStatusTip(trUtf8("Submit metadata to CDDB/FreeDB"));
  actionCapitalize->setToolTip(trUtf8("Capitalize all artist names and titles"));
  actionCapitalize->setStatusTip(trUtf8("Capitalize all artist names and titles"));
  actionSwap->setToolTip(trUtf8("Swap all artist names/titles and titles/subtitles"));
  actionSwap->setStatusTip(trUtf8("Swap all artist names/titles and titles/subtitles"));
  actionSplit_titles->setToolTip(trUtf8("Split all titles by a divider"));
  actionSplit_titles->setStatusTip(trUtf8("Split all titles by a divider"));
  actionAutofill_artists->setToolTip(trUtf8("Autofill artist of every single track with disc artist"));
  actionAutofill_artists->setStatusTip(trUtf8("Autofill artist of every single track with disc artist"));
  action_Settings->setToolTip(trUtf8("Change your settings and manage your profiles"));
  action_Settings->setStatusTip(trUtf8("Change your settings and manage your profiles"));
  actionEx_it->setToolTip(trUtf8("Exit this program"));
  actionEx_it->setStatusTip(trUtf8("Exit this program"));
  actionAbout_Qt->setToolTip(trUtf8("About Qt"));
  actionAbout_Qt->setStatusTip(trUtf8("About Qt"));
  action_Info->setToolTip(trUtf8("About this program"));
  action_Info->setStatusTip(trUtf8("About this program"));

  checkBox->setStatusTip(trUtf8("Check if your cd has various artists"));
  checkBox_2->setStatusTip(trUtf8("Check if your cd is part of a multi-cd album or sampler"));

  update_cdda_model_columns();
  update_drive_status(device->driveStatus());
  update_mode(settings->value("settings/sql_enabled", FALSE).toBool());
  update_cddb_actions();
  update_various(FALSE);
  update_title_correction_tools(FALSE);

  addDockWidget(Qt::TopDockWidgetArea, dockWidget);
  addDockWidget(Qt::TopDockWidgetArea, dockWidget_2);

  resize(settings->value("mainwindow/size", QSize(500, 600)).toSize());
  move(settings->value("mainwindow/pos", QPoint(100, 100)).toPoint());

  restoreState(settings->value("mainwindow/stat", QByteArray()).toByteArray());

}

mainwindow::~mainwindow() {

  delete cddb;
  delete settings;
  delete profile_model;
  delete cdda_metadata_model;
  delete device;
  delete statusLabel;
  delete modeLabel;
  delete cddbLabel;

}

void mainwindow::fetch_cddb_data() {

  if (!device->isDiscInDrive()) return;
  if ((!settings->value("settings/cddb_remote_enabled", TRUE).toBool()) && (!settings->value("settings/cddb_cache_enabled", TRUE).toBool())) return;

  quint16 sport = 80;
  if (settings->value("settings/cddb_protocol", 1).toInt()==1) {
    cddb->setTransport(QCDDB::HTTP);
  }
  else if (settings->value("settings/cddb_protocol", 1).toInt()==0) {
    sport = 8880;
    cddb->setTransport(QCDDB::CDDBP);
  }

  cddb->enableRemote(settings->value("settings/cddb_remote_enabled", TRUE).toBool());
  cddb->setHost(settings->value("settings/cddb_host", "freedb.org").toString());
  cddb->setPort(settings->value("settings/cddb_port", sport).toInt());
  cddb->setHTTPPathQuery(settings->value("settings/cddb_http_query_path", "/~cddb/cddb.cgi").toString());
  cddb->setEmailAddress(settings->value("settings/cddb_http_email_address", "info@anyaudio.de").toString());
  if (settings->value("settings/cddb_proxy_enabled", FALSE).toBool()) {
    int type = settings->value("settings/cddb_proxy_type", 0).toInt();
    switch (type) {
      case 0 : cddb->setProxyType(QNetworkProxy::Socks5Proxy); break;
      case 1 : cddb->setProxyType(QNetworkProxy::HttpProxy); break;
    }
    cddb->setProxyHost(settings->value("settings/cddb_proxy_host", "www.proxy.com").toString());
    cddb->setProxyPort(settings->value("settings/cddb_proxy_port", 8080).toInt());
    if (settings->value("settings/cddb_proxy_auth", FALSE).toBool()) {
      cddb->setProxyUsername(settings->value("settings/cddb_proxy_user", "audex").toString());
      cddb->setProxyPassword(settings->value("settings/cddb_proxy_pass", "audex").toString());
    }
  } else {
    cddb->setProxyType(QNetworkProxy::NoProxy);
  }

  cddb->enableLocalCache(settings->value("settings/cddb_cache_enabled", TRUE).toBool());
  cddb->setLocalCachePath(settings->value("settings/cddb_cache_path", "~/.cddb").toString());

  TrackOffsetList list;
  for (int i = 1; i <= device->numOfTracks(); i++) {
    list.append(device->frameOffsetOfTrack(i)+150);
  }
  list.append(device->numOfFrames()+150);
  cddb->query(list);

}

void mainwindow::submit_cddb_data() {

  if (!device->isDiscInDrive()) return;
  if (!settings->value("settings/cddb_remote_enabled", TRUE).toBool()) return;

  QStringList err = check_metadata();
  if (err.count() > 0) {
    reportdialog *reportDialog = new reportdialog(this, settings, QStringList(), err);
    if (reportDialog->exec()!=QDialog::Accepted) { delete reportDialog; return; }
    delete reportDialog;
  }

  quint16 sport = 80;
  if (settings->value("settings/cddb_protocol", 1).toInt()==1) {
    cddb->setTransport(QCDDB::HTTP);
  }
  else if (settings->value("settings/cddb_protocol", 1).toInt()==0) {
    sport = 8880;
    cddb->setTransport(QCDDB::CDDBP);
  }

  cddb->setHost(settings->value("settings/cddb_host", "freedb.org").toString());
  cddb->setPort(settings->value("settings/cddb_port", sport).toInt());
  cddb->setHTTPPathSubmit(settings->value("settings/cddb_http_submit_path", "/~cddb/submit.cgi").toString());
  cddb->setEmailAddress(settings->value("settings/cddb_http_email_address", "info@anyaudio.de").toString());
  if (settings->value("settings/cddb_proxy_enabled", FALSE).toBool()) {
    int type = settings->value("settings/cddb_proxy_type", 0).toInt();
    switch (type) {
      case 0 : cddb->setProxyType(QNetworkProxy::Socks5Proxy); break;
      case 1 : cddb->setProxyType(QNetworkProxy::HttpProxy); break;
    }
    cddb->setProxyHost(settings->value("settings/cddb_proxy_host", "www.proxy.com").toString());
    cddb->setProxyPort(settings->value("settings/cddb_proxy_port", 8080).toInt());
    if (settings->value("settings/cddb_proxy_auth", FALSE).toBool()) {
      cddb->setProxyUsername(settings->value("settings/cddb_proxy_user", "audex").toString());
      cddb->setProxyPassword(settings->value("settings/cddb_proxy_pass", "audex").toString());
    }
  } else {
    cddb->setProxyType(QNetworkProxy::NoProxy);
  }

  if (!cdda_metadata_model->metadata().headerData().isCategoryValid()) {
    QString category;
    categorydialog *categoryDialog = new categorydialog(this, settings, &category);
    if (categoryDialog->exec()!=QDialog::Accepted) { delete categoryDialog; return; }
    cdda_metadata_model->setCategory(category);
    delete categoryDialog;
  }

  cddb->submit(cdda_metadata_model->metadata());

  cdda_metadata_model->confirm();

}

void mainwindow::eject() {
  device->eject();
}

void mainwindow::open_extract_dialog() {

  //ein paar tests ob die eingaben sinnvoll sind...
  QStringList warn, err;

  if (profile_model->currentProfileRow()==-1) {
    err.append(trUtf8("No profile selected. Please select one."));
  }

  QAudexSQL *audex_sql = new QAudexSQL(this, settings, profile_model, cdda_metadata_model);
  connect(audex_sql, SIGNAL(error(const QString&, const QString&)), this, SLOT(show_error(const QString&, const QString&)));
  connect(audex_sql, SIGNAL(warning(const QString&)), this, SLOT(show_warn(const QString&)));
  connect(audex_sql, SIGNAL(log(const QString&)), this, SLOT(show_log(const QString&)));
  if ((settings->value("settings/sql_enabled", FALSE).toBool()) && (err.count()==0)) {
    QSqlQuery query = audex_sql->getDisc(cdda_metadata_model->discid(), FALSE);
    if (audex_sql->wasError()) { delete audex_sql; return; }
    if (query.size()>0) { //WARNING! works only with db returning query size
      query.next();
      int cdnum = audex_sql->isMultiCD(cdda_metadata_model->discid());
      if (cdnum > 0) {
        err.append(trUtf8("CD (\"%1\", \"%2\", cd #%3) already exists in database. Submitted by %4 on %5.")
		.arg(query.value(5).toString())
		.arg(query.value(4).toString())
		.arg(cdnum)
		.arg(query.value(2).toString())
		.arg(query.value(3).toDate().toString("yyyy-MM-dd")));
      } else {
        err.append(trUtf8("CD (\"%1\", \"%2\") already exists in database. Submitted by %3 on %4.")
		.arg(query.value(5).toString())
		.arg(query.value(4).toString())
		.arg(query.value(2).toString())
		.arg(query.value(3).toDate().toString("yyyy-MM-dd")));
      }
    } else {
      if (!checkBox_2->isChecked()) {
        query = audex_sql->getDisc(cdda_metadata_model->artist(), cdda_metadata_model->album());
        if (audex_sql->wasError()) { delete audex_sql; return; }
        if (query.size()>0) { //WARNING! works only with db returning query size
          sqlmatchingdialog *sqlMatchingDialog = new sqlmatchingdialog(this, settings, &query);
          if (sqlMatchingDialog->exec()!=QDialog::Accepted) {
            delete audex_sql; return;
          }
          delete sqlMatchingDialog;
        }
      }
    }
  }
  delete audex_sql;

  if (err.count()==0) {
    if (settings->value("settings/sql_enabled", FALSE).toBool()) {
      err << check_metadata();
      if ((settings->value("settings/sql_enabled", FALSE).toBool()) && (cdda_metadata_model->customData("cover").value<QImage>()==QImage())) {
        warn << trUtf8("No cover set. Please check.");
      }
    } else {
      warn << check_metadata();
    }
  }
  if ((warn.count() > 0) || (err.count() > 0)) {
    reportdialog *reportDialog = new reportdialog(this, settings, warn, err);
    if (reportDialog->exec()!=QDialog::Accepted) { delete reportDialog; return; }
    delete reportDialog;
  }

  //multi-cd
  cdda_metadata_model->setCustomData("cdnum_id", -1);
  cdda_metadata_model->setTrackOffset(0);
  cdda_metadata_model->setCDNum(0);
  if (checkBox_2->isChecked()) {
    audex_sql = NULL;
    if (settings->value("settings/sql_enabled", FALSE).toBool())
      audex_sql = new QAudexSQL(this, settings, profile_model, cdda_metadata_model);
    multicddialog *multicdDialog = new multicddialog(this, settings, audex_sql, cdda_metadata_model);
    if (multicdDialog->exec()!=QDialog::Accepted) { delete multicdDialog; if (audex_sql) delete audex_sql; return; }
    delete multicdDialog;
    if (audex_sql) delete audex_sql;
  }

  if (!settings->value("settings/sql_enabled", FALSE).toBool()) {
    cdda_metadata_model->setSelection(tableView->selectionModel()->selectedRows());
  }

  extractdialog *extractDialog = new extractdialog(this, device, settings, profile_model, cdda_metadata_model);
  extractDialog->exec();
  delete extractDialog;

}

void mainwindow::open_settings_dialog() {

  settingsdialog *settingsDialog = new settingsdialog(this, settings, device, cdda_metadata_model, profile_model, tableView);
  connect(settingsDialog, SIGNAL(profileChanged(int)), this, SLOT(update_labels(int)));
  connect(settingsDialog, SIGNAL(sqlEnabledChanged(bool)), this, SLOT(update_mode(bool)));
  connect(settingsDialog, SIGNAL(activateRemoteChanged(bool)), this, SLOT(update_cddb_actions()));
  connect(settingsDialog, SIGNAL(activateLocalCacheChanged(bool)), this, SLOT(update_cddb_actions()));
  settingsDialog->exec();
  delete settingsDialog;

}

void mainwindow::open_about_qt_dialog() {
  QMessageBox::aboutQt(this);
}

void mainwindow::open_about_dialog() {
  aboutdialog *aboutDialog = new aboutdialog(this);
  aboutDialog->exec();
  delete aboutDialog;
}

void mainwindow::open_amazon_cover_dialog() {
  QImage *image = new QImage;
  coverfromamazondialog *coverFromAmazonDialog = new coverfromamazondialog(this, settings, QString("%1 - %2").arg(cdda_metadata_model->artist()).arg(cdda_metadata_model->album()), image);
  if (coverFromAmazonDialog->exec()!=QDialog::Accepted) { delete coverFromAmazonDialog; delete image; return; }
  cdda_metadata_model->setCustomData("cover", *image);
  update_cover(*image);
  delete coverFromAmazonDialog;
  delete image;
}

void mainwindow::load_cover() {
  QString path = settings->value("mainwindow/last_cover_path", QDir::homePath()).toString();
  QDir dir(path); if (!dir.exists()) path = QDir::homePath();
  QString fileName = QFileDialog::getOpenFileName(this, trUtf8("Open cover image file"),
                                                path,
                                                tr("Image files (*.png *.jpg)"));
  if (!fileName.isEmpty()) {
    QFileInfo info(fileName);
    if (info.size()>262144) { //256 KiB
      show_error(trUtf8("Loading %1 failed.").arg(info.baseName()), trUtf8("Your image file is too large. Please select a cover image file with 256 KB at most."));
      return;
    }
    QImage image(fileName);
    if (image==QImage()) {
      show_error(trUtf8("Loading %1 failed.").arg(info.baseName()), trUtf8("Please check if this is really an image file."));
      return;
    }
    cdda_metadata_model->setCustomData("cover", image);
    update_cover(image);
    settings->setValue("mainwindow/last_cover_path", info.absolutePath());
  }
}

void mainwindow::save_cover() {
  QString path = settings->value("mainwindow/last_cover_save_path", QDir::homePath()).toString();
  QDir dir(path); if (!dir.exists()) path = QDir::homePath();
  QString fileName = QFileDialog::getSaveFileName(this, trUtf8("Save cover image file"),
                                                path,
                                                tr("Image files (*.jpg *.png)"));
  if (!fileName.isEmpty()) {
    QImage image(cdda_metadata_model->customData("cover").value<QImage>());
    image.save(fileName);
    QFileInfo info(fileName);
    settings->setValue("mainwindow/last_cover_save_path", info.absolutePath());
  }
}

void mainwindow::open_cover_dialog() {
  fullcoverdialog *fullCoverDialog = new fullcoverdialog(this, settings, cdda_metadata_model->customData("cover").value<QImage>());
  fullCoverDialog->exec();
  delete fullCoverDialog;
}

void mainwindow::remove_cover() {
  cdda_metadata_model->setCustomData("cover", QImage());
  update_cover(QImage());
}

void mainwindow::cover_context_menu(const QPoint& point) {

  QMenu contextMnu(this);
  QMouseEvent *mevent = new QMouseEvent(QEvent::MouseButtonPress, point, Qt::RightButton, Qt::RightButton, Qt::NoModifier);

  contextMnu.clear();
  contextMnu.addAction(actionFetchCover);
  contextMnu.addAction(actionLoadCover);
  contextMnu.addAction(actionSaveCover);
  contextMnu.addSeparator();
  contextMnu.addAction(actionShowFullSizeCover);
  contextMnu.addSeparator();
  contextMnu.addAction(actionRemoveCover);
  contextMnu.exec(mevent->globalPos());

}

void mainwindow::enable_main_overview(bool enabled) {

  frame->setEnabled(enabled);
  profileLabel->setEnabled(enabled);
  profileComboBox->setEnabled(enabled);
  tableView->setEnabled(enabled);

  lineEdit->blockSignals(TRUE);
  lineEdit_2->blockSignals(TRUE);
  comboBox_2->blockSignals(TRUE);
  lineEdit_3->blockSignals(TRUE);
  if (!enabled) {
    lineEdit->clear();
    comboBox_2->setEditText("");
    lineEdit_2->clear();
    //lineEdit_3->clear();
    lineEdit_3->setText("0");
    textEdit->clear();
    label_10->clear();
    update_cover(QImage());
    cdda_metadata_model->clear();
  } else {
    lineEdit->setText(cdda_metadata_model->artist());
    lineEdit_2->setText(cdda_metadata_model->album());
    comboBox_2->setEditText(cdda_metadata_model->genre());
    lineEdit_3->setText(QString("%1").arg(cdda_metadata_model->year()));
    textEdit->setText(cdda_metadata_model->extendedData().join("\n"));
    label_10->setText(QString("0x%1").arg(device->discid(), 0, 16));
  }
  lineEdit->blockSignals(FALSE);
  lineEdit_2->blockSignals(FALSE);
  comboBox_2->blockSignals(FALSE);
  lineEdit_3->blockSignals(FALSE);

}

void mainwindow::enable_fetch_cddb_data(bool enabled) {
  if ((!settings->value("settings/cddb_remote_enabled", TRUE).toBool()) && (!settings->value("settings/cddb_cache_enabled", TRUE).toBool())) {
    action_Fetch->setEnabled(FALSE);
  } else {
    action_Fetch->setEnabled(enabled);
  }
}

void mainwindow::enable_submit_cddb_data(bool enabled) {
  if ((!settings->value("settings/cddb_remote_enabled", TRUE).toBool()) || (!cdda_metadata_model->metadata().isValid())) {
    action_Submit->setEnabled(FALSE);
  } else {
    action_Submit->setEnabled(enabled);
  }
}

void mainwindow::enable_extract(bool enabled) {
  actionE_xtract->setEnabled(enabled);
  actionCapitalize->setEnabled(enabled);
  if (checkBox->isChecked()) {
    actionSplit_titles->setEnabled(enabled);
    actionSwap->setEnabled(enabled);
    actionAutofill_artists->setEnabled(enabled);
  }
}

void mainwindow::enable_fetch_cover_from_amazon(bool enabled) {
  if ((!cdda_metadata_model->metadata().headerData().isArtistValid()) || (!cdda_metadata_model->metadata().headerData().isTitleValid())) {
    actionFetchCover->setEnabled(FALSE);
  } else {
    actionFetchCover->setEnabled(enabled);
  }
}

void mainwindow::enable_load_cover(bool enabled) {
  actionLoadCover->setEnabled(enabled);
}

void mainwindow::enable_save_cover(bool enabled) {
  if ((cdda_metadata_model->customData("cover").value<QImage>()==QImage()) || (cdda_metadata_model->customData("cover").isNull())) {
    actionSaveCover->setEnabled(FALSE);
  } else {
    actionSaveCover->setEnabled(enabled);
  }
}

void mainwindow::enable_show_cover_dialog(bool enabled) {
  if ((cdda_metadata_model->customData("cover").value<QImage>()==QImage()) || (cdda_metadata_model->customData("cover").isNull())) {
    actionShowFullSizeCover->setEnabled(FALSE);
  } else {
    actionShowFullSizeCover->setEnabled(enabled);
  }
}

void mainwindow::enable_delete_cover(bool enabled) {
  if ((cdda_metadata_model->customData("cover").value<QImage>()==QImage()) || (cdda_metadata_model->customData("cover").isNull())) {
    actionRemoveCover->setEnabled(FALSE);
  } else {
    actionRemoveCover->setEnabled(enabled);
  }
}

void mainwindow::show_error(const QString& description, const QString& solution) {
  errordialog *errorDialog = new errordialog(this, settings, description, solution);
  errorDialog->exec();
  delete errorDialog;
}

void mainwindow::show_warn(const QString& description) {
  qDebug() << QString("WARNING: %1").arg(description);
}

void mainwindow::show_log(const QString& message) {
  qDebug() << message;
}

void mainwindow::update_drive_status(QCDDADrive::DriveStatus drivestatus) {
  switch (drivestatus) {
    case QCDDADrive::dsDriveEmpty :
      statusLabel->setText(trUtf8("No CD in drive"));
      enable_fetch_cddb_data(FALSE);
      enable_submit_cddb_data(FALSE);
      enable_main_overview(FALSE);
      enable_extract(FALSE);
      enable_fetch_cover_from_amazon(FALSE);
      enable_load_cover(FALSE);
      enable_save_cover(FALSE);
      enable_show_cover_dialog(FALSE);
      enable_delete_cover(FALSE);
      break;
    case QCDDADrive::dsDriveReady :
      if ((device->discType() == QCDDADrive::dtAudio) || (device->discType() == QCDDADrive::dtMixed)) {
        statusLabel->setText(trUtf8("Audio CD in drive"));
        enable_fetch_cddb_data(TRUE);
        enable_submit_cddb_data(TRUE);
        enable_main_overview(TRUE);
        enable_extract(TRUE);
        enable_fetch_cover_from_amazon(TRUE);
        enable_load_cover(TRUE);
        enable_save_cover(TRUE);
        enable_show_cover_dialog(TRUE);
        enable_delete_cover(TRUE);
        if (settings->value("settings/cddb_query_after_load", TRUE).toBool())
          QTimer::singleShot(1000, this, SLOT(fetch_cddb_data()));
      } else {
        statusLabel->setText(trUtf8("CD in drive (no audio)"));
        enable_fetch_cddb_data(FALSE);
        enable_submit_cddb_data(FALSE);
        enable_main_overview(FALSE);
        enable_extract(FALSE);
        enable_fetch_cover_from_amazon(FALSE);
        enable_load_cover(FALSE);
        enable_save_cover(FALSE);
        enable_show_cover_dialog(FALSE);
        enable_delete_cover(FALSE);
      }
      break;
    case QCDDADrive::dsDriveOpen :
      statusLabel->setText(trUtf8("CD tray open"));
      enable_fetch_cddb_data(FALSE);
      enable_submit_cddb_data(FALSE);
      enable_main_overview(FALSE);
      enable_extract(FALSE);
      enable_fetch_cover_from_amazon(FALSE);
      enable_load_cover(FALSE);
      enable_save_cover(FALSE);
      enable_show_cover_dialog(FALSE);
      enable_delete_cover(FALSE);
      break;
    case QCDDADrive::dsDriveNotReady :
      statusLabel->setText(trUtf8("Drive not ready"));
      enable_fetch_cddb_data(FALSE);
      enable_submit_cddb_data(FALSE);
      enable_main_overview(FALSE);
      enable_extract(FALSE);
      enable_fetch_cover_from_amazon(FALSE);
      enable_load_cover(FALSE);
      enable_save_cover(FALSE);
      enable_show_cover_dialog(FALSE);
      enable_delete_cover(FALSE);
      break;
    default :
      statusLabel->setText(trUtf8("Drive error"));
      enable_fetch_cddb_data(FALSE);
      enable_main_overview(FALSE);
      enable_extract(FALSE);
      enable_fetch_cover_from_amazon(FALSE);
      enable_load_cover(FALSE);
      enable_save_cover(FALSE);
      enable_show_cover_dialog(FALSE);
      enable_delete_cover(FALSE);
  }
}

void mainwindow::update_header_metadata() {
  lineEdit->blockSignals(TRUE);
  lineEdit_2->blockSignals(TRUE);
  comboBox_2->blockSignals(TRUE);
  lineEdit_3->blockSignals(TRUE);
  textEdit->blockSignals(TRUE);
  label_10->blockSignals(TRUE);
  lineEdit->setText(cdda_metadata_model->artist());
  lineEdit_2->setText(cdda_metadata_model->album());
  comboBox_2->setEditText(cdda_metadata_model->genre());
  lineEdit_3->setText(QString("%1").arg(cdda_metadata_model->year()));
  textEdit->setText(cdda_metadata_model->extendedData().join("\n"));
  label_10->setText(QString("0x%1").arg(device->discid(), 0, 16));
  lineEdit->blockSignals(FALSE);
  lineEdit_2->blockSignals(FALSE);
  comboBox_2->blockSignals(FALSE);
  lineEdit_3->blockSignals(FALSE);
  textEdit->blockSignals(FALSE);
  label_10->blockSignals(FALSE);
}

void mainwindow::update_labels(int profile_row) {
  if (profile_row == profileComboBox->currentIndex()) {
    label_2->setText(profile_model->data(profile_model->index(profile_row, Q_PROFILE_MODEL_COLUMN_ARTISTLABEL_INDEX)).toString());
    label_3->setText(profile_model->data(profile_model->index(profile_row, Q_PROFILE_MODEL_COLUMN_TITLELABEL_INDEX)).toString());
  }
}

void mainwindow::update_various(bool various) {
  cdda_metadata_model->setVarious(various);
  QString head = Q_CDDA_METADATA_MODEL_COLUMN_ARTIST_LABEL;
  head.replace(' ', '_'); head = head.toLower();
  if (!settings->value(QString("cdda_model/column_visible_%1").arg(head), TRUE).toBool()) return;
  if (!various) {
    tableView->hideColumn(Q_CDDA_METADATA_MODEL_COLUMN_ARTIST_INDEX);
  } else {
    tableView->showColumn(Q_CDDA_METADATA_MODEL_COLUMN_ARTIST_INDEX);
  }
  enable_submit_cddb_data(TRUE);
}

void mainwindow::update_title_correction_tools(bool enable) {
  actionSwap->setEnabled(enable);
  actionSplit_titles->setEnabled(enable);
  actionAutofill_artists->setEnabled(enable);
}

void mainwindow::update_artist() {
  cdda_metadata_model->setArtist(lineEdit->text());
  enable_fetch_cover_from_amazon(TRUE);
  enable_submit_cddb_data(TRUE);
}

void mainwindow::update_title() {
  cdda_metadata_model->setAlbum(lineEdit_2->text());
  enable_fetch_cover_from_amazon(TRUE);
  enable_submit_cddb_data(TRUE);
}

void mainwindow::update_genre() {
  cdda_metadata_model->setGenre(comboBox_2->currentText());
  enable_submit_cddb_data(TRUE);
}

void mainwindow::update_year() {
  cdda_metadata_model->setYear(lineEdit_3->text());
  enable_submit_cddb_data(TRUE);
}

void mainwindow::update_extended_data() {
  cdda_metadata_model->setExtendedData(textEdit->toPlainText().split("\n"));
}

void mainwindow::update_mode(bool enabled) {
  if (enabled) {
    modeLabel->setText("Extended database mode");
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
  } else {
    modeLabel->setText("Normal mode");
    tableView->setSelectionMode(QAbstractItemView::MultiSelection);
  }
}

void mainwindow::update_cdda_model_columns() {
  for (int i = 0; i < cdda_metadata_model->columnCount(); i++) {
    QString head = cdda_metadata_model->headerData(i, Qt::Horizontal).toString();
    head.replace(' ', '_'); head = head.toLower();
    if (!settings->value(QString("cdda_model/column_visible_%1").arg(head), TRUE).toBool())
      tableView->hideColumn(i);
    else
      tableView->showColumn(i);
  }
}

void mainwindow::update_cddb_actions() {
  if (device->driveStatus() != QCDDADrive::dsDriveReady) return;
  enable_fetch_cddb_data(TRUE);
  enable_submit_cddb_data(TRUE);
}

void mainwindow::update_fetch_cover_from_amazon_action() {
  enable_fetch_cover_from_amazon(TRUE);
}

void mainwindow::update_cover(const QImage& image) {
  if (image == QImage()) {
    label_19->setPixmap(QPixmap(":/misc/images/nocover.png"));
    label_19->setToolTip("");
    label_19->setMinimumSize(141, 128);
    label_19->setMaximumSize(141, 128);
    enable_delete_cover(FALSE);
    enable_save_cover(FALSE);
    enable_show_cover_dialog(FALSE);
  } else {
    label_19->setPixmap(QPixmap::fromImage(image));
    label_19->setToolTip(trUtf8("Size: %1x%2").arg(image.width()).arg(image.height()));
    label_19->setMinimumSize(128, 128);
    label_19->setMaximumSize(128, 128);
    enable_delete_cover(TRUE);
    enable_save_cover(TRUE);
    enable_show_cover_dialog(TRUE);
  }
}

void mainwindow::editNextTableItem(const QModelIndex& index) {
  if (index.row()<cdda_metadata_model->numOfAudioTracks()-1) {
    QModelIndex next_index = cdda_metadata_model->index(index.row()+1, index.column());
    tableView->setCurrentIndex(next_index);
    tableView->edit(next_index);
  }
}

void mainwindow::swap_metadata_artist_and_titles() {
  QMessageBox msgBox(this);
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setFont(this->font());
  msgBox.setText(trUtf8("Do you really want to swap %1 and %2?")
	.arg(profile_model->data(profile_model->index(profile_model->currentProfileRow(), Q_PROFILE_MODEL_COLUMN_ARTISTLABEL_INDEX)).toString())
	.arg(profile_model->data(profile_model->index(profile_model->currentProfileRow(), Q_PROFILE_MODEL_COLUMN_TITLELABEL_INDEX)).toString()));
  msgBox.addButton(trUtf8("Yes"), QMessageBox::AcceptRole);
  QPushButton *noButton = msgBox.addButton(trUtf8("No"), QMessageBox::RejectRole);
  msgBox.exec();
  if (msgBox.clickedButton() == noButton) return;

  cdda_metadata_model->swapArtistAndTitle();
  update_header_metadata();
  cdda_metadata_model->swapArtistAndTitleOfTracks();
}

void mainwindow::capitalize_metadata() {
  QMessageBox msgBox(this);
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setFont(this->font());
  msgBox.setText(trUtf8("Do you really want to capitalize all metadata?"));
  msgBox.addButton(trUtf8("Yes"), QMessageBox::AcceptRole);
  QPushButton *noButton = msgBox.addButton(trUtf8("No"), QMessageBox::RejectRole);
  msgBox.exec();
  if (msgBox.clickedButton() == noButton) return;

  cdda_metadata_model->capitalizeHeader();
  update_header_metadata();
  cdda_metadata_model->capitalizeTracks();
}

void mainwindow::split_metadata_titles() {
  bool ok;
  QString divider = QInputDialog::getText(this, trUtf8("Please set a divider string. Be aware of empty spaces."),
                                          trUtf8("Divider:"), QLineEdit::Normal,
                                          " - ", &ok);
  if (ok && !divider.isEmpty()) {
    cdda_metadata_model->splitTitleOfTracks(divider);
  }

}

void mainwindow::autofill_artists() {

  QMessageBox msgBox(this);
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setFont(this->font());
  msgBox.setText(trUtf8("Do you really want to autofill track artists?"));
  msgBox.addButton(trUtf8("Yes"), QMessageBox::AcceptRole);
  QPushButton *noButton = msgBox.addButton(trUtf8("No"), QMessageBox::RejectRole);
  msgBox.exec();
  if (msgBox.clickedButton() == noButton) return;

  cdda_metadata_model->setTitleArtistsFromHeader();

}

void mainwindow::slot_cddb_query_done(int matchcount) {
  int match = 0;
  if (matchcount > 1) {
    cddbmatchingdialog *cddbMatchingDialog = new cddbmatchingdialog(this, settings, cddb->queryResults(), &match);
    if (cddbMatchingDialog->exec()!=QDialog::Accepted) { delete cddbMatchingDialog; return; }
    delete cddbMatchingDialog;
  }
  cddb->read(match);
}

void mainwindow::slot_cddb_read_done() {
  cdda_metadata_model->setMetadata(cddb->readResult());
  update_header_metadata();
  tableView->resizeColumnsToContents();
  update_various(cdda_metadata_model->isVarious());
  checkBox->setChecked(cdda_metadata_model->isVarious());
  update_fetch_cover_from_amazon_action();
  update_cddb_actions();
}

void mainwindow::slot_cddb_submit_done() {
  QMessageBox msgBox(this);
  msgBox.setIcon(QMessageBox::Information);
  msgBox.setFont(this->font());
  msgBox.setText(trUtf8("CD information successfully submitted.\nPlease consider transmission of metadata to FreeDB could take some hours until the server confirms and accept the data."));
  msgBox.addButton(trUtf8("Ok"), QMessageBox::AcceptRole);
  msgBox.exec();
}

void mainwindow::slot_cddb_busy(bool indicator, int kind) {
  action_Fetch->setEnabled(!indicator);
  action_Submit->setEnabled(!indicator);
  if (indicator) {
    statusBar()->addWidget(cddbLabel);
    if (kind==0)
      cddbLabel->setText("<font color=red>"+trUtf8("Fetching CDDB information...")+"</font>");
    else if (kind==1)
      cddbLabel->setText("<font color=red>"+trUtf8("Sending CDDB information...")+"</font>");
  } else {
    statusBar()->removeWidget(cddbLabel);
  }
}

void mainwindow::slot_cddb_proxy_auth_req(const QNetworkProxy& proxy, QAuthenticator *authenticator) {
  Q_UNUSED(proxy);
  proxyauthdialog *proxyAuthDialog = new proxyauthdialog(this, settings, "cddb", trUtf8("CDDB/FreeDB server proxy authentication required.\nPlease log in:"), authenticator);
  proxyAuthDialog->exec();
  delete proxyAuthDialog;
}

void mainwindow::closeEvent(QCloseEvent *event) {

  if ((cdda_metadata_model->isCDDBRelevantDataModified()) && ((settings->value("settings/cddb_remote_enabled", TRUE).toBool()) || (settings->value("settings/cddb_cache_enabled", TRUE).toBool()))) {
    QMessageBox msgBox(this);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setFont(this->font());
    msgBox.setText(trUtf8("You have modified and unsubmitted data. Do you you really want to exit?"));
    msgBox.addButton(trUtf8("Yes"), QMessageBox::AcceptRole);
    QPushButton *noButton = msgBox.addButton(trUtf8("No"), QMessageBox::RejectRole);
    msgBox.setDefaultButton(noButton);
    msgBox.exec();
    if (msgBox.clickedButton() == noButton) { event->ignore(); return; }
  }

  settings->setValue("mainwindow/pos", pos());
  settings->setValue("mainwindow/size", size());
  settings->setValue("mainwindow/stat", saveState());
  event->accept();

}

void mainwindow::create_toolbar() {

  toolBar->addAction(action_Eject);
  toolBar->addSeparator();

  profileLabel = new QLabel();
  profileLabel->setMargin(8);
  profileLabel->setText(trUtf8("Profile"));
  toolBar->addWidget(profileLabel);
  profileComboBox = new QComboBox();
  toolBar->addWidget(profileComboBox);
  nullLabel = new QLabel();
  nullLabel->setMargin(2);
  toolBar->addWidget(nullLabel);
  toolBar->addSeparator();

  toolBar->addAction(action_Fetch);
  toolBar->addAction(actionE_xtract);
  toolBar->addSeparator();
  toolBar->addAction(action_Settings);
  toolBar->addSeparator();
  toolBar->addAction(actionEx_it);

}

void mainwindow::create_statusbar() {
  statusLabel = new QLabel();
  statusLabel->setAlignment(Qt::AlignHCenter);
  statusLabel->setMinimumSize(QSize(100, 10));
  statusBar()->addWidget(statusLabel);
  modeLabel = new QLabel();
  modeLabel->setAlignment(Qt::AlignHCenter);
  modeLabel->setMinimumSize(QSize(100, 10));
  statusBar()->addWidget(modeLabel);
  cddbLabel = new QLabel();
  cddbLabel->setAlignment(Qt::AlignHCenter);
  cddbLabel->setMinimumSize(QSize(100, 10));
}

QStringList mainwindow::check_metadata() const {

  QDateTime now = QDateTime::currentDateTime();
  QString label1, label2;
  if (profile_model->currentProfileRow()==-1) {
    label1 = trUtf8("Artist");
    label2 = trUtf8("Album");
  } else {
    label1 = profile_model->data(profile_model->index(profile_model->currentProfileRow(), Q_PROFILE_MODEL_COLUMN_ARTISTLABEL_INDEX)).toString();
    label2 = profile_model->data(profile_model->index(profile_model->currentProfileRow(), Q_PROFILE_MODEL_COLUMN_TITLELABEL_INDEX)).toString();
  }

  QStringList tmp;
  bool ok;
  int year = cdda_metadata_model->year().toInt(&ok);
  if (!ok) {
    tmp.append(trUtf8("Year (%1) maybe wrong. It is not a number.").arg(cdda_metadata_model->year()));
  } else {
    if (year < 1920) tmp.append(trUtf8("Year (%1) maybe wrong. It is unusual back in the past.").arg(cdda_metadata_model->year()));
    if (year > now.date().year()) tmp.append(trUtf8("Year (%1) given is in future.").arg(cdda_metadata_model->year()));
  }
  if (cdda_metadata_model->genre().isEmpty()) tmp.append(trUtf8("Genre is empty."));

  if (!cdda_metadata_model->isArtistValid()) tmp.append(trUtf8("CD %1 is invalid (standard value or empty). Please check.").arg(label1));
  if (!cdda_metadata_model->isTitleValid()) tmp.append(trUtf8("CD %1 is invalid (standard value or empty). Please check.").arg(label2));
  for (int i = 1; i <= cdda_metadata_model->numOfAudioTracks(); i++) {
    if (cdda_metadata_model->isVarious()) {
      if (!cdda_metadata_model->isTrackArtistValid(i)) tmp.append(trUtf8("Track artist of track #%1 is invalid (standard value or empty). Please check.").arg(i));
    }
    if (!cdda_metadata_model->isTrackTitleValid(i)) tmp.append(trUtf8("Track title of track #%1 is invalid (standard value or empty). Please check.").arg(i));
  }

  return tmp;

}
