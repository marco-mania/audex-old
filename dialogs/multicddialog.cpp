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

#include "multicddialog.h"

void FillTreeViewThread::run() {

  to_stop_mutex.lock();
  to_stop = FALSE;
  to_stop_mutex.unlock();

  QSqlQuery query = audex_sql->getAllDiscsRipped(days);

  bool stop = FALSE;
  while (query.next() && !stop) {

    to_stop_mutex.lock();
    stop = to_stop;
    to_stop_mutex.unlock();

    QString artistortitle = query.value(2).toString();
    QString titleorsubtitle = query.value(3).toString();
    int cdnum_id = query.value(0).toInt();

    Compilation comp;
    comp.title = QString("%1 - %2").arg(artistortitle).arg(titleorsubtitle);
    comp.cdnum_id = cdnum_id;

    QSqlQuery query2 = audex_sql->getAllDiscsByCDNumID(cdnum_id);
    while (query2.next()) {
      quint32 discid = query2.value(1).toUInt();
      int cdnum = query2.value(6).toInt();
      QDate added_on = query2.value(3).toDate();

      CD cd;
      cd.title = QString("CD number %1 (added on: %2)").arg(cdnum).arg(added_on.toString("yyyy-MM-dd"));
      cd.cdnum_id = cdnum_id;

      QSqlQuery query3 = audex_sql->getAllTracksByDiscid(discid);
      while (query3.next()) {
        QString title = query3.value(1).toString();
        QString artist = query3.value(2).toString();
        cd.titles.append(QString("%1 - %2").arg(artist).arg(title));
      }

      comp.cds.append(cd);

    }

    emit new_item(comp);

  }

  error = FALSE;

}

void FillTreeViewThread::stop() {
  to_stop_mutex.lock();
  to_stop = TRUE;
  to_stop_mutex.unlock();
}

multicddialog::multicddialog(QWidget *parent, QSettings *settings, QAudexSQL *audex_sql, QCDDAMetaDataModel *cdda_metadata_model) {

  Q_UNUSED(parent);

  setupUi(this);

  this->settings = settings;
  this->audex_sql = audex_sql;
  this->cdda_metadata_model = cdda_metadata_model;
  cdda_metadata_model->setCustomData("cdnum_id", -1);

  qRegisterMetaType<Compilation>("Compilation");
  connect(&fill_tree_view_thread, SIGNAL(new_item(const Compilation&)), this, SLOT(set_new_item(const Compilation&)));

  connect(pushButton, SIGNAL(clicked()), this, SLOT(extract()));
  connect(pushButton_2, SIGNAL(clicked()), this, SLOT(cancel()));

  treeWidget->header()->hide();
  if (audex_sql) {
    connect(treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(enableExtractButton()));
    connect(treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(change_view_index()));
    comboBox->addItem(trUtf8("today"));
    comboBox->addItem(trUtf8("last 2 days"));
    comboBox->addItem(trUtf8("last week"));
    comboBox->addItem(trUtf8("last month"));
    comboBox->addItem(trUtf8("last year"));
    comboBox->addItem(trUtf8("all"));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(change_view_era(int)));
    comboBox->setCurrentIndex(settings->value("multicddialog/view_index", 0).toInt());
    change_view_era(settings->value("multicddialog/view_index", 0).toInt());
    connect(checkBox, SIGNAL(clicked(bool)), this, SLOT(toggle_first(bool)));
    connect(checkBox, SIGNAL(clicked(bool)), this, SLOT(enableExtractButton()));
    pushButton->setEnabled(FALSE);
  } else {
    update_cdnum(1); spinBox_2->setValue(1);
    label->setEnabled(FALSE);
    treeWidget->setEnabled(FALSE);
    label_2->setEnabled(FALSE);
    comboBox->setEnabled(FALSE);
    checkBox->setEnabled(FALSE);
  }

  connect(spinBox_2, SIGNAL(valueChanged(int)), this, SLOT(update_cdnum(int)));
  connect(checkBox_2, SIGNAL(clicked(bool)), this, SLOT(toggle_track_offset(bool)));
  connect(spinBox, SIGNAL(valueChanged(int)), this, SLOT(update_track_offset(int)));
  spinBox_2->setValue(1);

  resize(settings->value("multicddialog/size", QSize(502, 459)).toSize());
  move(settings->value("multicddialog/pos", QPoint(100, 100)).toPoint());

}

multicddialog::~multicddialog() {

}

