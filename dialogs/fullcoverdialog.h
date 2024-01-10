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

#ifndef FULLCOVERDIALOG_HEADER
#define FULLCOVERDIALOG_HEADER

#include <QtCore>
#include <QtGui>
#include "ui_fullcoverdialog.h"

class fullcoverdialog : public QDialog, private Ui::FullCoverDialog {
  Q_OBJECT
public:
  fullcoverdialog(QWidget *parent, QSettings *settings, const QImage& image);
  ~fullcoverdialog();

private slots:
  void scaleChecked(bool checked);

protected:
  void closeEvent(QCloseEvent *event);

private:
  QImage image;
  QSettings *settings;

};

#endif
