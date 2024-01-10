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

#ifndef QCDDAMETADATAMODEL_HEADER
#define QCDDAMETADATAMODEL_HEADER

#include <QtCore>
#include <QtGui>

#include "qcddametadata.h"
#include "../device/qcddadevice.h"

#define Q_CDDA_METADATA_MODEL_COLUMN_COUNT		4

#define Q_CDDA_METADATA_MODEL_COLUMN_ARTIST_LABEL	trUtf8("Artist")
#define Q_CDDA_METADATA_MODEL_COLUMN_TITLE_LABEL	trUtf8("Title")
#define Q_CDDA_METADATA_MODEL_COLUMN_LENGTH_LABEL	trUtf8("Length")
#define Q_CDDA_METADATA_MODEL_COLUMN_FRAMEOFFSET_LABEL	trUtf8("Frame Offset")
#define Q_CDDA_METADATA_MODEL_COLUMN_AUDIOTRACK_LABEL	trUtf8("Audio Track")

#define Q_CDDA_METADATA_MODEL_COLUMN_ARTIST_INDEX	0
#define Q_CDDA_METADATA_MODEL_COLUMN_TITLE_INDEX	1
#define Q_CDDA_METADATA_MODEL_COLUMN_LENGTH_INDEX	2
#define Q_CDDA_METADATA_MODEL_COLUMN_FRAMEOFFSET_INDEX	3

#define Q_CDDA_METADATA_MODEL_INTERNAL_ROLE		1982

/** qt 4 audio disc model **/
class QCDDAMetaDataModel : public QAbstractTableModel {

  Q_OBJECT

public:
  QCDDAMetaDataModel(QObject *parent = 0, QCDDADevice *device = 0);
  ~QCDDAMetaDataModel();

  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  int columnCount(const QModelIndex &parent = QModelIndex()) const;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

  Qt::ItemFlags flags(const QModelIndex &index) const;

  quint32 discid() const;
  void setArtist(const QString& a);
  QString artist() const;
  void setAlbum(const QString& a);
  QString album() const;
  void setCategory(const QString& c);
  QString category() const;
  void setGenre(const QString& g);
  QString genre() const;
  void setYear(const QString& year);
  QString year() const;
  void setExtendedData(const QStringList& e);
  QStringList extendedData() const;
  void setCDNum(const int n);
  int cdNum() const;
  void setTrackOffset(const int n);
  int trackOffset() const;

  void setCustomData(const QString& type, const QVariant& data);
  QVariant customData(const QString& type) const;

  bool guessVarious();
  void setVarious(bool various);
  bool isVarious();

  void swapArtistAndTitleOfTracks();
  void swapArtistAndTitle();
  void splitTitleOfTracks(const QString& divider);
  void capitalizeTracks();
  void capitalizeHeader();
  void setTitleArtistsFromHeader();

  int firstSectorOfTrack(const int track) const;
  int lastSectorOfTrack(const int track) const;
  int numOfTracks() const;

  int numOfAudioTracks() const;
  int numOfAudioTracksInSelection() const;

  int length() const;
  int numOfFrames() const;
  int lengthOfAudioTracks() const;

  int numOfFramesOfAudioTracks() const;
  int numOfFramesOfAudioTracksInSelection() const;

  int numOfSkippedFrames(int n = 100) const;
  int lengthOfTrack(int n) const;
  int numOfFramesOfTrack(int n) const;
  double sizeOfTrack(int n) const; //in MiB
  int frameOffsetOfTrack(int n) const;
  bool isAudioTrack(int n) const;

  bool isArtistValid() const;
  bool isTitleValid() const;
  bool isTrackArtistValid(int trackno) const;
  bool isTrackTitleValid(int trackno) const;
  bool isValid() const;

  void setMetadata(const QCDDA_MetaData& metadata);
  QCDDA_MetaData metadata() const;
  void clear();

  void setSelection(const QModelIndexList& list);
  QModelIndexList selection() const;

  bool isCDDBRelevantDataModified() const;
  bool isModified() const;
  void confirm();

private slots:
  void slot_drive_status_changed(QCDDADrive::DriveStatus drivestatus);

private:
  QCDDA_MetaData _metadata;
  QCDDADevice *device;

  QModelIndexList _selection;

};

#endif
