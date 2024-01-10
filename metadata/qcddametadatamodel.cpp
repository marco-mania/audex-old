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

#include "qcddametadatamodel.h"

QCDDAMetaDataModel::QCDDAMetaDataModel(QObject *parent, QCDDADevice *device) : QAbstractTableModel(parent) {

  this->device = device;
  connect(device, SIGNAL(driveStatusChanged(QCDDADrive::DriveStatus)), this, SLOT(slot_drive_status_changed(QCDDADrive::DriveStatus)));

}

QCDDAMetaDataModel::~QCDDAMetaDataModel() {

}

int QCDDAMetaDataModel::rowCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return numOfAudioTracks();
}

int QCDDAMetaDataModel::columnCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return Q_CDDA_METADATA_MODEL_COLUMN_COUNT;
}

QVariant QCDDAMetaDataModel::data(const QModelIndex &index, int role) const {

  if (!index.isValid()) {
    return QVariant();
  }

  if ((index.row() < 0) || (index.row() >= numOfTracks())) {
    return QVariant();
  }

  if (role == Qt::TextAlignmentRole) {
    return int(Qt::AlignRight | Qt::AlignVCenter);
  }

  if (role == Qt::ForegroundRole) {
    switch (index.column()) {
      case Q_CDDA_METADATA_MODEL_COLUMN_ARTIST_INDEX :
			if (!isTrackArtistValid(index.row()+1)) return qVariantFromValue(QColor(Qt::gray));
			break;
      case Q_CDDA_METADATA_MODEL_COLUMN_TITLE_INDEX :
			if (!isTrackTitleValid(index.row()+1)) return qVariantFromValue(QColor(Qt::gray));
			break;
    }
  }

  if ((role == Qt::DisplayRole)
	|| (role == Q_CDDA_METADATA_MODEL_INTERNAL_ROLE)
	|| (role == Qt::EditRole)) {

    switch (index.column()) {
      case Q_CDDA_METADATA_MODEL_COLUMN_ARTIST_INDEX :
			if (isAudioTrack(index.row()+1))
			  return _metadata.trackArtist(index.row()+1);
			break;
      case Q_CDDA_METADATA_MODEL_COLUMN_TITLE_INDEX :
			if (isAudioTrack(index.row()+1))
			  return _metadata.trackTitle(index.row()+1);
			break;
      case Q_CDDA_METADATA_MODEL_COLUMN_LENGTH_INDEX : if (role==Q_CDDA_METADATA_MODEL_INTERNAL_ROLE) return lengthOfTrack(index.row()+1); else
							return QString("%1:%2").arg(lengthOfTrack(index.row()+1) / 60, 2, 10, QChar('0')).arg(lengthOfTrack(index.row()+1) % 60, 2, 10, QChar('0'));
      case Q_CDDA_METADATA_MODEL_COLUMN_FRAMEOFFSET_INDEX : return frameOffsetOfTrack(index.row()+1);
      default : ;
    }

  }

  return QVariant();

}

bool QCDDAMetaDataModel::setData(const QModelIndex &index, const QVariant &value, int role) {

  if (!index.isValid()) {
    return FALSE;
  }

  if ((index.row() < 0) || (index.row() >= numOfTracks())) {
    return FALSE;
  }

  if (role == Qt::EditRole) {
    int changed = TRUE;
    switch (index.column()) {
      case Q_CDDA_METADATA_MODEL_COLUMN_ARTIST_INDEX :
        changed = _metadata.track(index.row()+1).set(QCDDA_TrackMetaData::Artist, value);
        break;
      case Q_CDDA_METADATA_MODEL_COLUMN_TITLE_INDEX :
        changed = _metadata.track(index.row()+1).set(QCDDA_TrackMetaData::Title, value);
        break;
      default : return FALSE;
    }
    if (changed) emit dataChanged(index, index);
    return changed;
  }

  return FALSE;

}

QVariant QCDDAMetaDataModel::headerData(int section, Qt::Orientation orientation, int role) const {

  Q_UNUSED(orientation);

  if (orientation == Qt::Horizontal) {
    if (role == Qt::DisplayRole) {
      switch (section) {
        case Q_CDDA_METADATA_MODEL_COLUMN_ARTIST_INDEX : return Q_CDDA_METADATA_MODEL_COLUMN_ARTIST_LABEL;
        case Q_CDDA_METADATA_MODEL_COLUMN_TITLE_INDEX : return Q_CDDA_METADATA_MODEL_COLUMN_TITLE_LABEL;
        case Q_CDDA_METADATA_MODEL_COLUMN_LENGTH_INDEX : return Q_CDDA_METADATA_MODEL_COLUMN_LENGTH_LABEL;
        case Q_CDDA_METADATA_MODEL_COLUMN_FRAMEOFFSET_INDEX : return Q_CDDA_METADATA_MODEL_COLUMN_FRAMEOFFSET_LABEL;
        default : ;
      }
    }
  } else if (orientation == Qt::Vertical) {
    if (role == Qt::DisplayRole) {
      return QVariant(section+1);
    }
  }

  return QVariant();

}

