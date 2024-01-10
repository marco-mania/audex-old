/* AUDEX CDDA EXTRACTOR
 * Copyright (C) 2007-2009 by Marco Nelles (audex@maniatek.de)
 * http://opensource.maniatek.de/audex
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

#include "profiledatacoverdialog.h"

ProfileDataCoverDialog::ProfileDataCoverDialog(const bool scale, const QSize& size, const QString& format, const QString &mask, QWidget *parent) : KDialog(parent) {

  Q_UNUSED(parent);

  this->scale = scale;
  this->size = size;
  this->format = format;
  this->mask = mask;

  QWidget *widget = new QWidget(this);
  ui.setupUi(widget);

  setMainWidget(widget);

  connect(ui.checkBox_scale, SIGNAL(toggled(bool)), this, SLOT(enable_scale(bool)));

  setCaption(i18n("Cover Settings"));

  setButtons(KDialog::Ok | KDialog::Cancel | KDialog::Apply);

  connect(ui.kpushbutton_mask, SIGNAL(clicked()), this, SLOT(mask_wizard()));
  ui.kpushbutton_mask->setIcon(KIcon("tools-wizard"));

  ui.checkBox_scale->setChecked(scale);
  enable_scale(ui.checkBox_scale->isChecked());
  connect(ui.checkBox_scale, SIGNAL(toggled(bool)), this, SLOT(trigger_changed()));

  ui.kintspinbox_x->setValue(size.width());
  connect(ui.kintspinbox_x, SIGNAL(valueChanged(int)), this, SLOT(trigger_changed()));

  ui.kintspinbox_y->setValue(size.height());
  connect(ui.kintspinbox_y, SIGNAL(valueChanged(int)), this, SLOT(trigger_changed()));

  ui.kcombobox_format->addItem(i18n("JPEG (Joint Photographic Experts Group)"), "JPG");
  ui.kcombobox_format->addItem(i18n("PNG (Portable Network Graphics)"), "PNG");
  ui.kcombobox_format->addItem(i18n("BMP (Windows Bitmap)"), "BMP");
  {
    int i = ui.kcombobox_format->findData(format);
    ui.kcombobox_format->setCurrentIndex(i);
  }
  connect(ui.kcombobox_format, SIGNAL(currentIndexChanged(int)), this, SLOT(trigger_changed()));

  ui.klineedit_mask->setText(mask);
  connect(ui.klineedit_mask, SIGNAL(textEdited(const QString&)), this, SLOT(trigger_changed()));

  enableButtonApply(FALSE);

}

ProfileDataCoverDialog::~ProfileDataCoverDialog() {
}

void ProfileDataCoverDialog::slotButtonClicked(int button) {
  if (button == KDialog::Ok) {
    save();
    accept();
  } else if (button == KDialog::Apply) {
    save();
  } else {
    KDialog::slotButtonClicked(button);
  }
}

void ProfileDataCoverDialog::mask_wizard() {

  QString suffix = ui.kcombobox_format->itemData(ui.kcombobox_format->currentIndex()).toString().toLower();

  SimpleMaskWizardDialog *dialog = new SimpleMaskWizardDialog(ui.klineedit_mask->text(), suffix, this);

  if (dialog->exec() != QDialog::Accepted) { delete dialog; return; }

  ui.klineedit_mask->setText(dialog->mask);

  delete dialog;

  trigger_changed();

}

void ProfileDataCoverDialog::trigger_changed() {
  if (ui.checkBox_scale->isChecked() != scale) { enableButtonApply(TRUE); return; }
  if (ui.kintspinbox_x->value() != size.width()) { enableButtonApply(TRUE); return; }
  if (ui.kintspinbox_y->value() != size.height()) { enableButtonApply(TRUE); return; }
  if (ui.kcombobox_format->itemData(ui.kcombobox_format->currentIndex()).toString() != format) { enableButtonApply(TRUE); return; }
  if (ui.klineedit_mask->text() != mask) { enableButtonApply(TRUE); return; }
  enableButtonApply(FALSE);
}

void ProfileDataCoverDialog::enable_scale(bool enabled) {
  ui.label_x->setEnabled(enabled);
  ui.label_y->setEnabled(enabled);
  ui.kintspinbox_x->setEnabled(enabled);
  ui.kintspinbox_y->setEnabled(enabled);
}

bool ProfileDataCoverDialog::save() {
  scale = ui.checkBox_scale->isChecked();
  size = QSize(ui.kintspinbox_x->value(), ui.kintspinbox_y->value());
  format = ui.kcombobox_format->itemData(ui.kcombobox_format->currentIndex()).toString();
  mask = ui.klineedit_mask->text();
  enableButtonApply(FALSE);
  return TRUE;
}
