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

#include "profiledatadialog.h"

ProfileDataDialog::ProfileDataDialog(ProfileModel *profileModel, const int profileRow, QWidget *parent) : KDialog(parent) {

  Q_UNUSED(parent);

  profile_model = profileModel;
  if (!profile_model) {
    kDebug() << "ProfileModel is NULL!";
    return;
  }
  profile_row = profileRow;

  QWidget *widget = new QWidget(this);
  ui.setupUi(widget);

  setMainWidget(widget);

  ui.kpushbutton_mask->setIcon(KIcon("tools-wizard"));
  ui.kpushbutton_command->setIcon(KIcon("tools-wizard"));

  connect(ui.kpushbutton_mask, SIGNAL(clicked()), this, SLOT(mask_wizard()));
  connect(ui.kpushbutton_command, SIGNAL(clicked()), this, SLOT(command_wizard()));

  connect(ui.kpushbutton_cover, SIGNAL(clicked()), this, SLOT(cover_settings()));
  connect(ui.kpushbutton_playlist, SIGNAL(clicked()), this, SLOT(playlist_settings()));
  connect(ui.kpushbutton_info, SIGNAL(clicked()), this, SLOT(info_settings()));
  connect(ui.kpushbutton_hashlist, SIGNAL(clicked()), this, SLOT(hashlist_settings()));

  connect(ui.checkBox_cover, SIGNAL(toggled(bool)), this, SLOT(enable_settings_cover(bool)));
  connect(ui.checkBox_playlist, SIGNAL(toggled(bool)), this, SLOT(enable_settings_playlist(bool)));
  connect(ui.checkBox_info, SIGNAL(toggled(bool)), this, SLOT(enable_settings_info(bool)));
  connect(ui.checkBox_hashlist, SIGNAL(toggled(bool)), this, SLOT(enable_settings_hashlist(bool)));

  if (profile_row >= 0) {

    setCaption(i18n("Modify profile"));

    setButtons(KDialog::Ok | KDialog::Cancel | KDialog::Apply);

    ui.klineedit_name->setText(profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_NAME_INDEX)).toString());
    connect(ui.klineedit_name, SIGNAL(textEdited(const QString&)), this, SLOT(trigger_changed()));
    ui.klineedit_name->setCursorPosition(0);

    ui.klineedit_mask->setText(profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_MASK_INDEX)).toString());
    connect(ui.klineedit_mask, SIGNAL(textEdited(const QString&)), this, SLOT(trigger_changed()));
    ui.klineedit_mask->setCursorPosition(0);

    ui.klineedit_command->setText(profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_COMMAND_INDEX)).toString());
    connect(ui.klineedit_command, SIGNAL(textEdited(const QString&)), this, SLOT(trigger_changed()));
    ui.klineedit_command->setCursorPosition(0);

    ui.klineedit_suffix->setText(profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_SUFFIX_INDEX)).toString());
    connect(ui.klineedit_suffix, SIGNAL(textEdited(const QString&)), this, SLOT(trigger_changed()));

    ui.kintspinbox_profileindex->setValue(profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_PROFILEINDEX_INDEX)).toInt());
    connect(ui.kintspinbox_profileindex, SIGNAL(valueChanged(int)), this, SLOT(trigger_changed()));

    ui.checkBox_fat32compatible->setChecked(profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_FAT32COMPATIBLE_INDEX)).toBool());
    connect(ui.checkBox_fat32compatible, SIGNAL(toggled(bool)), this, SLOT(trigger_changed()));

    ui.checkBox_cover->setChecked(profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_SC_INDEX)).toBool());
    enable_settings_cover(ui.checkBox_cover->isChecked());
    connect(ui.checkBox_cover, SIGNAL(toggled(bool)), this, SLOT(trigger_changed()));

    ui.checkBox_playlist->setChecked(profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_PL_INDEX)).toBool());
    enable_settings_playlist(ui.checkBox_playlist->isChecked());
    connect(ui.checkBox_playlist, SIGNAL(toggled(bool)), this, SLOT(trigger_changed()));

    ui.checkBox_info->setChecked(profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_INF_INDEX)).toBool());
    enable_settings_info(ui.checkBox_info->isChecked());
    connect(ui.checkBox_info, SIGNAL(toggled(bool)), this, SLOT(trigger_changed()));

    pdcd_scale = profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_SC_SCALE_INDEX)).toBool();
    pdcd_size = profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_SC_SIZE_INDEX)).toSize();
    pdcd_format = profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_SC_FORMAT_INDEX)).toString();
    pdcd_mask = profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_SC_NAME_INDEX)).toString();

    pdpd_format = profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_PL_FORMAT_INDEX)).toString();
    pdpd_mask = profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_PL_NAME_INDEX)).toString();

    pdid_text = profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_INF_TEXT_INDEX)).toStringList();
    pdid_mask = profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_INF_NAME_INDEX)).toString();
    pdid_suffix = profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_INF_SUFFIX_INDEX)).toString();

    pdhd_format = profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_HL_FORMAT_INDEX)).toString();
    pdhd_mask = profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_HL_NAME_INDEX)).toString();

    ui.checkBox_hashlist->setChecked(profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_HL_INDEX)).toBool());
    enable_settings_hashlist(ui.checkBox_hashlist->isChecked());
    connect(ui.checkBox_hashlist, SIGNAL(toggled(bool)), this, SLOT(trigger_changed()));

    ui.checkBox_discid->setChecked(profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_DISCID_INDEX)).toBool());
    connect(ui.checkBox_discid, SIGNAL(toggled(bool)), this, SLOT(trigger_changed()));

    enableButtonApply(FALSE);

  } else {

    setCaption(i18n("Add new profile"));

    setButtons(KDialog::Ok | KDialog::Cancel);

    ui.kintspinbox_profileindex->setValue(-1);

    ui.klineedit_name->setText("");
    ui.klineedit_mask->setText("");
    ui.klineedit_command->setText("");
    ui.klineedit_suffix->setText("");
    ui.kintspinbox_profileindex->setValue(-1);
    ui.checkBox_fat32compatible->setChecked(FALSE);
    ui.checkBox_cover->setChecked(FALSE);
    ui.checkBox_playlist->setChecked(FALSE);
    ui.checkBox_info->setChecked(FALSE);
    pdcd_scale = FALSE;
    pdcd_size = QSize(300, 300);
    pdcd_format = "JPG";
    pdcd_mask = "cover";
    pdpd_format = "M3U";
    pdpd_mask = "playlist";
    pdid_text.clear();
    pdid_mask = "info";
    pdid_suffix = "nfo";
    pdhd_format = "SFV";
    pdhd_mask = "checksums";
    ui.checkBox_hashlist->setChecked(FALSE);
    ui.checkBox_discid->setChecked(FALSE);

    enable_settings_cover(FALSE);
    enable_settings_playlist(FALSE);
    enable_settings_info(FALSE);
    enable_settings_hashlist(FALSE);

  }

}

