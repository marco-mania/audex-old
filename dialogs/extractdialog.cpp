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

#include "extractdialog.h"

extractdialog::extractdialog(QWidget *parent, QCDDADevice *device, QSettings *settings, QProfileModel *profile_model, QCDDAMetaDataModel *cdda_metadata_model) {

  Q_UNUSED(parent);

  setupUi(this);

  this->device = device;
  this->settings = settings;
  this->profile_model = profile_model;
  this->cdda_metadata_model = cdda_metadata_model;

  connect(pushButton, SIGNAL(clicked()), this, SLOT(cancel()));
  connect(pushButton_2, SIGNAL(clicked()), this, SLOT(open_encoder_protocol_dialog()));
  connect(pushButton_3, SIGNAL(clicked()), this, SLOT(open_extract_protocol_dialog()));
  pushButton_2->hide();
  pushButton_3->hide();

  label->setText(QString("%1 - %2").arg(cdda_metadata_model->artist()).arg(cdda_metadata_model->album()));
  QFont font = label->font();
  font.setBold(TRUE);
  label->setFont(font);

  resize(settings->value("extractdialog/size", QSize(460, 520)).toSize());
  move(settings->value("extractdialog/pos", QPoint(100, 100)).toPoint());

  label_2->setText(trUtf8("Extracting Track  0 / %1").arg(cdda_metadata_model->numOfAudioTracks()));
  label_10->setText(trUtf8("Encoding Track  0 / %1").arg(cdda_metadata_model->numOfAudioTracks()));

  audex = new QAudex(this, device, settings, profile_model, cdda_metadata_model);

  connect(audex, SIGNAL(error(const QString&, const QString&)), this, SLOT(show_error(const QString&, const QString&)));
  connect(audex, SIGNAL(warning(const QString&)), this, SLOT(show_warning(const QString&)));
  connect(audex, SIGNAL(info(const QString&)), this, SLOT(show_info(const QString&)));
  connect(audex, SIGNAL(finished(bool)), this, SLOT(conclusion(bool)));
  connect(audex, SIGNAL(speedEncode(double)), this, SLOT(show_speed_encode(double)));
  connect(audex, SIGNAL(speedExtract(double)), this, SLOT(show_speed_extract(double)));
  connect(audex, SIGNAL(progressExtractTrack(int)), this, SLOT(show_progress_extract_track(int)));
  connect(audex, SIGNAL(progressExtractOverall(int)), this, SLOT(show_progress_extract_overall(int)));
  connect(audex, SIGNAL(progressEncodeTrack(int)), this, SLOT(show_progress_encode_track(int)));
  connect(audex, SIGNAL(changedExtractTrack(int, int, const QString&, const QString&)), this, SLOT(show_changed_extract_track(int, int, const QString&, const QString&)));
  connect(audex, SIGNAL(changedEncodeTrack(int, int, const QString&)), this, SLOT(show_changed_encode_track(int, int, const QString&)));
  connect(audex, SIGNAL(timeout()), this, SLOT(ask_timeout()));
  connect(audex, SIGNAL(startDBSync()), this, SLOT(start_db_sync()));
  connect(audex, SIGNAL(finishedDBSync()), this, SLOT(finished_db_sync()));
  connect(audex, SIGNAL(longerDBSyncProgress()), this, SLOT(db_sync_longer_progress()));
  connect(audex, SIGNAL(longerDBSyncProgressFinished()), this, SLOT(db_sync_longer_progress_finished()));

  finished = FALSE;

}

extractdialog::~extractdialog() {

  delete audex;

}

int extractdialog::exec() {
  show();
  setModal(TRUE);
  if (audex->prepare()) {
    audex->start();
  }
  return QDialog::exec();
}

void extractdialog::cancel() {
  if (finished) {
    close();
  } else {
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setFont(this->font());
    msgBox.setText(trUtf8("Do you really want to cancel?"));
    QPushButton *yesButton = msgBox.addButton(trUtf8("Yes"), QMessageBox::AcceptRole);
    QPushButton *noButton = msgBox.addButton(trUtf8("No"), QMessageBox::RejectRole);
    msgBox.setDefaultButton(noButton);
    msgBox.exec();
    if (msgBox.clickedButton() == yesButton) {
      pushButton->setEnabled(FALSE);
      audex->cancel();
    }
  }
}

void extractdialog::show_changed_extract_track(int no, int total, const QString& artist, const QString& title) {
  label_2->setText("<b>"+trUtf8("Extracting Track&nbsp;&nbsp;%1 / %2").arg(no).arg(total)+"</b>");
  lineEdit_2->setText(QString("%1").arg(artist));
  lineEdit_2->setCursorPosition(0);
  lineEdit_3->setText(QString("%1").arg(title));
  lineEdit_3->setCursorPosition(0);
}

void extractdialog::show_changed_encode_track(int no, int total, const QString& filename) {
  if (no==0) {
    label_10->setText("<b>"+trUtf8("Waiting for an encoding job...")+"</b>");
    lineEdit_4->clear();
  } else {
    label_10->setText("<b>"+trUtf8("Encoding Track&nbsp;&nbsp;%1 / %2").arg(no).arg(total)+"</b>");
    lineEdit_4->setText(filename);
    lineEdit_4->setCursorPosition(0);
  }
}

void extractdialog::show_progress_extract_track(int percent) {
  progressBar->setValue(percent);
}

void extractdialog::show_progress_extract_overall(int percent) {
  progressBar_2->setValue(percent);
}

void extractdialog::show_progress_encode_track(int percent) {
  if (percent >= 0) {
    progressBar_3->setValue(percent);
  } else {
    progressBar_3->setMinimum(0);
    progressBar_3->setMaximum(0);
    progressBar_3->setValue(10);
  }
}

