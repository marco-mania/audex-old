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

/*
  Some of this code is based on libkcddb 3.91
  (a library for retrieving and sending cddb information)

  Copyright (C) 2002 Rik Hemsley (rikkus) <rik@kde.org>
  Copyright (C) 2002-2005 Benjamin Meyer <ben-devel@meyerhome.net>
  Copyright (C) 2002-2004 Nadeem Hasan <nhasan@nadmm.com>
  Copyright (C) 2006 Richard Lärkäng <nouseforaname@home.se>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
*/

#include "qcddametadata.h"

QCDDA_HeaderMetaData::QCDDA_HeaderMetaData() {
  cddb_relevant_data_modified = FALSE;
  modified = FALSE;
}

QCDDA_HeaderMetaData::QCDDA_HeaderMetaData(const QCDDA_HeaderMetaData& other) {
  data = other.data;
  modified = other.modified;
  cddb_relevant_data_modified = other.cddb_relevant_data_modified;
}

QCDDA_HeaderMetaData& QCDDA_HeaderMetaData::operator=(const QCDDA_HeaderMetaData& other) {
  data = other.data;
  modified = other.modified;
  cddb_relevant_data_modified = other.cddb_relevant_data_modified;
  return *this;
}

QCDDA_HeaderMetaData::~QCDDA_HeaderMetaData() {

}

QVariant QCDDA_HeaderMetaData::get(QCDDA_HeaderMetaData::HeaderDataType type) const {
  switch (type) {
    case (Artist) : {
      QVariant a = data["artist"];
      if (!a.isValid()) {
        return QVariant("Artist");
      } else {
        if (a.toString().isEmpty()) return QVariant("Artist");
        return a;
      }
    }
    case (Title) : {
      QVariant t = data["title"];
      if (!t.isValid()) {
        return QVariant("Album");
      } else {
        if (t.toString().isEmpty()) return QVariant("Album");
        return t;
      }
    }
    case (Category) : {
      QVariant c = data["category"];
      if (!isCategoryValid()) {
        return QVariant("misc");
      } else {
        if (c.toString().isEmpty()) return QVariant("misc");
        return c;
      }
    }
    case (Discid) : return data["discid"];
    default : return QVariant();
  }
  return QVariant();
}

QVariant QCDDA_HeaderMetaData::get(const QString& type) const {
  return data[type];
}

bool QCDDA_HeaderMetaData::set(QCDDA_HeaderMetaData::HeaderDataType type, const QVariant &d) {
  switch (type) {
    case (Artist) : if (d==data["artist"]) return FALSE;
                    data["artist"] = d.toString().simplified();
                    cddb_relevant_data_modified = TRUE;
                    modified = TRUE;
                    return TRUE;
    case (Title) : { if (d==data["title"]) return FALSE;
                     QString t = d.toString();
                     t.replace(QRegExp("[\\(]*(cd|dis[k|c]) *[1-4][\\)]* *$", Qt::CaseInsensitive), QString());
                     data["title"] = t.simplified();
                     cddb_relevant_data_modified = TRUE;
                     modified = TRUE;
                     return TRUE; }
    case (Category) : if (d==data["category"]) return FALSE;
                      data["category"] = d;
                      cddb_relevant_data_modified = TRUE;
                      modified = TRUE;
                      return TRUE;
    case (Discid) : if (d==data["discid"]) return FALSE;
                    data["discid"] = d;
                    cddb_relevant_data_modified = TRUE;
                    modified = TRUE;
                    return TRUE;
    default : break;
  }
  return FALSE;
}

bool QCDDA_HeaderMetaData::set(const QString& type, const QVariant &d) {
  if (d==get(type)) return FALSE;
  data[type] = d;
  modified = TRUE;
  return TRUE;
}

bool QCDDA_HeaderMetaData::isCDDBRelevantDataModified() const {
  return cddb_relevant_data_modified;
}

bool QCDDA_HeaderMetaData::isModified() const {
  return modified;
}

