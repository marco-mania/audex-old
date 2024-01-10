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

#include "storecoverdialog.h"

storecoverdialog::storecoverdialog(QWidget *parent, QSettings *settings, QProfileModel *profile_model, bool* scale, QSize* size, QString* format, QString* name, bool readonly) {

  Q_UNUSED(parent);

  setupUi(this);

  this->settings = settings;
  this->profile_model = profile_model;
  this->scale = scale;
  this->_size = size;
  this->format = format;
  this->name = name;

  connect(pushButton, SIGNAL(clicked()), this, SLOT(close()));
  connect(toolButton, SIGNAL(clicked()), this, SLOT(open_mask_dialog()));

  checkBox->setChecked(*scale);
  connect(checkBox, SIGNAL(toggled(bool)), this, SLOT(enable_scale(bool)));
  label->setEnabled(*scale);
  spinBox->setEnabled(*scale);
  label_2->setEnabled(*scale);
  spinBox_2->setEnabled(*scale);

  spinBox->setValue(_size->width());
  spinBox_2->setValue(_size->height());
  connect(spinBox, SIGNAL(valueChanged(int)), this, SLOT(setHorizontalValue(int)));
  connect(spinBox_2, SIGNAL(valueChanged(int)), this, SLOT(setVerticalValue(int)));

  comboBox->addItem("JPEG (Joint Photographic Experts Group)", "JPG");
  comboBox->addItem("PNG (Portable Network Graphics)", "PNG");
  comboBox->addItem("TIFF (Tagged Image File Format)", "TIFF");
  comboBox->addItem("BMP (Windows Bitmap)", "BMP");
  comboBox->setCurrentIndex(comboBox->findData(*format));
  connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setImageFormat(int)));

  lineEdit->setText(*name);
  connect(lineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(setFilename(const QString&)));

  if (readonly) {
    disconnect(checkBox, SIGNAL(toggled(bool)), this, SLOT(enable_scale(bool)));
    connect(checkBox, SIGNAL(clicked(bool)), this, SLOT(block_checkbox(bool)));

    spinBox->setReadOnly(TRUE);
    QPalette palette = spinBox->palette();
    palette.setColor(QPalette::Base, QColor(239, 239, 239));
    spinBox->setPalette(palette);

    spinBox_2->setReadOnly(TRUE);
    palette = spinBox_2->palette();
    palette.setColor(QPalette::Base, QColor(239, 239, 239));
    spinBox_2->setPalette(palette);

    disconnect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setImageFormat(int)));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(block_combobox(int)));

    lineEdit->setReadOnly(TRUE);
    palette = lineEdit->palette();
    palette.setColor(QPalette::Base, QColor(239, 239, 239));
    lineEdit->setPalette(palette);

  }

  resize(settings->value("storecoverdialog/size", QSize(420, 230)).toSize());
  move(settings->value("storecoverdialog/pos", QPoint(100, 100)).toPoint());

}

storecoverdialog::~storecoverdialog() {

}

void storecoverdialog::enable_scale(bool enabled) {
  label->setEnabled(enabled);
  spinBox->setEnabled(enabled);
  label_2->setEnabled(enabled);
  spinBox_2->setEnabled(enabled);
  *scale = enabled;
}

void storecoverdialog::setHorizontalValue(int pixel) {
  _size->setWidth(pixel);
}

void storecoverdialog::setVerticalValue(int pixel) {
  _size->setHeight(pixel);
}

void storecoverdialog::setImageFormat(int index) {
  *format = comboBox->itemData(index).toString();
}

void storecoverdialog::setFilename(const QString& filename) {
  *name = filename;
}

void storecoverdialog::open_mask_dialog() {
  QString mask = lineEdit->text();
  simplemaskwizarddialog *simpleMaskWizardDialog = new simplemaskwizarddialog(this, settings, profile_model, &mask, comboBox->itemData(comboBox->currentIndex()).toString().toLower());
  if (simpleMaskWizardDialog->exec()==QDialog::Accepted) lineEdit->setText(mask);
  delete simpleMaskWizardDialog;
}

void storecoverdialog::closeEvent(QCloseEvent *event) {
  settings->setValue("storecoverdialog/pos", pos());
  settings->setValue("storecoverdialog/size", size());
  event->accept();
}
