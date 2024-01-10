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

#ifndef SETTINGSDIALOG_HEADER
#define SETTINGSDIALOG_HEADER

#include <QtCore>
#include <QtGui>
#include "ui_settingsdialog.h"

#include "addprofiledialog.h"
#include "errordialog.h"
#include "proxydialog.h"

#include "../device/qcddadevice.h"
#include "../metadata/qcddametadatamodel.h"
#include "../base/qprofilemodel.h"
#include "../sql/qaudexsql.h"

class settingsdialog : public QDialog, private Ui::SettingsDialog {

  Q_OBJECT

public:
  settingsdialog(QWidget *parent, QSettings *settings, QCDDADevice *device, QCDDAMetaDataModel *cdda_metadata_model,
	QProfileModel *profile_model, QTableView *table_view);
  ~settingsdialog();

signals:
  void overwriteExistingFilesChanged(bool enabled);
  void deletePartialFilesChanged(bool enabled);
  void ejectCDTrayChanged(bool enabled);
  void amazonProxyChanged(bool enabled);
  void amazonLocaleChanged(const QString& locale);
  void deviceChanged(const QString& device);
  void paranoiaModeChanged(bool enabled);
  void neverSkipChanged(bool enabled);
  void tempPathChanged(const QString& path);
  void basePathChanged(const QString& path);
  void columnViewChanged(int column);
  void activateRemoteChanged(bool enabled);
  void hostChanged(const QString& host);
  void portChanged(int port);
  void CDDBProtoChanged(int protocol);
  void HTTPQueryPathChanged(const QString& path);
  void HTTPSubmitPathChanged(const QString& path);
  void emailAddressChanged(const QString& email);
  void cddbProxyChanged(bool enabled);
  void activateLocalCacheChanged(bool enabled);
  void localCachePathChanged(const QString& path);
  void queryAutomaticChanged(bool enabled);
  void profileChanged(int profileRow);
  void sqlNameChanged(const QString& name);
  void sqlDriverChanged(const QString& driver);
  void sqlEnabledChanged(bool enabled);
  void sqlHostChanged(const QString& host);
  void sqlPortChanged(int port);
  void sqlDatabaseChanged(const QString& database);
  void sqlUserChanged(const QString& user);
  void sqlPasswordChanged(const QString& password);
  void sqlOptionsChanged(const QString& options);

private slots:
  void setPage(QListWidgetItem *current, QListWidgetItem *previous);

  void overwriteExistingFiles(bool enabled);
  void deletePartialFiles(bool enabled);
  void ejectCDTray(bool enabled);
  void enableAmazonProxy(bool enabled);
  void amazonLocale(int index);
  void openAmazonProxyDialog();
  void checkDevice();
  void enableParanoiaMode(bool enable);
  void enableNeverSkip(bool enable);
  void changeTempPath();
  void changeBasePath();
  void tempPathSelection();
  void basePathSelection();

  void modifyColumnView(QListWidgetItem* item);

  void activateRemote(bool enabled);
  void changeHost();
  void changePort();
  void CDDBProtoActions(int proto);
  void changeHTTPQueryPath();
  void changeHTTPSubmitPath();
  void changeEmailAddress();
  void enableCDDBProxy(bool enabled);
  void openCDDBProxyDialog();
  void activateLocalCache(bool enabled);
  void changeLocalCachePath();
  void localCachePathSelection();
  void queryAutomatic(bool enabled);

  void changeProfile(const QModelIndex&);
  void addProfile();
  void delProfile();
  void modProfile();
  void copyProfile();
  void saveProfiles();
  void loadProfiles();
  void cursorPos0();
  void changeFAT32Compatible(bool checked) { checkBox_10->setChecked(!checked); }
  void changeStorePlaylistByHand(bool checked) { checkBox_14->setChecked(!checked); }
  void changeStoreCoverByHand(bool checked) { checkBox_11->setChecked(!checked); }

  void changeSQLName();
  void changeSQLDriver(int driver);
  void enableSQL(bool enabled);
  void changeSQLHost();
  void changeSQLPort();
  void changeSQLDatabase();
  void changeSQLUser();
  void changeSQLPassword();
  void changeSQLOptions();
  void checkSQLConnection();
  void createSQLTables();

  void show_error(const QString& description, const QString& solution);
  void show_warn(const QString& description);
  void show_log(const QString& message);

protected:
  void closeEvent(QCloseEvent *event);

private:
  QSettings *settings;
  QCDDADevice *device;
  QCDDAMetaDataModel *cdda_metadata_model;
  QProfileModel *profile_model;
  QTableView *table_view;
  QDataWidgetMapper *mapper;

  QListWidgetItem general;
  QListWidgetItem views;
  QListWidgetItem cddb;
  QListWidgetItem profiles;
  QListWidgetItem database;
  QListWidgetItem filebase;

};

#endif
