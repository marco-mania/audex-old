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

#include "settingsdialog.h"

settingsdialog::settingsdialog(QWidget *parent, QSettings *settings, QCDDADevice *device,
	QCDDAMetaDataModel *cdda_metadata_model, QProfileModel *profile_model, QTableView *table_view) {

  Q_UNUSED(parent);

  setupUi(this);

  this->settings = settings;
  this->device = device;
  this->cdda_metadata_model = cdda_metadata_model;
  this->profile_model = profile_model;
  this->table_view = table_view;

  connect(listWidget, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)), this, SLOT(setPage(QListWidgetItem*, QListWidgetItem*)));

  connect(pushButton_8, SIGNAL(clicked()), this, SLOT(close()));

  connect(checkBox_9, SIGNAL(toggled(bool)), this, SLOT(overwriteExistingFiles(bool)));
  connect(checkBox, SIGNAL(toggled(bool)), this, SLOT(deletePartialFiles(bool)));
  connect(checkBox_2, SIGNAL(toggled(bool)), this, SLOT(ejectCDTray(bool)));
  connect(checkBox_8, SIGNAL(toggled(bool)), this, SLOT(enableAmazonProxy(bool)));
  comboBox_5->addItem(QIcon(":/flags/images/usa.png"), trUtf8("International/US"), "us");
  comboBox_5->addItem(QIcon(":/flags/images/germany.png"), trUtf8("Germany"), "de");
  comboBox_5->addItem(QIcon(":/flags/images/france.png"), trUtf8("France"), "fr");
  comboBox_5->addItem(QIcon(":/flags/images/japan.png"), trUtf8("Japan"), "jp");
  comboBox_5->addItem(QIcon(":/flags/images/canada.png"), trUtf8("Canada"), "ca");
  connect(comboBox_5, SIGNAL(currentIndexChanged(int)), this, SLOT(amazonLocale(int)));
  connect(pushButton_14, SIGNAL(clicked()), this, SLOT(openAmazonProxyDialog()));
  connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(checkDevice()));
  connect(checkBox_12, SIGNAL(toggled(bool)), this, SLOT(enableParanoiaMode(bool)));
  connect(checkBox_13, SIGNAL(toggled(bool)), this, SLOT(enableNeverSkip(bool)));
  connect(lineEdit_16, SIGNAL(editingFinished()), this, SLOT(changeTempPath()));
  connect(lineEdit_18, SIGNAL(editingFinished()), this, SLOT(changeBasePath()));
  connect(pushButton_9, SIGNAL(clicked()), this, SLOT(tempPathSelection()));
  connect(pushButton_11, SIGNAL(clicked()), this, SLOT(basePathSelection()));

  connect(listWidget_2, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(modifyColumnView(QListWidgetItem*)));

  connect(checkBox_6, SIGNAL(toggled(bool)), this, SLOT(activateRemote(bool)));
  connect(lineEdit, SIGNAL(editingFinished()), this, SLOT(changeHost()));
  connect(spinBox, SIGNAL(editingFinished()), this, SLOT(changePort()));
  connect(comboBox_2, SIGNAL(currentIndexChanged(int)), this, SLOT(CDDBProtoActions(int)));
  connect(lineEdit_9, SIGNAL(editingFinished()), this, SLOT(changeHTTPQueryPath()));
  connect(lineEdit_3, SIGNAL(editingFinished()), this, SLOT(changeHTTPSubmitPath()));
  connect(lineEdit_4, SIGNAL(editingFinished()), this, SLOT(changeEmailAddress()));
  connect(checkBox_4, SIGNAL(toggled(bool)), this, SLOT(enableCDDBProxy(bool)));
  connect(pushButton_13, SIGNAL(clicked()), this, SLOT(openCDDBProxyDialog()));
  connect(checkBox_7, SIGNAL(toggled(bool)), this, SLOT(activateLocalCache(bool)));
  connect(lineEdit_2, SIGNAL(editingFinished()), this, SLOT(changeLocalCachePath()));
  connect(pushButton, SIGNAL(clicked()), this, SLOT(localCachePathSelection()));
  connect(checkBox_3, SIGNAL(toggled(bool)), this, SLOT(queryAutomatic(bool)));

  mapper = new QDataWidgetMapper(this);
  mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
  mapper->setModel(profile_model);
  mapper->addMapping(lineEdit_12, 0);
  mapper->addMapping(lineEdit_11, 1);
  mapper->addMapping(lineEdit_10, 2);
  mapper->addMapping(lineEdit_13, 3);
  mapper->addMapping(lineEdit_14, 4);
  mapper->addMapping(lineEdit_15, 5);
  mapper->addMapping(spinBox_3, 6);
  mapper->addMapping(checkBox_10, 7);
  mapper->addMapping(checkBox_11, 8);
  mapper->addMapping(checkBox_14, 13);
  listView->setModel(profile_model);
  listView->setModelColumn(0);
  connect(listView->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex&, const QModelIndex&)), mapper, SLOT(setCurrentModelIndex(const QModelIndex&)));
  connect(listView->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(changeProfile(const QModelIndex&)));
  connect(listView, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(modProfile()));
  connect(pushButton_5, SIGNAL(clicked()), this, SLOT(addProfile()));
  connect(pushButton_6, SIGNAL(clicked()), this, SLOT(delProfile()));
  connect(pushButton_7, SIGNAL(clicked()), this, SLOT(modProfile()));
  connect(pushButton_10, SIGNAL(clicked()), this, SLOT(copyProfile()));
  connect(pushButton_2, SIGNAL(clicked()), this, SLOT(loadProfiles()));
  connect(pushButton_3, SIGNAL(clicked()), this, SLOT(saveProfiles()));
  connect(lineEdit_11, SIGNAL(textChanged(const QString&)), this, SLOT(cursorPos0()));
  connect(lineEdit_14, SIGNAL(textChanged(const QString&)), this, SLOT(cursorPos0()));
  connect(checkBox_10, SIGNAL(clicked(bool)), this, SLOT(changeFAT32Compatible(bool)));
  connect(checkBox_14, SIGNAL(clicked(bool)), this, SLOT(changeStorePlaylistByHand(bool)));
  connect(checkBox_11, SIGNAL(clicked(bool)), this, SLOT(changeStoreCoverByHand(bool)));

  connect(lineEdit_17, SIGNAL(editingFinished()), this, SLOT(changeSQLName()));
  QAudexSQL *audexsql = new QAudexSQL(this, settings, profile_model, cdda_metadata_model);
  QStringList drivers = audexsql->drivers();
  delete audexsql;
  for (int i = 0; i < drivers.count(); i++) {
    if (drivers[i] == "QMYSQL") comboBox_3->addItem(QIcon(":/logo/images/mysql.png"), "MySQL", "QMYSQL");
    if (drivers[i] == "QPSQL") comboBox_3->addItem(QIcon(":/logo/images/postgresql.png"), "PostgreSQL", "QPSQL");
    if (drivers[i] == "QIBASE") comboBox_3->addItem(QIcon(":/logo/images/interbase.png"), "Firebird/InterBase", "QIBASE");
  }
  connect(comboBox_3, SIGNAL(currentIndexChanged(int)), this, SLOT(changeSQLDriver(int)));
  connect(checkBox_5, SIGNAL(toggled(bool)), this, SLOT(enableSQL(bool)));
  connect(lineEdit_5, SIGNAL(editingFinished()), this, SLOT(changeSQLHost()));
  connect(spinBox_2, SIGNAL(editingFinished()), this, SLOT(changeSQLPort()));
  connect(lineEdit_6, SIGNAL(editingFinished()), this, SLOT(changeSQLDatabase()));
  connect(lineEdit_7, SIGNAL(editingFinished()), this, SLOT(changeSQLUser()));
  connect(lineEdit_8, SIGNAL(editingFinished()), this, SLOT(changeSQLPassword()));
  connect(lineEdit_19, SIGNAL(editingFinished()), this, SLOT(changeSQLOptions()));
  connect(pushButton_4, SIGNAL(clicked()), this, SLOT(checkSQLConnection()));
  connect(pushButton_12, SIGNAL(clicked()), this, SLOT(createSQLTables()));

  general.setIcon(QIcon(":/buttons/images/settings.png"));
  general.setText(trUtf8("General"));
  listWidget->addItem(&general);
  views.setIcon(QIcon(":/buttons/images/view.png"));
  views.setText(trUtf8("Views"));
  listWidget->addItem(&views);
  cddb.setIcon(QIcon(":/buttons/images/cddb.png"));
  cddb.setText(trUtf8("CDDB"));
  listWidget->addItem(&cddb);
  profiles.setIcon(QIcon(":/buttons/images/profiles.png"));
  profiles.setText(trUtf8("Profiles"));
  listWidget->addItem(&profiles);
  database.setIcon(QIcon(":/buttons/images/database.png"));
  database.setText(trUtf8("Database"));
  listWidget->addItem(&database);
  filebase.setIcon(QIcon(":/buttons/images/filebase.png"));
  filebase.setText(trUtf8("Filebase"));
  listWidget->addItem(&filebase);
  listWidget->setCurrentRow(0);

  resize(settings->value("settingsdialog/size", QSize(600, 570)).toSize());
  move(settings->value("settingsdialog/pos", QPoint(100, 100)).toPoint());

}

