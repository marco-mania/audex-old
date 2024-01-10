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

#include "categorydialog.h"

categorydialog::categorydialog(QWidget *parent, QSettings *settings, QString *category) {

  Q_UNUSED(parent);

  setupUi(this);

  this->settings = settings;
  this->category = category;

  connect(pushButton, SIGNAL(clicked()), this, SLOT(select()));
  connect(pushButton_2, SIGNAL(clicked()), this, SLOT(cancel()));

  connect(listWidget, SIGNAL(itemSelectionChanged()), this, SLOT(enableSelectButton()));

  QStringList validCategories;
    validCategories << "blues" << "classical" << "country"
      << "data" << "folk" << "jazz" << "misc" << "newage" << "reggae"
      << "rock" << "soundtrack";
  listWidget->addItems(validCategories);

  pushButton->setEnabled(FALSE);

  resize(settings->value("categorydialog/size", QSize(370, 300)).toSize());
  move(settings->value("categorydialog/pos", QPoint(100, 100)).toPoint());

}

categorydialog::~categorydialog() {

}

void categorydialog::select() {
  *category = listWidget->selectedItems().at(0)->text();
  setResult(QDialog::Accepted);
  close();
}

void categorydialog::cancel() {
  setResult(QDialog::Rejected);
  close();
}

void categorydialog::enableSelectButton() {
  pushButton->setEnabled(listWidget->selectedItems().count() > 0);
}

void categorydialog::closeEvent(QCloseEvent *event) {
  settings->setValue("categorydialog/pos", pos());
  settings->setValue("categorydialog/size", size());
  event->accept();
}