Qt::ItemFlags QCDDAMetaDataModel::flags(const QModelIndex &index) const {

  Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
  if ((index.column()==Q_CDDA_METADATA_MODEL_COLUMN_ARTIST_INDEX)
	|| (index.column()==Q_CDDA_METADATA_MODEL_COLUMN_TITLE_INDEX)) {
    flags |= Qt::ItemIsEditable;
  }
  if (!isAudioTrack(index.row()+1)) return Qt::ItemIsEnabled;
  return flags;

}

quint32 QCDDAMetaDataModel::discid() const {
  if (device) return device->discid();
  return 0;
}

void QCDDAMetaDataModel::setArtist(const QString& a) {
  _metadata.headerData().set(QCDDA_HeaderMetaData::Artist, a);
}

QString QCDDAMetaDataModel::artist() const {
  return _metadata.headerData().get(QCDDA_HeaderMetaData::Artist).toString();
}

void QCDDAMetaDataModel::setAlbum(const QString& a) {
  _metadata.headerData().set(QCDDA_HeaderMetaData::Title, a);
}

QString QCDDAMetaDataModel::album() const {
  return _metadata.headerData().get(QCDDA_HeaderMetaData::Title).toString();
}

void QCDDAMetaDataModel::setCategory(const QString& c) {
  _metadata.headerData().set(QCDDA_HeaderMetaData::Category, c);
}

QString QCDDAMetaDataModel::category() const {
  return _metadata.headerData().get(QCDDA_HeaderMetaData::Category).toString();
}

void QCDDAMetaDataModel::setGenre(const QString& g) {
  _metadata.set(QCDDA_MetaData::Genre, g);
}

QString QCDDAMetaDataModel::genre() const {
  return _metadata.get(QCDDA_MetaData::Genre).toString();
}

void QCDDAMetaDataModel::setYear(const QString& year) {
  _metadata.set(QCDDA_MetaData::Year, year);
}

QString QCDDAMetaDataModel::year() const {
  return _metadata.get(QCDDA_MetaData::Year).toString();
}

void QCDDAMetaDataModel::setExtendedData(const QStringList& e) {
  _metadata.set(QCDDA_MetaData::ExtData, e);
}

QStringList QCDDAMetaDataModel::extendedData() const {
  return _metadata.get(QCDDA_MetaData::ExtData).toStringList();
}

void QCDDAMetaDataModel::setCDNum(const int n) {
  _metadata.set(QCDDA_MetaData::CDNum, n);
}

int QCDDAMetaDataModel::cdNum() const {
  return _metadata.get(QCDDA_MetaData::CDNum).toInt();
}

void QCDDAMetaDataModel::setTrackOffset(const int n) {
  _metadata.set(QCDDA_MetaData::TrackNoOffset, n);
}

int QCDDAMetaDataModel::trackOffset() const {
  return _metadata.get(QCDDA_MetaData::TrackNoOffset).toInt();
}

void QCDDAMetaDataModel::setCustomData(const QString& type, const QVariant& data) {
  _metadata.set(type, data);
}

QVariant QCDDAMetaDataModel::customData(const QString& type) const {
  return _metadata.get(type);
}

bool QCDDAMetaDataModel::guessVarious() {
  return _metadata.guessVarious();
}

void QCDDAMetaDataModel::setVarious(bool various) {
  _metadata.set(QCDDA_MetaData::Various, various);
}

bool QCDDAMetaDataModel::isVarious() {
  return _metadata.get(QCDDA_MetaData::Various).toBool();
}

void QCDDAMetaDataModel::swapArtistAndTitleOfTracks() {
  _metadata.swapArtistAndTitleOfTracks();
  reset();
}

void QCDDAMetaDataModel::swapArtistAndTitle() {
  _metadata.swapArtistAndTitle();
  reset();
}

void QCDDAMetaDataModel::splitTitleOfTracks(const QString& divider) {
  _metadata.splitTitleOfTracks(divider);
  reset();
}

void QCDDAMetaDataModel::capitalizeTracks() {
  _metadata.capitalizeTracks();
  reset();
}

void QCDDAMetaDataModel::capitalizeHeader() {
  _metadata.capitalizeHeader();
}

void QCDDAMetaDataModel::setTitleArtistsFromHeader() {
  _metadata.setTitleArtistsFromHeader(numOfAudioTracks());
  reset();
}

int QCDDAMetaDataModel::firstSectorOfTrack(const int track) const {
  if (device) return device->firstSectorOfTrack(track);
  return 0;
}

int QCDDAMetaDataModel::lastSectorOfTrack(const int track) const {
  if (device) return device->lastSectorOfTrack(track);
  return 0;
}

int QCDDAMetaDataModel::numOfTracks() const {
  if (device) return device->numOfTracks();
  return 0;
}

int QCDDAMetaDataModel::numOfAudioTracks() const {
  if (device) return device->numOfAudioTracks();
  return 0;
}

int QCDDAMetaDataModel::numOfAudioTracksInSelection() const {
  if (device) {
    if (_selection.count()==0) return device->numOfAudioTracks();
    int c = 0;
    for (int i = 0; i < _selection.count(); i++) {
      if ((_selection[i].row() < 0) || (_selection[i].row() >= numOfTracks()) || (!isAudioTrack(_selection[i].row()+1))) continue;
      c++;
    }
    return c;
  }
  return 0;
}

