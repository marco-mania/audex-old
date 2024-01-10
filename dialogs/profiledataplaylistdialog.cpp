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

#include "profiledataplaylistdialog.h"

ProfileDataPlaylistDialog::ProfileDataPlaylistDialog(const QString& format, const QString &mask, QWidget *parent) : KDialog(parent) {

  Q_UNUSED(parent);

  this->format = format;
  this->mask = mask;

  QWidget *widget = new QWidget(this);
  ui.setupUi(widget);

  setMainWidget(widget);

  setCaption(i18n("Playlist Settings"));

  setButtons(KDialog::Ok | KDialog::Cancel | KDialog::Apply);

  connect(ui.kpushbutton_mask, SIGNAL(clicked()), this, SLOT(mask_wizard()));
  ui.kpushbutton_mask->setIcon(KIcon("tools-wizard"));

  ui.kcombobox_format->addItem("M3U (Textbased Winamp Playlist)", "M3U");
  ui.kcombobox_format->addItem("PLS (Textbased Playlist)", "PLS");
  ui.kcombobox_format->addItem("XSPF (XML Shareable Playlist Format)", "XSPF");
  {
    int i = ui.kcombobox_format->findData(format);
    ui.kcombobox_format->setCurrentIndex(i);
  }
  connect(ui.kcombobox_format, SIGNAL(currentIndexChanged(int)), this, SLOT(trigger_changed()));

  ui.klineedit_mask->setText(mask);
  connect(ui.klineedit_mask, SIGNAL(textEdited(const QString&)), this, SLOT(trigger_changed()));

  enableButtonApply(FALSE);

}

ProfileDataPlaylistDialog::~ProfileDataPlaylistDialog() {
}

void ProfileDataPlaylistDialog::slotButtonClicked(int button) {
  if (button == KDialog::Ok) {
    save();
    accept();
  } else if (button == KDialog::Apply) {
    save();
  } else {
    KDialog::slotButtonClicked(button);
  }
}

void ProfileDataPlaylistDialog::mask_wizard() {

  QString suffix = ui.kcombobox_format->itemData(ui.kcombobox_format->currentIndex()).toString().toLower();

  SimpleMaskWizardDialog *dialog = new SimpleMaskWizardDialog(ui.klineedit_mask->text(), suffix, this);

  if (dialog->exec() != QDialog::Accepted) { delete dialog; return; }

  ui.klineedit_mask->setText(dialog->mask);

  delete dialog;

  trigger_changed();

}

void ProfileDataPlaylistDialog::trigger_changed() {
  if (ui.kcombobox_format->itemData(ui.kcombobox_format->currentIndex()).toString() != format) { enableButtonApply(TRUE); return; }
  if (ui.klineedit_mask->text() != mask) { enableButtonApply(TRUE); return; }
  enableButtonApply(FALSE);
}

bool ProfileDataPlaylistDialog::save() {
  format = ui.kcombobox_format->itemData(ui.kcombobox_format->currentIndex()).toString();
  mask = ui.klineedit_mask->text();
  enableButtonApply(FALSE);
  return TRUE;
}
