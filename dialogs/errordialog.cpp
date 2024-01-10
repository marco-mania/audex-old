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

#include "errordialog.h"

errordialog::errordialog(QWidget *parent, QSettings *settings, const QString& description, const QString& solution) {

  Q_UNUSED(parent);

  setupUi(this);
  this->settings = settings;
  connect(pushButton, SIGNAL(clicked()), this, SLOT(close()));

  textEdit->setPlainText(description);
  textEdit_2->setPlainText(solution);

  resize(settings->value("errordialog/size", QSize(490, 340)).toSize());
  move(settings->value("errordialog/pos", QPoint(200, 200)).toPoint());

}

errordialog::~errordialog() {

}

void errordialog::closeEvent(QCloseEvent *event) {
  settings->setValue("errordialog/pos", pos());
  settings->setValue("errordialog/size", size());
  event->accept();
}
