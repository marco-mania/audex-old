/* AUDEX CDDA EXTRACTOR
 * Copyright (C) 2007-2008 by Marco Nelles (marcomaniac@gmx.de)
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

#ifndef PROFILEDATAHASHLISTDIALOG_H
#define PROFILEDATAHASHLISTDIALOG_H

#include <QWidget>

#include <KDebug>
#include <KDialog>

#include "simplemaskwizarddialog.h"

#include "ui_profiledatahashlistwidgetUI.h"

class ProfileDataHashlistDialog : public KDialog {

  Q_OBJECT

public:
  ProfileDataHashlistDialog(const QString& mask, const QString& format, QWidget *parent = 0);
  ~ProfileDataHashlistDialog();

  QString format;
  QString mask;

protected slots:
  virtual void slotButtonClicked(int button);

  void mask_wizard();

private slots:
  void trigger_changed();

private:
  Ui::ProfileDataHashlistWidgetUI ui;

  bool save();

};

#endif
