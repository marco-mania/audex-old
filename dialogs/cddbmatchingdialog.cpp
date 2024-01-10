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

#include "cddbmatchingdialog.h"

cddbmatchingdialog::cddbmatchingdialog(QWidget *parent, QSettings *settings, const QCDDA_HeaderMetaDataList& metadataheaderlist, int *match) {

  if (!match) return;

  Q_UNUSED(parent);

  setupUi(this);

  this->settings = settings;
  this->match = match;

  connect(pushButton, SIGNAL(clicked()), this, SLOT(select()));
  connect(pushButton_2, SIGNAL(clicked()), this, SLOT(back()));

  this->metadataheaderlist = metadataheaderlist;

  for (int i = 0; i < metadataheaderlist.count(); i++) {

    QTreeWidgetItem *parent = new QTreeWidgetItem(treeWidget);
    parent->setData(0, Qt::DisplayRole, trUtf8("%1 - %2 (category : %3)").arg(metadataheaderlist.at(i).get(QCDDA_HeaderMetaData::Artist).toString())
	.arg(metadataheaderlist[i].get(QCDDA_HeaderMetaData::Title).toString())
	.arg(metadataheaderlist[i].get(QCDDA_HeaderMetaData::Category).toString()));
    parent->setData(0, Qt::UserRole, i);
    parent->setIcon(0, QIcon(":/buttons/images/cdaudio.png"));

  }

  treeWidget->header()->hide();

  connect(treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(enableSelectButton()));
  enableSelectButton();

  resize(settings->value("cddbmatchingdialog/size", QSize(460, 520)).toSize());
  move(settings->value("cddbmatchingdialog/pos", QPoint(100, 100)).toPoint());

}

cddbmatchingdialog::~cddbmatchingdialog() {

}

void cddbmatchingdialog::select() {
  *match = treeWidget->selectedItems().at(0)->data(0, Qt::UserRole).toInt();
  setResult(QDialog::Accepted);
  close();
}

void cddbmatchingdialog::back() {
  setResult(QDialog::Rejected);
  close();
}

void cddbmatchingdialog::enableSelectButton() {
  pushButton->setEnabled(treeWidget->selectedItems().count() > 0);
}

void cddbmatchingdialog::closeEvent(QCloseEvent *event) {
  settings->setValue("cddbmatchingdialog/pos", pos());
  settings->setValue("cddbmatchingdialog/size", size());
  event->accept();
}
