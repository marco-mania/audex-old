/* AUDEX CDDA EXTRACTOR
 * Copyright (C) 2007-2011 Marco Nelles (audex@maniatek.com)
 * <http://kde.maniatek.com/audex>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "simplepatternwizarddialog.h"

SimplePatternWizardDialog::SimplePatternWizardDialog(const QString& pattern, const QString& suffix, QWidget *parent) : KDialog(parent) {

  Q_UNUSED(parent);

  QWidget *widget = new QWidget(this);
  ui.setupUi(widget);

  setMainWidget(widget);

  setCaption(i18n("Pattern Wizard"));

  setButtons(KDialog::Ok | KDialog::Cancel | KDialog::Apply);

  ui.klineedit_pattern->setText(pattern);
  connect(ui.klineedit_pattern, SIGNAL(textEdited(const QString&)), this, SLOT(trigger_changed()));
  connect(ui.klineedit_pattern, SIGNAL(textChanged(const QString&)), this, SLOT(update_example()));
  ui.klineedit_pattern->setCursorPosition(0);

  connect(ui.kpushbutton_albumartist, SIGNAL(clicked()), this, SLOT(insAlbumArtist()));
  connect(ui.kpushbutton_albumtitle, SIGNAL(clicked()), this, SLOT(insAlbumTitle()));
  connect(ui.kpushbutton_cdno, SIGNAL(clicked()), this, SLOT(insCDNo()));
  connect(ui.kpushbutton_date, SIGNAL(clicked()), this, SLOT(insDate()));
  connect(ui.kpushbutton_genre, SIGNAL(clicked()), this, SLOT(insGenre()));

  this->pattern = pattern;
  this->suffix = suffix;

  enableButtonApply(FALSE);
  showButtonSeparator(true);
  update_example();

}

SimplePatternWizardDialog::~SimplePatternWizardDialog() {

}

void SimplePatternWizardDialog::slotButtonClicked(int button) {
  if (button == KDialog::Ok) {
    save();
    accept();
  } else if (button == KDialog::Apply) {
    save();
  } else {
    KDialog::slotButtonClicked(button);
  }
}

void SimplePatternWizardDialog::trigger_changed() {
  if (ui.klineedit_pattern->text() != pattern) { enableButtonApply(TRUE); return; }
  enableButtonApply(FALSE);
}

void SimplePatternWizardDialog::insAlbumArtist() {
  QString text = ui.klineedit_pattern->text();
  text.insert(ui.klineedit_pattern->cursorPosition(), QString("$"VAR_ALBUM_ARTIST));
  ui.klineedit_pattern->setText(text);
  update_example();
}

void SimplePatternWizardDialog::insAlbumTitle() {
  QString text = ui.klineedit_pattern->text();
  text.insert(ui.klineedit_pattern->cursorPosition(), QString("$"VAR_ALBUM_TITLE));
  ui.klineedit_pattern->setText(text);
  update_example();
}

void SimplePatternWizardDialog::insCDNo() {
  QString text = ui.klineedit_pattern->text();
  text.insert(ui.klineedit_pattern->cursorPosition(), QString("$"VAR_CDNO));
  ui.klineedit_pattern->setText(text);
  update_example();
}

void SimplePatternWizardDialog::insDate() {
  QString text = ui.klineedit_pattern->text();
  text.insert(ui.klineedit_pattern->cursorPosition(), QString("$"VAR_DATE));
  ui.klineedit_pattern->setText(text);
  update_example();
}

void SimplePatternWizardDialog::insGenre() {
  QString text = ui.klineedit_pattern->text();
  text.insert(ui.klineedit_pattern->cursorPosition(), QString("$"VAR_GENRE));
  ui.klineedit_pattern->setText(text);
  update_example();
}

bool SimplePatternWizardDialog::save() {
  pattern = ui.klineedit_pattern->text();
  enableButtonApply(FALSE);
  return TRUE;
}

void SimplePatternWizardDialog::update_example() {
  PatternParser patternparser;
  QString filename = patternparser.parseSimplePattern(ui.klineedit_pattern->text(),
	1, "Meat Loaf", "Bat Out Of Hell III", "2006", "Rock", suffix, FALSE);
  ui.klineedit_example->setText(filename);
  ui.klineedit_example->setCursorPosition(0);
}