bool QCDDA_HeaderMetaData::isArtistValid() const {
  bool ok = data["artist"].isValid();
  if (!ok) return FALSE;
  return !data["artist"].toString().isEmpty();
}

bool QCDDA_HeaderMetaData::isTitleValid() const {
  bool ok = data["title"].isValid();
  if (!ok) return FALSE;
  return !data["title"].toString().isEmpty();
}

bool QCDDA_HeaderMetaData::isValid() const {
  return (isArtistValid() && isTitleValid());
}

bool QCDDA_HeaderMetaData::isCategoryValid() const {
  QStringList validCategories;
  validCategories << "blues" << "classical" << "country"
    << "data" << "folk" << "jazz" << "misc" << "newage" << "reggae"
    << "rock" << "soundtrack";
  return validCategories.contains(data["category"].toString());
}

void QCDDA_HeaderMetaData::clear() {
  data.clear();
  cddb_relevant_data_modified = FALSE;
  modified = FALSE;
}

void QCDDA_HeaderMetaData::confirm() {
  cddb_relevant_data_modified = FALSE;
  modified = FALSE;
}



QCDDA_TrackMetaData::QCDDA_TrackMetaData() {
  cddb_relevant_data_modified = FALSE;
  modified = FALSE;
}

QCDDA_TrackMetaData::QCDDA_TrackMetaData(const QCDDA_TrackMetaData& other) {
  data = other.data;
  modified = other.modified;
  cddb_relevant_data_modified = other.cddb_relevant_data_modified;
}

QCDDA_TrackMetaData& QCDDA_TrackMetaData::operator=(const QCDDA_TrackMetaData& other) {
  data = other.data;
  modified = other.modified;
  cddb_relevant_data_modified = other.cddb_relevant_data_modified;
  return *this;
}

QCDDA_TrackMetaData::QCDDA_TrackMetaData(const int trackno, const bool audiotrack,
			const QString& artist, const QString& title,
			const int year, const QStringList& extendeddata) {
  set(TrackNo, trackno);
  set(AudioTrack, audiotrack);
  if (!artist.isEmpty()) set(Artist, artist);
  if (!title.isEmpty()) set(Title, title);
  if (year!=0) set(Year, year);
  if (extendeddata.count()>0) set(ExtData, extendeddata);
  cddb_relevant_data_modified = FALSE;
  modified = FALSE;
}

QCDDA_TrackMetaData::~QCDDA_TrackMetaData() {

}

QVariant QCDDA_TrackMetaData::get(QCDDA_TrackMetaData::TrackDataType type) const {
  switch (type) {
    case (TrackNo) : return data["trackno"];
    case (TrackOffset) : return data["trackoffset"];
    case (Length) : return data["length"];
    case (Artist) : {
      if (!data["audiotrack"].toBool()) return QString("Data");
      QVariant a = data["artist"];
      if (!a.isValid()) {
        return QVariant("Artist");
      } else {
        if (a.toString().isEmpty()) return QVariant("Artist");
        return a;
      }
    }
    case (Title) : {
      if (!data["audiotrack"].toBool()) return QString("Data");
      QVariant t = data["title"];
      if (!t.isValid()) {
        return QString("Title %1").arg(get(TrackNo).toInt());
      } else {
        if (t.toString().isEmpty()) return QCoreApplication::tr("Title %1").arg(get(TrackNo).toInt());
        return t;
      }
    }
    case (Year) : return data["year"];
    case (ExtData) : return data["extdata"];
    case (AudioTrack) : return data["audiotrack"];
    default : return QVariant();
  }
  return QVariant();
}

QVariant QCDDA_TrackMetaData::get(const QString& type) const {
  return data[type];
}

