/* AUDEX CDDA EXTRACTOR
 * Copyright (C) 2007-2009 Marco Nelles (audex@maniatek.de)
 * <http://opensource.maniatek.de/audex>
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

#include <KApplication>
#include <KAboutData>
#include <KCmdLineArgs>

#include "config.h"
#include "mainwindow.h"

int main (int argc, char *argv[]) {

  KAboutData aboutData("audex", 0, ki18n("Audex"), AUDEX_VERSION,
                       ki18n("CDDA Extractor for KDE"),
                       KAboutData::License_GPL,
                       ki18n("(c) 2007-2009 by Marco Nelles"),
                       KLocalizedString(),
                       "http://opensource.maniatek.de/audex",
                       "audex@maniatek.de");
  aboutData.addAuthor(ki18n("Marco Nelles"), ki18n("Current maintainer"), "marco@maniatek.de");
  aboutData.addCredit(ki18n("freedb.org"), ki18n("Special thanks to freedb.org for providing a free CDDB-like CD database"), 0, "http://freedb.org");
  aboutData.addCredit(ki18n("Craig Drummond"), ki18n("GUI improvements"), 0, "");
  aboutData.setTranslator(ki18nc("NAME OF TRANSLATORS", "Your names"), ki18nc("EMAIL OF TRANSLATORS", "Your emails"));

  KCmdLineArgs::init(argc, argv, &aboutData);
  KApplication app;

  MainWindow* window = new MainWindow();
  window->show();

  return app.exec();

}