int QCDDAMetaDataModel::length() const {
  if (device) return device->length();
  return 0;
}

int QCDDAMetaDataModel::numOfFrames() const {
  if (device) return device->numOfFrames();
  return 0;
}

int QCDDAMetaDataModel::lengthOfAudioTracks() const {
  if (device) return device->lengthOfAudioTracks();
  return 0;
}

int QCDDAMetaDataModel::numOfFramesOfAudioTracks() const {
  if (device) return device->numOfFramesOfAudioTracks();
  return 0;
}

int QCDDAMetaDataModel::numOfFramesOfAudioTracksInSelection() const {
  if (device) {
    if (_selection.count()==0) return device->numOfFramesOfAudioTracks();
    int frames = 0;
    for (int i = 0; i < _selection.count(); i++) {
      if ((_selection[i].row() < 0) || (_selection[i].row() >= numOfTracks()) || (!isAudioTrack(_selection[i].row()+1))) continue;
      frames += numOfFramesOfTrack(_selection[i].row()+1);
    }
    return frames;
  }
  return 0;
}

int QCDDAMetaDataModel::numOfSkippedFrames(int n) const {
  if (device) return device->numOfSkippedFrames(n);
  return 0;
}

int QCDDAMetaDataModel::lengthOfTrack(int n) const {
  if (device) return device->lengthOfTrack(n);
  return 0;
}

int QCDDAMetaDataModel::numOfFramesOfTrack(int n) const {
  if (device) return device->numOfFramesOfTrack(n);
  return 0;
}

double QCDDAMetaDataModel::sizeOfTrack(int n) const {
  if (device) return device->sizeOfTrack(n);
  return 0.0f;
}

int QCDDAMetaDataModel::frameOffsetOfTrack(int n) const {
  if (device) return device->frameOffsetOfTrack(n);
  return 0;
}

bool QCDDAMetaDataModel::isAudioTrack(int n) const {
  if (device) return device->isAudioTrack(n);
  return TRUE;
}

bool QCDDAMetaDataModel::isArtistValid() const {
  return _metadata.headerData().isArtistValid();
}

bool QCDDAMetaDataModel::isTitleValid() const {
  return _metadata.headerData().isTitleValid();
}

bool QCDDAMetaDataModel::isTrackArtistValid(int trackno) const {
  return _metadata.track(trackno).isArtistValid();
}

bool QCDDAMetaDataModel::isTrackTitleValid(int trackno) const {
  return _metadata.track(trackno).isTitleValid();
}

bool QCDDAMetaDataModel::isValid() const {
  if (_metadata.trackCount() < numOfAudioTracks()) return FALSE;
  return _metadata.isValid();
}

void QCDDAMetaDataModel::setMetadata(const QCDDA_MetaData& metadata) {
  this->_metadata = metadata;
  _metadata.headerData().set(QCDDA_HeaderMetaData::Discid, device->discid());
  _metadata.set(QCDDA_MetaData::Length, device->length()+2);
  for (int i = 1; i <= device->numOfTracks(); i++) {
    _metadata.track(i).set(QCDDA_TrackMetaData::TrackOffset, device->frameOffsetOfTrack(i)+150);
    _metadata.track(i).set(QCDDA_TrackMetaData::Length, device->lengthOfTrack(i));
    _metadata.track(i).set(QCDDA_TrackMetaData::AudioTrack, device->isAudioTrack(i));
  }
  reset();
  _metadata.confirm();
}

QCDDA_MetaData QCDDAMetaDataModel::metadata() const {
  return this->_metadata;
}

void QCDDAMetaDataModel::clear() {
  _metadata.clear();
  reset();
}

void QCDDAMetaDataModel::setSelection(const QModelIndexList& selection) {
  this->_selection = selection;
}

QModelIndexList QCDDAMetaDataModel::selection() const {
  return _selection;
}

bool QCDDAMetaDataModel::isCDDBRelevantDataModified() const {
  return _metadata.isCDDBRelevantDataModified();
}

bool QCDDAMetaDataModel::isModified() const {
  return _metadata.isModified();
}

void QCDDAMetaDataModel::confirm() {
  _metadata.confirm();
}

void QCDDAMetaDataModel::slot_drive_status_changed(QCDDADrive::DriveStatus drivestatus) {
  if (drivestatus!=QCDDADrive::dsDriveReady) {
    clear();
  } else {
    _metadata.headerData().set(QCDDA_HeaderMetaData::Discid, device->discid());
    _metadata.set(QCDDA_MetaData::Length, device->length());
    for (int i = 1; i <= device->numOfTracks(); i++) {
      _metadata.addTrack(i, device->isAudioTrack(i));
      _metadata.track(i).set(QCDDA_TrackMetaData::TrackOffset, device->frameOffsetOfTrack(i));
      _metadata.track(i).set(QCDDA_TrackMetaData::Length, device->lengthOfTrack(i));
    }
    reset();
    _metadata.confirm();
  }
}
