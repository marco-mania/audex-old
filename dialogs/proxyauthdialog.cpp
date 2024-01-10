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

#include "proxyauthdialog.h"

proxyauthdialog::proxyauthdialog(QWidget *parent, QSettings *settings, const QString& prefix, const QString& message, QAuthenticator *authenticator) {

  Q_UNUSED(parent);

  setupUi(this);

  this->settings = settings;
  this->prefix = prefix;
  this->authenticator = authenticator;

  connect(buttonBox, SIGNAL(accepted()), this, SLOT(ok()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(cancel()));

  if (message.isEmpty()) {
    label_2->setText(trUtf8("Proxy authentication required.\nPlease log in:"));
  } else {
    label_2->setText(message);
  }

  checkBox->setChecked(settings->value("proxyauthdialog/save_account", FALSE).toBool());

  move(settings->value("proxyauthdialog/pos", QPoint(100, 100)).toPoint());

}

proxyauthdialog::~proxyauthdialog() {

}

void proxyauthdialog::ok() {
  authenticator->setUser(lineEdit->text());
  authenticator->setPassword(lineEdit_2->text());
  settings->setValue("proxyauthdialog/save_account", checkBox->isChecked());
  if (checkBox->isChecked()) {
    settings->setValue(QString("settings/%1_proxy_user").arg(prefix), lineEdit->text());
    settings->setValue(QString("settings/%1_proxy_pass").arg(prefix), lineEdit_2->text());
  }
  setResult(QDialog::Accepted);
  close();
}

void proxyauthdialog::cancel() {
  setResult(QDialog::Rejected);
  close();
}

void proxyauthdialog::closeEvent(QCloseEvent *event) {
  settings->setValue("proxyauthdialog/pos", pos());
  event->accept();
}