ProfileDataDialog::~ProfileDataDialog() {

}

void ProfileDataDialog::slotButtonClicked(int button) {
  if (button == KDialog::Ok) {
    if (save()) {
      accept();
    } else {
      KMessageBox::detailedError(this, profile_model->lastError().message(), profile_model->lastError().details());
    }
  } else if (button == KDialog::Apply) {
    if (!save()) {
      KMessageBox::detailedError(this, profile_model->lastError().message(), profile_model->lastError().details());
    }
  } else {
    KDialog::slotButtonClicked(button);
  }
}

void ProfileDataDialog::trigger_changed() {
  if (ui.klineedit_name->text() != profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_NAME_INDEX)).toString()) { enableButtonApply(TRUE); return; }
  if (ui.klineedit_mask->text() != profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_MASK_INDEX)).toString()) { enableButtonApply(TRUE); return; }
  if (ui.klineedit_command->text() != profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_COMMAND_INDEX)).toString()) { enableButtonApply(TRUE); return; }
  if (ui.klineedit_suffix->text() != profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_SUFFIX_INDEX)).toString()) { enableButtonApply(TRUE); return; }
  if (ui.kintspinbox_profileindex->value() != profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_PROFILEINDEX_INDEX)).toInt()) { enableButtonApply(TRUE); return; }
  if (ui.checkBox_fat32compatible->isChecked() != profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_FAT32COMPATIBLE_INDEX)).toBool()) { enableButtonApply(TRUE); return; }
  if (ui.checkBox_cover->isChecked() != profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_SC_INDEX)).toBool()) { enableButtonApply(TRUE); return; }
  if (ui.checkBox_playlist->isChecked() != profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_PL_INDEX)).toBool()) { enableButtonApply(TRUE); return; }
  if (ui.checkBox_info->isChecked() != profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_INF_INDEX)).toBool()) { enableButtonApply(TRUE); return; }
  if (pdcd_scale != profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_SC_SCALE_INDEX)).toBool()) { enableButtonApply(TRUE); return; }
  if (pdcd_size != profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_SC_SIZE_INDEX)).toSize()) { enableButtonApply(TRUE); return; }
  if (pdcd_format != profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_SC_FORMAT_INDEX)).toString()) { enableButtonApply(TRUE); return; }
  if (pdcd_mask != profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_SC_NAME_INDEX)).toString()) { enableButtonApply(TRUE); return; }
  if (pdpd_format != profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_PL_FORMAT_INDEX)).toString()) { enableButtonApply(TRUE); return; }
  if (pdpd_mask != profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_PL_NAME_INDEX)).toString()) { enableButtonApply(TRUE); return; }
  if (pdid_text != profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_INF_TEXT_INDEX)).toStringList()) { enableButtonApply(TRUE); return; }
  if (pdid_mask != profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_INF_NAME_INDEX)).toString()) { enableButtonApply(TRUE); return; }
  if (pdid_suffix != profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_INF_SUFFIX_INDEX)).toString()) { enableButtonApply(TRUE); return; }
  if (ui.checkBox_hashlist->isChecked() != profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_HL_INDEX)).toBool()) { enableButtonApply(TRUE); return; }
  if (pdhd_format != profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_HL_FORMAT_INDEX)).toString()) { enableButtonApply(TRUE); return; }
  if (pdhd_mask != profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_HL_NAME_INDEX)).toString()) { enableButtonApply(TRUE); return; }
  if (ui.checkBox_discid->isChecked() != profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_DISCID_INDEX)).toBool()) { enableButtonApply(TRUE); return; }
  enableButtonApply(FALSE);
}

