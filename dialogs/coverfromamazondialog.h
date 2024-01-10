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

#ifndef COVERFROMAMAZONDIALOG_HEADER
#define COVERFROMAMAZONDIALOG_HEADER

#include <QtCore>
#include <QtGui>
#include "ui_coverfromamazondialog.h"

#include "../cover/qcoverfetcher.h"
#include "../dialogs/proxyauthdialog.h"

class coverfromamazondialog : public QDialog, private Ui::CoverFromAmazonDialog {
  Q_OBJECT
public:
  coverfromamazondialog(QWidget *parent, QSettings *settings, const QString& searchstring, QImage *image);
  ~coverfromamazondialog();

private slots:
  void select();
  void select_this(QListWidgetItem* item);
  void back();

  void enableSelectButton();

  void addItem(const QImage& cover, const QString& caption, const QString& date, int no);
  void allFetched();
  void nothingFetched();

  void slot_proxy_auth_req(const QNetworkProxy& proxy, QAuthenticator *authenticator);

protected:
  void closeEvent(QCloseEvent *event);

private:
  QCoverFetcher cover_fetcher;

  QImage *image;

  QSettings *settings;

};

#endif