bool QCDDA_TrackMetaData::set(QCDDA_TrackMetaData::TrackDataType type, const QVariant &d) {
  switch (type) {
    case (TrackNo) : if (d==data["trackno"]) return FALSE;
                     data["trackno"] = d;
                     modified = TRUE;
                     return TRUE;
    case (TrackOffset) : if (d==data["trackoffset"]) return FALSE;
                         data["trackoffset"] = d;
                         modified = TRUE;
                         return TRUE;
    case (Length) : if (d==data["length"]) return FALSE;
                    data["length"] = d;
                    modified = TRUE;
                    return TRUE;
    case (Artist) : if (d==data["artist"]) return FALSE;
                    data["artist"] = d.toString().simplified();
                    cddb_relevant_data_modified = TRUE;
                    modified = TRUE;
                    return TRUE;
    case (Title) : if (d==data["title"]) return FALSE;
                   data["title"] = d.toString().simplified();
                   cddb_relevant_data_modified = TRUE;
                   modified = TRUE;
                   return TRUE;
    case (Year) : if (d==data["year"]) return FALSE;
                  data["year"] = d;
                  cddb_relevant_data_modified = TRUE;
                  modified = TRUE;
                  return TRUE;
    case (ExtData) : if (d==data["extdata"]) return FALSE;
                     data["extdata"] = d;
                     cddb_relevant_data_modified = TRUE;
                     modified = TRUE;
                     return TRUE;
    case (AudioTrack) : if (d==data["audiotrack"]) return FALSE;
                        data["audiotrack"] = d;
                        modified = TRUE;
                        return TRUE;
    default : break;
  }
  return FALSE;
}

bool QCDDA_TrackMetaData::set(const QString& type, const QVariant &d) {
  if (d==get(type)) return FALSE;
  data[type] = d;
  modified = TRUE;
  return TRUE;
}

bool QCDDA_TrackMetaData::isCDDBRelevantDataModified() const {
  return cddb_relevant_data_modified;
}

bool QCDDA_TrackMetaData::isModified() const {
  return modified;
}

bool QCDDA_TrackMetaData::isArtistValid() const {
  if (!data["audiotrack"].toBool()) return TRUE;
  bool ok = data["artist"].isValid();
  if (!ok) return FALSE;
  return !data["artist"].toString().isEmpty();
}

bool QCDDA_TrackMetaData::isTitleValid() const {
  if (!data["audiotrack"].toBool()) return TRUE;
  bool ok = data["title"].isValid();
  if (!ok) return FALSE;
  return !data["title"].toString().isEmpty();
}

bool QCDDA_TrackMetaData::isValid() const {
  return (isArtistValid() && isTitleValid());
}

void QCDDA_TrackMetaData::clear() {
  data.clear();
  cddb_relevant_data_modified = FALSE;
  modified = FALSE;
}

void QCDDA_TrackMetaData::confirm() {
  cddb_relevant_data_modified = FALSE;
  modified = FALSE;
}




QCDDA_MetaData::QCDDA_MetaData() {
  cddb_relevant_data_modified = FALSE;
  modified = FALSE;
}

QCDDA_MetaData::QCDDA_MetaData(const QCDDA_MetaData& other) {
  data = other.data;
  headerdata = other.headerdata;
  tracklist = other.tracklist;
  modified = other.modified;
  cddb_relevant_data_modified = other.cddb_relevant_data_modified;
}

QCDDA_MetaData& QCDDA_MetaData::operator=(const QCDDA_MetaData& other) {
  data = other.data;
  headerdata = other.headerdata;
  tracklist = other.tracklist;
  modified = other.modified;
  cddb_relevant_data_modified = other.cddb_relevant_data_modified;
  return *this;
}

QCDDA_MetaData::~QCDDA_MetaData() {

}

void QCDDA_MetaData::setHeaderData(const QCDDA_HeaderMetaData& headerData) {
  headerdata = headerData;
}

QCDDA_HeaderMetaData& QCDDA_MetaData::headerData() {
  return headerdata;
}

QCDDA_HeaderMetaData QCDDA_MetaData::headerData() const {
  return headerdata;
}

