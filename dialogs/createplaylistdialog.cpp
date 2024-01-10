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

#include "createplaylistdialog.h"

createplaylistdialog::createplaylistdialog(QWidget *parent, QSettings *settings, QProfileModel *profile_model, QString* format, QString* name, bool readonly) {

  Q_UNUSED(parent);

  setupUi(this);

  this->settings = settings;
  this->profile_model = profile_model;
  this->format = format;
  this->name = name;

  connect(pushButton_2, SIGNAL(clicked()), this, SLOT(close()));
  connect(toolButton, SIGNAL(clicked()), this, SLOT(open_mask_dialog()));

  comboBox->addItem("M3U (Textbased Winamp Playlist)", "M3U");
  comboBox->addItem("PLS (Textbased Playlist)", "PLS");
  comboBox->addItem("XSPF (XML Shareable Playlist Format)", "XSPF");
  comboBox->setCurrentIndex(comboBox->findData(*format));
  connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setPlaylistFormat(int)));

  lineEdit->setText(*name);
  connect(lineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(setName(const QString&)));

  if (readonly) {
    disconnect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setPlaylistFormat(int)));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(block_combobox(int)));

    lineEdit->setReadOnly(TRUE);
    QPalette palette = lineEdit->palette();
    palette.setColor(QPalette::Base, QColor(239, 239, 239));
    lineEdit->setPalette(palette);

  }

  resize(settings->value("createplaylistdialog/size", QSize(420, 150)).toSize());
  move(settings->value("createplaylistdialog/pos", QPoint(100, 100)).toPoint());

}

createplaylistdialog::~createplaylistdialog() {

}

void createplaylistdialog::setPlaylistFormat(int index) {
  *format = comboBox->itemData(index).toString();
}

void createplaylistdialog::setName(const QString& _name) {
  *name = _name;
}

void createplaylistdialog::open_mask_dialog() {
  QString mask = lineEdit->text();
  simplemaskwizarddialog *simpleMaskWizardDialog = new simplemaskwizarddialog(this, settings, profile_model, &mask, comboBox->itemData(comboBox->currentIndex()).toString().toLower());
  if (simpleMaskWizardDialog->exec()==QDialog::Accepted) lineEdit->setText(mask);
  delete simpleMaskWizardDialog;
}

void createplaylistdialog::closeEvent(QCloseEvent *event) {
  settings->setValue("createplaylistdialog/pos", pos());
  settings->setValue("createplaylistdialog/size", size());
  event->accept();
}
