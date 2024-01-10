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

#include "qplaylist.h"

QPlaylist::QPlaylist(const QStringList& filenames, const QCDDA_MetaData& metadata) {

  this->filenamelist = filenames;
  this->metadata = metadata;

}

QPlaylist::~QPlaylist() {

}

QStringList QPlaylist::asM3U(bool absolutePath) const {

  QStringList playlist;
  playlist.append("#EXTM3U");

  for (int i = 0; i < metadata.trackCount(); i++) {
    if (!metadata.track(i+1).get(QCDDA_TrackMetaData::AudioTrack).toBool()) continue;
    if (metadata.get(QCDDA_MetaData::Various).toBool()) {
      playlist.append(QString("#EXTINF:%1,%2 - %3").arg(metadata.track(i+1).get(QCDDA_TrackMetaData::Length).toInt()).arg(metadata.track(i+1).get(QCDDA_TrackMetaData::Artist).toString()).arg(metadata.track(i+1).get(QCDDA_TrackMetaData::Title).toString()));
    } else {
      playlist.append(QString("#EXTINF:%1,%2").arg(metadata.track(i+1).get(QCDDA_TrackMetaData::Length).toInt()).arg(metadata.track(i+1).get(QCDDA_TrackMetaData::Title).toString()));
    }
    if (absolutePath) {
      playlist.append(filenamelist[i]);
    } else {
      QFileInfo info(filenamelist[i]);
      playlist.append(info.fileName());
    }
  }

  return playlist;

}

QStringList QPlaylist::asPLS(bool absolutePath) const {

  QStringList playlist;
  playlist.append("[Playlist]");

  int j = 0;
  for (int i = 0; i < metadata.trackCount(); i++) {
    if (!metadata.track(i+1).get(QCDDA_TrackMetaData::AudioTrack).toBool()) continue;
    j++;
    if (absolutePath) {
      playlist.append(QString("File%1=%2").arg(i+1).arg(filenamelist[i]));
    } else {
      QFileInfo info(filenamelist[i]);
      playlist.append(QString("File%1=%2").arg(i+1).arg(info.fileName()));
    }
    if (metadata.get(QCDDA_MetaData::Various).toBool()) {
      playlist.append(QString("Title%1=%2 - %3").arg(i+1).arg(metadata.track(i+1).get(QCDDA_TrackMetaData::Artist).toString()).arg(metadata.track(i+1).get(QCDDA_TrackMetaData::Title).toString()));
    } else {
      playlist.append(QString("Title%1=%2").arg(i+1).arg(metadata.track(i+1).get(QCDDA_TrackMetaData::Title).toString()));
    }
    playlist.append(QString("Length%1=%2").arg(i+1).arg(metadata.track(i+1).get(QCDDA_TrackMetaData::Length).toInt()));
  }

  playlist.append(QString("NumberOfEntries=%1").arg(j));
  playlist.append(QString("Version=2"));

  return playlist;

}

QStringList QPlaylist::asXSPF() const {

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
  for (int i = 0; i < metadata.trackCount(); i++) {

    if (!metadata.track(i+1).get(QCDDA_TrackMetaData::AudioTrack).toBool()) continue;
    j++;

    QDomElement track = doc.createElement("track");

    QDomElement ch = doc.createElement("location");
    QDomText text = doc.createTextNode(QFileInfo(filenamelist[i]).absoluteFilePath());

    ch.appendChild(text);
    track.appendChild(ch);

    ch = doc.createElement("title");
    text = doc.createTextNode(metadata.track(i+1).get(QCDDA_TrackMetaData::Title).toString());
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
  QString xml_header("<?xml version='1.0' encoding='UTF-8'?>\n");
  return doc.toString().prepend(xml_header).split('\n');

}