settingsdialog::~settingsdialog() {
  delete mapper;
}

void settingsdialog::setPage(QListWidgetItem *current, QListWidgetItem *previous) {
  Q_UNUSED(previous);
  if (current==&general) {
    checkBox_9->setChecked(settings->value("settings/overwrite_existing_files", FALSE).toBool());
    checkBox->setChecked(settings->value("settings/delete_part_files", TRUE).toBool());
    checkBox_2->setChecked(settings->value("settings/eject_after_extract", FALSE).toBool());
    checkBox_8->setChecked(settings->value("settings/amazon_proxy_enabled", FALSE).toBool());
    comboBox_5->setCurrentIndex(comboBox_5->findData(settings->value("settings/amazon_locale", "us").toString()));
    pushButton_14->setEnabled(checkBox_8->isChecked());
    comboBox->setEditText(settings->value("settings/device", "/dev/cdrom").toString());
    checkBox_12->setChecked(settings->value("settings/full_paranoia_mode", TRUE).toBool());
    checkBox_13->setChecked(settings->value("settings/never_skip_mode", TRUE).toBool());
    lineEdit_16->setText(settings->value("settings/temp_path", "/tmp/").toString());
    lineEdit_18->setText(settings->value("settings/base_path", QString("%1").arg(QDir::homePath())).toString());
    stackedWidget->setCurrentIndex(0);
  } else if (current==&views) {
    listWidget_2->clear();
    for (int i = 0; i < cdda_metadata_model->columnCount(); i++) {
      QListWidgetItem *newItem = new QListWidgetItem;
      newItem->setText(cdda_metadata_model->headerData(i, Qt::Horizontal).toString());
      newItem->setData(Qt::UserRole, i);
      newItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
      QString head = cdda_metadata_model->headerData(i, Qt::Horizontal).toString();
      head.replace(' ', '_'); head = head.toLower();
      if (settings->value(QString("cdda_model/column_visible_%1").arg(head), TRUE).toBool())
        newItem->setCheckState(Qt::Checked);
      else
        newItem->setCheckState(Qt::Unchecked);
      listWidget_2->addItem(newItem);
    }
    stackedWidget->setCurrentIndex(1);
  } else if (current==&cddb) {
    checkBox_6->setChecked(settings->value("settings/cddb_remote_enabled", TRUE).toBool());
    groupBox_4->setEnabled(checkBox_6->isChecked());
    lineEdit->setText(settings->value("settings/cddb_host", "freedb.org").toString());
    lineEdit_9->setText(settings->value("settings/cddb_http_query_path", "/~cddb/cddb.cgi").toString());
    lineEdit_3->setText(settings->value("settings/cddb_http_submit_path", "/~cddb/submit.cgi").toString());
    lineEdit_4->setText(settings->value("settings/cddb_http_email_address", "info@anyaudio.de").toString());
    comboBox_2->setCurrentIndex(settings->value("settings/cddb_protocol", 1).toInt());
    label_12->setEnabled(comboBox_2->currentIndex()==1);
    lineEdit_9->setEnabled(comboBox_2->currentIndex()==1);
    label_4->setEnabled(comboBox_2->currentIndex()==1);
    lineEdit_3->setEnabled(comboBox_2->currentIndex()==1);
    quint16 sport = 80;
    if (comboBox_2->currentIndex()==0) sport = 8880;
    spinBox->setValue(settings->value("settings/cddb_port", sport).toInt());
    checkBox_4->setChecked(settings->value("settings/cddb_proxy_enabled", FALSE).toBool());
    pushButton_13->setEnabled(checkBox_4->isChecked());
    checkBox_7->setChecked(settings->value("settings/cddb_cache_enabled", TRUE).toBool());
    groupBox_5->setEnabled(checkBox_7->isChecked());
    lineEdit_2->setText(settings->value("settings/cddb_cache_path", "~/.cddb").toString());
    checkBox_3->setChecked(settings->value("settings/cddb_query_after_load", TRUE).toBool());
    stackedWidget->setCurrentIndex(2);
  } else if (current==&profiles) {
    if (profile_model->rowCount() > 0) {
      int row = settings->value("profiles/std", 0).toInt();
      listView->setCurrentIndex(profile_model->index(row, 0));
    }
    stackedWidget->setCurrentIndex(3);
  } else if (current==&database) {
    lineEdit_17->setText(settings->value("settings/sql_name", "audex").toString());
    int port = 0;
    if (settings->value("settings/sql_driver", "QMYSQL").toString()=="QMYSQL") {
      port = 3306;
      comboBox_3->setCurrentIndex(comboBox_3->findData("QMYSQL"));
      label_27->setPixmap(QPixmap(":/logo/images/mysql.png"));
      pushButton_12->setEnabled(TRUE);
    } else if (settings->value("settings/sql_driver", "QMYSQL").toString()=="QPSQL") {
      port = 5432;
      comboBox_3->setCurrentIndex(comboBox_3->findData("QPSQL"));
      label_27->setPixmap(QPixmap(":/logo/images/postgresql.png"));
      pushButton_12->setEnabled(TRUE);
    } else if (settings->value("settings/sql_driver", "QMYSQL").toString()=="QIBASE") {
      port = 3050;
      comboBox_3->setCurrentIndex(comboBox_3->findData("QIBASE"));
      label_27->setPixmap(QPixmap(":/logo/images/firebird.png"));
      pushButton_12->setEnabled(FALSE);
    } else {
      checkBox_5->setEnabled(FALSE);
      groupBox_2->setEnabled(FALSE);
      if (!settings->value("settings/sql_no_plugins_msg", FALSE).toBool()) { show_error(trUtf8("No database drivers found."), trUtf8("Please check your Qt installation.")); settings->setValue("settings/sql_no_plugins_msg", TRUE); }
    }
    checkBox_5->setChecked(settings->value("settings/sql_enabled", FALSE).toBool());
    groupBox_2->setEnabled(checkBox_5->isChecked());
    label_16->setEnabled(checkBox_5->isChecked());
    spinBox_3->setEnabled(checkBox_5->isChecked());
    lineEdit_5->setText(settings->value("settings/sql_host", "localhost").toString());
    spinBox_2->setValue(settings->value("settings/sql_port", port).toInt());
    lineEdit_6->setText(settings->value("settings/sql_db", "audex").toString());
    lineEdit_7->setText(settings->value("settings/sql_user", "audex").toString());
    lineEdit_8->setText(QString(QByteArray::fromBase64(settings->value("settings/sql_password", QByteArray("audex").toBase64()).toByteArray())));
    lineEdit_19->setText(settings->value("settings/sql_options", "").toString());
    stackedWidget->setCurrentIndex(4);
  } else if (current==&filebase) {
    stackedWidget->setCurrentIndex(5);
  }
}

