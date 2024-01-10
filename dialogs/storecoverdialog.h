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

#ifndef STORECOVERDIALOG_HEADER
#define STORECOVERDIALOG_HEADER

#include <QtCore>
#include <QtGui>
#include "ui_storecoverdialog.h"

#include "simplemaskwizarddialog.h"

#include "../base/qprofilemodel.h"

class storecoverdialog : public QDialog, private Ui::StoreCoverDialog {
  Q_OBJECT
public:
  storecoverdialog(QWidget *parent, QSettings *settings, QProfileModel *profile_model, bool *scale, QSize* size, QString* format, QString* name, bool readonly = FALSE);
  ~storecoverdialog();

private slots:
  void enable_scale(bool enabled);

  void setHorizontalValue(int pixel);
  void setVerticalValue(int pixel);
  void setImageFormat(int index);
  void setFilename(const QString& filename);

  void block_combobox(int index) { Q_UNUSED(index); comboBox->setCurrentIndex(comboBox->findData(*format)); }
  void block_checkbox(bool checked) { checkBox->setChecked(!checked); }

  void open_mask_dialog();

protected:
  void closeEvent(QCloseEvent *event);

private:
  QSettings *settings;
  QProfileModel *profile_model;
  bool *scale;
  QSize *_size;
  QString *format;
  QString *name;

};

#endif
