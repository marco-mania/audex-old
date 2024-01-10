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

#include "addprofiledialog.h"

addprofiledialog::addprofiledialog(QWidget *parent, QSettings *settings, QProfileModel *profile_model, int profile_index) {

  Q_UNUSED(parent);

  setupUi(this);

  this->settings = settings;
  this->profile_model = profile_model;
  this->profile_index = profile_index;

  connect(buttonBox, SIGNAL(accepted()), this, SLOT(ok()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(cancel()));
  connect(toolButton, SIGNAL(clicked()), this, SLOT(open_wizard_dialog()));
  connect(toolButton_2, SIGNAL(clicked()), this, SLOT(open_command_wizard_dialog()));
  connect(pushButton, SIGNAL(clicked()), this, SLOT(open_store_cover_dialog()));
  connect(pushButton_2, SIGNAL(clicked()), this, SLOT(open_store_playlist_dialog()));
  connect(checkBox_3, SIGNAL(toggled(bool)), this, SLOT(enable_open_store_playlist_dialog_button(bool)));
  connect(checkBox_2, SIGNAL(toggled(bool)), this, SLOT(enable_open_store_cover_dialog_button(bool)));

  to_add = (profile_index<0);

  if (!to_add) {
    lineEdit->setText(profile_model->data(profile_model->index(profile_index, 0)).toString());
    lineEdit_2->setText(profile_model->data(profile_model->index(profile_index, 1)).toString());
    lineEdit_3->setText(profile_model->data(profile_model->index(profile_index, 2)).toString());
    lineEdit_4->setText(profile_model->data(profile_model->index(profile_index, 3)).toString());
    lineEdit_5->setText(profile_model->data(profile_model->index(profile_index, 4)).toString());
    lineEdit_6->setText(profile_model->data(profile_model->index(profile_index, 5)).toString());
    spinBox->setValue(profile_model->data(profile_model->index(profile_index, 6)).toInt());
    checkBox->setChecked(profile_model->data(profile_model->index(profile_index, 7)).toBool());
    checkBox_2->setChecked(profile_model->data(profile_model->index(profile_index, 8)).toBool());
    enable_open_store_cover_dialog_button(checkBox_2->isChecked());
    checkBox_3->setChecked(profile_model->data(profile_model->index(profile_index, 13)).toBool());
    enable_open_store_playlist_dialog_button(checkBox_3->isChecked());
  }

  scale = profile_model->data(profile_model->index(profile_index, Q_PROFILE_MODEL_COLUMN_SC_SCALE_INDEX)).toBool();
  _size = profile_model->data(profile_model->index(profile_index, Q_PROFILE_MODEL_COLUMN_SC_SIZE_INDEX)).toSize();
  format = profile_model->data(profile_model->index(profile_index, Q_PROFILE_MODEL_COLUMN_SC_FORMAT_INDEX)).toString();
  name = profile_model->data(profile_model->index(profile_index, Q_PROFILE_MODEL_COLUMN_SC_NAME_INDEX)).toString();

  pl_format = profile_model->data(profile_model->index(profile_index, Q_PROFILE_MODEL_COLUMN_PL_FORMAT_INDEX)).toString();
  pl_name = profile_model->data(profile_model->index(profile_index, Q_PROFILE_MODEL_COLUMN_PL_NAME_INDEX)).toString();

  int x = (parent->size().width()-(size().width()))/2;
  int y = (parent->size().height()-(size().height()))/2;

  resize(settings->value("addprofiledialog/size", QSize(455, 393)).toSize());
  move(settings->value("addprofiledialog/pos", QPoint(parent->pos().x()+x, parent->pos().y()+y)).toPoint());

  label_6->setEnabled(settings->value("settings/pg_enabled", false).toBool());
  spinBox->setEnabled(settings->value("settings/pg_enabled", false).toBool());

}

addprofiledialog::~addprofiledialog() {
}

void addprofiledialog::ok() {
  if (submit()) close();
}

void addprofiledialog::cancel() {
  close();
}

void addprofiledialog::open_wizard_dialog() {
  QString mask = lineEdit_2->text();
  maskwizarddialog *maskWizardDialog = new maskwizarddialog(this, settings, profile_model, &mask);
  if (maskWizardDialog->exec()==QDialog::Accepted) lineEdit_2->setText(mask);
  delete maskWizardDialog;
}

void addprofiledialog::open_command_wizard_dialog() {
  QString mask = lineEdit_5->text();
  commandmaskwizarddialog *commandMaskWizardDialog = new commandmaskwizarddialog(this, settings, profile_model, &mask);
  if (commandMaskWizardDialog->exec()==QDialog::Accepted) lineEdit_5->setText(mask);
  delete commandMaskWizardDialog;
}

void addprofiledialog::open_store_cover_dialog() {
  storecoverdialog *storeCoverDialog = new storecoverdialog(this, settings, profile_model, &scale, &_size, &format, &name);
  storeCoverDialog->exec();
  delete storeCoverDialog;
}

void addprofiledialog::open_store_playlist_dialog() {
  createplaylistdialog *createPlaylistDialog = new createplaylistdialog(this, settings, profile_model, &pl_format, &pl_name);
  createPlaylistDialog->exec();
  delete createPlaylistDialog;
}

void addprofiledialog::enable_open_store_playlist_dialog_button(bool enable) {
  pushButton_2->setEnabled(enable);
}

void addprofiledialog::enable_open_store_cover_dialog_button(bool enable) {
  pushButton->setEnabled(enable);
}

bool addprofiledialog::submit() {
  int row;
  if (to_add) {
    row = profile_model->rowCount();
    profile_model->insertRows(row, 1);
  } else {
    row = profile_index;
  }
  bool success = TRUE;
  success = profile_model->setData(profile_model->index(row, 0), lineEdit->text());
  if (success) success = profile_model->setData(profile_model->index(row, 1), lineEdit_2->text());
  if (success) success = profile_model->setData(profile_model->index(row, 2), lineEdit_3->text());
  if (success) success = profile_model->setData(profile_model->index(row, 3), lineEdit_4->text());
  if (success) success = profile_model->setData(profile_model->index(row, 4), lineEdit_5->text());
  if (success) success = profile_model->setData(profile_model->index(row, 5), lineEdit_6->text());
  if (success) success = profile_model->setData(profile_model->index(row, 6), spinBox->value());
  if (success) success = profile_model->setData(profile_model->index(row, 7), checkBox->isChecked());
  if (success) success = profile_model->setData(profile_model->index(row, 8), checkBox_2->isChecked());
  if (success) success = profile_model->setData(profile_model->index(row, 9), scale);
  if (success) success = profile_model->setData(profile_model->index(row, 10), _size);
  if (success) success = profile_model->setData(profile_model->index(row, 11), format);
  if (success) success = profile_model->setData(profile_model->index(row, 12), name);
  if (success) success = profile_model->setData(profile_model->index(row, 13), checkBox_3->isChecked());
  if (success) success = profile_model->setData(profile_model->index(row, 14), pl_format);
  if (success) success = profile_model->setData(profile_model->index(row, 15), pl_name);
  if (!success) {
    if (to_add) profile_model->removeRows(row, 1);
    return FALSE;
  }
  profile_model->submit();
  return TRUE;
}

void addprofiledialog::closeEvent(QCloseEvent *event) {
  settings->setValue("addprofiledialog/pos", pos());
  settings->setValue("addprofiledialog/size", size());
  event->accept();
}