void settingsdialog::overwriteExistingFiles(bool enabled) {
  settings->setValue("settings/overwrite_existing_files", enabled);
  emit overwriteExistingFilesChanged(enabled);
}

void settingsdialog::deletePartialFiles(bool enabled) {
  settings->setValue("settings/delete_part_files", enabled);
  emit deletePartialFilesChanged(enabled);
}

void settingsdialog::ejectCDTray(bool enabled) {
  settings->setValue("settings/eject_after_extract", enabled);
  emit ejectCDTrayChanged(enabled);
}

void settingsdialog::enableAmazonProxy(bool enabled) {
  settings->setValue("settings/amazon_proxy_enabled", enabled);
  pushButton_14->setEnabled(enabled);
  emit amazonProxyChanged(enabled);
}

void settingsdialog::amazonLocale(int index) {
  QString locale = comboBox_5->itemData(index).toString();
  settings->setValue("settings/amazon_locale", locale);
  emit amazonLocaleChanged(locale);
}

void settingsdialog::openAmazonProxyDialog() {
  proxydialog *proxyDialog = new proxydialog(this, settings, "amazon");
  proxyDialog->exec();
  delete proxyDialog;
}

void settingsdialog::checkDevice() {
  device->setDevice(comboBox->currentText());
  settings->setValue("settings/device", comboBox->currentText());
  emit deviceChanged(comboBox->currentText());
}

