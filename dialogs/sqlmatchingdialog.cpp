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

#include "sqlmatchingdialog.h"

sqlmatchingdialog::sqlmatchingdialog(QWidget *parent, QSettings *settings, QSqlQuery *query) {

  Q_UNUSED(parent);

  setupUi(this);

  this->settings = settings;

  connect(pushButton, SIGNAL(clicked()), this, SLOT(_continue()));
  connect(pushButton_2, SIGNAL(clicked()), this, SLOT(cancel()));

  this->query = query;

  while (query->next()) {
    QString artistortitle = query->value(4).toString();
    QString albumorsubtitle = query->value(5).toString();
    QString contributor = query->value(2).toString();
    QDate addedon = query->value(3).toDate();
    QListWidgetItem *newItem = new QListWidgetItem;
    newItem->setText(QString("\"%1\", \"%2\" submitted by %3 on %4").arg(artistortitle).arg(albumorsubtitle).arg(contributor).arg(addedon.toString("yyyy-MM-dd")));
    newItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    listWidget->addItem(newItem);
  }

  resize(settings->value("sqlmatchingdialog/size", QSize(490, 340)).toSize());
  move(settings->value("sqlmatchingdialog/pos", QPoint(100, 100)).toPoint());

}

sqlmatchingdialog::~sqlmatchingdialog() {

}

void sqlmatchingdialog::_continue() {
  setResult(QDialog::Accepted);
  close();
}

void sqlmatchingdialog::cancel() {
  setResult(QDialog::Rejected);
  close();
}

void sqlmatchingdialog::closeEvent(QCloseEvent *event) {
  settings->setValue("sqlmatchingdialog/pos", pos());
  settings->setValue("sqlmatchingdialog/size", size());
  event->accept();
}
