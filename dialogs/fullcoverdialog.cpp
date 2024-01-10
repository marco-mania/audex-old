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

#include "fullcoverdialog.h"

fullcoverdialog::fullcoverdialog(QWidget *parent, QSettings *settings, const QImage& image) {

  Q_UNUSED(parent);

  setupUi(this);

  this->settings = settings;
  this->image = image;

  connect(pushButton, SIGNAL(clicked()), this, SLOT(close()));
  connect(checkBox, SIGNAL(toggled(bool)), this, SLOT(scaleChecked(bool)));

  label->setPixmap(QPixmap::fromImage(image));

  move(settings->value("fullcoverdialog/pos", QPoint(100, 100)).toPoint());

  resize(1, 1);

}

fullcoverdialog::~fullcoverdialog() {

}

void fullcoverdialog::scaleChecked(bool checked) {
  label->setScaledContents(checked);
}

void fullcoverdialog::closeEvent(QCloseEvent *event) {
  settings->setValue("fullcoverdialog/pos", pos());
  event->accept();
}