void settingsdialog::enableParanoiaMode(bool enable) {
  settings->setValue("settings/full_paranoia_mode", enable);
  emit paranoiaModeChanged(enable);
}

void settingsdialog::enableNeverSkip(bool enable) {
  settings->setValue("settings/never_skip_mode", enable);
  emit neverSkipChanged(enable);
}

void settingsdialog::changeBasePath() {
  settings->setValue("settings/base_path", lineEdit_18->text());
  emit basePathChanged(lineEdit_18->text());
}

void settingsdialog::changeTempPath() {
  settings->setValue("settings/temp_path", lineEdit_16->text());
  emit tempPathChanged(lineEdit_16->text());
}

void settingsdialog::tempPathSelection() {
  QString path = QFileDialog::getExistingDirectory(this,
	trUtf8("Please choose a directory"),
	lineEdit_16->text().replace(QRegExp("~"), QDir::homePath()),
	QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
  if (!path.isEmpty()) { lineEdit_16->setText(path); changeTempPath(); }
}

void settingsdialog::basePathSelection() {
  QString path = QFileDialog::getExistingDirectory(this,
	trUtf8("Please choose a directory"),
	lineEdit_18->text().replace(QRegExp("~"), QDir::homePath()),
	QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
  if (!path.isEmpty()) { lineEdit_18->setText(path); changeBasePath(); }
}

void settingsdialog::modifyColumnView(QListWidgetItem* item) {

  int i = item->data(Qt::UserRole).toInt();
  if (item->checkState()==Qt::Checked) {
    if (!((i == Q_CDDA_METADATA_MODEL_COLUMN_ARTIST_INDEX)
	&& (!cdda_metadata_model->isVarious())
	&& (table_view->isEnabled())))
      table_view->showColumn(i);
  } else {
    table_view->hideColumn(i);
  }

  QString head = item->text();
  head.replace(' ', '_'); head = head.toLower();
  if (item->checkState()==Qt::Checked)
    settings->setValue(QString("cdda_model/column_visible_%1").arg(head), TRUE);
  else
    settings->setValue(QString("cdda_model/column_visible_%1").arg(head), FALSE);

  emit columnViewChanged(i);

}

void settingsdialog::activateRemote(bool enabled) {
  groupBox_4->setEnabled(enabled);
  settings->setValue("settings/cddb_remote_enabled", enabled);
  emit activateRemoteChanged(enabled);
}

void settingsdialog::changeHost() {
  settings->setValue("settings/cddb_host", lineEdit->text());
  emit hostChanged(lineEdit->text());
}

void settingsdialog::changePort() {
  settings->setValue("settings/cddb_port", spinBox->value());
  emit portChanged(spinBox->value());
}

void settingsdialog::CDDBProtoActions(int proto) {
  switch (proto) {
    case 0 :
      spinBox->setValue(8880);
      changePort();
      label_12->setEnabled(FALSE);
      lineEdit_9->setEnabled(FALSE);
      label_4->setEnabled(FALSE);
      lineEdit_3->setEnabled(FALSE);
      break;
    case 1 :
      spinBox->setValue(80);
      changePort();
      label_12->setEnabled(TRUE);
      lineEdit_9->setEnabled(TRUE);
      label_4->setEnabled(TRUE);
      lineEdit_3->setEnabled(TRUE);
      break;
  }
  settings->setValue("settings/cddb_protocol", comboBox_2->currentIndex());
  emit CDDBProtoChanged(comboBox_2->currentIndex());
}

void settingsdialog::changeHTTPQueryPath() {
  settings->setValue("settings/cddb_http_query_path", lineEdit_9->text());
  emit HTTPQueryPathChanged(lineEdit_9->text());
}

void settingsdialog::changeHTTPSubmitPath() {
  settings->setValue("settings/cddb_http_submit_path", lineEdit_3->text());
  emit HTTPSubmitPathChanged(lineEdit_3->text());
}

void settingsdialog::changeEmailAddress() {
  settings->setValue("settings/cddb_http_email_address", lineEdit_4->text());
  emit emailAddressChanged(lineEdit_4->text());
}

void settingsdialog::enableCDDBProxy(bool enabled) {
  settings->setValue("settings/cddb_proxy_enabled", enabled);
  pushButton_13->setEnabled(enabled);
  emit cddbProxyChanged(enabled);
}

void settingsdialog::openCDDBProxyDialog() {
  proxydialog *proxyDialog = new proxydialog(this, settings, "cddb");
  proxyDialog->exec();
  delete proxyDialog;
}

void settingsdialog::activateLocalCache(bool enabled) {
  groupBox_5->setEnabled(enabled);
  settings->setValue("settings/cddb_cache_enabled", enabled);
  emit activateLocalCacheChanged(enabled);
}

void settingsdialog::changeLocalCachePath() {
  settings->setValue("settings/cddb_cache_path", lineEdit_2->text());
  emit localCachePathChanged(lineEdit_2->text());
}

void settingsdialog::localCachePathSelection() {
  QString path = QFileDialog::getExistingDirectory(this,
	trUtf8("Please choose a directory"),
	lineEdit_2->text().replace(QRegExp("~"), QDir::homePath()),
	QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
  if (!path.isEmpty()) lineEdit_2->setText(path);
}

void settingsdialog::queryAutomatic(bool enabled) {
  settings->setValue("settings/cddb_query_after_load", enabled);
  emit queryAutomaticChanged(enabled);
}

void settingsdialog::changeProfile(const QModelIndex& index) {

  bool pl = profile_model->data(profile_model->index(index.row(), Q_PROFILE_MODEL_COLUMN_PL_INDEX)).toBool();
  QString pl_format = profile_model->data(profile_model->index(index.row(), Q_PROFILE_MODEL_COLUMN_PL_FORMAT_INDEX)).toString();
  QString pl_name = profile_model->data(profile_model->index(index.row(), Q_PROFILE_MODEL_COLUMN_PL_NAME_INDEX)).toString();

  bool sc = profile_model->data(profile_model->index(index.row(), Q_PROFILE_MODEL_COLUMN_SC_INDEX)).toBool();
  bool scale = profile_model->data(profile_model->index(index.row(), Q_PROFILE_MODEL_COLUMN_SC_SCALE_INDEX)).toBool();
  QSize size = profile_model->data(profile_model->index(index.row(), Q_PROFILE_MODEL_COLUMN_SC_SIZE_INDEX)).toSize();
  QString format = profile_model->data(profile_model->index(index.row(), Q_PROFILE_MODEL_COLUMN_SC_FORMAT_INDEX)).toString();
  QString name = profile_model->data(profile_model->index(index.row(), Q_PROFILE_MODEL_COLUMN_SC_NAME_INDEX)).toString();

  if (pl) {
    checkBox_14->setText(trUtf8("Create playlist in target directory (Format: %1, Name: %2)").arg(pl_format).arg(pl_name));
  } else {
    checkBox_14->setText(trUtf8("Create playlist in target directory"));
  }

  if (sc) {
    if (scale) {
      checkBox_11->setText(trUtf8("Store cover file in target directory (Format: %1, Name: %2, Scale: %3x%4)").arg(format).arg(name).arg(size.width()).arg(size.height()));
    } else {
      checkBox_11->setText(trUtf8("Store cover file in target directory (Format: %1, Name: %2)").arg(format).arg(name));
    }
  } else {
    checkBox_11->setText(trUtf8("Store cover file in target directory"));
  }

  settings->setValue("profiles/std", index.row());
  emit profileChanged(index.row());
}

void settingsdialog::addProfile() {
  addprofiledialog *addProfileDialog = new addprofiledialog(this, settings, profile_model, -1);
  addProfileDialog->exec();
  delete addProfileDialog;
  mapper->revert();
  changeProfile(listView->currentIndex());
  profile_model->submit();
}

void settingsdialog::delProfile() {
  if (!listView->currentIndex().isValid()) return;
  QMessageBox msgBox;
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setText(trUtf8("Do you really want to delete profile %1?").arg(profile_model->data(profile_model->index(listView->currentIndex().row(), 0)).toString()));
  msgBox.addButton(trUtf8("Yes"), QMessageBox::AcceptRole);
  QPushButton *noButton = msgBox.addButton(trUtf8("No"), QMessageBox::RejectRole);
  msgBox.exec();
  if (msgBox.clickedButton() == noButton) return;
  profile_model->removeRows(listView->currentIndex().row(), 1);
  if (profile_model->rowCount()<1) {
    lineEdit_12->clear();
    lineEdit_11->clear();
    lineEdit_10->clear();
    lineEdit_13->clear();
    lineEdit_14->clear();
    lineEdit_15->clear();
    spinBox_3->setValue(-1);
  }
  profile_model->submit();
}

void settingsdialog::modProfile() {
  addprofiledialog *addProfileDialog = new addprofiledialog(this, settings, profile_model, listView->currentIndex().row());
  addProfileDialog->exec();
  delete addProfileDialog;
  mapper->revert();
  changeProfile(listView->currentIndex());
  profile_model->submit();
}

void settingsdialog::copyProfile() {
  profile_model->copy();
}

void settingsdialog::saveProfiles() {
  QFileDialog fileDialog;
  fileDialog.setAcceptMode(QFileDialog::AcceptSave);
  fileDialog.setWindowTitle(trUtf8("Save Audex Profile File"));
  fileDialog.setDirectory(QDir::homePath());
  fileDialog.setFilter(trUtf8("Audex Profile File (*.apf)"));
  fileDialog.setDefaultSuffix("apf");
  fileDialog.exec();
  QString fileName = fileDialog.selectedFiles().at(0);
  if (!fileName.isEmpty()) profile_model->saveProfilesToFile(fileName);
}

void settingsdialog::loadProfiles() {
  QFileDialog fileDialog;
  fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
  fileDialog.setWindowTitle(trUtf8("Open Audex Profile File"));
  fileDialog.setDirectory(QDir::homePath());
  fileDialog.setFilter(trUtf8("Audex Profile File (*.apf)"));
  fileDialog.setDefaultSuffix("apf");
  fileDialog.exec();
  QString fileName = fileDialog.selectedFiles().at(0);
  if (!fileName.isEmpty()) profile_model->loadProfilesFromFile(fileName);
  profile_model->submit();
}

void settingsdialog::cursorPos0() {
  lineEdit_11->setCursorPosition(0);
  lineEdit_14->setCursorPosition(0);
}

void settingsdialog::changeSQLName() {
  settings->setValue("settings/sql_name", lineEdit_17->text());
  emit sqlNameChanged(lineEdit_17->text());
}

void settingsdialog::changeSQLDriver(int driver) {

  if (comboBox_3->itemData(driver).toString()=="QMYSQL") {
    label_27->setPixmap(QPixmap(":/logo/images/mysql.png"));
    spinBox_2->setValue(3306); changeSQLPort();
    pushButton_12->setEnabled(TRUE);
  } else if (comboBox_3->itemData(driver).toString()=="QPSQL") {
    label_27->setPixmap(QPixmap(":/logo/images/postgresql.png"));
    spinBox_2->setValue(5432); changeSQLPort();
    pushButton_12->setEnabled(TRUE);
  } else if (comboBox_3->itemData(driver).toString()=="QIBASE") {
    label_27->setPixmap(QPixmap(":/logo/images/firebird.png"));
    spinBox_2->setValue(3050); changeSQLPort();
    pushButton_12->setEnabled(FALSE);
  }

  settings->setValue("settings/sql_driver", comboBox_3->itemData(driver).toString());
  emit sqlDriverChanged(comboBox_3->itemData(driver).toString());

}

void settingsdialog::enableSQL(bool enabled) {
  groupBox_2->setEnabled(enabled);
  label_16->setEnabled(enabled);
  spinBox_3->setEnabled(enabled);
  settings->setValue("settings/sql_enabled", enabled);
  emit sqlEnabledChanged(enabled);
}

void settingsdialog::changeSQLHost() {
  settings->setValue("settings/sql_host", lineEdit_5->text());
  emit sqlHostChanged(lineEdit_5->text());
}

void settingsdialog::changeSQLPort() {
  settings->setValue("settings/sql_port", spinBox_2->value());
  emit sqlPortChanged(spinBox_2->value());
}

void settingsdialog::changeSQLDatabase() {
  settings->setValue("settings/sql_db", lineEdit_6->text());
  emit sqlDatabaseChanged(lineEdit_6->text());
}

void settingsdialog::changeSQLUser() {
  settings->setValue("settings/sql_user", lineEdit_7->text());
  emit sqlUserChanged(lineEdit_7->text());
}

void settingsdialog::changeSQLPassword() {
  settings->setValue("settings/sql_password", lineEdit_8->text().toAscii().toBase64());
  emit sqlPasswordChanged(lineEdit_8->text());
}

void settingsdialog::changeSQLOptions() {
  settings->setValue("settings/sql_options", lineEdit_19->text());
  emit sqlOptionsChanged(lineEdit_19->text());
}

void settingsdialog::checkSQLConnection() {
  QAudexSQL *audexsql = new QAudexSQL(this, settings, profile_model, cdda_metadata_model);
  int result = audexsql->checkConnection();
  if (result==0) {
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setText(trUtf8("Connection checked and okay."));
    msgBox.addButton(trUtf8("OK"), QMessageBox::AcceptRole);
    msgBox.exec();
  } else if (result==1) {
    show_error(trUtf8("Connection to database failed."), trUtf8("Please check if your connection is up. Also check your host, port, database name, user name and password."));
  } else if (result==2) {
    show_error(trUtf8("Connection succeeded and database found but structure checking failed."), trUtf8("Please check your database and make sure it has proper structure. If you just want to create new tables for audex you can let audex create tables automatically.\nOtherwise consult the documentation."));
  }
  delete audexsql;
}

void settingsdialog::createSQLTables() {

  QMessageBox msgBox;
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setText(trUtf8("Do you really want to create audex tables on the database?"));
  msgBox.addButton(trUtf8("Yes"), QMessageBox::AcceptRole);
  QPushButton *noButton = msgBox.addButton(trUtf8("No"), QMessageBox::RejectRole);
  msgBox.exec();
  if (msgBox.clickedButton() == noButton) return;

  QAudexSQL *audexsql = new QAudexSQL(this, settings, profile_model, cdda_metadata_model);
  connect(audexsql, SIGNAL(error(const QString&, const QString&)), this, SLOT(show_error(const QString&, const QString&)));
  if (audexsql->createTables()) {
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setText(trUtf8("All tables successfully created."));
    msgBox.addButton(trUtf8("OK"), QMessageBox::AcceptRole);
    msgBox.exec();
  }
  delete audexsql;

}

void settingsdialog::show_error(const QString& description, const QString& solution) {
  errordialog *errorDialog = new errordialog(this, settings, description, solution);
  errorDialog->exec();
  delete errorDialog;
}

void settingsdialog::show_warn(const QString& description) {
  qDebug() << QString("WARNING: %1").arg(description);
}

void settingsdialog::show_log(const QString& message) {
  qDebug() << message;
}

void settingsdialog::closeEvent(QCloseEvent *event) {
  settings->setValue("settingsdialog/pos", pos());
  settings->setValue("settingsdialog/size", size());
  event->accept();
}