void multicddialog::extract() {
  if (audex_sql) if (!checkBox->isChecked()) {
    if (exclude_list.contains(cdda_metadata_model->cdNum())) {
      errordialog *errorDialog = new errordialog(this, settings,
	trUtf8("You can't set the cd number to an already existings value (%1) in this mulit-cd album/sampler.").arg(cdda_metadata_model->cdNum()),
	trUtf8("Check your cd number. If there is a wrong entry in the database call your database administrator."));
      errorDialog->exec();
      delete errorDialog;
      return;
    }
    cdda_metadata_model->setCustomData("cdnum_id", treeWidget->selectedItems().at(0)->data(0, Qt::UserRole).toInt());
  }
  stop_thread();
  setResult(QDialog::Accepted);
  close();
}

void multicddialog::cancel() {
  stop_thread();
  setResult(QDialog::Rejected);
  close();
}

void multicddialog::toggle_first(bool checked) {
  if (checked) update_cdnum(1);
  label->setEnabled(!checked);
  treeWidget->setEnabled(!checked);
  label_2->setEnabled(!checked);
  comboBox->setEnabled(!checked);
}

void multicddialog::update_cdnum(int cdnum) {
  cdda_metadata_model->setCDNum(cdnum);
}

void multicddialog::toggle_track_offset(bool checked) {
  if (!checked) spinBox->setValue(0);
  spinBox->setEnabled(checked);
}

void multicddialog::update_track_offset(int offset) {
  cdda_metadata_model->setTrackOffset(offset);
}

void multicddialog::change_view_era(int index) {
  switch (index) {
    case 0 : fill_tree_view(0); break;
    case 1 : fill_tree_view(2); break;
    case 2 : fill_tree_view(7); break;
    case 3 : fill_tree_view(31); break;
    case 4 : fill_tree_view(365); break;
    case 5 : fill_tree_view(-1); break;
  }
  settings->setValue("multicddialog/view_index", index);
}

void multicddialog::change_view_index() {
  if (treeWidget->selectedItems().count()==0) return;
  int cdnum_id = treeWidget->selectedItems().at(0)->data(0, Qt::UserRole).toInt();
  QSqlQuery query = audex_sql->getAllDiscsByCDNumID(cdnum_id);
  exclude_list.clear();
  int next_cdnum = 1;
  while (query.next()) {
    int cdnum = query.value(6).toInt();
    if (cdnum==next_cdnum) next_cdnum++;
    exclude_list.append(cdnum);
  }
  spinBox_2->setValue(next_cdnum);
  update_cdnum(next_cdnum);
}

void multicddialog::enableExtractButton() {
  pushButton->setEnabled((treeWidget->selectedItems().count() > 0) || (checkBox->isChecked()));
}

void multicddialog::closeEvent(QCloseEvent *event) {
  settings->setValue("multicddialog/pos", pos());
  settings->setValue("multicddialog/size", size());
  event->accept();
}

void multicddialog::set_new_item(const Compilation& compilation) {

  QTreeWidgetItem *item = new QTreeWidgetItem(treeWidget);
  item->setData(0, Qt::DisplayRole, compilation.title);
  item->setData(0, Qt::UserRole, compilation.cdnum_id);
  item->setData(0, Qt::DecorationRole, QIcon(":/buttons/images/cdaudio.png"));

  for (int i = 0; i < compilation.cds.count(); i++) {
    QTreeWidgetItem *child = new QTreeWidgetItem(item);
    child->setData(0, Qt::DisplayRole, compilation.cds.at(i).title);
    child->setData(0, Qt::UserRole, compilation.cdnum_id);
    child->setData(0, Qt::DecorationRole, QIcon(":/buttons/images/cdaudio.png"));
    for (int j = 0; j < compilation.cds.at(i).titles.count(); j++) {
      QTreeWidgetItem *grandchild = new QTreeWidgetItem(child);
      grandchild->setData(0, Qt::DisplayRole, QString("%1. %2").arg(j+1).arg(compilation.cds.at(i).titles.at(j)));
      grandchild->setData(0, Qt::UserRole, compilation.cdnum_id);
      grandchild->setData(0, Qt::DecorationRole, QIcon(":/buttons/images/cdtrack.png"));
    }
  }

  treeWidget->addTopLevelItem(item);

}

void multicddialog::fill_tree_view(int days) {

  fill_tree_view_thread.days = days;
  fill_tree_view_thread.audex_sql = audex_sql;

  stop_thread();

  treeWidget->clear();

  fill_tree_view_thread.start();

}

void multicddialog::stop_thread() {
  if (fill_tree_view_thread.isRunning()) {
    fill_tree_view_thread.stop();
    while (fill_tree_view_thread.isRunning()) {
      Wait::usleep(100);
      QCoreApplication::processEvents();
    }
  }
}
