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

#include "qmaskparser.h"

QMaskParser::QMaskParser() {

}

QMaskParser::~QMaskParser() {

}

QString QMaskParser::parseFilenameMask(const QString& mask,
	int trackno, int cdno, int trackoffset,
	const QString& artist, const QString& album,
	const QString& tartist, const QString& ttitle,
	const QString& date, const QString& genre, const QString& ext, const QString& basepath,
	bool fat32compatible) {
  QString filename = mask;
  filename.replace(QRegExp(VAR_HOME_PATH), QDir::homePath());
  QString _artist = artist;
  if (fat32compatible) make_fat32_compatible(_artist); else make_compatible(_artist);
  filename.replace(VAR_ALBUM_ARTIST, _artist, Qt::CaseInsensitive);
  QString _album = album;
  if (fat32compatible) make_fat32_compatible(_album); else make_compatible(_album);
  filename.replace(VAR_ALBUM_TITLE, _album, Qt::CaseInsensitive);
  QString _tartist = tartist;
  if (fat32compatible) make_fat32_compatible(_tartist); else make_compatible(_tartist);
  filename.replace(VAR_TRACK_ARTIST, _tartist, Qt::CaseInsensitive);
  QString _ttitle = ttitle;
  if (fat32compatible) make_fat32_compatible(_ttitle); else make_compatible(_ttitle);
  filename.replace(VAR_TRACK_TITLE, _ttitle, Qt::CaseInsensitive);

  if (trackoffset>0) trackno += trackoffset;
  if (filename.indexOf(VAR_TRACK_NO+".000") > -1)
    filename.replace(VAR_TRACK_NO+".000", QString("%1").arg(trackno, 4, 10, QChar('0')), Qt::CaseInsensitive);
  else if (filename.indexOf(VAR_TRACK_NO+".00") > -1)
    filename.replace(VAR_TRACK_NO+".00", QString("%1").arg(trackno, 3, 10, QChar('0')), Qt::CaseInsensitive);
  else if (filename.indexOf(VAR_TRACK_NO+".0") > -1)
    filename.replace(VAR_TRACK_NO+".0", QString("%1").arg(trackno, 2, 10, QChar('0')), Qt::CaseInsensitive);
  else
    filename.replace(VAR_TRACK_NO, QString("%1").arg(trackno), Qt::CaseInsensitive);

  if (cdno>0) {
    if (filename.indexOf(VAR_CDNO+".000") > -1)
      filename.replace(VAR_CDNO+".000", QString("%1").arg(cdno, 4, 10, QChar('0')), Qt::CaseInsensitive);
    else if (filename.indexOf(VAR_CDNO+".00") > -1)
      filename.replace(VAR_CDNO+".00", QString("%1").arg(cdno, 3, 10, QChar('0')), Qt::CaseInsensitive);
    else if (filename.indexOf(VAR_CDNO+".0") > -1)
      filename.replace(VAR_CDNO+".0", QString("%1").arg(cdno, 2, 10, QChar('0')), Qt::CaseInsensitive);
    else
      filename.replace(VAR_CDNO, QString("%1").arg(cdno), Qt::CaseInsensitive);
  } else {
    filename.replace(VAR_CDNO, QString(), Qt::CaseInsensitive);
  }
  filename.replace(VAR_DATE, date, Qt::CaseInsensitive);
  filename.replace(VAR_GENRE, genre, Qt::CaseInsensitive);
  filename.replace(VAR_EXTENSION, ext, Qt::CaseInsensitive);

  QString bp = basepath;
  if (basepath.contains(VAR_HOME_PATH)) bp.replace(QRegExp(VAR_HOME_PATH), QDir::homePath());
  filename.replace(VAR_BASE_PATH, bp, Qt::CaseInsensitive);

  filename.replace("//", "/");

  return filename.simplified();

}

QString QMaskParser::parseCommandMask(const QString& mask,
	const QString& input, const QString& output,
	int trackno, int cdno, int trackoffset,
	const QString& artist, const QString& album,
	const QString& tartist, const QString& ttitle,
	const QString& date, const QString& genre, const QString& ext, const QString& basepath,
	bool fatcompatible) {
  QString command = mask;
  command.replace(VAR_INPUT_FILE, "\""+input+"\"", Qt::CaseInsensitive);
  command.replace(VAR_OUTPUT_FILE, "\""+output+"\"", Qt::CaseInsensitive);
  command = parseFilenameMask(command, trackno, cdno, trackoffset, artist, album, tartist, ttitle, date, genre, ext, basepath, fatcompatible);
  command.replace("//", "/");
  return command.simplified();
}

QString QMaskParser::parseSimpleMask(const QString& mask,
	int cdno, const QString& artist, const QString& album,
	const QString& date, const QString& genre, const QString& ext,
	bool fat32compatible) {
  QString filename = mask;
  QString _artist = artist;
  if (fat32compatible) make_fat32_compatible(_artist); else make_compatible(_artist);
  filename.replace(VAR_ALBUM_ARTIST, _artist, Qt::CaseInsensitive);
  QString _album = album;
  if (fat32compatible) make_fat32_compatible(_album); else make_compatible(_album);
  filename.replace(VAR_ALBUM_TITLE, _album, Qt::CaseInsensitive);

  if (cdno>0) {
    if (filename.indexOf(VAR_CDNO+".000") > -1)
      filename.replace(VAR_CDNO+".000", QString("%1").arg(cdno, 4, 10, QChar('0')), Qt::CaseInsensitive);
    else if (filename.indexOf(VAR_CDNO+".00") > -1)
      filename.replace(VAR_CDNO+".00", QString("%1").arg(cdno, 3, 10, QChar('0')), Qt::CaseInsensitive);
    else if (filename.indexOf(VAR_CDNO+".0") > -1)
      filename.replace(VAR_CDNO+".0", QString("%1").arg(cdno, 2, 10, QChar('0')), Qt::CaseInsensitive);
    else
      filename.replace(VAR_CDNO, QString("%1").arg(cdno), Qt::CaseInsensitive);
  } else {
    filename.replace(VAR_CDNO, QString(), Qt::CaseInsensitive);
  }
  filename.replace(VAR_DATE, date, Qt::CaseInsensitive);
  filename.replace(VAR_GENRE, genre, Qt::CaseInsensitive);
  filename += "."+ext;

  filename.replace("/", "\\/");

  return filename.simplified();
}

void QMaskParser::make_compatible(QString& string) {
  string.replace("/", "\\/");
  string.replace("\"", "\\\"");
}

//remove \ / : * ? " < > |
void QMaskParser::make_fat32_compatible(QString& string) {
  string.replace("\\", "_");
  string.replace("/", "_");
  string.replace(":", "_");
  string.replace("*", "_");
  string.replace("?", "_");
  string.replace("<", "_");
  string.replace(">", "_");
  string.replace("|", "_");
}


