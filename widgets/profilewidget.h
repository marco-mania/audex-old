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

#ifndef PROFILEWIDGET_H
#define PROFILEWIDGET_H

#include <QWidget>
#include <QDir>

#include <KDebug>
#include <KDialog>
#include <KFileDialog>
#include <KIcon>

#include "../models/profilemodel.h"

#include "../dialogs/profiledatadialog.h"
#include "../dialogs/assistantdialog.h"

#include "ui_profilewidgetUI.h"

class profileWidgetUI : public QWidget, public Ui::ProfileWidgetUI {
public:
  explicit profileWidgetUI(QWidget *parent) : QWidget(parent) {
    setupUi(this);
  }
};

class profileWidget : public profileWidgetUI {
  Q_OBJECT
public:
  explicit profileWidget(ProfileModel *profileModel, QWidget *parent = 0);
  ~profileWidget();
private slots:
  void update_profile(const QModelIndex& current, const QModelIndex& previous);
  void update();
  void add_profile();
  void rem_profile();
  void mod_profile();
  void copy_profile();
  void assistant();
  void save_profiles();
  void load_profiles();
private:
  ProfileModel *profile_model;
};

#endif
