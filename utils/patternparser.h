/* AUDEX CDDA EXTRACTOR
 * Copyright (C) 2007-2009 by Marco Nelles (audex@maniatek.de)
 * http://opensource.maniatek.de/audex
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

#ifndef PATTERNPARSER_H
#define PATTERNPARSER_H

#include <QObject>
#include <QString>
#include <QXmlDefaultHandler>
#include <QDir>
#include <QDate>
#include <QDateTime>
#include <QImage>
#include <QCryptographicHash>

#include <KDebug>
#include <KLocale>
#include <KStandardDirs>

#include "utils/pid.h"

#define IS_TRUE(val) ( ((val.toLower()=="true")||(val=="1")||(val.toLower()=="on")) ? TRUE : FALSE)

#define VAR_FILENAME_PATTERN				"filenamepattern"
#define VAR_COMMAND_PATTERN				"commandpattern"
#define VAR_SIMPLE_PATTERN				"simplepattern"
#define VAR_TEXT_PATTERN				"textpattern"

#define VAR_ALBUM_ARTIST				"artist"
#define VAR_ALBUM_TITLE					"title"
#define VAR_TRACK_ARTIST				"tartist"
#define VAR_TRACK_TITLE					"ttitle"
#define VAR_TRACK_NO					"trackno"
#define VAR_CDNO					"cdno"
#define VAR_DATE					"date"
#define VAR_GENRE					"genre"
#define VAR_SUFFIX					"suffix"

#define VAR_INPUT_FILE					"i"
#define VAR_OUTPUT_FILE					"o"
#define VAR_COVER_FILE					"cover"

#define VAR_DISCID					"discid"
#define VAR_CD_SIZE					"size"
#define VAR_CD_LENGTH					"length"
#define VAR_CD_NO_OF_TRACKS				"nooftracks"
#define VAR_TODAY					"today"
#define VAR_NOW						"now"
#define VAR_LINEBREAK					"br"


class SaxHandler : public QXmlDefaultHandler {

public:
  SaxHandler();
  ~SaxHandler();

  bool startElement(const QString& namespaceURI, const QString &localName,
		     const QString& qName, const QXmlAttributes& atts);
  bool endElement(const QString& namespaceURI, const QString& localName, const QString& qName);
  bool characters(const QString& ch);
  bool fatalError(const QXmlParseException& exception);

  void setInputFile(const QString& input) { this->input = input; }
  void setOutputFile(const QString& output) { this->output = output; }
  void setTrackNo(const int trackno) { this->trackno = trackno; }
  void setCDNo(const int cdno) { this->cdno = cdno; }
  void setTrackOffset(const int trackoffset) { this->trackoffset = trackoffset; }
  void setArtist(const QString& artist) { this->artist = artist; }
  void setTitle(const QString& title) { this->title = title; }
  void setTrackArtist(const QString& tartist) { this->tartist = tartist; }
  void setTrackTitle(const QString& ttitle) { this->ttitle = ttitle; }
  void setDate(const QString& date) { this->date = date; }
  void setGenre(const QString& genre) { this->genre = genre; }
  void setSuffix(const QString& suffix) { this->suffix = suffix; }
  void setCover(const QImage& cover) { this->cover = cover; }
  void setFAT32Compatible(const bool fat32compatible) { this->fat32compatible = fat32compatible; }
  void setTMPPath(const QString& tmppath) { this->tmppath = tmppath; }
  void setDiscid(const quint32 discid) { this->discid = discid; }
  void setSize(const qreal size) { this->size = size; }
  void setLength(const int length) { this->length = length; }
  void setNoOfTracks(const int nooftracks) { this->nooftracks = nooftracks; }
  void setDemoMode(const bool demomode) { this->demomode = demomode; }

  inline const QString text() const { return _text; }

private:
  QString pattern;
  QString input;
  QString output;
  int trackno;
  int cdno;
  int trackoffset;
  QString artist;
  QString title;
  QString tartist;
  QString ttitle;
  QString date;
  QString genre;
  QString suffix;
  QImage cover;
  bool fat32compatible;
  QString tmppath;
  quint32 discid;
  qreal size;
  int length;
  int nooftracks;

  bool demomode;

  QString _text;

  bool is_filename_pattern;
  bool is_command_pattern;
  bool is_simple_pattern;
  bool is_text_pattern;

  const QString make_compatible(const QString& string);
  const QString make_compatible_2(const QString& string);
  const QString make_fat32_compatible(const QString& string);

};

class PatternParser : public QObject {
  Q_OBJECT
public:
  PatternParser(QObject *parent = 0);
  ~PatternParser();

  const QString parseFilenamePattern(const QString& pattern,
	int trackno, int cdno, int trackoffset,
	const QString& artist, const QString& title,
	const QString& tartist, const QString& ttitle,
	const QString& date, const QString& genre, const QString& suffix,
	bool fat32compatible);

  const QString parseCommandPattern(const QString& pattern,
	const QString& input, const QString& output,
	int trackno, int cdno, int trackoffset,
	const QString& artist, const QString& title,
	const QString& tartist, const QString& ttitle,
	const QString& date, const QString& genre, const QString& suffix, const QImage& cover,
	bool fat32compatible, const QString& tmppath,
	const bool demomode = FALSE);

  const QString parseSimplePattern(const QString& pattern,
	int cdno, const QString& artist, const QString& title,
	const QString& date, const QString& genre, const QString& suffix,
	bool fat32compatible);

  void parseInfoText(QStringList& text,
	const QString& artist, const QString& title,
	const QString& date, const QString& genre,
	const quint32 discid, const qreal size, const int length, const int nooftracks);

signals:
  void error(const QString& message,
	const QString& details = QString());

private:
  const QString p_xmlize_pattern(const QString& pattern);

};

#endif