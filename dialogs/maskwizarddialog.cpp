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

#include "maskwizarddialog.h"

MaskWizardDialog::MaskWizardDialog(const QString& mask, QWidget *parent) : KDialog(parent) {

  Q_UNUSED(parent);

  QWidget *widget = new QWidget(this);
  ui.setupUi(widget);

  setMainWidget(widget);

  setCaption(i18n("Filename Mask Wizard"));

  setButtons(KDialog::Ok | KDialog::Cancel | KDialog::Apply);

  ui.klineedit_mask->setText(mask);
  connect(ui.klineedit_mask, SIGNAL(textEdited(const QString&)), this, SLOT(trigger_changed()));
  connect(ui.klineedit_mask, SIGNAL(textChanged(const QString&)), this, SLOT(update_example()));
  ui.klineedit_mask->setCursorPosition(0);

  connect(ui.kpushbutton_albumartist, SIGNAL(clicked()), this, SLOT(insAlbumArtist()));
  connect(ui.kpushbutton_albumtitle, SIGNAL(clicked()), this, SLOT(insAlbumTitle()));
  connect(ui.kpushbutton_trackartist, SIGNAL(clicked()), this, SLOT(insTrackArtist()));
  connect(ui.kpushbutton_tracktitle, SIGNAL(clicked()), this, SLOT(insTrackTitle()));
  connect(ui.kpushbutton_trackno, SIGNAL(clicked()), this, SLOT(insTrackNo()));
  connect(ui.kpushbutton_cdno, SIGNAL(clicked()), this, SLOT(insCDNo()));
  connect(ui.kpushbutton_date, SIGNAL(clicked()), this, SLOT(insDate()));
  connect(ui.kpushbutton_genre, SIGNAL(clicked()), this, SLOT(insGenre()));
  connect(ui.kpushbutton_suffix, SIGNAL(clicked()), this, SLOT(insSuffix()));
  connect(ui.kpushbutton_basepath, SIGNAL(clicked()), this, SLOT(insBasePath()));
  connect(ui.kpushbutton_homepath, SIGNAL(clicked()), this, SLOT(insHomePath()));

  this->mask = mask;

  enableButtonApply(FALSE);

  update_example();

}

MaskWizardDialog::~MaskWizardDialog() {

}

void MaskWizardDialog::slotButtonClicked(int button) {
  if (button == KDialog::Ok) {
    save();
    accept();
  } else if (button == KDialog::Apply) {
    save();
  } else {
    KDialog::slotButtonClicked(button);
  }
}

void MaskWizardDialog::trigger_changed() {
  if (ui.klineedit_mask->text() != mask) { enableButtonApply(TRUE); return; }
  enableButtonApply(FALSE);
}

void MaskWizardDialog::insHomePath() {
  QString text = ui.klineedit_mask->text();
  text.insert(ui.klineedit_mask->cursorPosition(), "$"+QString(TAG_HOME_PATH));
  ui.klineedit_mask->setText(text);
  update_example();
}

void MaskWizardDialog::insBasePath() {
  QString text = ui.klineedit_mask->text();
  text.insert(ui.klineedit_mask->cursorPosition(), "$"+QString(TAG_BASE_PATH));
  ui.klineedit_mask->setText(text);
  update_example();
}

void MaskWizardDialog::insAlbumArtist() {
  QString text = ui.klineedit_mask->text();
  text.insert(ui.klineedit_mask->cursorPosition(), "$"+QString(TAG_ALBUM_ARTIST));
  ui.klineedit_mask->setText(text);
  update_example();
}

void MaskWizardDialog::insAlbumTitle() {
  QString text = ui.klineedit_mask->text();
  text.insert(ui.klineedit_mask->cursorPosition(), "$"+QString(TAG_ALBUM_TITLE));
  ui.klineedit_mask->setText(text);
  update_example();
}

void MaskWizardDialog::insTrackArtist() {
  QString text = ui.klineedit_mask->text();
  text.insert(ui.klineedit_mask->cursorPosition(), "$"+QString(TAG_TRACK_ARTIST));
  ui.klineedit_mask->setText(text);
  update_example();
}

void MaskWizardDialog::insTrackTitle() {
  QString text = ui.klineedit_mask->text();
  text.insert(ui.klineedit_mask->cursorPosition(), "$"+QString(TAG_TRACK_TITLE));
  ui.klineedit_mask->setText(text);
  update_example();
}

void MaskWizardDialog::insTrackNo() {
  QString text = ui.klineedit_mask->text();
  text.insert(ui.klineedit_mask->cursorPosition(), "$"+QString(TAG_TRACK_NO));
  ui.klineedit_mask->setText(text);
  update_example();
}

void MaskWizardDialog::insCDNo() {
  QString text = ui.klineedit_mask->text();
  text.insert(ui.klineedit_mask->cursorPosition(), "$"+QString(TAG_CDNO));
  ui.klineedit_mask->setText(text);
  update_example();
}

void MaskWizardDialog::insDate() {
  QString text = ui.klineedit_mask->text();
  text.insert(ui.klineedit_mask->cursorPosition(), "$"+QString(TAG_DATE));
  ui.klineedit_mask->setText(text);
  update_example();
}

void MaskWizardDialog::insGenre() {
  QString text = ui.klineedit_mask->text();
  text.insert(ui.klineedit_mask->cursorPosition(), "$"+QString(TAG_GENRE));
  ui.klineedit_mask->setText(text);
  update_example();
}

void MaskWizardDialog::insSuffix() {
  QString text = ui.klineedit_mask->text();
  text.insert(ui.klineedit_mask->cursorPosition(), "$"+QString(TAG_SUFFIX));
  ui.klineedit_mask->setText(text);
  update_example();
}

bool MaskWizardDialog::save() {
  mask = ui.klineedit_mask->text();
  enableButtonApply(FALSE);
  return TRUE;
}

void MaskWizardDialog::update_example() {
  MaskParser maskparser;
  QString filename = maskparser.parseFilenameMask(ui.klineedit_mask->text(),
	2, 1, 1,
	"Meat Loaf", "Bat Out Of Hell III", "Meat Loaf", "Blind As A Bat",
	"2006", "Rock", "ogg", "~", FALSE);
  ui.klineedit_album_example->setText(filename);
  ui.klineedit_album_example->setCursorPosition(0);
  filename = maskparser.parseFilenameMask(ui.klineedit_mask->text(),
	4, 2, 1,
	"Bravo Hits", "Volume 41", "Wolfsheim", "Kein Zurueck",
	"2003", "Darkwave", "ogg", "~", FALSE);
  ui.klineedit_sampler_example->setText(filename);
  ui.klineedit_sampler_example->setCursorPosition(0);
}
