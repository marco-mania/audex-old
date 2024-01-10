/* AUDEX CDDA EXTRACTOR
 * Copyright (C) 2007-2009 by Marco Nelles (audex@maniatek.de)
 * http://opensource.maniatek.de/audex
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

#include "extractingprogressdialog.h"

ExtractingProgressDialog::ExtractingProgressDialog(ProfileModel *profile_model, CDDAModel *cdda_model, QWidget *parent) : KDialog(parent) {

  Q_UNUSED(parent);

  QWidget *widget = new QWidget(this);
  ui.setupUi(widget);

  setMainWidget(widget);

  setCaption(i18n("Extracting Progress"));

  setButtons(None);

  this->profile_model = profile_model;
  this->cdda_model = cdda_model;

  connect(ui.kpushbutton_leave, SIGNAL(clicked()), this, SLOT(cancel()));
  connect(ui.kpushbutton_encoding_protocol, SIGNAL(clicked()), this, SLOT(open_encoder_protocol_dialog()));
  connect(ui.kpushbutton_extracting_protocol, SIGNAL(clicked()), this, SLOT(open_extract_protocol_dialog()));
  ui.kpushbutton_encoding_protocol->hide();
  ui.kpushbutton_extracting_protocol->hide();

  QFont font = ui.label_header->font();
  font.setBold(TRUE);
  ui.label_header->setFont(font);
  ui.label_header->setText(QString("%1 - %2").arg(cdda_model->artist()).arg(cdda_model->title()));

  ui.label_extracting->setText(trUtf8("Extracting Track  0 / %1").arg(cdda_model->numOfAudioTracks()));
  ui.label_encoding->setText(trUtf8("Encoding Track  0 / %1").arg(cdda_model->numOfAudioTracks()));

  audex = new Audex(this, profile_model, cdda_model);

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
  /*connect(audex, SIGNAL(ftpUploadStart(const QString&)), this, SLOT(start_ftp_file_transfer(const QString&)));
  connect(audex, SIGNAL(ftpUploadProgress(const int, const int)), this, SLOT(progress_ftp_file_transfer(const int, const int)));
  connect(audex, SIGNAL(ftpUploadFinished(const QString&)), this, SLOT(finished_ftp_file_transfer(const QString&)));*/

  finished = FALSE;

  progressbar_np_flag = FALSE;

}

ExtractingProgressDialog::~ExtractingProgressDialog() {

  delete audex;

}

int ExtractingProgressDialog::exec() {

  show();
  setModal(TRUE);
  if (audex->prepare()) {
    audex->start();
  }
  return KDialog::exec();

}

void ExtractingProgressDialog::cancel() {

  if (finished) {

    close();

  } else {

    if (KMessageBox::warningYesNo(this, i18n("Do you really want to cancel?"),
				i18n("Cancel"),
				KStandardGuiItem::yes(),
				KStandardGuiItem::no())== KMessageBox::Yes) {
      ui.kpushbutton_leave->setEnabled(FALSE);
      audex->cancel();
    }

  }

}

void ExtractingProgressDialog::show_changed_extract_track(int no, int total, const QString& artist, const QString& title) {

  ui.label_extracting->setText("<b>"+i18n("Extracting Track")+QString("&nbsp;&nbsp;%1 / %2").arg(no).arg(total)+"</b>");
  ui.klineedit_artist->setText(QString("%1").arg(artist));
  //ui.klineedit_artist->setCursorPosition(0);
  ui.klineedit_title->setText(QString("%1").arg(title));
  //ui.klineedit_title->setCursorPosition(0);

}

void ExtractingProgressDialog::show_changed_encode_track(int no, int total, const QString& filename) {

  if (no==0) {
    ui.label_encoding->setText("<b>"+i18n("Waiting for an encoding job...")+"</b>");
    ui.label_speed_encoding->clear();
  } else {
    ui.label_encoding->setText("<b>"+i18n("Encoding Track")+QString("&nbsp;&nbsp;%1 / %2").arg(no).arg(total)+"</b>");
    ui.klineedit_filename->setText(filename);
    //ui.klineedit_filename->setCursorPosition(0);
  }

}

void ExtractingProgressDialog::show_progress_extract_track(int percent) {

  ui.progressBar_extracting->setValue(percent);

}

void ExtractingProgressDialog::show_progress_extract_overall(int percent) {

  ui.progressBar_overall->setValue(percent);

}

void ExtractingProgressDialog::show_progress_encode_track(int percent) {

  //ui.progressBar_encoding->setValue(percent);

  if (percent >= 0) {
    ui.progressBar_encoding->setValue(percent);
    if (progressbar_np_flag) {
      ui.progressBar_encoding->setRange(0, 100);
      ui.progressBar_encoding->setTextVisible(TRUE);
      progressbar_np_flag = FALSE;
    }
  } else {
    if (!progressbar_np_flag) {
      progressbar_np_flag = TRUE;
      ui.progressBar_encoding->setRange(0, 0);
      ui.progressBar_encoding->setTextVisible(FALSE);
    }
  }

}

void ExtractingProgressDialog::show_speed_encode(double speed) {

  QString s = QString("%1").arg((double)speed, 0, 'f', 2);
  ui.label_speed_encoding->setText("<i>"+i18n("Speed: %1x", s)+"</i>");

}

