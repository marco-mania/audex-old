#include <KApplication>
#include <KAboutData>
#include <KCmdLineArgs>

#include "mainwindow.h"

int main (int argc, char *argv[]) {

  KAboutData aboutData("audex", 0, ki18n("Audex"), "0.6",
                       ki18n("CDDA Extractor for KDE"),
                       KAboutData::License_GPL,
                       ki18n("(c) 2007-2008 by Marco Nelles"),
                       KLocalizedString(),
                       "http://www.anyaudio.de/audex",
                       "bugs@anyaudio.de");
  aboutData.addAuthor(ki18n("Marco Nelles"), ki18n("Current maintainer"), "marco@maniatek.de");
  aboutData.addCredit(ki18n("freedb.org"), ki18n("Special thanks to freedb.org for providing a free CDDB-like CD database"), 0, "http://freedb.org");

  KCmdLineArgs::init(argc, argv, &aboutData);
  KApplication app;

  MainWindow* window = new MainWindow();
  window->show();
  window->startAssistant();

  return app.exec();

}
