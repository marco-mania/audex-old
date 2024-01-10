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

#ifndef SQLMATCHINGDIALOG_HEADER
#define SQLMATCHINGDIALOG_HEADER

#include <QtCore>
#include <QtGui>
#include <QtSql>
#include "ui_sqlmatchingdialog.h"

class sqlmatchingdialog : public QDialog, private Ui::SQLMatchingDialog {
  Q_OBJECT
public:
  sqlmatchingdialog(QWidget *parent, QSettings *settings, QSqlQuery *query);
  ~sqlmatchingdialog();

private slots:
  void _continue();
  void cancel();

protected:
  void closeEvent(QCloseEvent *event);

private:
  QSettings *settings;

  QSqlQuery *query;

};

#endif