QCDDA_TrackMetaData& QCDDA_MetaData::track(int trackno) {
  while (tracklist.count() < trackno) {
    int count = tracklist.count();
    tracklist.append(QCDDA_TrackMetaData(count+1));
  }
  return tracklist[trackno-1];
}

QCDDA_TrackMetaData QCDDA_MetaData::track(int trackno) const {
  if (trackno <= tracklist.count()) {
    return tracklist[trackno-1];
  } else {
    return QCDDA_TrackMetaData(trackno);
  }
}

void QCDDA_MetaData::addTrack(const int trackno, const bool audiotrack,
			const QString& artist, const QString& title,
			const int year, const QStringList& extendeddata) {
  tracklist.append(QCDDA_TrackMetaData(trackno, audiotrack, artist, title, year, extendeddata));
}

QVariant QCDDA_MetaData::trackArtist(int trackno) const {
  if (!get(Various).toBool()) return headerdata.get(QCDDA_HeaderMetaData::Artist);
  return track(trackno).get(QCDDA_TrackMetaData::Artist);
}

QVariant QCDDA_MetaData::trackTitle(int trackno) const {
  return track(trackno).get(QCDDA_TrackMetaData::Title);
}

int QCDDA_MetaData::trackCount() const {
  return tracklist.count();
}

QVariant QCDDA_MetaData::get(QCDDA_MetaData::CDDataType type) const {
  switch (type) {
    case (Year) : return data["year"];
    case (Genre) : return data["genre"];
    case (ExtData) : return data["extdata"];
    case (TrackNoOffset) : return data["tracknooffset"];
    case (CDNum) : return data["cdnum"];
    case (Length) : return data["length"];
    case (Revision) : return data["revision"];
    case (Various) : return data["various"];
    default : return QVariant();
  }
  return QVariant();
}

QVariant QCDDA_MetaData::get(const QString& type) const {
  return data[type];
}

bool QCDDA_MetaData::set(QCDDA_MetaData::CDDataType type, const QVariant& d) {
  switch (type) {
    case (Year) : if (d==data["year"]) return FALSE;
                  data["year"] = d;
                  cddb_relevant_data_modified = TRUE;
                  modified = TRUE;
                  return TRUE;
    case (Genre) : if (d==data["genre"]) return FALSE;
                   data["genre"] = d;
                   cddb_relevant_data_modified = TRUE;
                   modified = TRUE;
                   return TRUE;
    case (ExtData) : if (d==data["extdata"]) return FALSE;
                     data["extdata"] = d;
                     cddb_relevant_data_modified = TRUE;
                     modified = TRUE;
                     return TRUE;
    case (TrackNoOffset) : if (d==data["tracknooffset"]) return FALSE;
                           data["tracknooffset"] = d;
                           modified = TRUE;
                           return TRUE;
    case (CDNum) : if (d==data["cdnum"]) return FALSE;
                   data["cdnum"] = d;
                   modified = TRUE;
                   return TRUE;
    case (Length) : if (d==data["cdnum"]) return FALSE;
                    data["length"] = d;
                    modified = TRUE;
                    return TRUE;
    case (Revision) : if (d==data["revision"]) return FALSE;
                      data["revision"] = d;
                      modified = TRUE;
                      return TRUE;
    case (Various) : if (d==data["various"]) return FALSE;
                     data["various"] = d;
                     modified = TRUE;
                     return TRUE;
    default : break;
  }
  return FALSE;
}

bool QCDDA_MetaData::set(const QString& type, const QVariant &d) {
  if (d==get(type)) return FALSE;
  data[type] = d;
  modified = TRUE;
  return TRUE;
}

bool QCDDA_MetaData::isCDDBRelevantDataModified() const {
  if (headerdata.isCDDBRelevantDataModified()) return TRUE;
  for (int i = 0; i < tracklist.count(); i++) if (tracklist[i].isCDDBRelevantDataModified()) return TRUE;
  return cddb_relevant_data_modified;
}

