/* ADEX CDDA EXTRACTOR
 * Copyright (C) 2007-2008 by Marco Nelles (marcomaniac@gmx.de)
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

#include "commandwizarddialog.h"

CommandWizardDialog::CommandWizardDialog(const QString& command, QWidget *parent) : KDialog(parent) {

  Q_UNUSED(parent);

  QWidget *widget = new QWidget(this);
  ui.setupUi(widget);

  setMainWidget(widget);

  setCaption(i18n("Command Mask Wizard"));

  setButtons(KDialog::Ok | KDialog::Cancel | KDialog::Apply);

  ui.klineedit_command->setText(command);
  connect(ui.klineedit_command, SIGNAL(textEdited(const QString&)), this, SLOT(trigger_changed()));
  connect(ui.klineedit_command, SIGNAL(textChanged(const QString&)), this, SLOT(update_example()));
  ui.klineedit_command->setCursorPosition(0);

  connect(ui.kpushbutton_albumartist, SIGNAL(clicked()), this, SLOT(insAlbumArtist()));
  connect(ui.kpushbutton_albumtitle, SIGNAL(clicked()), this, SLOT(insAlbumTitle()));
  connect(ui.kpushbutton_trackartist, SIGNAL(clicked()), this, SLOT(insTrackArtist()));
  connect(ui.kpushbutton_tracktitle, SIGNAL(clicked()), this, SLOT(insTrackTitle()));
  connect(ui.kpushbutton_trackno, SIGNAL(clicked()), this, SLOT(insTrackNo()));
  connect(ui.kpushbutton_cdno, SIGNAL(clicked()), this, SLOT(insCDNo()));
  connect(ui.kpushbutton_date, SIGNAL(clicked()), this, SLOT(insDate()));
  connect(ui.kpushbutton_genre, SIGNAL(clicked()), this, SLOT(insGenre()));
  connect(ui.kpushbutton_input_file, SIGNAL(clicked()), this, SLOT(insInFile()));
  connect(ui.kpushbutton_output_file, SIGNAL(clicked()), this, SLOT(insOutFile()));

  this->command = command;

  enableButtonApply(FALSE);

  update_example();

}

CommandWizardDialog::~CommandWizardDialog() {

}

void CommandWizardDialog::slotButtonClicked(int button) {
  if (button == KDialog::Ok) {
    save();
    accept();
  } else if (button == KDialog::Apply) {
    save();
  } else {
    KDialog::slotButtonClicked(button);
  }
}

void CommandWizardDialog::trigger_changed() {
  if (ui.klineedit_command->text() != command) { enableButtonApply(TRUE); return; }
  enableButtonApply(FALSE);
}

void CommandWizardDialog::insAlbumArtist() {
  QString text = ui.klineedit_command->text();
  text.insert(ui.klineedit_command->cursorPosition(), "<"+QString(TAG_ALBUM_ARTIST)+" />");
  ui.klineedit_command->setText(text);
  update_example();
}

void CommandWizardDialog::insAlbumTitle() {
  QString text = ui.klineedit_command->text();
  text.insert(ui.klineedit_command->cursorPosition(), "<"+QString(TAG_ALBUM_TITLE)+" />");
  ui.klineedit_command->setText(text);
  update_example();
}

void CommandWizardDialog::insTrackArtist() {
  QString text = ui.klineedit_command->text();
  text.insert(ui.klineedit_command->cursorPosition(), "<"+QString(TAG_TRACK_ARTIST)+" />");
  ui.klineedit_command->setText(text);
  update_example();
}

void CommandWizardDialog::insTrackTitle() {
  QString text = ui.klineedit_command->text();
  text.insert(ui.klineedit_command->cursorPosition(), "<"+QString(TAG_TRACK_TITLE)+" />");
  ui.klineedit_command->setText(text);
  update_example();
}

void CommandWizardDialog::insTrackNo() {
  QString text = ui.klineedit_command->text();
  text.insert(ui.klineedit_command->cursorPosition(), "<"+QString(TAG_TRACK_NO)+" />");
  ui.klineedit_command->setText(text);
  update_example();
}

void CommandWizardDialog::insCDNo() {
  QString text = ui.klineedit_command->text();
  text.insert(ui.klineedit_command->cursorPosition(), "<"+QString(TAG_CDNO)+" />");
  ui.klineedit_command->setText(text);
  update_example();
}

void CommandWizardDialog::insDate() {
  QString text = ui.klineedit_command->text();
  text.insert(ui.klineedit_command->cursorPosition(), "<"+QString(TAG_DATE)+" />");
  ui.klineedit_command->setText(text);
  update_example();
}

void CommandWizardDialog::insGenre() {
  QString text = ui.klineedit_command->text();
  text.insert(ui.klineedit_command->cursorPosition(), "<"+QString(TAG_GENRE)+" />");
  ui.klineedit_command->setText(text);
  update_example();
}

void CommandWizardDialog::insInFile() {
  QString text = ui.klineedit_command->text();
  text.insert(ui.klineedit_command->cursorPosition(), "<"+QString(TAG_INPUT_FILE)+" />");
  ui.klineedit_command->setText(text);
  update_example();
}

void CommandWizardDialog::insOutFile() {
  QString text = ui.klineedit_command->text();
  text.insert(ui.klineedit_command->cursorPosition(), "<"+QString(TAG_OUTPUT_FILE)+" />");
  ui.klineedit_command->setText(text);
  update_example();
}

bool CommandWizardDialog::save() {
  command = ui.klineedit_command->text();
  enableButtonApply(FALSE);
  return TRUE;
}

void CommandWizardDialog::update_example() {
  MaskParser maskparser;
  QString filename = maskparser.parseCommandMask(ui.klineedit_command->text(),
	"/tmp/tmp.wav", QString("%1/music/Meat Loaf/02 - Meat Loaf - Blind As A Bat.ogg").arg(QDir::homePath()),
	2, 1, 1,
	"Meat Loaf", "Bat Out Of Hell III", "Meat Loaf", "Blind As A Bat",
	"2006", "Rock", "ogg", "~", FALSE);
  ui.klineedit_album_example->setText(filename);
  ui.klineedit_album_example->setCursorPosition(0);
  filename = maskparser.parseCommandMask(ui.klineedit_command->text(),
	"/tmp/tmp.wav", QString("%1/music/Alternative Hits/Volume 4/04 - Wolfsheim - Approaching Lightspeed.ogg").arg(QDir::homePath()),
	4, 2, 1,
	"Alternative Hits", "Volume 4", "Wolfsheim", "Approaching Lightspeed",
	"2003", "Darkwave", "ogg", "~", FALSE);
  ui.klineedit_sampler_example->setText(filename);
  ui.klineedit_sampler_example->setCursorPosition(0);
}