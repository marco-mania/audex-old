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

#include "protocoldialog.h"

ProtocolDialog::ProtocolDialog(const QStringList& protocol, const QString& title, QWidget *parent) : KDialog(parent) {

  Q_UNUSED(parent);

  QWidget *widget = new QWidget(this);
  ui.setupUi(widget);

  setMainWidget(widget);

  setCaption(title);

  setButtons(None);

  ui.ktextedit->setPlainText(protocol.join("\n"));

  connect(ui.kpushbutton_close, SIGNAL(clicked()), this, SLOT(close()));
  connect(ui.kpushbutton_save, SIGNAL(clicked()), this, SLOT(save()));

  this->protocol = protocol;
  this->title = title;

}

ProtocolDialog::~ProtocolDialog() {

}

void ProtocolDialog::save() {
  QString fileName = KFileDialog::getSaveFileName(KUrl(QDir::homePath()), "*.pro", this, i18n("Save ")+title);
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