bool QCDDA_MetaData::isModified() const {
  if (headerdata.isModified()) return TRUE;
  for (int i = 0; i < tracklist.count(); i++) if (tracklist[i].isModified()) return TRUE;
  return modified;
}

bool QCDDA_MetaData::isValid() const {
  if (!headerdata.isValid()) return FALSE;
  if (tracklist.count()==0) return FALSE;
  for (int i = 0; i < tracklist.count(); i++) {
    if (get(Various).toBool()) {
      if (!tracklist[i].isValid()) return FALSE;
    } else {
      if (!tracklist[i].isTitleValid()) return FALSE;
    }
  }
  return TRUE;
}

bool QCDDA_MetaData::offsetsValid() const {
  if (tracklist.count()==0) return FALSE;
  for (int i = 0; i < tracklist.count(); i++)
    if (!tracklist[i].get(QCDDA_TrackMetaData::TrackOffset).isValid()) return FALSE;
  return TRUE;
}

//all artists the same value?
bool QCDDA_MetaData::guessVarious() const {

  QString a;
  for (int i = 0; i < tracklist.count(); i++) {
    if ((i > 0) && (tracklist[i].get(QCDDA_TrackMetaData::Artist).toString().toLower()!=a.toLower())) return TRUE;
    a = tracklist[i].get(QCDDA_TrackMetaData::Artist).toString();
  }
  return FALSE;

}

//correction tools
void QCDDA_MetaData::swapArtistAndTitleOfTracks() {

  for (int i = 0; i < tracklist.count(); i++) {
    QVariant tmp = tracklist[i].get(QCDDA_TrackMetaData::Artist);
    tracklist[i].set(QCDDA_TrackMetaData::Artist, tracklist[i].get(QCDDA_TrackMetaData::Title));
    tracklist[i].set(QCDDA_TrackMetaData::Title, tmp);
  }

}

void QCDDA_MetaData::swapArtistAndTitle() {

  QVariant tmp = headerdata.get(QCDDA_HeaderMetaData::Artist);
  headerdata.set(QCDDA_HeaderMetaData::Artist, headerdata.get(QCDDA_HeaderMetaData::Title));
  headerdata.set(QCDDA_HeaderMetaData::Title, tmp);

}

void QCDDA_MetaData::splitTitleOfTracks(const QString& divider) {

  for (int i = 0; i < tracklist.count(); i++) {
    int splitPos = tracklist[i].get(QCDDA_TrackMetaData::Title).toString().indexOf(divider);
    if (splitPos >= 0) {
      // split
      QString title = tracklist[i].get(QCDDA_TrackMetaData::Title).toString().mid(splitPos+divider.length());
      QString artist = tracklist[i].get(QCDDA_TrackMetaData::Title).toString().left(splitPos);
      tracklist[i].set(QCDDA_TrackMetaData::Artist, artist);
      tracklist[i].set(QCDDA_TrackMetaData::Title, title);
    }
  }

}

/* from "MEAT LOAF" or "meat loaf" to "Meat Loaf" */
void QCDDA_MetaData::capitalizeTracks() {

  for (int i = 0; i < tracklist.count(); i++) {
    tracklist[i].set(QCDDA_TrackMetaData::Artist, capitalize(tracklist[i].get(QCDDA_TrackMetaData::Artist).toString()));
    tracklist[i].set(QCDDA_TrackMetaData::Title, capitalize(tracklist[i].get(QCDDA_TrackMetaData::Title).toString()));
  }

}

void QCDDA_MetaData::capitalizeHeader() {

  headerdata.set(QCDDA_HeaderMetaData::Artist, capitalize(headerdata.get(QCDDA_HeaderMetaData::Artist).toString()));
  headerdata.set(QCDDA_HeaderMetaData::Title, capitalize(headerdata.get(QCDDA_HeaderMetaData::Title).toString()));

}

