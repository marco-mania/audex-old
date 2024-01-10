/* AUDEX CDDA EXTRACTOR
 * Copyright (C) 2007-2008 by Marco Nelles (marcomaniac@gmx.de)
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

MainWindow::MainWindow(QWidget *parent) : KXmlGuiWindow(parent) {

  cdda_model = new CDDAModel(this, Preferences::cdDevice());
  if (!cdda_model) {
    kDebug() << "Unable to create CDDAModel object. Low mem?";
    KMessageBox::detailedError(this, i18n("Unable to create CDDAModel object."), i18n("Internal error. Check your hardware. If all okay please make bug report."));
    return;
  }
  if (cdda_model->lastError().isValid()) {
    KMessageBox::detailedError(this, cdda_model->lastError().message(), cdda_model->lastError().details());
    return;
  }

  connect(cdda_model, SIGNAL(driveStatusChanged(const CDDAModel::DriveStatus)), this, SLOT(drive_status_changed(const CDDAModel::DriveStatus)));
  connect(cdda_model, SIGNAL(discStatusChanged(const CDDAModel::DiscStatus)), this, SLOT(disc_status_changed(const CDDAModel::DiscStatus)));
  connect(cdda_model, SIGNAL(discChanged(const CDDAModel::DiscType)), this, SLOT(disc_changed(const CDDAModel::DiscType)));
  connect(cdda_model, SIGNAL(discInfoChanged(const CDDAModel::DiscInfo)), this, SLOT(disc_info_changed(const CDDAModel::DiscInfo)));
  connect(cdda_model, SIGNAL(cddbLookupStarted()), this, SLOT(cddb_lookup_start()));
  connect(cdda_model, SIGNAL(cddbLookupDone(const bool)), this, SLOT(cddb_lookup_done(const bool)));
  connect(cdda_model, SIGNAL(cddbDataModified()), this, SLOT(enable_submit()));
  connect(cdda_model, SIGNAL(cddbDataModified()), this, SLOT(update_layout()));
  connect(cdda_model, SIGNAL(cddbDataSubmited(bool)), this, SLOT(enable_submit(bool)));

  profile_model = new ProfileModel(this);
  if (!profile_model) {
    kDebug() << "Unable to create ProfileModel object. Low mem?";
    KMessageBox::detailedError(this, i18n("Unable to create ProfileModel object."), i18n("Internal error. Check your hardware. If all okay please make bug report."));
    return;
  }
  if (profile_model->lastError().isValid()) {
    KMessageBox::detailedError(this, profile_model->lastError().message(), profile_model->lastError().details());
    return;
  }

  setup_actions();
  setup_layout();
  setupGUI();

  enable_layout(FALSE);

}

void MainWindow::startAssistant() {
  if (Preferences::firstStart()) {
    AssistantDialog *dialog = new AssistantDialog(profile_model, this);
    if (dialog->exec() == QDialog::Accepted) update();
    delete dialog;
    Preferences::setFirstStart(FALSE);
    Preferences::self()->writeConfig();
  }
}

MainWindow::~MainWindow() {
  delete profile_model;
  delete cdda_model;
}

void MainWindow::eject() {
  cdda_model->eject();
}

void MainWindow::cddb_lookup() {
  cdda_model->lookupCDDB();
}

void MainWindow::cddb_submit() {
  //cdda_model->submitCDDB();
}

void MainWindow::extract() {

  if (cdda_model->discInfo()==CDDAModel::DiscNoInfo) {

    if (KMessageBox::warningYesNo(this, i18n("No disc information set. Do you really want to continue?"),
				i18n("Disc information not found"),
				KStandardGuiItem::yes(),
				KStandardGuiItem::no(),
				i18n("no_disc_info_warn"))== KMessageBox::No) return;

  }

  cdda_model->setSelection(cdda_table_view->selectionModel()->selectedRows());

  ExtractingProgressDialog *dialog = new ExtractingProgressDialog(profile_model, cdda_model, this);

  dialog->exec();

  delete dialog;

}

void MainWindow::configure() {

  if (KConfigDialog::showDialog("settings")) return;

  KConfigDialog* dialog = new KConfigDialog(this, "settings", Preferences::self());

  KPageWidgetItem *generalPage = dialog->addPage(new generalSettingsWidget(), i18n("General settings"));
  generalPage->setIcon(KIcon(KApplication::windowIcon()));

  KPageWidgetItem *profilePage = dialog->addPage(new profileWidget(profile_model), i18n("Profiles"));
  profilePage->setIcon(KIcon("document-multiple"));

  KService::Ptr libkcddb = KService::serviceByDesktopName("libkcddb");
  if (libkcddb && libkcddb->isValid()) {
    KCModuleInfo info(libkcddb->entryPath());
    if (info.service()->isValid()) {
      KCModule *m = KCModuleLoader::loadModule(info, KCModuleLoader::Inline);
      if (m) {
        m->load();
        KCDDB::Config* cfg = new KCDDB::Config();
        cfg->readConfig();
        dialog->addPage(m, cfg, QString("CDDB settings"), "text-xmcd");
        connect(dialog, SIGNAL(okClicked()), m, SLOT(save()));
        connect(dialog, SIGNAL(applyClicked()), m, SLOT(save()));
        connect(dialog, SIGNAL(defaultClicked()), m, SLOT(defaults()));
      }
    }
  }

  KPageWidgetItem *ftpPage = dialog->addPage(new ftpSettingsWidget(), i18n("FTP"));
  ftpPage->setIcon(KIcon("network-server"));

  connect(dialog, SIGNAL(settingsChanged(const QString&)), this, SLOT(configuration_updated(const QString&)));

  dialog->exec();

}

void MainWindow::drive_status_changed(const CDDAModel::DriveStatus status) {
  switch (status) {
    case CDDAModel::DriveNoStatus :
      status_label->setText(i18n("No Status information available"));
      enable_layout(FALSE);
      break;
    case CDDAModel::DriveEmpty :
      status_label->setText(i18n("Audio disc in drive"));
      enable_layout(TRUE);
      break;
    case CDDAModel::DriveReady :
      status_label->setText(i18n("Audio disc in drive"));
      enable_layout(TRUE);
      break;
    case CDDAModel::DriveOpen :
      status_label->setText(i18n("Drive tray open"));
      enable_layout(FALSE);
      break;
    case CDDAModel::DriveNotReady :
      status_label->setText(i18n("Drive not ready"));
      enable_layout(FALSE);
      break;
    case CDDAModel::DriveError :
    status_label->setText(i18n("Drive error"));
      enable_layout(FALSE);
      break;
    default :
      break;
  }
}

void MainWindow::disc_status_changed(const CDDAModel::DiscStatus status) {
  Q_UNUSED(status);
}

void MainWindow::disc_changed(const CDDAModel::DiscType type) {
  Q_UNUSED(type);
  update_layout();
}

void MainWindow::disc_info_changed(const CDDAModel::DiscInfo info) {
  Q_UNUSED(info);
  update_layout();
}

void MainWindow::cddb_lookup_start() {
  statusBar()->addWidget(cddb_label);
  cddb_label->setText("<font color=red>"+i18n("Fetching CDDB information...")+"</font>");
}

void MainWindow::cddb_lookup_done(const bool successful) {
  if (!successful) {
    KMessageBox::detailedError(this, cdda_model->lastError().message(), cdda_model->lastError().details());
  }
  statusBar()->removeWidget(cddb_label);
  update_layout();
  disable_submit();
}

void MainWindow::update_layout() {
  cdda_table_view->resizeColumnsToContents();
  if (!cdda_model->isVarious()) {
    cdda_table_view->hideColumn(CDDA_MODEL_COLUMN_ARTIST_INDEX);
  } else {
    cdda_table_view->showColumn(CDDA_MODEL_COLUMN_ARTIST_INDEX);
  }
}

void MainWindow::enable_layout(bool enabled) {
  cdda_table_view->setEnabled(enabled);
  cdda_header_dock->setEnabled(enabled);
  cdda_header_widget->setEnabled(enabled);
  actionCollection()->action("profile")->setEnabled(enabled);
  profile_combobox->setEnabled(enabled);
  actionCollection()->action("fetch")->setEnabled(enabled);
  if (cdda_model->isModified())
    actionCollection()->action("submit")->setEnabled(enabled);
  else
    actionCollection()->action("submit")->setEnabled(FALSE);
  actionCollection()->action("extract")->setEnabled(enabled);
  actionCollection()->action("splittitles")->setEnabled(enabled);
  actionCollection()->action("swapartistsandtitles")->setEnabled(enabled);
  actionCollection()->action("capitalize")->setEnabled(enabled);
  actionCollection()->action("autofillartists")->setEnabled(enabled);
}

void MainWindow::enable_submit(bool enabled) {
  actionCollection()->action("submit")->setEnabled(enabled);
}

void MainWindow::disable_submit() {
  actionCollection()->action("submit")->setEnabled(FALSE);
}

void MainWindow::configuration_updated(const QString& dialog_name) {
  Q_UNUSED(dialog_name);
  Preferences::self()->writeConfig();
}

void MainWindow::profile_updated() {
  KConfig config;
  KConfigGroup uicg(&config, "Profiles");
  uicg.writeEntry("Standard", profile_combobox->currentIndex());
  profile_model->setCurrentProfileRow(profile_combobox->currentIndex());
}

void MainWindow::split_titles() {

  bool ok;
  QString divider = QInputDialog::getText(this, i18n("Split titles"),
                                          i18n("Please set a divider string. Be aware of empty spaces.\n\nDivider:"), QLineEdit::Normal,
                                          " - ", &ok);
  if (ok && !divider.isEmpty()) {
    cdda_model->splitTitleOfTracks(divider);
  }

}

void MainWindow::swap_artists_and_titles() {

  if (KMessageBox::warningYesNo(this, i18n("Do you really want to swap all artists and titles?"),
				i18n("Swap artists and titles"),
				KStandardGuiItem::yes(),
				KStandardGuiItem::no(),
				i18n("no_swap_artists_and_titles_warn"))== KMessageBox::No) return;

  cdda_model->swapArtistAndTitle();
  cdda_model->swapArtistAndTitleOfTracks();

}

void MainWindow::capitalize() {

  if (KMessageBox::warningYesNo(this, i18n("Do you really want to capitalize all artists and titles?"),
				i18n("Capitalize artists and titles"),
				KStandardGuiItem::yes(),
				KStandardGuiItem::no(),
				i18n("no_capitalize_warn"))== KMessageBox::No) return;

  cdda_model->capitalizeHeader();
  cdda_model->capitalizeTracks();

}

void MainWindow::auto_fill_artists() {

  if (KMessageBox::warningYesNo(this, i18n("Do you really want to autofill track artists?"),
				i18n("Autofill artists"),
				KStandardGuiItem::yes(),
				KStandardGuiItem::no(),
				i18n("no_autofill_warn"))== KMessageBox::No) return;

  cdda_model->setTitleArtistsFromHeader();

}

void MainWindow::setup_actions() {

  KAction* ejectAction = new KAction(this);
  ejectAction->setText(i18n("Eject"));
  ejectAction->setIcon(KIcon("media-eject"));
  ejectAction->setShortcut(Qt::CTRL + Qt::Key_E);
  actionCollection()->addAction("eject", ejectAction);
  connect(ejectAction, SIGNAL(triggered(bool)), this, SLOT(eject()));

  profile_label = new QLabel(this);
  profile_label->setText(i18n("Profile: "));
  profile_combobox = new QComboBox(this);
  profile_combobox->setModel(profile_model);
  profile_combobox->setModelColumn(0);
  profile_combobox->setMinimumWidth(80);
  if (profile_model->rowCount() > 0) {
    KConfig config;
    KConfigGroup uicg(&config, "Profiles");
    int row = uicg.readEntry("Standard", 0);
    if ((row < 0) || (row >= profile_model->rowCount())) row = 0;
    profile_combobox->setCurrentIndex(row);
    profile_model->setCurrentProfileRow(row);
  }
  connect(profile_combobox, SIGNAL(currentIndexChanged(int)), this, SLOT(profile_updated()));

  KAction *plabelAction = new KAction(this);
  plabelAction->setText(i18n("&Profile: "));
  plabelAction->setDefaultWidget(profile_label);
  profile_label->setBuddy(profile_combobox);
  actionCollection()->addAction("profile_label", plabelAction);

  KAction *profileAction = new KAction(this);
  profileAction->setText(i18n("Profile"));
  profileAction->setShortcut(Qt::Key_F6);
  profileAction->setDefaultWidget(profile_combobox);
  profileAction->setShortcutConfigurable(FALSE);
  actionCollection()->addAction("profile", profileAction);

  KAction* cddbLookupAction = new KAction(this);
  cddbLookupAction->setText(i18n("Fetch Info"));
  cddbLookupAction->setIcon(KIcon("view-list-text"));
  cddbLookupAction->setShortcut(Qt::CTRL + Qt::Key_F);
  actionCollection()->addAction("fetch", cddbLookupAction);
  connect(cddbLookupAction, SIGNAL(triggered(bool)), this, SLOT(cddb_lookup()));

  KAction* cddbSubmitAction = new KAction(this);
  cddbSubmitAction->setText(i18n("Submit Info"));
  cddbSubmitAction->setShortcut(Qt::CTRL + Qt::Key_S);
  actionCollection()->addAction("submit", cddbSubmitAction);
  connect(cddbSubmitAction, SIGNAL(triggered(bool)), this, SLOT(cddb_submit()));

  KAction* extractAction = new KAction(this);
  extractAction->setText(i18n("Extract CDDA..."));
  extractAction->setIcon(KIcon("media-optical-audio"));
  extractAction->setShortcut(Qt::CTRL + Qt::Key_X);
  actionCollection()->addAction("extract", extractAction);
  connect(extractAction, SIGNAL(triggered(bool)), this, SLOT(extract()));

  KAction* configureAction = new KAction(this);
  configureAction->setText(i18n("Configure..."));
  configureAction->setIcon(KIcon("configure"));
  configureAction->setShortcut(Qt::CTRL + Qt::Key_C);
  actionCollection()->addAction("configure", configureAction);
  connect(configureAction, SIGNAL(triggered(bool)), this, SLOT(configure()));

  KAction* splitTitlesAction = new KAction(this);
  splitTitlesAction->setText(i18n("Split titles..."));
  actionCollection()->addAction("splittitles", splitTitlesAction);
  connect(splitTitlesAction, SIGNAL(triggered(bool)), this, SLOT(split_titles()));

  KAction* swapArtistsAndTitlesAction = new KAction(this);
  swapArtistsAndTitlesAction->setText(i18n("Swap artists and titles"));
  actionCollection()->addAction("swapartistsandtitles", swapArtistsAndTitlesAction);
  connect(swapArtistsAndTitlesAction, SIGNAL(triggered(bool)), this, SLOT(swap_artists_and_titles()));

  KAction* capitalizeAction = new KAction(this);
  capitalizeAction->setText(i18n("Capitalize"));
  actionCollection()->addAction("capitalize", capitalizeAction);
  connect(capitalizeAction, SIGNAL(triggered(bool)), this, SLOT(capitalize()));

  KAction* autoFillArtistsAction = new KAction(this);
  autoFillArtistsAction->setText(i18n("Auto fill artists"));
  actionCollection()->addAction("autofillartists", autoFillArtistsAction);
  connect(autoFillArtistsAction, SIGNAL(triggered(bool)), this, SLOT(auto_fill_artists()));

  KStandardAction::quit(kapp, SLOT(quit()), actionCollection());

}

void MainWindow::setup_layout() {

  cdda_table_view = new QTableView(this);
  cdda_table_view->setModel(cdda_model);
  cdda_table_view->setAlternatingRowColors(TRUE);
  cdda_table_view->setSelectionBehavior(QAbstractItemView::SelectRows);
  cdda_table_view->setEditTriggers(QAbstractItemView::EditKeyPressed | QAbstractItemView::DoubleClicked);

  cdda_header_dock = new QDockWidget(this);
  cdda_header_dock->setObjectName("cdda_header_dock");
  cdda_header_dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
  cdda_header_dock->setAllowedAreas(Qt::AllDockWidgetAreas);

  setCentralWidget(cdda_table_view);
  cdda_header_widget = new CDDAHeaderWidget(cdda_model, cdda_header_dock);
  connect(cdda_header_widget, SIGNAL(headerDataChanged()), this, SLOT(update_layout()));
  cdda_header_dock->setWidget(cdda_header_widget);
  addDockWidget(Qt::TopDockWidgetArea, cdda_header_dock);

  status_label = new QLabel();
  status_label->setAlignment(Qt::AlignHCenter);
  status_label->setMinimumSize(QSize(120, 10));
  statusBar()->addWidget(status_label);
  cddb_label = new QLabel();
  cddb_label->setAlignment(Qt::AlignHCenter);
  cddb_label->setMinimumSize(QSize(100, 10));

}
