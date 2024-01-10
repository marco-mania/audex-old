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

#ifndef CREATEPLAYLISTDIALOG_HEADER
#define CREATEPLAYLISTDIALOG_HEADER

#include <QtCore>
#include <QtGui>
#include "ui_createplaylistdialog.h"

#include "simplemaskwizarddialog.h"

#include "../base/qprofilemodel.h"

class createplaylistdialog : public QDialog, private Ui::CreatePlaylistDialog {
  Q_OBJECT
public:
  createplaylistdialog(QWidget *parent, QSettings *settings, QProfileModel *profile_model, QString* format, QString* name, bool readonly = FALSE);
  ~createplaylistdialog();

private slots:
  void setPlaylistFormat(int index);
  void setName(const QString& _name);

  void block_combobox(int index) { Q_UNUSED(index); comboBox->setCurrentIndex(comboBox->findData(*format)); }

  void open_mask_dialog();

protected:
  void closeEvent(QCloseEvent *event);

private:
  QSettings *settings;
  QProfileModel *profile_model;
  QString *format;
  QString *name;

};

#endif
