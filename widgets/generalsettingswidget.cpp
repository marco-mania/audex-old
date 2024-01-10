/* AUDEX CDDA EXTRACTOR
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

#include "generalsettingswidget.h"

generalSettingsWidget::generalSettingsWidget(QWidget* parent) : generalSettingsWidgetUI(parent) {

  kcfg_amazonLocale->clear();
  kcfg_amazonLocale->addItem(QIcon(KStandardDirs::locate("data", "audex/images/usa.png")), i18n("International/USA"), "us");
  kcfg_amazonLocale->addItem(QIcon(KStandardDirs::locate("data", "audex/images/germany.png")), i18n("Germany"), "de");
  kcfg_amazonLocale->addItem(QIcon(KStandardDirs::locate("data", "audex/images/france.png")), i18n("France"), "fr");
  kcfg_amazonLocale->addItem(QIcon(KStandardDirs::locate("data", "audex/images/canada.png")), i18n("Canada"), "ca");
  kcfg_amazonLocale->addItem(QIcon(KStandardDirs::locate("data", "audex/images/japan.png")), i18n("Japan"), "jp");

  kcfg_wikipediaLocale->clear();
  kcfg_wikipediaLocale->addItem(QIcon(KStandardDirs::locate("data", "audex/images/usa.png")), i18n("English"), "en");
  kcfg_wikipediaLocale->addItem(QIcon(KStandardDirs::locate("data", "audex/images/germany.png")), i18n("Deutsch"), "de");
  kcfg_wikipediaLocale->addItem(QIcon(KStandardDirs::locate("data", "audex/images/france.png")), i18n("Français"), "fr");
  kcfg_wikipediaLocale->addItem(QIcon(KStandardDirs::locate("data", "audex/images/poland.png")), i18n("Polski"), "pl");
  kcfg_wikipediaLocale->addItem(QIcon(KStandardDirs::locate("data", "audex/images/japan.png")), i18n("日本語"), "ja");
  kcfg_wikipediaLocale->addItem(QIcon(KStandardDirs::locate("data", "audex/images/italy.png")), i18n("Italiano"), "it");
  kcfg_wikipediaLocale->addItem(QIcon(KStandardDirs::locate("data", "audex/images/netherlands.png")), i18n("Nederlands"), "nl");
  kcfg_wikipediaLocale->addItem(QIcon(KStandardDirs::locate("data", "audex/images/spain.png")), i18n("Español"), "es");
  kcfg_wikipediaLocale->addItem(QIcon(KStandardDirs::locate("data", "audex/images/portugal.png")), i18n("Português"), "pt");
  kcfg_wikipediaLocale->addItem(QIcon(KStandardDirs::locate("data", "audex/images/sweden.png")), i18n("Svenska"), "sv");

  urlreq_tempPath->setMode(KFile::Directory);
  urlreq_tempPath->lineEdit()->setObjectName("kcfg_tempPath");
  urlreq_basePath->setMode(KFile::Directory);
  urlreq_basePath->lineEdit()->setObjectName("kcfg_basePath");

}

generalSettingsWidget::~generalSettingsWidget() {

}
