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

#include "coverfromamazondialog.h"

coverfromamazondialog::coverfromamazondialog(QWidget *parent, QSettings *settings, const QString& searchstring, QImage *image) {

  Q_UNUSED(parent);

  setupUi(this);

  this->settings = settings;
  this->image = image;

  connect(&cover_fetcher, SIGNAL(fetched(const QImage&, const QString&, const QString&, int)), this, SLOT(addItem(const QImage&, const QString&, const QString&, int)));
  connect(&cover_fetcher, SIGNAL(allFetched()), this, SLOT(allFetched()));
  connect(&cover_fetcher, SIGNAL(nothingFetched()), this, SLOT(nothingFetched()));
  connect(&cover_fetcher, SIGNAL(proxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*)), this, SLOT(slot_proxy_auth_req(const QNetworkProxy&, QAuthenticator*)));

  connect(pushButton, SIGNAL(clicked()), this, SLOT(select()));
  connect(pushButton_2, SIGNAL(clicked()), this, SLOT(back()));

  listWidget->setIconSize(QSize(128, 128));
  listWidget->setWordWrap(TRUE);
  listWidget->setViewMode(QListView::IconMode);
  connect(listWidget, SIGNAL(itemSelectionChanged()), this, SLOT(enableSelectButton()));
  connect(listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(select_this(QListWidgetItem*)));
  enableSelectButton();

  resize(settings->value("coverfromamazondialog/size", QSize(500, 330)).toSize());
  move(settings->value("coverfromamazondialog/pos", QPoint(100, 100)).toPoint());

  cover_fetcher.setLocale(settings->value("settings/amazon_locale", "us").toString());
  cover_fetcher.startFetch(searchstring);

  if (settings->value("settings/amazon_proxy_enabled", FALSE).toBool()) {
    int type = settings->value("settings/amazon_proxy_type", 0).toInt();
    switch (type) {
      case 0 : cover_fetcher.setProxyType(QNetworkProxy::Socks5Proxy); break;
      case 1 : cover_fetcher.setProxyType(QNetworkProxy::HttpProxy); break;
    }
    cover_fetcher.setProxyHost(settings->value("settings/amazon_proxy_host", "www.proxy.com").toString());
    cover_fetcher.setProxyPort(settings->value("settings/amazon_proxy_port", 8080).toInt());
    if (settings->value("settings/amazon_proxy_auth", FALSE).toBool()) {
      cover_fetcher.setProxyUsername(settings->value("settings/amazon_proxy_user", "audex").toString());
      cover_fetcher.setProxyPassword(settings->value("settings/amazon_proxy_pass", "audex").toString());
    }
  } else {
    cover_fetcher.setProxyType(QNetworkProxy::NoProxy);
  }

  label->setText("Searching for covers...");

}

coverfromamazondialog::~coverfromamazondialog() {

}

void coverfromamazondialog::select() {
  select_this(listWidget->selectedItems().at(0));
}

void coverfromamazondialog::select_this(QListWidgetItem* item) {
  int match = item->data(Qt::UserRole).toInt();
  *image = cover_fetcher.cover(match);
  setResult(QDialog::Accepted);
  close();
}

void coverfromamazondialog::back() {
  setResult(QDialog::Rejected);
  close();
}

void coverfromamazondialog::enableSelectButton() {
  pushButton->setEnabled(listWidget->selectedItems().count() > 0);
}

void coverfromamazondialog::addItem(const QImage& cover, const QString& caption, const QString& date, int no) {
  QListWidgetItem *item = new QListWidgetItem;
  item->setText(caption);
  item->setToolTip(trUtf8("%1\nRelease: %2\nCover size: %3x%4").arg(caption).arg(date).arg(cover.width()).arg(cover.height()));
  item->setIcon(QIcon(QPixmap::fromImage(cover).scaled(128, 128, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
  item->setData(Qt::UserRole, no-1);
  listWidget->addItem(item);
  label->setText(trUtf8("Fetching cover %1 / %2...").arg(no).arg(cover_fetcher.count()));
}

void coverfromamazondialog::allFetched() {
  label->setText(trUtf8("Found %1 covers").arg(cover_fetcher.count()));
}

void coverfromamazondialog::nothingFetched() {
  label->setText(trUtf8("Found no covers"));
}

void coverfromamazondialog::slot_proxy_auth_req(const QNetworkProxy& proxy, QAuthenticator *authenticator) {
  Q_UNUSED(proxy);
  proxyauthdialog *proxyAuthDialog = new proxyauthdialog(this, settings, "amazon", trUtf8("Amazon server proxy authentication required.\nPlease log in:"), authenticator);
  proxyAuthDialog->exec();
  delete proxyAuthDialog;
}

void coverfromamazondialog::closeEvent(QCloseEvent *event) {
  settings->setValue("coverfromamazondialog/pos", pos());
  settings->setValue("coverfromamazondialog/size", size());
  event->accept();
}
