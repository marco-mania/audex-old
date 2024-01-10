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

#include "remoteserversettingswidget.h"

remoteServerSettingsWidget::remoteServerSettingsWidget(QWidget *parent) : remoteServerSettingsWidgetUI(parent) {

  Q_UNUSED(parent);

  connect(kcfg_upload, SIGNAL(toggled(bool)), this, SLOT(toggle_upload(bool)));
  toggle_upload(kcfg_upload->isChecked());
  kcfg_url->setMode(KFile::Directory|KFile::ExistingOnly|KFile::File); // Dont want files, but KDirSelect doesnt seem to like remote urls :-(

}

remoteServerSettingsWidget::~remoteServerSettingsWidget() {

}

void remoteServerSettingsWidget::toggle_upload(bool enabled) {
  kcfg_url->setEnabled(enabled);
  label->setEnabled(enabled);
}