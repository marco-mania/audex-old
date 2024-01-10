/* AUDEX CDDA EXTRACTOR
 * Copyright (C) 2007-2011 Marco Nelles (audex@maniatek.com)
 * <http://kde.maniatek.com/audex>
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

#include "oggencwidget.h"

oggencWidget::oggencWidget(Parameters *parameters, QWidget *parent) : oggencWidgetUI(parent) {

  Q_UNUSED(parent);

  this->parameters = parameters;
  if (!parameters) {
    kDebug() << "ParameterString is NULL!";
    return;
  }

  horizontalSlider_quality->setValue(parameters->valueToInt(ENCODER_OGGENC_QUALITY_KEY, ENCODER_OGGENC_QUALITY));
  kintspinbox_quality->setValue(parameters->valueToInt(ENCODER_OGGENC_QUALITY_KEY, ENCODER_OGGENC_QUALITY));
  set_bitrate(parameters->valueToInt(ENCODER_OGGENC_QUALITY_KEY, ENCODER_OGGENC_QUALITY));
  enable_min_bitrate(parameters->valueToBool(ENCODER_OGGENC_MINBITRATE_KEY));
  kintspinbox_minbitrate->setValue(parameters->valueToInt(ENCODER_OGGENC_MINBITRATE_VALUE_KEY, ENCODER_OGGENC_MINBITRATE_VALUE));
  enable_max_bitrate(parameters->valueToBool(ENCODER_OGGENC_MAXBITRATE_KEY));
  kintspinbox_maxbitrate->setValue(parameters->valueToInt(ENCODER_OGGENC_MAXBITRATE_VALUE_KEY, ENCODER_OGGENC_MAXBITRATE_VALUE));
  klineedit_suffix->setText(parameters->value(ENCODER_OGGENC_SUFFIX_KEY, ENCODER_OGGENC_SUFFIX));

  connect(horizontalSlider_quality, SIGNAL(valueChanged(int)), this, SLOT(quality_changed_by_slider(int)));
  connect(horizontalSlider_quality, SIGNAL(valueChanged(int)), this, SLOT(trigger_changed()));

  connect(kintspinbox_quality, SIGNAL(valueChanged(int)), this, SLOT(quality_changed_by_spinbox(int)));
  connect(kintspinbox_quality, SIGNAL(valueChanged(int)), this, SLOT(trigger_changed()));

  connect(checkBox_minbitrate, SIGNAL(toggled(bool)), this, SLOT(enable_min_bitrate(bool)));
  connect(checkBox_minbitrate, SIGNAL(toggled(bool)), this, SLOT(trigger_changed()));
  connect(checkBox_maxbitrate, SIGNAL(toggled(bool)), this, SLOT(enable_max_bitrate(bool)));
  connect(checkBox_maxbitrate, SIGNAL(toggled(bool)), this, SLOT(trigger_changed()));

  connect(kintspinbox_minbitrate, SIGNAL(valueChanged(int)), this, SLOT(trigger_changed()));
  connect(kintspinbox_maxbitrate, SIGNAL(valueChanged(int)), this, SLOT(trigger_changed()));

  connect(klineedit_suffix, SIGNAL(textEdited(const QString&)), this, SLOT(trigger_changed()));

  changed = FALSE;

}

oggencWidget::~oggencWidget() {

}

bool oggencWidget::save() {

  bool success = TRUE;

  parameters->setValue(ENCODER_OGGENC_QUALITY_KEY, horizontalSlider_quality->value());
  parameters->setValue(ENCODER_OGGENC_MINBITRATE_KEY, checkBox_minbitrate->isChecked());
  parameters->setValue(ENCODER_OGGENC_MINBITRATE_VALUE_KEY, kintspinbox_minbitrate->value());
  parameters->setValue(ENCODER_OGGENC_MAXBITRATE_KEY, checkBox_maxbitrate->isChecked());
  parameters->setValue(ENCODER_OGGENC_MAXBITRATE_VALUE_KEY, kintspinbox_maxbitrate->value());
  parameters->setValue(ENCODER_OGGENC_SUFFIX_KEY, klineedit_suffix->text());

  changed = FALSE;

  return success;

}

void oggencWidget::quality_changed_by_slider(int quality) {

  kintspinbox_quality->blockSignals(TRUE);
  kintspinbox_quality->setValue(quality);
  kintspinbox_quality->blockSignals(FALSE);

  set_bitrate(quality);

}

void oggencWidget::quality_changed_by_spinbox(int quality) {

  horizontalSlider_quality->blockSignals(TRUE);
  horizontalSlider_quality->setValue(quality);
  horizontalSlider_quality->blockSignals(FALSE);

  set_bitrate(quality);

}

void oggencWidget::set_bitrate(int quality) {

  switch (quality) {

    case -1 : lineEdit_kbps->setText("48"); break;
    case 0 : lineEdit_kbps->setText("64"); break;
    case 1 : lineEdit_kbps->setText("80"); break;
    case 2 : lineEdit_kbps->setText("96"); break;
    case 3 : lineEdit_kbps->setText("112"); break;
    case 4 : lineEdit_kbps->setText("128"); break;
    case 5 : lineEdit_kbps->setText("160"); break;
    case 6 : lineEdit_kbps->setText("192"); break;
    case 7 : lineEdit_kbps->setText("224"); break;
    case 8 : lineEdit_kbps->setText("256"); break;
    case 9 : lineEdit_kbps->setText("320"); break;
    case 10 : lineEdit_kbps->setText("500"); break;

  }

}

void oggencWidget::enable_min_bitrate(bool enable) {

  kintspinbox_minbitrate->setEnabled(enable);
  label_minbitrate_kbps->setEnabled(enable);

}

void oggencWidget::enable_max_bitrate(bool enable) {

  kintspinbox_maxbitrate->setEnabled(enable);
  label_maxbitrate_kbps->setEnabled(enable);

}

void oggencWidget::trigger_changed() {

  changed = (
    horizontalSlider_quality->value() != parameters->valueToInt(ENCODER_OGGENC_QUALITY_KEY, ENCODER_OGGENC_QUALITY) ||
    kintspinbox_minbitrate->value() != parameters->valueToInt(ENCODER_OGGENC_MINBITRATE_VALUE_KEY, ENCODER_OGGENC_MINBITRATE_VALUE) ||
    checkBox_minbitrate->isChecked() != parameters->valueToBool(ENCODER_OGGENC_MINBITRATE_KEY) ||
    kintspinbox_maxbitrate->value() != parameters->valueToInt(ENCODER_OGGENC_MAXBITRATE_VALUE_KEY, ENCODER_OGGENC_MAXBITRATE_VALUE) ||
    checkBox_maxbitrate->isChecked() != parameters->valueToBool(ENCODER_OGGENC_MAXBITRATE_KEY) ||
    klineedit_suffix->text() != parameters->value(ENCODER_OGGENC_SUFFIX_KEY, ENCODER_OGGENC_SUFFIX)
  );

  emit triggerChanged();

}
