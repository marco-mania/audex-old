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

#include "simplemaskwizarddialog.h"

SimpleMaskWizardDialog::SimpleMaskWizardDialog(const QString& mask, const QString& suffix, QWidget *parent) : KDialog(parent) {

  Q_UNUSED(parent);

  QWidget *widget = new QWidget(this);
  ui.setupUi(widget);

  setMainWidget(widget);

  setCaption(i18n("Mask Wizard"));

  setButtons(KDialog::Ok | KDialog::Cancel | KDialog::Apply);

  ui.klineedit_mask->setText(mask);
  connect(ui.klineedit_mask, SIGNAL(textEdited(const QString&)), this, SLOT(trigger_changed()));
  connect(ui.klineedit_mask, SIGNAL(textChanged(const QString&)), this, SLOT(update_example()));
  ui.klineedit_mask->setCursorPosition(0);

  connect(ui.kpushbutton_albumartist, SIGNAL(clicked()), this, SLOT(insAlbumArtist()));
  connect(ui.kpushbutton_albumtitle, SIGNAL(clicked()), this, SLOT(insAlbumTitle()));
  connect(ui.kpushbutton_cdno, SIGNAL(clicked()), this, SLOT(insCDNo()));
  connect(ui.kpushbutton_date, SIGNAL(clicked()), this, SLOT(insDate()));
  connect(ui.kpushbutton_genre, SIGNAL(clicked()), this, SLOT(insGenre()));

  this->mask = mask;
  this->suffix = suffix;

  enableButtonApply(FALSE);

  update_example();

}

SimpleMaskWizardDialog::~SimpleMaskWizardDialog() {

}

void SimpleMaskWizardDialog::slotButtonClicked(int button) {
  if (button == KDialog::Ok) {
    save();
    accept();
  } else if (button == KDialog::Apply) {
    save();
  } else {
    KDialog::slotButtonClicked(button);
  }
}

void SimpleMaskWizardDialog::trigger_changed() {
  if (ui.klineedit_mask->text() != mask) { enableButtonApply(TRUE); return; }
  enableButtonApply(FALSE);
}

void SimpleMaskWizardDialog::insAlbumArtist() {
  QString text = ui.klineedit_mask->text();
  text.insert(ui.klineedit_mask->cursorPosition(), "<"+QString(TAG_ALBUM_ARTIST)+" />");
  ui.klineedit_mask->setText(text);
  update_example();
}

void SimpleMaskWizardDialog::insAlbumTitle() {
  QString text = ui.klineedit_mask->text();
  text.insert(ui.klineedit_mask->cursorPosition(), "<"+QString(TAG_ALBUM_TITLE)+" />");
  ui.klineedit_mask->setText(text);
  update_example();
}

void SimpleMaskWizardDialog::insCDNo() {
  QString text = ui.klineedit_mask->text();
  text.insert(ui.klineedit_mask->cursorPosition(), "<"+QString(TAG_CDNO)+" />");
  ui.klineedit_mask->setText(text);
  update_example();
}

void SimpleMaskWizardDialog::insDate() {
  QString text = ui.klineedit_mask->text();
  text.insert(ui.klineedit_mask->cursorPosition(), "<"+QString(TAG_DATE)+" />");
  ui.klineedit_mask->setText(text);
  update_example();
}

void SimpleMaskWizardDialog::insGenre() {
  QString text = ui.klineedit_mask->text();
  text.insert(ui.klineedit_mask->cursorPosition(), "<"+QString(TAG_GENRE)+" />");
  ui.klineedit_mask->setText(text);
  update_example();
}

bool SimpleMaskWizardDialog::save() {
  mask = ui.klineedit_mask->text();
  enableButtonApply(FALSE);
  return TRUE;
}

void SimpleMaskWizardDialog::update_example() {
  MaskParser maskparser;
  QString filename = maskparser.parseSimpleMask(ui.klineedit_mask->text(),
	1, "Meat Loaf", "Bat Out Of Hell III", "2006", "Rock", suffix, FALSE);
  ui.klineedit_example->setText(filename);
  ui.klineedit_example->setCursorPosition(0);
}
