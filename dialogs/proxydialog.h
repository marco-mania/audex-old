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

#ifndef PROXYDIALOG_HEADER
#define PROXYDIALOG_HEADER

#include <QtCore>
#include <QtGui>
#include "ui_proxydialog.h"

class proxydialog : public QDialog, private Ui::ProxyDialog {
  Q_OBJECT
public:
  proxydialog(QWidget *parent, QSettings *settings, const QString& prefix);
  ~proxydialog();

private slots:
  void change_type(int type);
  void change_host();
  void change_port();
  void change_user();
  void change_pass();

  void enable_auth(bool enable);

protected:
  void closeEvent(QCloseEvent *event);

private:
  QSettings *settings;
  QString prefix;

};

#endif
