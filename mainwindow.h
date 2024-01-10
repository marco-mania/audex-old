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

#ifndef MAINWINDOW_HEADER
#define MAINWINDOW_HEADER

#include <QtCore>
#include <QtGui>
#include <QtSql>
#include "ui_mainwindow.h"

#include "base/qprofilemodel.h"
#include "metadata/qcddametadatamodel.h"
#include "base/qencoderwrapper.h"
#include "metadata/qcddb.h"
#include "sql/qaudexsql.h"

#include "dialogs/extractdialog.h"
#include "dialogs/settingsdialog.h"
#include "dialogs/reportdialog.h"
#include "dialogs/cddbmatchingdialog.h"
#include "dialogs/sqlmatchingdialog.h"
#include "dialogs/aboutdialog.h"
#include "dialogs/errordialog.h"
#include "dialogs/multicddialog.h"
#include "dialogs/coverfromamazondialog.h"
#include "dialogs/fullcoverdialog.h"
#include "dialogs/categorydialog.h"
#include "dialogs/proxyauthdialog.h"

#define		LABEL		"Audex"
#define		MARK		"audex"
#define		NAME		"Little Linz^2"
#define		VERSION		"0.46"
#define		VERSION_MARK	"beta"

class mainwindow : public QMainWindow, private Ui::MainWindow {

  Q_OBJECT

public:
  mainwindow(QWidget *parent = 0);
  ~mainwindow();

private slots:

  void fetch_cddb_data();
  void submit_cddb_data();
  void eject();

  void open_extract_dialog();
  void open_settings_dialog();
  void open_about_qt_dialog();
  void open_about_dialog();
  void open_amazon_cover_dialog();
  void load_cover();
  void save_cover();
  void open_cover_dialog();
  void remove_cover();

  void cover_context_menu(const QPoint& point);

  void enable_main_overview(bool enabled);
  void enable_fetch_cddb_data(bool enabled);
  void enable_submit_cddb_data(bool enabled);
  void enable_extract(bool enabled);
  void enable_fetch_cover_from_amazon(bool enabled);
  void enable_load_cover(bool enabled);
  void enable_save_cover(bool enabled);
  void enable_show_cover_dialog(bool enabled);
  void enable_delete_cover(bool enabled);

  void show_error(const QString& description, const QString& solution);
  void show_warn(const QString& description);
  void show_log(const QString& message);

  void update_drive_status(QCDDADrive::DriveStatus drivestatus);
  void update_header_metadata();
  void update_labels(int profile_row);
  void update_various(bool various);
  void update_title_correction_tools(bool enable);
  void update_artist();
  void update_title();
  void update_genre();
  void update_year();
  void update_extended_data();
  void update_mode(bool enabled);
  void update_cdda_model_columns();
  void update_cddb_actions();
  void update_fetch_cover_from_amazon_action();
  void update_cover(const QImage& image);

  void editNextTableItem(const QModelIndex&);

  void swap_metadata_artist_and_titles();
  void capitalize_metadata();
  void split_metadata_titles();
  void autofill_artists();

  void slot_cddb_query_done(int matchcount);
  void slot_cddb_read_done();
  void slot_cddb_submit_done();
  void slot_cddb_busy(bool indicator, int kind);

  void slot_cddb_proxy_auth_req(const QNetworkProxy& proxy, QAuthenticator *authenticator);

protected:
  void closeEvent(QCloseEvent *event);

private:
  QSettings *settings;
  QProfileModel *profile_model;
  QCDDADevice *device;
  QCDDB *cddb;
  QCDDAMetaDataModel *cdda_metadata_model;

  void create_toolbar();
  QLabel *profileLabel;
  QComboBox *profileComboBox;
  QLabel *nullLabel;

  void create_statusbar();
  QLabel *statusLabel;
  QLabel *modeLabel;
  QLabel *cddbLabel;

  QAction *actionFetchCover;
  QAction *actionLoadCover;
  QAction *actionSaveCover;
  QAction *actionShowFullSizeCover;
  QAction *actionRemoveCover;

  QStringList check_metadata() const;

};

#endif
