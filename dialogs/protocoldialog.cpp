/* AUDEX CDDA EXTRACTOR
 * Copyright (C) 2007 by Marco Nelles (marcomaniac@gmx.de)
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

#include "protocoldialog.h"

protocoldialog::protocoldialog(QWidget *parent, QSettings *settings, const QString& title, const QStringList& protocol) {

  Q_UNUSED(parent);

  this->title = title;
  this->protocol = protocol;

  setupUi(this);
  this->settings = settings;
  connect(pushButton, SIGNAL(clicked()), this, SLOT(close()));
  connect(pushButton_2, SIGNAL(clicked()), this, SLOT(saveToFile()));
  textEdit->setPlainText(protocol.join("\n"));

  setWindowTitle(title);

  resize(settings->value("protocoldialog/size", QSize(529, 379)).toSize());
  move(settings->value("protocoldialog/pos", QPoint(100, 100)).toPoint());
}

protocoldialog::~protocoldialog() {
}

void protocoldialog::saveToFile() {

  QFileDialog fileDialog;
  fileDialog.setAcceptMode(QFileDialog::AcceptSave);
  fileDialog.setWindowTitle(trUtf8("Save %1").arg(title));
  fileDialog.setDirectory(QDir::homePath());
  fileDialog.setFilter(trUtf8("Audex Protocol (*.pro)"));
  fileDialog.setDefaultSuffix("pro");
  fileDialog.exec();
  QString fileName = fileDialog.selectedFiles().at(0);

  if (!fileName.isEmpty()) {
    QFile data(fileName);
    if (data.open(QFile::WriteOnly | QFile::Truncate)) {
      QTextStream out(&data);
      out << "AUDEX " << title << "\n";
      out << trUtf8("created on ") << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "\n";
      out << "\n";
      for (int i = 0; i < protocol.count(); i++) {
        out << protocol.at(i) << "\n";
      }
    }
  }

}

void protocoldialog::closeEvent(QCloseEvent *event) {
  settings->setValue("protocoldialog/pos", pos());
  settings->setValue("protocoldialog/size", size());
  event->accept();
}
