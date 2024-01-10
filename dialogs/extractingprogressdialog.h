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

#ifndef EXTRACTINGPROGRESSDIALOG_H
#define EXTRACTINGPROGRESSDIALOG_H

#include <QWidget>

#include <KDebug>
#include <KDialog>
#include <KMessageBox>
#include <KColorScheme>

#include "models/cddamodel.h"
#include "models/profilemodel.h"
#include "core/audex.h"

#include "protocoldialog.h"

#include "ui_extractingprogresswidgetUI.h"

class ExtractingProgressDialog : public KDialog {

  Q_OBJECT

public:
  ExtractingProgressDialog(ProfileModel *profile_model, CDDAModel *cdda_model, QWidget *parent = 0);
  ~ExtractingProgressDialog();

public slots:
  int exec();

private slots:
  void slotButtonClicked(int button);
  void cancel();

  void show_changed_extract_track(int no, int total, const QString& artist, const QString& title);
  void show_changed_encode_track(int no, int total, const QString& filename);

  void show_progress_extract_track(int percent);
  void show_progress_extract_overall(int percent);
  void show_progress_encode_track(int percent);

  void show_speed_encode(double speed);
  void show_speed_extract(double speed);

  void conclusion(bool successful);

  void show_info(const QString& message);
  void show_warning(const QString& message);
  void show_error(const QString& message, const QString& details);

  void ask_timeout();

private:
  void open_encoder_protocol_dialog();
  void open_extract_protocol_dialog();

private:
  Ui::ExtractingProgressWidgetUI ui;

  Audex *audex;
  ProfileModel *profile_model;
  CDDAModel *cdda_model;

  bool finished;

  bool progressbar_np_flag;

};

#endif