void ProfileDataDialog::enable_settings_cover(bool enabled) {
  ui.kpushbutton_cover->setEnabled(enabled);
}

void ProfileDataDialog::enable_settings_playlist(bool enabled) {
  ui.kpushbutton_playlist->setEnabled(enabled);
}

void ProfileDataDialog::enable_settings_info(bool enabled) {
  ui.kpushbutton_info->setEnabled(enabled);
}

void ProfileDataDialog::enable_settings_hashlist(bool enabled) {
  ui.kpushbutton_hashlist->setEnabled(enabled);
}

void ProfileDataDialog::mask_wizard() {

  MaskWizardDialog *dialog = new MaskWizardDialog(ui.klineedit_mask->text(), this);

  if (dialog->exec() != QDialog::Accepted) { delete dialog; return; }

  ui.klineedit_mask->setText(dialog->mask);

  delete dialog;

  trigger_changed();

}

void ProfileDataDialog::command_wizard() {

  CommandWizardDialog *dialog = new CommandWizardDialog(ui.klineedit_command->text(), this);

  if (dialog->exec() != QDialog::Accepted) { delete dialog; return; }

  ui.klineedit_command->setText(dialog->command);

  delete dialog;

  trigger_changed();

}

void ProfileDataDialog::cover_settings() {

  ProfileDataCoverDialog *dialog = new ProfileDataCoverDialog(pdcd_scale, pdcd_size, pdcd_format, pdcd_mask, this);

  if (dialog->exec() != QDialog::Accepted) { delete dialog; return; }

  pdcd_scale = dialog->scale;
  pdcd_size = dialog->size;
  pdcd_format = dialog->format;
  pdcd_mask = dialog->mask;

  delete dialog;

  trigger_changed();

}

