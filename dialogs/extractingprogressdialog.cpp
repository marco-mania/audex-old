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

  setCaption(i18n("Rip And Encode"));

  setButtons(Cancel);

  this->profile_model = profile_model;
  this->cdda_model = cdda_model;

  ui.label_header->setText(QString("<h2>%1 - %2</h2>").arg(cdda_model->artist()).arg(cdda_model->title()));

  ui.label_extracting->setText(i18n("Ripping Track  0 / %1", cdda_model->numOfAudioTracks()));
  ui.label_encoding->setText(i18n("Encoding Track  0 / %1", cdda_model->numOfAudioTracks()));

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

  showButtonSeparator(true);
  finished = FALSE;

  progressbar_np_flag = FALSE;
  ui.filename->setText(QString());

  if(cdda_model->numOfAudioTracksInSelection()<2) {
    ui.label_overall->setVisible(false);
    ui.progressBar_overall->setVisible(false);
  }
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

void ExtractingProgressDialog::slotButtonClicked(int button) {
  switch(button) {
    case Cancel:
      cancel();
      break;
    case Close:
      close();
      break;
    case User1:
      open_encoder_protocol_dialog();
      break;
    case User2:
      open_extract_protocol_dialog();
      break;
    default:
      KDialog::slotButtonClicked(button);
  }
}

void ExtractingProgressDialog::cancel() {

  if (finished) {

    close();

  } else {

    if (KMessageBox::warningYesNo(this, i18n("Do you really want to cancel?"),
				i18n("Cancel"),
				KStandardGuiItem::yes(),
				KStandardGuiItem::no())== KMessageBox::Yes) {
      setButtons(Close);
      audex->cancel();
    }

  }

}

void ExtractingProgressDialog::show_changed_extract_track(int no, int total, const QString& artist, const QString& title) {

  ui.label_extracting->setText(1==total ? i18n("Ripping Track") : i18n("Ripping Track %1 / %2", no, total));
  ui.label_track->setText(cdda_model->isVarious() ? QString("(%1 - %2)").arg(artist).arg(title) : QString("(%1)").arg(title));

}

void ExtractingProgressDialog::show_changed_encode_track(int no, int total, const QString& filename) {

  if (no==0) {
    ui.label_encoding->setText("<i>"+i18n("Waiting for an encoding job...")+"</i>");
    ui.label_speed_encoding->clear();
    ui.filename->setText(QString());
  } else {
    ui.label_encoding->setText(1==total ? i18n("Encoding Track") : i18n("Encoding Track %1 / %2", no, total));
    ui.filename->setText(QChar('(')+filename+QChar(')'));
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

void ExtractingProgressDialog::conclusion(bool successful) {

  QFlags<KDialog::ButtonCode> buttons=Close;

  finished = TRUE;
  QPalette pal(ui.label_extracting->palette());
  KColorScheme kcs(QPalette::Active);
  if (successful) {
    QListWidgetItem *item = new QListWidgetItem(KIcon("dialog-ok-apply"), i18n("All jobs successfully done."));
    ui.klistwidget->addItem(item);
    ui.klistwidget->scrollToItem(item);
    pal.setBrush(QPalette::Text, kcs.foreground(KColorScheme::PositiveText));
    ui.label_extracting->setText("<font style=\"font-weight:bold;\">"+i18n("Finished!")+"</font>");
    ui.label_encoding->setText("<font style=\"font-weight:bold;\">"+i18n("Finished!")+"</font>");
    ui.progressBar_extracting->setValue(100);
    ui.progressBar_encoding->setValue(100);
  } else {
    QListWidgetItem *item = new QListWidgetItem(KIcon("dialog-cancel"), i18n("At least one job failed."));
    pal.setBrush(QPalette::Text, kcs.foreground(KColorScheme::NegativeText));
    ui.klistwidget->addItem(item);
    ui.klistwidget->scrollToItem(item);
    ui.label_extracting->setText("<font style=\"color:red;font-weight:bold;\">"+i18n("Failed!")+"</font>");
    ui.label_encoding->setText("<font style=\"color:red;font-weight:bold;\">"+i18n("Failed!")+"</font>");
    if (audex->encoderProtocol().count()>0) { buttons|=User1; }
    if (audex->extractProtocol().count()>0) { buttons|=User2; }
  }
  ui.label_extracting->setPalette(pal);
  ui.label_encoding->setPalette(pal);
  setButtons(buttons);
  if (audex->encoderProtocol().count()>0) {
    setButtonText(User1, i18n("Show encoding log..."));
    setButtonIcon(User1, KIcon("media-optical-audio"));
  }
  if (audex->extractProtocol().count()>0) {
    setButtonText(User2, i18n("Show rip log..."));
    setButtonIcon(User2, KIcon("media-optical"));
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

  if (KMessageBox::questionYesNo(this, i18n("Ripping speed is extremly slow for the last 5 minutes by now.\nDue to extraction quality, audex is so configured, never skip any detected error. "
				"So if your disc is really broken extracting maybe never ends.\nIn some cases only this drive can't rip audio data from this disc. Maybe try another one.\n\n"
				"However, do you want to continue extraction?"),
				i18n("Cancel extraction"),
				KStandardGuiItem::yes(),
				KStandardGuiItem::no())== KMessageBox::Yes) {
    audex->cancel();
  }

}

void ExtractingProgressDialog::open_encoder_protocol_dialog() {
  ProtocolDialog *protocolDialog = new ProtocolDialog(audex->encoderProtocol(), i18n("Encoding protocol"), this);
  protocolDialog->exec();
  delete protocolDialog;
}

void ExtractingProgressDialog::open_extract_protocol_dialog() {
  ProtocolDialog *protocolDialog = new ProtocolDialog(audex->extractProtocol(), i18n("Ripping protocol"), this);
  protocolDialog->exec();
  delete protocolDialog;
}