void extractdialog::show_speed_encode(double speed) {
  label_12->setText("<i>"+trUtf8("Speed: %1x").arg((double)speed, 0, 'f', 2)+"</i>");
}

void extractdialog::show_speed_extract(double speed) {
  label_4->setText("<i>"+trUtf8("Speed: %1x").arg((double)speed, 0, 'f', 2)+"</i>");
}

void extractdialog::start_db_sync() {
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  pushButton->setEnabled(FALSE);
}

void extractdialog::finished_db_sync() {
  QApplication::restoreOverrideCursor();
  pushButton->setEnabled(TRUE);
}

void extractdialog::db_sync_longer_progress() {
  slabel1 = label_2->text();
  slabel2 = label_3->text();
  slabel3 = label_4->text();
  label_2->setText(trUtf8("Database syncing takes some time..."));
  label_4->hide();
  label_3->setText(trUtf8("Please wait. Syncing..."));
  progressBar->setMaximum(0); progressBar->setValue(-1);
  progressBar_2->setMaximum(0); progressBar_2->setValue(-1);
  label_7->setEnabled(FALSE);
  lineEdit_2->setEnabled(FALSE);
  label_9->setEnabled(FALSE);
  lineEdit_3->setEnabled(FALSE);
}

void extractdialog::db_sync_longer_progress_finished() {
  label_2->setText(slabel1);
  label_3->setText(slabel2);
  label_4->show();
  progressBar->setMaximum(100); progressBar->setValue(100);
  progressBar_2->setMaximum(100); progressBar_2->setValue(100);
  label_7->setEnabled(TRUE);
  lineEdit_2->setEnabled(TRUE);
  label_9->setEnabled(TRUE);
  lineEdit_3->setEnabled(TRUE);
}

void extractdialog::conclusion(bool successful) {
  pushButton->setEnabled(TRUE);
  pushButton->setText(trUtf8("Close"));
  if (audex->encoderProtocol().count()>0) { pushButton_2->show(); pushButton_3->show(); }
  finished = TRUE;
  if (successful) {
    QIcon *icon = new QIcon(":/buttons/images/success.png");
    QListWidgetItem *item = new QListWidgetItem(*icon, trUtf8("All jobs successfully done."));
    listWidget->addItem(item);
    listWidget->scrollToItem(item);
    label_2->setText("<b><font color=green>"+trUtf8("Finished!")+"</font></b>");
    label_10->setText("<b><font color=green>"+trUtf8("Finished!")+"</font></b>");
    progressBar->setValue(100);
    progressBar_2->setValue(100);
  } else {
    QIcon *icon = new QIcon(":/buttons/images/fail.png");
    QListWidgetItem *item = new QListWidgetItem(*icon, trUtf8("At least one job failed."));
    listWidget->addItem(item);
    listWidget->scrollToItem(item);
    label_2->setText("<b><font color=red>"+trUtf8("Failed!")+"</font></b>");
    label_10->setText("<b><font color=red>"+trUtf8("Failed!")+"</font></b>");
  }
}

void extractdialog::show_info(const QString& description) {

  QIcon *icon = new QIcon(":/buttons/images/info.png");
  QListWidgetItem *item = new QListWidgetItem(*icon, description);
  listWidget->addItem(item);
  listWidget->scrollToItem(item);

}

void extractdialog::show_warning(const QString& description) {

  QIcon *icon = new QIcon(":/buttons/images/warning.png");
  QListWidgetItem *item = new QListWidgetItem(*icon, description);
  listWidget->addItem(item);
  listWidget->scrollToItem(item);

}

void extractdialog::show_error(const QString& description, const QString& solution) {

  QIcon *icon = new QIcon(":/buttons/images/error.png");
  QListWidgetItem *item;
  if (solution.isEmpty()) {
    item = new QListWidgetItem(*icon, QString("%1").arg(description));
  } else {
    item = new QListWidgetItem(*icon, QString("%1 (%2)").arg(description).arg(solution));
  }
  listWidget->addItem(item);
  listWidget->scrollToItem(item);

}

void extractdialog::ask_timeout() {

  QMessageBox msgBox;
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setFont(this->font());
  msgBox.setText(trUtf8("Extraction speed is extremly slow for the last 5 minutes by now.\nDue to extraction quality, audex is so configured, never skip any detected error. "
	"So if your disc is really broken extracting maybe never ends.\nIn some cases only this drive can't extract audio data from this disc. Maybe try another one.\n\n"
	"However, do you want to continue extraction?"));
  msgBox.addButton(trUtf8("Yes"), QMessageBox::AcceptRole);
  QPushButton *noButton = msgBox.addButton(trUtf8("No"), QMessageBox::RejectRole);
  msgBox.exec();
  if (msgBox.clickedButton() == noButton) {
    pushButton->setEnabled(FALSE);
    audex->cancel();
  }

}

void extractdialog::open_encoder_protocol_dialog() {
  protocoldialog *protocolDialog = new protocoldialog(this, settings, trUtf8("Encoding protocol"), audex->encoderProtocol());
  protocolDialog->exec();
  delete protocolDialog;
}

void extractdialog::open_extract_protocol_dialog() {
  protocoldialog *protocolDialog = new protocoldialog(this, settings, trUtf8("Extracting protocol"), audex->extractProtocol());
  protocolDialog->exec();
  delete protocolDialog;
}

void extractdialog::closeEvent(QCloseEvent *event) {
  settings->setValue("extractdialog/pos", pos());
  settings->setValue("extractdialog/size", size());
  event->accept();
}

