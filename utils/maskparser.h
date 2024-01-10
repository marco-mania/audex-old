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

#ifndef MASKPARSER_H
#define MASKPARSER_H

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

#include "../utils/pid.h"

#define IS_TRUE(val) ( ((val.toLower()=="true")||(val=="1")||(val.toLower()=="on")) ? TRUE : FALSE)

#define TAG_FILENAME_MASK				"filenamemask"
#define TAG_COMMAND_MASK				"commandmask"
#define TAG_SIMPLE_MASK					"simplemask"
#define TAG_TEXT_MASK					"textmask"

#define TAG_HOME_PATH					"home"
#define TAG_BASE_PATH					"basepath"
#define TAG_ALBUM_ARTIST				"artist"
#define TAG_ALBUM_TITLE					"title"
#define TAG_TRACK_ARTIST				"tartist"
#define TAG_TRACK_TITLE					"ttitle"
#define TAG_TRACK_NO					"trackno"
#define TAG_CDNO					"cdno"
#define TAG_DATE					"date"
#define TAG_GENRE					"genre"
#define TAG_SUFFIX					"suffix"

#define TAG_INPUT_FILE					"i"
#define TAG_OUTPUT_FILE					"o"
#define TAG_COVER_FILE					"cover"

#define TAG_DISCID					"discid"
#define TAG_CD_SIZE					"size"
#define TAG_CD_LENGTH					"length"
#define TAG_CD_NO_OF_TRACKS				"nooftracks"
#define TAG_TODAY					"today"
#define TAG_NOW						"now"
#define TAG_LINEBREAK					"br"


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
  void setBasepath(const QString& basepath) { this->basepath = basepath; }
  void setFAT32Compatible(const bool fat32compatible) { this->fat32compatible = fat32compatible; }
  void setTMPPath(const QString& tmppath) { this->tmppath = tmppath; }
  void setDiscid(const quint32 discid) { this->discid = discid; }
  void setSize(const qreal size) { this->size = size; }
  void setLength(const int length) { this->length = length; }
  void setNoOfTracks(const int nooftracks) { this->nooftracks = nooftracks; }
  void setDemoMode(const bool demomode) { this->demomode = demomode; }

  inline const QString text() const { return _text; }

private:
  QString mask;
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
  QString basepath;
  bool fat32compatible;
  QString tmppath;
  quint32 discid;
  qreal size;
  int length;
  int nooftracks;

  bool demomode;

  QString _text;

  bool is_filename_mask;
  bool is_command_mask;
  bool is_simple_mask;
  bool is_text_mask;

  void make_compatible(QString& string);
  void make_fat32_compatible(QString& string);

};

class MaskParser : public QObject {
  Q_OBJECT
public:
  MaskParser(QObject *parent = 0);
  ~MaskParser();

  QString parseFilenameMask(const QString& mask,
	int trackno, int cdno, int trackoffset,
	const QString& artist, const QString& title,
	const QString& tartist, const QString& ttitle,
	const QString& date, const QString& genre, const QString& suffix, const QString& basepath,
	bool fat32compatible);

  QString parseCommandMask(const QString& mask,
	const QString& input, const QString& output,
	int trackno, int cdno, int trackoffset,
	const QString& artist, const QString& title,
	const QString& tartist, const QString& ttitle,
	const QString& date, const QString& genre, const QString& suffix, const QImage& cover, const QString& basepath,
	bool fat32compatible, const QString& tmppath,
	const bool demomode = FALSE);

  QString parseSimpleMask(const QString& mask,
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
  const QString p_xmlize_mask(const QString& mask);

};

#endif
