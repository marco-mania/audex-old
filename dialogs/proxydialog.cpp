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

#include "proxydialog.h"

proxydialog::proxydialog(QWidget *parent, QSettings *settings, const QString& prefix) {

  Q_UNUSED(parent);

  setupUi(this);

  this->settings = settings;
  this->prefix = prefix;

  connect(pushButton, SIGNAL(clicked()), this, SLOT(close()));

  comboBox->setCurrentIndex(settings->value(QString("settings/%1_proxy_type").arg(prefix), 0).toInt());
  connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(change_type(int)));
  connect(lineEdit, SIGNAL(editingFinished()), this, SLOT(change_host()));
  lineEdit->setText(settings->value(QString("settings/%1_proxy_host").arg(prefix), "www.proxy.com").toString());
  connect(spinBox, SIGNAL(editingFinished()), this, SLOT(change_port()));
  spinBox->setValue(settings->value(QString("settings/%1_proxy_port").arg(prefix), 8080).toInt());
  connect(lineEdit_2, SIGNAL(editingFinished()), this, SLOT(change_user()));
  lineEdit_2->setText(settings->value(QString("settings/%1_proxy_user").arg(prefix), "audex").toString());
  connect(lineEdit_3, SIGNAL(editingFinished()), this, SLOT(change_pass()));
  lineEdit_3->setText(settings->value(QString("settings/%1_proxy_pass").arg(prefix), "audex").toString());

  checkBox->setChecked(settings->value(QString("settings/%1_proxy_auth").arg(prefix), FALSE).toBool());
  connect(checkBox, SIGNAL(toggled(bool)), this, SLOT(enable_auth(bool)));
  enable_auth(checkBox->isChecked());

  resize(settings->value(QString("%1proxydialog/size").arg(prefix), QSize(390, 175)).toSize());
  move(settings->value(QString("%1proxydialog/pos").arg(prefix), QPoint(100, 100)).toPoint());

}

proxydialog::~proxydialog() {

}

void proxydialog::change_type(int type) {
  settings->setValue(QString("settings/%1_proxy_type").arg(prefix), type);
}

void proxydialog::change_host() {
  settings->setValue(QString("settings/%1_proxy_host").arg(prefix), lineEdit->text());
}

void proxydialog::change_port() {
  settings->setValue(QString("settings/%1_proxy_port").arg(prefix), spinBox->value());
}

void proxydialog::change_user() {
  settings->setValue(QString("settings/%1_proxy_user").arg(prefix), lineEdit_2->text());
}

void proxydialog::change_pass() {
  settings->setValue(QString("settings/%1_proxy_pass").arg(prefix), lineEdit_3->text());
}

void proxydialog::enable_auth(bool enable) {
  settings->setValue(QString("settings/%1_proxy_auth").arg(prefix), enable);
  label_3->setEnabled(enable);
  lineEdit_2->setEnabled(enable);
  label_4->setEnabled(enable);
  lineEdit_3->setEnabled(enable);
}

void proxydialog::closeEvent(QCloseEvent *event) {
  settings->setValue(QString("%1proxydialog/pos").arg(prefix), pos());
  settings->setValue(QString("%1proxydialog/size").arg(prefix), size());
  event->accept();
}
