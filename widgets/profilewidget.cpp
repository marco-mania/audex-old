/* ADEX CDDA EXTRACTOR
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

#include "profilewidget.h"

profileWidget::profileWidget(ProfileModel *profileModel, QWidget *parent) : profileWidgetUI(parent) {

  profile_model = profileModel;
  if (!profile_model) {
    kDebug() << "ProfileModel is NULL!";
    return;
  }

  listView->setModel(profile_model);
  listView->setModelColumn(0);
  connect(listView->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(update_profile(const QModelIndex&, const QModelIndex&)));
  connect(listView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(update()));
  connect(listView, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(mod_profile()));
  connect(kpushbutton_add, SIGNAL(clicked()), this, SLOT(add_profile()));
  connect(kpushbutton_rem, SIGNAL(clicked()), this, SLOT(rem_profile()));
  connect(kpushbutton_mod, SIGNAL(clicked()), this, SLOT(mod_profile()));
  connect(kpushbutton_copy, SIGNAL(clicked()), this, SLOT(copy_profile()));
  connect(kpushbutton_wizard, SIGNAL(clicked()), this, SLOT(assistant()));
  connect(kpushbutton_load, SIGNAL(clicked()), this, SLOT(load_profiles()));
  connect(kpushbutton_save, SIGNAL(clicked()), this, SLOT(save_profiles()));

  kpushbutton_add->setIcon(KIcon("list-add"));
  kpushbutton_rem->setIcon(KIcon("list-remove"));

  kpushbutton_wizard->setIcon(KIcon("tools-wizard"));

  kpushbutton_load->setIcon(KIcon("document-open"));
  kpushbutton_save->setIcon(KIcon("document-save"));

  if (profile_model->rowCount()>0)
    listView->setCurrentIndex(profile_model->index(0, 0));

  update();

}

profileWidget::~profileWidget() {

}

void profileWidget::update_profile(const QModelIndex& current, const QModelIndex& previous) {
  Q_UNUSED(previous);
  profile_model->setCurrentProfileRow(current.row());
}

void profileWidget::update() {
  kpushbutton_rem->setEnabled(listView->selectionModel()->selectedIndexes().count()>0);
  kpushbutton_mod->setEnabled(listView->selectionModel()->selectedIndexes().count()>0);
  kpushbutton_copy->setEnabled(listView->selectionModel()->selectedIndexes().count()>0);
  kpushbutton_save->setEnabled(profile_model->rowCount()>0);
}

void profileWidget::add_profile() {

  ProfileDataDialog *dialog = new ProfileDataDialog(profile_model, -1, this);

  if (dialog->exec() != QDialog::Accepted) { delete dialog; return; }
  delete dialog;

  profile_model->commit();

  listView->setCurrentIndex(profile_model->index(profile_model->rowCount(), 0));

  update();

}

void profileWidget::rem_profile() {

  if (KMessageBox::warningYesNo(this, i18n("Do you really want to delete profile \"%1\"?", profile_model->data(profile_model->index(listView->currentIndex().row(), PROFILE_MODEL_COLUMN_NAME_INDEX)).toString()),
				i18n("Delete profile"),
				KStandardGuiItem::yes(),
				KStandardGuiItem::no())== KMessageBox::No) return;

  int rs = listView->currentIndex().row();
  profile_model->removeRows(rs, 1);

  profile_model->commit();

  if (rs>=profile_model->rowCount()) rs = profile_model->rowCount()-1;
  if (profile_model->rowCount()>0)
    listView->setCurrentIndex(profile_model->index(rs, 0));

  update();

}

void profileWidget::mod_profile() {

  ProfileDataDialog *dialog = new ProfileDataDialog(profile_model, profile_model->currentProfileRow(), this);

  if (dialog->exec() == QDialog::Accepted) update_profile(profile_model->index(profile_model->currentProfileRow(), 0), QModelIndex());

  delete dialog;

  profile_model->commit();

  update();

}

void profileWidget::copy_profile() {
  int rs = profile_model->rowCount();
  profile_model->copy(listView->currentIndex().row());
  profile_model->commit();
  listView->setCurrentIndex(profile_model->index(rs, 0));
  update();
}

void profileWidget::assistant() {
  AssistantDialog *dialog = new AssistantDialog(profile_model, this);
  if (dialog->exec() == QDialog::Accepted) update();
  delete dialog;
}

void profileWidget::save_profiles() {
  QString filename = KFileDialog::getSaveFileName(KUrl(QDir::homePath()), "*.apf", this, i18n("Save cover"));
  if (!filename.isEmpty()) {
    profile_model->saveProfilesToFile(filename);
  }
}

void profileWidget::load_profiles() {
  QString filename = KFileDialog::getOpenFileName(KUrl(QDir::homePath()), "*.apf", this, i18n("Load profiles"));
  if (!filename.isEmpty()) {
    profile_model->loadProfilesFromFile(filename);
  }
}