void QCDDA_MetaData::setTitleArtistsFromHeader(int forcecount) {

  for (int i = 0; i < forcecount; i++) {
    track(i+1).set(QCDDA_TrackMetaData::Artist, headerdata.get(QCDDA_HeaderMetaData::Artist));
  }

}

void QCDDA_MetaData::clear() {

  data.clear();
  headerdata.clear();
  tracklist.clear();
  cddb_relevant_data_modified = FALSE;
  modified = FALSE;

}

void QCDDA_MetaData::confirm() {

  headerdata.confirm();
  for (int i = 0; i < tracklist.count(); i++) tracklist[i].confirm();
  cddb_relevant_data_modified = FALSE;
  modified = FALSE;

}

bool QCDDA_MetaData::importFromCDDB(const QStringList& lineList) {

  clear();

  // We'll append to this until we've seen all the lines, then parse it after.
  QString dtitle;

  QRegExp rev("# Revision: (\\d+)");
  QRegExp eol("[\r\n]");

  QString extended;

  for (int i = 0; i < lineList.count(); i++) {

    QString line = lineList[i];
    line.replace(eol, "");

    if (rev.indexIn(line) != -1) {
      set(Revision, rev.cap(1).toUInt());
      continue;
    }

    int eqPos = line.indexOf("=");

    QString key, value;
    if (eqPos == -1) {
      continue;
    } else {
      key = line.left(eqPos).trimmed();
      value = unescape(line.mid(eqPos+1).trimmed());
    }

    if (key == "DISCID") {
      bool ok;
      uint discid = value.toUInt(&ok, 16);
      if (ok) headerdata.set(QCDDA_HeaderMetaData::Discid, discid);
    } else if (key == "DTITLE") {
      dtitle += value;
    } else if (key.startsWith("TTITLE")) {
      uint trackNumber = key.mid(6).toUInt();

      int slashPos = value.indexOf(" / ");
      if (slashPos == -1) {
        tracklist.append(QCDDA_TrackMetaData(trackNumber+1, TRUE, "", value));
      } else {
        set(Various, TRUE);
        tracklist.append(QCDDA_TrackMetaData(trackNumber+1, TRUE, value.left(slashPos).trimmed(), value.mid(slashPos+3).trimmed()));
      }

    } else if (key == "EXTD") {
      extended += value;
    } else if (key == "DGENRE") {
      set(Genre, value);
    } else if (key == "DYEAR") {
      set(Year, value);
    } else if (key.startsWith("EXTT")) {
      uint trackNumber = key.mid(4).toUInt();
      QCDDA_TrackMetaData& t = track(trackNumber+1);
      QStringList extt = t.get(QCDDA_TrackMetaData::ExtData).toStringList();
      extt << value.split('\n', QString::SkipEmptyParts);
      t.set(QCDDA_TrackMetaData::ExtData, extt);
    } else if (key.startsWith("T")) {

      // Custom Track data
      uint trackNumber = key.mid(key.indexOf('_')+1).toUInt();

      QRegExp data(QString("^T.*_%1$").arg(trackNumber));
      if (key.contains(data)) {
        QString k = key.mid(1, key.indexOf('_')-1);
        QCDDA_TrackMetaData& t = track(trackNumber);
        t.set(k, t.get(k).toString().append(value));
      }

    }

  }

  QString artist;
  int slashPos = dtitle.indexOf(" / ");
  if (slashPos == -1) {
    headerdata.set(QCDDA_HeaderMetaData::Title, dtitle);
  } else {
    artist = dtitle.left(slashPos).trimmed();
    headerdata.set(QCDDA_HeaderMetaData::Artist, artist);
    headerdata.set(QCDDA_HeaderMetaData::Title, dtitle.mid(slashPos+3).trimmed());
  }

  if (extended.count()>0) {
    set(ExtData, extended.split('\n', QString::SkipEmptyParts));
  }

  return TRUE;

}

bool QCDDA_MetaData::importFromCDDB(const QString& string) {
  return importFromCDDB(string.split('\n', QString::SkipEmptyParts));
}