void ExtractingProgressDialog::show_speed_extract(double speed) {

  QString s = QString("%1").arg((double)speed, 0, 'f', 2);
  ui.label_speed_extracting->setText("<i>"+i18n("Speed: %1x", s)+"</i>");

}

/*void ExtractingProgressDialog::start_ftp_file_transfer(const QString& filename) {
  show_info(QString("Uploading file %1 to ftp server. Please wait...").arg(filename));
  QIcon *icon = new QIcon(":/buttons/images/info.png");
  QListWidgetItem *item = new QListWidgetItem(*icon, "");
  listWidget->addItem(item);
  progress_bar = new QProgressBar(listWidget);
  progress_bar->setValue(0);
  progress_bar->setMinimum(0);
  progress_bar->setMaximum(100);
  progress_bar->setMaximumSize(200, 18);
  listWidget->setItemWidget(item, progress_bar);
  listWidget->scrollToItem(item);
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  pushButton->setEnabled(FALSE);
}

void ExtractingProgressDialog::progress_ftp_file_transfer(const int percent, const int overall) {
  Q_UNUSED(overall);
  progress_bar->setValue(percent);
}

void ExtractingProgressDialog::finished_ftp_file_transfer(const QString& filename) {
  show_info(QString("Finished uploading file %1 to ftp server.").arg(filename));
  QApplication::restoreOverrideCursor();
  pushButton->setEnabled(TRUE);
}*/

void ExtractingProgressDialog::conclusion(bool successful) {

  ui.kpushbutton_leave->setEnabled(TRUE);
  ui.kpushbutton_leave->setText(i18n("Close"));
  if (audex->encoderProtocol().count()>0) { ui.kpushbutton_encoding_protocol->show(); }
  if (audex->extractProtocol().count()>0) { ui.kpushbutton_extracting_protocol->show(); }
  finished = TRUE;
  if (successful) {
    QListWidgetItem *item = new QListWidgetItem(KIcon("dialog-ok-apply"), i18n("All jobs successfully done."));
    ui.klistwidget->addItem(item);
    ui.klistwidget->scrollToItem(item);
    ui.label_extracting->setText("<b><font color=green>"+i18n("Finished!")+"</font></b>");
    ui.label_encoding->setText("<b><font color=green>"+i18n("Finished!")+"</font></b>");
    ui.progressBar_extracting->setValue(100);
    ui.progressBar_encoding->setValue(100);
  } else {
    QListWidgetItem *item = new QListWidgetItem(KIcon("dialog-cancel"), i18n("At least one job failed."));
    ui.klistwidget->addItem(item);
    ui.klistwidget->scrollToItem(item);
    ui.label_extracting->setText("<b><font color=red>"+i18n("Failed!")+"</font></b>");
    ui.label_encoding->setText("<b><font color=red>"+i18n("Failed!")+"</font></b>");
  }

}

void ExtractingProgressDialog::show_info(const QString& message) {

  QListWidgetItem *item = new QListWidgetItem(KIcon("dialog-information"), message);
  ui.klistwidget->addItem(item);
  ui.klistwidget->scrollToItem(item);

}

void ExtractingProgressDialog::show_warning(const QString& message) {

  QListWidgetItem *item = new QListWidgetItem(KIcon("dialog-warning"), message);
  ui.klistwidget->addItem(item);
  ui.klistwidget->scrollToItem(item);

}

void ExtractingProgressDialog::show_error(const QString& message, const QString& details) {

  QListWidgetItem *item;
  if (details.isEmpty()) {
    item = new QListWidgetItem(KIcon("dialog-error"), QString("%1").arg(message));
  } else {
    item = new QListWidgetItem(KIcon("dialog-error"), QString("%1 (%2)").arg(message).arg(details));
  }
  ui.klistwidget->addItem(item);
  ui.klistwidget->scrollToItem(item);

}

void ExtractingProgressDialog::ask_timeout() {

  if (KMessageBox::questionYesNo(this, i18n("Extraction speed is extremly slow for the last 5 minutes by now.\nDue to extraction quality, audex is so configured, never skip any detected error. "
				"So if your disc is really broken extracting maybe never ends.\nIn some cases only this drive can't extract audio data from this disc. Maybe try another one.\n\n"
				"However, do you want to continue extraction?"),
				i18n("Cancel extraction"),
				KStandardGuiItem::yes(),
				KStandardGuiItem::no())== KMessageBox::Yes) {
    ui.kpushbutton_leave->setEnabled(FALSE);
    audex->cancel();
  }

}

void ExtractingProgressDialog::open_encoder_protocol_dialog() {
  ProtocolDialog *protocolDialog = new ProtocolDialog(audex->encoderProtocol(), trUtf8("Encoding protocol"), this);
  protocolDialog->exec();
  delete protocolDialog;
}

void ExtractingProgressDialog::open_extract_protocol_dialog() {
  ProtocolDialog *protocolDialog = new ProtocolDialog(audex->extractProtocol(), trUtf8("Extracting protocol"), this);
  protocolDialog->exec();
  delete protocolDialog;
}
