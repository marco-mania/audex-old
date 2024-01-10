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

#ifndef CDDAMODEL_H
#define CDDAMODEL_H

#include <QAbstractTableModel>
#include <QImage>
#include <QString>
#include <QVariant>
#include <QModelIndexList>

#include <KDebug>
#include <KLocale>
#include <KInputDialog>
#include <libkcompactdisc/kcompactdisc.h>

#include <libkcddb/kcddb.h>
#include <libkcddb/client.h>
#include <libkcddb/cdinfo.h>

#include "../utils/error.h"

#define CDDA_MODEL_COLUMN_COUNT		3

#define CDDA_MODEL_COLUMN_ARTIST_LABEL	i18n("Artist")
#define CDDA_MODEL_COLUMN_TITLE_LABEL	i18n("Title")
#define CDDA_MODEL_COLUMN_LENGTH_LABEL	i18n("Length")

#define CDDA_MODEL_COLUMN_ARTIST_INDEX	0
#define CDDA_MODEL_COLUMN_TITLE_INDEX	1
#define CDDA_MODEL_COLUMN_LENGTH_INDEX	2

#define CDDA_MODEL_INTERNAL_ROLE	1982

/** kde audio disc model **/
class CDDAModel : public QAbstractTableModel {

  Q_OBJECT

public:
  CDDAModel(QObject *parent = 0, const QString& device = "/dev/cdrom");
  ~CDDAModel();

  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  int columnCount(const QModelIndex &parent = QModelIndex()) const;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

  Qt::ItemFlags flags(const QModelIndex &index) const;

  unsigned int discid() const;
  void setArtist(const QString& a);
  const QString artist() const;
  void setTitle(const QString& t);
  const QString title() const;
  void setCategory(const QString& c);
  const QString category() const;
  void setGenre(const QString& g);
  const QString genre() const;
  void setYear(const QString& year);
  const QString year() const;
  void setExtendedData(const QStringList& e);
  const QStringList extendedData() const;
  void setCDNum(const int n);
  int cdNum() const;
  void setTrackOffset(const int n);
  int trackOffset() const;
  void setMultiCD(const bool multi);
  bool isMultiCD() const;

  void setCustomData(const QString& type, const QVariant& data);
  const QVariant customData(const QString& type) const;

  void setCustomDataPerTrack(const int n, const QString& type, const QVariant& data);
  const QVariant getCustomDataPerTrack(const int n, const QString& type);

  const QImage cover() const;
  void setCover(const QImage& image);
  void clearCover();

  bool guessVarious();
  void setVarious(bool various);
  bool isVarious();

  void swapArtistAndTitleOfTracks();
  void swapArtistAndTitle();
  void splitTitleOfTracks(const QString& divider);
  void capitalizeTracks();
  void capitalizeHeader();
  void setTitleArtistsFromHeader();

  int numOfTracks() const;
  int numOfAudioTracks() const;
  int numOfAudioTracksInSelection() const;

  int length() const;
  int lengthOfAudioTracks() const;
  int lengthOfAudioTracksInSelection() const;
  int lengthOfTrack(int n) const;

  bool isAudioTrack(int n) const;

  void clear();

  void setSelection(const QModelIndexList& list);
  QModelIndexList selection() const;
  bool isTrackInSelection(int n) const;

  bool isModified() const;
  void confirm();

  //enum MessageType { mtWarning, mtError };
  //bool isValid(QString *message, QCDDAModel::MessageType *type);

  Error lastError() const;

  enum DriveStatus {
    DriveNoStatus,
    DriveEmpty,
    DriveReady,
    DriveOpen,
    DriveNotReady,
    DriveError
  };

  enum DiscStatus {
    DiscNoStatus,
    DiscPlaying,
    DiscPaused,
    DiscStopped
  };

  enum DiscType {
    DiscNoType,
    DiscContainsAudioTracks,
    DiscContainsNoAudioTracks
  };

  enum DiscInfo {
    DiscNoInfo,
    DiscManualInfo, /*user input*/
    DiscCDTEXTInfo,
    DiscCDDBInfo,
    DiscPhonenMetadataInfo
  };

  DriveStatus driveStatus() const;
  const QString driveStatusString() const;

  DiscStatus discStatus() const;
  const QString discStatusString() const;

  DiscType discType() const;
  const QString discTypeString() const;

  DiscInfo discInfo() const;
  const QString discInfoString() const;

public slots:
  void lookupCDDB();
  void submitCDDB();
  void eject();

  void play(const unsigned int track = 1);
  void playPos(const unsigned int position);
  unsigned int position() const;
  void prev();
  void next();
  void pause();
  void stop();

signals:
  void driveStatusChanged(const CDDAModel::DriveStatus status);
  void discStatusChanged(const CDDAModel::DiscStatus status);
  void discChanged(const CDDAModel::DiscType type);
  void discInfoChanged(const CDDAModel::DiscInfo info);

  void cddbLookupStarted();
  void cddbLookupDone(const bool successful);

  void cddbDataModified();
  void cddbDataSubmited(const bool successful);

private slots:
  void slot_disc_changed(unsigned int tracks);
  void slot_disc_information(KCompactDisc::DiscInfo info);
  void slot_disc_status_changed(KCompactDisc::DiscStatus status);

  void lookup_cddb_done(KCDDB::Result);
  //void submit_cddb_done(KCDDB::Result);

private:
  QString device;
  KCompactDisc::KCompactDisc *compact_disc;
  KCDDB::Client *cddb;
  KCDDB::CDInfo cd_info;
  QImage _cover;
  bool modified;
  DriveStatus drive_status;
  DiscStatus disc_status;
  DiscType disc_type;
  DiscInfo disc_info;
  Error error;

  QModelIndexList _selection;

  QString capitalize(const QString &s);

  void check_artist_and_title_are_valid();

  void set_default_values();

  void modify();

};

#endif
