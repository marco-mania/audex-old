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

#ifndef COMMANDWIZARDDIALOG_H
#define COMMANDWIZARDDIALOG_H

#include <QWidget>

#include <KDebug>
#include <KDialog>

#include "../utils/maskparser.h"

#include "ui_commandwizardwidgetUI.h"

class CommandWizardDialog : public KDialog {

  Q_OBJECT

public:
  CommandWizardDialog(const QString& command, QWidget *parent = 0);
  ~CommandWizardDialog();

  QString command;

protected slots:
  virtual void slotButtonClicked(int button);

private slots:
  void trigger_changed();

  void insAlbumArtist();
  void insAlbumTitle();
  void insTrackArtist();
  void insTrackTitle();
  void insTrackNo();
  void insCDNo();
  void insDate();
  void insGenre();
  void insInFile();
  void insOutFile();

  void update_example();

private:
  Ui::CommandWizardWidgetUI ui;

  bool save();

};

#endif