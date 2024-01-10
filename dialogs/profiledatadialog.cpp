/* AUDEX CDDA EXTRACTOR
 * Copyright (C) 2007-2009 Marco Nelles (audex@maniatek.de)
 * <http://opensource.maniatek.de/audex>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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

  if (profile_row >= 0) {

    lame_parameters.fromString(profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_ENCODER_LAME_PARAMETERS_INDEX)).toString());
    oggenc_parameters.fromString(profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_ENCODER_OGGENC_PARAMETERS_INDEX)).toString());
    flac_parameters.fromString(profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_ENCODER_FLAC_PARAMETERS_INDEX)).toString());
    faac_parameters.fromString(profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_ENCODER_FAAC_PARAMETERS_INDEX)).toString());
    wave_parameters.fromString(profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_ENCODER_WAVE_PARAMETERS_INDEX)).toString());
    custom_parameters.fromString(profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_ENCODER_CUSTOM_PARAMETERS_INDEX)).toString());

  }

  lame_widget = new lameWidget(&lame_parameters, this);
  connect(lame_widget, SIGNAL(triggerChanged()), this, SLOT(trigger_changed()));
  oggenc_widget = new oggencWidget(&oggenc_parameters, this);
  connect(oggenc_widget, SIGNAL(triggerChanged()), this, SLOT(trigger_changed()));
  flac_widget = new flacWidget(&flac_parameters, this);
  connect(flac_widget, SIGNAL(triggerChanged()), this, SLOT(trigger_changed()));
  faac_widget = new faacWidget(&faac_parameters, this);
  connect(faac_widget, SIGNAL(triggerChanged()), this, SLOT(trigger_changed()));
  wave_widget = new waveWidget(&wave_parameters, this);
  connect(wave_widget, SIGNAL(triggerChanged()), this, SLOT(trigger_changed()));
  custom_widget = new customWidget(&custom_parameters, this);
  connect(custom_widget, SIGNAL(triggerChanged()), this, SLOT(trigger_changed()));

  ui.stackedWidget_encoder->addWidget(lame_widget);
  ui.stackedWidget_encoder->addWidget(oggenc_widget);
  ui.stackedWidget_encoder->addWidget(flac_widget);
  ui.stackedWidget_encoder->addWidget(faac_widget);
  ui.stackedWidget_encoder->addWidget(wave_widget);
  ui.stackedWidget_encoder->addWidget(custom_widget);

  QMap<int,QString> encoders = EncoderAssistant::availableEncoderNameList();
  QMap<int,QString>::const_iterator i = encoders.constBegin();
  while (i != encoders.constEnd()) {
    ui.kcombobox_encoder->addItem(i.value(), i.key());
    ++i;
  }
  connect(ui.kcombobox_encoder, SIGNAL(activated(int)), this, SLOT(set_encoder_by_combobox(int)));

  connect(ui.kpushbutton_pattern, SIGNAL(clicked()), this, SLOT(pattern_wizard()));
  ui.kpushbutton_pattern->setIcon(KIcon("tools-wizard"));

  connect(ui.kpushbutton_cover, SIGNAL(clicked()), this, SLOT(cover_settings()));
  connect(ui.kpushbutton_playlist, SIGNAL(clicked()), this, SLOT(playlist_settings()));
  connect(ui.kpushbutton_info, SIGNAL(clicked()), this, SLOT(info_settings()));
  connect(ui.kpushbutton_hashlist, SIGNAL(clicked()), this, SLOT(hashlist_settings()));

  connect(ui.checkBox_cover, SIGNAL(toggled(bool)), this, SLOT(enable_settings_cover(bool)));
  connect(ui.checkBox_playlist, SIGNAL(toggled(bool)), this, SLOT(enable_settings_playlist(bool)));
  connect(ui.checkBox_info, SIGNAL(toggled(bool)), this, SLOT(enable_settings_info(bool)));
  connect(ui.checkBox_hashlist, SIGNAL(toggled(bool)), this, SLOT(enable_settings_hashlist(bool)));

  if (profile_row >= 0) {

    setCaption(i18n("Modify Profile"));

    setButtons(KDialog::Ok | KDialog::Cancel | KDialog::Apply);

    ui.klineedit_name->setText(profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_NAME_INDEX)).toString());
    connect(ui.klineedit_name, SIGNAL(textEdited(const QString&)), this, SLOT(trigger_changed()));
    ui.klineedit_name->setCursorPosition(0);

    set_encoder(profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_ENCODER_SELECTED_INDEX)).toInt());
    connect(ui.kcombobox_encoder, SIGNAL(activated(int)), this, SLOT(trigger_changed()));

    ui.klineedit_pattern->setText(profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_PATTERN_INDEX)).toString());
    connect(ui.klineedit_pattern, SIGNAL(textEdited(const QString&)), this, SLOT(trigger_changed()));
    ui.klineedit_pattern->setCursorPosition(0);

    ui.checkBox_fat32compatible->setChecked(profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_FAT32COMPATIBLE_INDEX)).toBool());
    connect(ui.checkBox_fat32compatible, SIGNAL(toggled(bool)), this, SLOT(trigger_changed()));

    ui.checkBox_underscore->setChecked(profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_UNDERSCORE_INDEX)).toBool());
    connect(ui.checkBox_underscore, SIGNAL(toggled(bool)), this, SLOT(trigger_changed()));

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
    pdcd_pattern = profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_SC_NAME_INDEX)).toString();

    pdpd_format = profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_PL_FORMAT_INDEX)).toString();
    pdpd_pattern = profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_PL_NAME_INDEX)).toString();

    pdid_text = profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_INF_TEXT_INDEX)).toStringList();
    pdid_pattern = profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_INF_NAME_INDEX)).toString();
    pdid_suffix = profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_INF_SUFFIX_INDEX)).toString();

    pdhd_format = profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_HL_FORMAT_INDEX)).toString();
    pdhd_pattern = profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_HL_NAME_INDEX)).toString();

    ui.checkBox_hashlist->setChecked(profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_HL_INDEX)).toBool());
    enable_settings_hashlist(ui.checkBox_hashlist->isChecked());
    connect(ui.checkBox_hashlist, SIGNAL(toggled(bool)), this, SLOT(trigger_changed()));

    ui.checkBox_discid->setChecked(profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_DISCID_INDEX)).toBool());
    connect(ui.checkBox_discid, SIGNAL(toggled(bool)), this, SLOT(trigger_changed()));

    enableButtonApply(FALSE);

  } else {

    setCaption(i18n("Create Profile"));

    setButtons(KDialog::Ok | KDialog::Cancel);

    ui.klineedit_name->setText(i18n("New Profile"));

    set_encoder(DEFAULT_ENCODER_SELECTED);

    ui.klineedit_pattern->setText(DEFAULT_PATTERN);
    ui.checkBox_fat32compatible->setChecked(DEFAULT_FAT32);
    ui.checkBox_underscore->setChecked(DEFAULT_UNDERSCORE);

    ui.checkBox_cover->setChecked(DEFAULT_SC);
    ui.checkBox_playlist->setChecked(DEFAULT_PL);
    ui.checkBox_info->setChecked(DEFAULT_INF);
    pdcd_scale = DEFAULT_SC_SCALE;
    pdcd_size = DEFAULT_SC_SIZE;
    pdcd_format = DEFAULT_SC_FORMAT;
    pdcd_pattern = DEFAULT_SC_NAME;
    pdpd_format = DEFAULT_PL_FORMAT;
    pdpd_pattern = DEFAULT_PL_NAME;
    pdid_text.clear();
    pdid_pattern = DEFAULT_INF_NAME;
    pdid_suffix = DEFAULT_INF_SUFFIX;
    pdhd_format = DEFAULT_HL_FORMAT;
    pdhd_pattern = DEFAULT_HL_NAME;
    ui.checkBox_hashlist->setChecked(DEFAULT_HL);
    ui.checkBox_discid->setChecked(DEFAULT_DISCID);

    enable_settings_cover(ui.checkBox_cover->isChecked());
    enable_settings_playlist(ui.checkBox_playlist->isChecked());
    enable_settings_info(ui.checkBox_info->isChecked());
    enable_settings_hashlist(ui.checkBox_hashlist->isChecked());

  }

  showButtonSeparator(TRUE);
  resize(0, 0);  // For some reason dialog start of big...

}

ProfileDataDialog::~ProfileDataDialog() {

  delete lame_widget;
  delete oggenc_widget;
  delete flac_widget;
  delete faac_widget;
  delete wave_widget;
  delete custom_widget;

}

void ProfileDataDialog::slotButtonClicked(int button) {
  if (button == KDialog::Ok) {
    if (save()) {
      accept();
    } else {
      KMessageBox::detailedError(this, error.message(), error.details());
    }
  } else if (button == KDialog::Apply) {
    if (!save()) {
      KMessageBox::detailedError(this, error.message(), error.details());
    }
  } else {
    KDialog::slotButtonClicked(button);
  }
}

void ProfileDataDialog::set_encoder(const int encoder) {

  set_encoder_widget((EncoderAssistant::Encoder)encoder);

  ui.kcombobox_encoder->setCurrentIndex(ui.kcombobox_encoder->findData(encoder));

}

void ProfileDataDialog::set_encoder_by_combobox(const int index) {

   set_encoder_widget((EncoderAssistant::Encoder)ui.kcombobox_encoder->itemData(index).toInt());

}

void ProfileDataDialog::trigger_changed() {
  enableButtonApply(
      ui.klineedit_name->text() != profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_NAME_INDEX)).toString() ||
      ui.kcombobox_encoder->itemData(ui.kcombobox_encoder->currentIndex()) != profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_ENCODER_SELECTED_INDEX)).toString() ||
      ui.klineedit_pattern->text() != profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_PATTERN_INDEX)).toString() ||
      ui.checkBox_fat32compatible->isChecked() != profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_FAT32COMPATIBLE_INDEX)).toBool() ||
      ui.checkBox_underscore->isChecked() != profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_UNDERSCORE_INDEX)).toBool() ||
      ui.checkBox_cover->isChecked() != profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_SC_INDEX)).toBool() ||
      ui.checkBox_playlist->isChecked() != profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_PL_INDEX)).toBool() ||
      ui.checkBox_info->isChecked() != profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_INF_INDEX)).toBool() ||
      pdcd_scale != profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_SC_SCALE_INDEX)).toBool() ||
      pdcd_size != profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_SC_SIZE_INDEX)).toSize() ||
      pdcd_format != profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_SC_FORMAT_INDEX)).toString() ||
      pdcd_pattern != profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_SC_NAME_INDEX)).toString() ||
      pdpd_format != profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_PL_FORMAT_INDEX)).toString() ||
      pdpd_pattern != profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_PL_NAME_INDEX)).toString() ||
      pdid_text != profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_INF_TEXT_INDEX)).toStringList() ||
      pdid_pattern != profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_INF_NAME_INDEX)).toString() ||
      pdid_suffix != profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_INF_SUFFIX_INDEX)).toString() ||
      ui.checkBox_hashlist->isChecked() != profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_HL_INDEX)).toBool() ||
      pdhd_format != profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_HL_FORMAT_INDEX)).toString() ||
      pdhd_pattern != profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_HL_NAME_INDEX)).toString() ||
      ui.checkBox_discid->isChecked() != profile_model->data(profile_model->index(profile_row, PROFILE_MODEL_COLUMN_DISCID_INDEX)).toBool() ||
      lame_widget->isChanged() ||
      oggenc_widget->isChanged() ||
      flac_widget->isChanged() ||
      faac_widget->isChanged() ||
      wave_widget->isChanged() ||
      custom_widget->isChanged()
  );
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

void ProfileDataDialog::pattern_wizard() {

  PatternWizardDialog *dialog = new PatternWizardDialog(ui.klineedit_pattern->text(), this);

  if (dialog->exec() != QDialog::Accepted) { delete dialog; return; }

  ui.klineedit_pattern->setText(dialog->pattern);

  delete dialog;

  trigger_changed();

}

void ProfileDataDialog::cover_settings() {

  ProfileDataCoverDialog *dialog = new ProfileDataCoverDialog(pdcd_scale, pdcd_size, pdcd_format, pdcd_pattern, this);

  if (dialog->exec() != QDialog::Accepted) { delete dialog; return; }

  pdcd_scale = dialog->scale;
  pdcd_size = dialog->size;
  pdcd_format = dialog->format;
  pdcd_pattern = dialog->pattern;

  delete dialog;

  trigger_changed();

}

void ProfileDataDialog::playlist_settings() {

  ProfileDataPlaylistDialog *dialog = new ProfileDataPlaylistDialog(pdpd_format, pdpd_pattern, this);

  if (dialog->exec() != QDialog::Accepted) { delete dialog; return; }

  pdpd_format = dialog->format;
  pdpd_pattern = dialog->pattern;

  delete dialog;

  trigger_changed();

}

void ProfileDataDialog::info_settings() {

  ProfileDataInfoDialog *dialog = new ProfileDataInfoDialog(pdid_text, pdid_pattern, pdid_suffix, this);

  if (dialog->exec() != QDialog::Accepted) { delete dialog; return; }

  pdid_text = dialog->text;
  pdid_pattern = dialog->pattern;
  pdid_suffix = dialog->suffix;

  delete dialog;

  trigger_changed();

}

void ProfileDataDialog::hashlist_settings() {

  ProfileDataHashlistDialog *dialog = new ProfileDataHashlistDialog(pdhd_pattern, pdhd_format, this);

  if (dialog->exec() != QDialog::Accepted) { delete dialog; return; }

  pdhd_format = dialog->format;
  pdhd_pattern = dialog->pattern;

  delete dialog;

  trigger_changed();

}

void ProfileDataDialog::set_encoder_widget(const EncoderAssistant::Encoder encoder) {

  switch (encoder) {

    case EncoderAssistant::LAME : ui.stackedWidget_encoder->setCurrentWidget(lame_widget); break;
    case EncoderAssistant::OGGENC : ui.stackedWidget_encoder->setCurrentWidget(oggenc_widget); break;
    case EncoderAssistant::FLAC : ui.stackedWidget_encoder->setCurrentWidget(flac_widget); break;
    case EncoderAssistant::FAAC : ui.stackedWidget_encoder->setCurrentWidget(faac_widget); break;
    case EncoderAssistant::WAVE : ui.stackedWidget_encoder->setCurrentWidget(wave_widget); break;
    case EncoderAssistant::CUSTOM : ui.stackedWidget_encoder->setCurrentWidget(custom_widget); break;
    case EncoderAssistant::NUM : break;

  }

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

  error = Error();

  if (success) success = lame_widget->save();
  if (!success) error = lame_widget->lastError();

  if (success) {
    success = oggenc_widget->save();
    if (!success) error = oggenc_widget->lastError();
  }

  if (success) {
    success = flac_widget->save();
    if (!success) error = flac_widget->lastError();
  }

  if (success) {
    success = faac_widget->save();
    if (!success) error = faac_widget->lastError();
  }

  if (success) {
    success = wave_widget->save();
    if (!success) error = wave_widget->lastError();
  }

  if (success) {
    success = custom_widget->save();
    if (!success) error = custom_widget->lastError();
  }

  if (success) {
    if (success) success = profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_NAME_INDEX), ui.klineedit_name->text());
    if (success) success = profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_ENCODER_SELECTED_INDEX), ui.kcombobox_encoder->currentIndex());
    if (success) success = profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_PATTERN_INDEX), ui.klineedit_pattern->text());
    if (success) success = profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_FAT32COMPATIBLE_INDEX), ui.checkBox_fat32compatible->isChecked());
    if (success) success = profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_UNDERSCORE_INDEX), ui.checkBox_underscore->isChecked());
    if (success) success = profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_SC_INDEX), ui.checkBox_cover->isChecked());
    if (success) success = profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_SC_SCALE_INDEX), pdcd_scale);
    if (success) success = profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_SC_SIZE_INDEX), pdcd_size);
    if (success) success = profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_SC_FORMAT_INDEX), pdcd_format);
    if (success) success = profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_SC_NAME_INDEX), pdcd_pattern);
    if (success) success = profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_PL_INDEX), ui.checkBox_playlist->isChecked());
    if (success) success = profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_INF_INDEX), ui.checkBox_info->isChecked());
    if (success) success = profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_PL_FORMAT_INDEX), pdpd_format);
    if (success) success = profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_PL_NAME_INDEX), pdpd_pattern);
    if (success) success = profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_INF_TEXT_INDEX), pdid_text);
    if (success) success = profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_INF_NAME_INDEX), pdid_pattern);
    if (success) success = profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_INF_SUFFIX_INDEX), pdid_suffix);
    if (success) success = profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_HL_INDEX), ui.checkBox_hashlist->isChecked());
    if (success) success = profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_DISCID_INDEX), ui.checkBox_discid->isChecked());
    if (success) success = profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_HL_FORMAT_INDEX), pdhd_format);
    if (success) success = profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_HL_NAME_INDEX), pdhd_pattern);
    if (success) success = profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_ENCODER_LAME_PARAMETERS_INDEX), lame_parameters.toString());
    if (success) success = profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_ENCODER_OGGENC_PARAMETERS_INDEX), oggenc_parameters.toString());
    if (success) success = profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_ENCODER_FLAC_PARAMETERS_INDEX), flac_parameters.toString());
    if (success) success = profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_ENCODER_FAAC_PARAMETERS_INDEX), faac_parameters.toString());
    if (success) success = profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_ENCODER_WAVE_PARAMETERS_INDEX), wave_parameters.toString());
    if (success) success = profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_ENCODER_CUSTOM_PARAMETERS_INDEX), custom_parameters.toString());
  }

  if (!success) error = profile_model->lastError();

  if (!success) {
    if (profile_row < 0) profile_model->removeRows(row, 1);
    return FALSE;
  }

  profile_model->commit();

  if (profile_row >= 0) enableButtonApply(FALSE);

  return TRUE;

}
