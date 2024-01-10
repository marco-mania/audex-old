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

#ifndef QMASKPARSER_HEADER
#define QMASKPARSER_HEADER

#include <QtCore>

#define VAR_HOME_PATH					QString("~")
#define VAR_BASE_PATH					QString("${basepath}")
#define VAR_ALBUM_ARTIST				QString("${artist}")
#define VAR_ALBUM_TITLE					QString("${album}")
#define VAR_TRACK_ARTIST				QString("${tartist}")
#define VAR_TRACK_TITLE					QString("${ttitle}")
#define VAR_TRACK_NO					QString("${trackno}")
#define VAR_CDNO					QString("${cdno}")
#define VAR_DATE					QString("${date}")
#define VAR_GENRE					QString("${genre}")
#define VAR_EXTENSION					QString("${extension}")

#define VAR_INPUT_FILE					QString("$i")
#define VAR_OUTPUT_FILE					QString("$o")

class QMaskParser : public QObject {
  Q_OBJECT
public:
  QMaskParser();
  ~QMaskParser();

  QString parseFilenameMask(const QString& mask,
	int trackno, int cdno, int trackoffset,
	const QString& artist, const QString& album,
	const QString& tartist, const QString& ttitle,
	const QString& date, const QString& genre, const QString& ext, const QString& basepath,
	bool fat32compatible);

  QString parseCommandMask(const QString& mask,
	const QString& input, const QString& output,
	int trackno, int cdno, int trackoffset,
	const QString& artist, const QString& album,
	const QString& tartist, const QString& ttitle,
	const QString& date, const QString& genre, const QString& ext, const QString& basepath,
	bool fat32compatible);

  QString parseSimpleMask(const QString& mask,
	int cdno, const QString& artist, const QString& album,
	const QString& date, const QString& genre, const QString& ext,
	bool fat32compatible);

private:
  void make_compatible(QString& string);
  void make_fat32_compatible(QString& string);

};

#endif
