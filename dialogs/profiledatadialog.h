/* AUDEX CDDA EXTRACTOR
 * Copyright (C) 2007-2009 by Marco Nelles (audex@maniatek.de)
 * http://opensource.maniatek.de/audex
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

#ifndef PROFILEDATADIALOG_H
#define PROFILEDATADIALOG_H

#include <QWidget>

#include <KDebug>
#include <KDialog>

#include "../models/profilemodel.h"

#include "profiledatacoverdialog.h"
#include "profiledataplaylistdialog.h"
#include "profiledatainfodialog.h"
#include "profiledatahashlistdialog.h"
#include "maskwizarddialog.h"
#include "commandwizarddialog.h"

#include "ui_profiledatawidgetUI.h"

class ProfileDataDialog : public KDialog {

  Q_OBJECT

public:
  ProfileDataDialog(ProfileModel *profileModel, const int profileRow, QWidget *parent = 0);
  ~ProfileDataDialog();

protected slots:
  virtual void slotButtonClicked(int button);

private slots:
  void trigger_changed();
  void enable_settings_cover(bool enabled);
  void enable_settings_playlist(bool enabled);
  void enable_settings_info(bool enabled);
  void enable_settings_hashlist(bool enabled);

  void mask_wizard();
  void command_wizard();

  void cover_settings();
  void playlist_settings();
  void info_settings();
  void hashlist_settings();

private:
  Ui::ProfileDataWidgetUI ui;
  ProfileModel *profile_model;
  int profile_row;

  bool pdcd_scale;
  QSize pdcd_size;
  QString pdcd_format;
  QString pdcd_mask;

  QString pdpd_format;
  QString pdpd_mask;

  QStringList pdid_text;
  QString pdid_mask;
  QString pdid_suffix;

  QString pdhd_format;
  QString pdhd_mask;

  bool save();

};

#endif
