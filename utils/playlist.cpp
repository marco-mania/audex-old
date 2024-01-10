/* AUDEX CDDA EXTRACTOR
 * Copyright (C) 2007-2013 Marco Nelles (audex@maniatek.com)
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

#include "playlist.h"

Playlist::Playlist(const QStringList& filenames, CDDAModel *model) {

  this->filenamelist = filenames;
  this->model = model;

}

Playlist::~Playlist() {

}

QStringList Playlist::asM3U(bool absolutePath, bool merge) const {

  QStringList playlist;
  if (!merge) playlist.append("#EXTM3U");

  int j = 0;
  for (int i = 0; i < model->rowCount(); ++i) {

    if (!model->isAudioTrack(i+1)) continue;
    if (!model->isTrackInSelection(i+1)) continue;
    if (j >= filenamelist.count()) continue;

    if (model->isVarious()) {
      playlist.append(QString("#EXTINF:%1,%2 - %3").arg(model->lengthOfTrack(i+1)).arg(model->data(model->index(i, CDDA_MODEL_COLUMN_ARTIST_INDEX)).toString()).arg(model->data(model->index(i, CDDA_MODEL_COLUMN_TITLE_INDEX)).toString()));
    } else {
      playlist.append(QString("#EXTINF:%1,%2").arg(model->lengthOfTrack(i+1)).arg(model->data(model->index(i, CDDA_MODEL_COLUMN_TITLE_INDEX)).toString()));
    }
    if (absolutePath) {
      playlist.append(filenamelist[j++]);
    } else {
      QFileInfo info(filenamelist[j++]);
      playlist.append(info.fileName());
    }

  }

  return playlist;

}

QStringList Playlist::asPLS(bool absolutePath) const {

  QStringList playlist;
  playlist.append("[Playlist]");

  int j = 0;
  for (int i = 0; i < model->rowCount(); ++i) {
    if (!model->isAudioTrack(i+1)) continue;
    if (!model->isTrackInSelection(i+1)) continue;
    if (j >= filenamelist.count()) continue;
    ++j;
    if (absolutePath) {
      playlist.append(QString("File%1=%2").arg(i+1).arg(filenamelist[i]));
    } else {
      QFileInfo info(filenamelist[i]);
      playlist.append(QString("File%1=%2").arg(i+1).arg(info.fileName()));
    }
    if (model->isVarious()) {
      playlist.append(QString("Title%1=%2 - %3").arg(i+1).arg(model->data(model->index(i, CDDA_MODEL_COLUMN_ARTIST_INDEX)).toString()).arg(model->data(model->index(i, CDDA_MODEL_COLUMN_TITLE_INDEX)).toString()));
    } else {
      playlist.append(QString("Title%1=%2").arg(i+1).arg(model->data(model->index(i, CDDA_MODEL_COLUMN_TITLE_INDEX)).toString()));
    }
    playlist.append(QString("Length%1=%2").arg(i+1).arg(model->lengthOfTrack(i+1)));
  }

  playlist.append(QString("NumberOfEntries=%1").arg(j));
  playlist.append(QString("Version=2"));

  return playlist;

}

QStringList Playlist::asXSPF() const {

  QDomDocument doc;
  QDomElement root = doc.createElement("playlist");
  root.setAttribute("version", "1");
  root.setAttribute("xmlns", "http://xspf.org/ns/0");

  QDomElement creator = doc.createElement("creator");
  QDomText text = doc.createTextNode("audex");
  creator.appendChild(text);
  root.appendChild(creator);

  QDomElement tracklist = doc.createElement("trackList");

  int j = 0;
  for (int i = 0; i < model->rowCount(); ++i) {

    if (!model->isAudioTrack(i+1)) continue;
    if (!model->isTrackInSelection(i+1)) continue;
    if (j >= filenamelist.count()) continue;
    ++j;

    QDomElement track = doc.createElement("track");

    QDomElement ch = doc.createElement("location");
    QDomText text = doc.createTextNode(QFileInfo(filenamelist[i]).absoluteFilePath());

    ch.appendChild(text);
    track.appendChild(ch);

    ch = doc.createElement("title");
    text = doc.createTextNode(model->data(model->index(i, CDDA_MODEL_COLUMN_TITLE_INDEX)).toString());
    ch.appendChild(text);
    track.appendChild(ch);

    ch = doc.createElement("trackNum");
    text = doc.createTextNode(QString::number(j));
    ch.appendChild(text);
    track.appendChild(ch);

    tracklist.appendChild(track);

  }

  root.appendChild(tracklist);
  doc.appendChild(root);
  QString xml_header("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  return doc.toString().prepend(xml_header).split('\n');

}