QStringList QCDDA_MetaData::exportToCDDBStringList(bool submit) const {

  QStringList l;

  int rev = get(Revision).toInt();
  if (rev < 0) rev = 0;
  if (submit) { rev++; /*data["revision"] = rev;*/ }
  l << QString("# Revision: %1").arg(rev);

  // If we are submiting make it a fully compliant CDDB entry
  if (submit) {
    l << "#";
    l << QString("# Submitted via: %1 %2").arg(QCDDB_CLIENTNAME, QCDDB_CLIENTVERSION);
    l << "#";
  }

  l << create_line("DISCID", QString("%1").arg(headerdata.get(QCDDA_HeaderMetaData::Discid).toUInt(), 0, 16));
  QString artist = headerdata.get(QCDDA_HeaderMetaData::Artist).toString();
  l << create_line("DTITLE", artist + " / " + headerdata.get(QCDDA_HeaderMetaData::Title).toString());
  int year = get(Year).toInt();
  l << "DYEAR=" + (0 == year ? QString::null : QString::number(year));
  if (get(Genre) == "Unknown")
    l << create_line("DGENRE", "");
  else
    l << create_line("DGENRE", get(Genre).toString());

  for (int i = 0; i < tracklist.count(); ++i) {
    if (!tracklist[i].get(QCDDA_TrackMetaData::AudioTrack).toBool()) {
      l << create_line(QString("TTITLE%1").arg(i), "Data");
      continue;
    }
    QString trackTitle = tracklist[i].get(QCDDA_TrackMetaData::Title).toString();
    QString trackArtist = tracklist[i].get(QCDDA_TrackMetaData::Artist).toString();
    if (get(Various).toBool() && guessVarious())
      l << create_line(QString("TTITLE%1").arg(i), QString("%1 / %2").arg(trackArtist).arg(trackTitle));
    else
      l << create_line(QString("TTITLE%1").arg(i), trackTitle);
  }

  l << create_line(QString("EXTD"), get(ExtData).toStringList().join("\n"));

  for (int i = 0; i < tracklist.count(); i++)
    l << create_line(QString("EXTT%1").arg(i), tracklist[i].get(QCDDA_TrackMetaData::ExtData).toString());

  if (submit) {
    l << create_line("PLAYORDER", "");
  } else {
    l << create_line("PLAYORDER", get("playorder").toString());
  }

  return l;

}

QString QCDDA_MetaData::exportToCDDB(bool submit) const {
  return exportToCDDBStringList(submit).join("\n");
}

QString QCDDA_MetaData::capitalize(const QString &s) {

  QStringList stringlist = s.split(" ", QString::SkipEmptyParts);
  for (int i = 0; i < stringlist.count(); i++) {
    QString string = stringlist[i].toLower();
    int j = 0;
    while (((string[j] == '(')
		|| (string[j] == '[')
		|| (string[j] == '{')
		) && (j < string.length())) j++;
    string[j] = string[j].toUpper();
    stringlist[i] = string;
  }
  return stringlist.join(" ");

}

QString QCDDA_MetaData::unescape(const QString &value) {
  QString s = value;
  s.replace("\\n", "\n");
  s.replace("\\t", "\t");
  s.replace("\\\\", "\\");
  return s;
}

QStringList QCDDA_MetaData::create_line(const QString& name, const QString& value) {

  Q_ASSERT(name.length() < 254);

  int maxLength = 256-name.length()-2;

  QString tmpValue = value;
  tmpValue.replace("\\", "\\\\");
  tmpValue.replace("\n", "\\n");
  tmpValue.replace("\t", "\\t");

  QStringList lines;

  while (tmpValue.length() > maxLength) {
    lines.append(QString("%1=%2").arg(name, tmpValue.left(maxLength)));
    tmpValue = tmpValue.mid(maxLength);
  }

  lines.append(QString("%1=%2").arg(name, tmpValue));

  return lines;

}