void ProfileDataDialog::playlist_settings() {

  ProfileDataPlaylistDialog *dialog = new ProfileDataPlaylistDialog(pdpd_format, pdpd_mask, this);

  if (dialog->exec() != QDialog::Accepted) { delete dialog; return; }

  pdpd_format = dialog->format;
  pdpd_mask = dialog->mask;

  delete dialog;

  trigger_changed();

}

void ProfileDataDialog::info_settings() {

  ProfileDataInfoDialog *dialog = new ProfileDataInfoDialog(pdid_text, pdid_mask, pdid_suffix, this);

  if (dialog->exec() != QDialog::Accepted) { delete dialog; return; }

  pdid_text = dialog->text;
  pdid_mask = dialog->mask;
  pdid_suffix = dialog->suffix;

  delete dialog;

  trigger_changed();

}

void ProfileDataDialog::hashlist_settings() {

  ProfileDataHashlistDialog *dialog = new ProfileDataHashlistDialog(pdhd_mask, pdhd_format, this);

  if (dialog->exec() != QDialog::Accepted) { delete dialog; return; }

  pdhd_format = dialog->format;
  pdhd_mask = dialog->mask;

  delete dialog;

  trigger_changed();

}

bool ProfileDataDialog::save() {

  int row;
  if (profile_row < 0) {
    row = profile_model->rowCount();
    profile_model->insertRows(row, 1);
  } else {
    row = profile_row;
  }

  bool success = TRUE;
  success = profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_NAME_INDEX), ui.klineedit_name->text());
  if (success) success = profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_MASK_INDEX), ui.klineedit_mask->text());
  if (success) success = profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_COMMAND_INDEX), ui.klineedit_command->text());
  if (success) success = profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_SUFFIX_INDEX), ui.klineedit_suffix->text());
  if (success) success = profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_PROFILEINDEX_INDEX), ui.kintspinbox_profileindex->value());
  if (success) success = profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_FAT32COMPATIBLE_INDEX), ui.checkBox_fat32compatible->isChecked());
  if (success) success = profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_SC_INDEX), ui.checkBox_cover->isChecked());
  if (success) success = profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_SC_SCALE_INDEX), pdcd_scale);
  if (success) success = profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_SC_SIZE_INDEX), pdcd_size);
  if (success) success = profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_SC_FORMAT_INDEX), pdcd_format);
  if (success) success = profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_SC_NAME_INDEX), pdcd_mask);
  if (success) success = profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_PL_INDEX), ui.checkBox_playlist->isChecked());
  if (success) success = profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_INF_INDEX), ui.checkBox_info->isChecked());
  if (success) success = profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_PL_FORMAT_INDEX), pdpd_format);
  if (success) success = profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_PL_NAME_INDEX), pdpd_mask);
  if (success) success = profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_INF_TEXT_INDEX), pdid_text);
  if (success) success = profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_INF_NAME_INDEX), pdid_mask);
  if (success) success = profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_INF_SUFFIX_INDEX), pdid_suffix);
  if (success) success = profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_HL_INDEX), ui.checkBox_hashlist->isChecked());
  if (success) success = profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_DISCID_INDEX), ui.checkBox_discid->isChecked());
  if (success) success = profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_HL_FORMAT_INDEX), pdhd_format);
  if (success) success = profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_HL_NAME_INDEX), pdhd_mask);
  if (!success) {
    if (profile_row < 0) profile_model->removeRows(row, 1);
    return FALSE;
  }

  profile_model->commit();

  if (profile_row >= 0) enableButtonApply(FALSE);

  return TRUE;

}
