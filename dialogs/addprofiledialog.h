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

#ifndef ADDPROFILEDIALOG_HEADER
#define ADDPROFILEDIALOG_HEADER

#include <QtCore>
#include <QtGui>
#include "ui_addprofiledialog.h"

#include "../base/qprofilemodel.h"

#include "maskwizarddialog.h"
#include "commandmaskwizarddialog.h"
#include "storecoverdialog.h"
#include "createplaylistdialog.h"

class addprofiledialog : public QDialog, private Ui::AddProfileDialog {

  Q_OBJECT

public:
  addprofiledialog(QWidget *parent, QSettings *settings, QProfileModel *profile_model, int profile_index);
  ~addprofiledialog();

private slots:
  void ok();
  void cancel();
  void open_wizard_dialog();
  void open_command_wizard_dialog();
  void open_store_cover_dialog();
  void open_store_playlist_dialog();

  void enable_open_store_playlist_dialog_button(bool enable);
  void enable_open_store_cover_dialog_button(bool enable);

protected:
  void closeEvent(QCloseEvent *event);

private:
  QSettings *settings;
  QProfileModel *profile_model;
  int profile_index;

  bool to_add;

  bool submit();

  bool scale;
  QSize _size;
  QString format;
  QString name;

  QString pl_format;
  QString pl_name;

};

#endif
