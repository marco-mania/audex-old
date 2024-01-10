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

#ifndef QCDDA_MetaData_HEADER
#define QCDDA_MetaData_HEADER

#include <QtCore>

#define QCDDB_CLIENTNAME	"QCDDB"
#define QCDDB_CLIENTVERSION	"0.1"

class QCDDA_HeaderMetaData {

public:
  QCDDA_HeaderMetaData();
  QCDDA_HeaderMetaData(const QCDDA_HeaderMetaData& other);
  QCDDA_HeaderMetaData& operator=(const QCDDA_HeaderMetaData& other);
  ~QCDDA_HeaderMetaData();

  enum HeaderDataType {
    Artist,		/**< The artist of the CD */
    Title,		/**< The title of the CD */
    Category,		/**< The freedb category of the entry.
				Needs to be one of: blues, classical,
				country, data, fold, jazz, misc, newage,
				reggae, rock, soundtrack */
    Discid		/**< The discid of the CD **/
  };

  QVariant get(HeaderDataType type) const;
  QVariant get(const QString& type) const;
  bool set(HeaderDataType type, const QVariant &d);
  bool set(const QString& type, const QVariant &d);

  bool isCDDBRelevantDataModified() const;
  bool isModified() const;
  bool isArtistValid() const;
  bool isTitleValid() const;
  bool isValid() const;
  bool isCategoryValid() const;

  void clear();
  void confirm();

private:
  QMap<QString, QVariant> data;
  bool cddb_relevant_data_modified;
  bool modified;

};

typedef QList<QCDDA_HeaderMetaData> QCDDA_HeaderMetaDataList;


class QCDDA_TrackMetaData {

public:
  QCDDA_TrackMetaData();
  QCDDA_TrackMetaData(const QCDDA_TrackMetaData& other);
  QCDDA_TrackMetaData& operator=(const QCDDA_TrackMetaData& other);
  QCDDA_TrackMetaData(const int trackno, const bool audiotrack = TRUE,
			const QString& artist = "", const QString& title = "",
			const int year = 0, const QStringList& extendeddata = QStringList());
  ~QCDDA_TrackMetaData();

  enum TrackDataType {
    Artist,		/**< The artist of the track */
    Title,		/**< The title of the track */
    Year,			/**< The year the track was produced
				By default, the year of the track is the
				same as for the whole cd **/
    ExtData,		/**< A comment for the track */
    Length,		/**< Length in sec **/
    TrackNo,		/**< The track # of the track **/
    TrackOffset,		/**< Track sector offset **/
    AudioTrack		/**< Track # audio track? **/
  };

  QVariant get(TrackDataType type) const;
  QVariant get(const QString& type) const;
  bool set(TrackDataType type, const QVariant &d);
  bool set(const QString& type, const QVariant &d);

  bool isCDDBRelevantDataModified() const;
  bool isModified() const;
  bool isArtistValid() const;
  bool isTitleValid() const;
  bool isValid() const;

  void clear();
  void confirm();

private:
  QMap<QString, QVariant> data;
  bool cddb_relevant_data_modified;
  bool modified;

};

typedef QList<QCDDA_TrackMetaData> QCDDA_TrackMetaDataList;



class QCDDA_MetaData {

public:
  QCDDA_MetaData();
  QCDDA_MetaData(const QCDDA_MetaData& other);
  QCDDA_MetaData& operator=(const QCDDA_MetaData& other);
  ~QCDDA_MetaData();

  enum CDDataType {
    Year,		/**< The year the cd was produced **/
    Genre,		/**< The genre of the CD */
    ExtData,		/**< A comment for the CD */
    TrackNoOffset,	/**< Track # offset **/
    CDNum,		/**< CD # **/
    Length,		/**< Length in sec **/
    Revision,		/**< Revision **/
    Various,		/**< Various artists? **/
    Cover		/**< Cover **/
  };

  void setHeaderData(const QCDDA_HeaderMetaData& headerData);
  QCDDA_HeaderMetaData& headerData();
  QCDDA_HeaderMetaData headerData() const;


  QCDDA_TrackMetaData& track(int trackno);
  QCDDA_TrackMetaData track(int trackno) const;
  void addTrack(const int trackno, const bool audiotrack = TRUE,
			const QString& artist = "", const QString& title = "",
			const int year = 0, const QStringList& extendeddata = QStringList());
  QVariant trackArtist(int trackno) const;
  QVariant trackTitle(int trackno) const;
  int trackCount() const;


  QVariant get(CDDataType type) const;
  QVariant get(const QString& type) const;
  bool set(CDDataType type, const QVariant& d);
  bool set(const QString& type, const QVariant &d);

  bool isCDDBRelevantDataModified() const;
  bool isModified() const;
  bool isValid() const;
  bool offsetsValid() const;
  bool guessVarious() const;

  void swapArtistAndTitleOfTracks();
  void swapArtistAndTitle();
  void splitTitleOfTracks(const QString& divider);
  void capitalizeTracks();
  void capitalizeHeader();
  void setTitleArtistsFromHeader(int forcecount);

  void clear();
  void confirm();

  bool importFromCDDB(const QStringList& lineList);
  bool importFromCDDB(const QString& string);
  QStringList exportToCDDBStringList(bool submit) const;
  QString exportToCDDB(bool submit) const;

private:
  QMap<QString, QVariant> data;
  bool cddb_relevant_data_modified;
  bool modified;

  QCDDA_HeaderMetaData headerdata;
  QCDDA_TrackMetaDataList tracklist;

  static QString capitalize(const QString &s);
  static QString unescape(const QString &value);

  /**
  * Creates a line in the form NAME=VALUE, and splits it into several
  * lines if the line gets longer than 256 chars
  */
  static QStringList create_line(const QString& name, const QString& value);

};

typedef QList<QCDDA_MetaData> QCDDA_MetaDataList;

#endif
