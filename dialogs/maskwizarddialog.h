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

#ifndef MASKWIZARDDIALOG_HEADER
#define MASKWIZARDDIALOG_HEADER

#include <QtCore>
#include <QtGui>
#include "ui_maskwizarddialog.h"

#include "../base/qprofilemodel.h"
#include "../base/qmaskparser.h"

class maskwizarddialog : public QDialog, private Ui::MaskWizardDialog {
  Q_OBJECT
public:
  maskwizarddialog(QWidget *parent, QSettings *settings, QProfileModel *profile_model, QString *mask);
  ~maskwizarddialog();

private slots:
  void insHomePath();
  void insBasePath();
  void insAlbumArtist();
  void insAlbumTitle();
  void insTrackArtist();
  void insTrackTitle();
  void insTrackNo();
  void insCDNo();
  void insDate();
  void insGenre();
  void insExtension();

  void ok();
  void cancel();

  void update_example();

  void enable_help(bool enabled);

protected:
  bool eventFilter(QObject *target, QEvent *event);

  void closeEvent(QCloseEvent *event);

private:
  QSettings *settings;
  QProfileModel *profile_model;

  QString *mask;

};

#endif
