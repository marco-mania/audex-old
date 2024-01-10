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

#include "cddamodel.h"

CDDAModel::CDDAModel(QObject *parent, const QString& device) : QAbstractTableModel(parent) {

  compact_disc = new KCompactDisc();
  if (!compact_disc) {
    kDebug() << "Unable to create KCompactDisc object. Low mem?";
    error = Error(i18n("Unable to create KCompactDisc object."), i18n("This is an internal error. Check your hardware. If all okay please make bug report."), Error::ERROR, this);
    return;
  }

  setDevice(device);
  connect(compact_disc, SIGNAL(discChanged(unsigned int)), this, SLOT(slot_disc_changed(unsigned int)));
  connect(compact_disc, SIGNAL(discInformation(KCompactDisc::DiscInfo)), this, SLOT(slot_disc_information(KCompactDisc::DiscInfo)));
  connect(compact_disc, SIGNAL(discStatusChanged(KCompactDisc::DiscStatus)), this, SLOT(slot_disc_status_changed(KCompactDisc::DiscStatus)));

  cddb = new KCDDB::Client();
  if (!cddb) {
    kDebug() << "Unable to create KCDDB object. Low mem?";
    error = Error(i18n("Unable to create KCDDB object."), i18n("This is an internal error. Check your hardware. If all okay please make bug report."), Error::ERROR, this);
    return;
  }
  connect(cddb, SIGNAL(finished(KCDDB::Result)), this, SLOT(lookup_cddb_done(KCDDB::Result)));

  cd_info.clear();
  modified = FALSE;

  drive_status = DriveNoStatus;
  disc_status = DiscNoStatus;
  disc_type = DiscNoType;
  disc_info = DiscNoInfo;

}

CDDAModel::~CDDAModel() {

  delete cddb;
  delete compact_disc;

}

void CDDAModel::setDevice(const QString& device) {
  this->_device = device;
  compact_disc->setDevice(device, 50, FALSE);
}

int CDDAModel::rowCount(const QModelIndex &parent) const {
  return parent.isValid()?0:compact_disc->tracks();
}

int CDDAModel::columnCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return CDDA_MODEL_COLUMN_COUNT;
}

QVariant CDDAModel::data(const QModelIndex &index, int role) const {

  if (compact_disc->isNoDisc() || compact_disc->discId()==0) {
    return QVariant();
  }

  if (!index.isValid()) {
    return QVariant();
  }

  if ((index.row() < 0) || (index.row() >= numOfTracks())) {
    return QVariant();
  }

  if (role == Qt::TextAlignmentRole) {
    return int(Qt::AlignLeft | Qt::AlignVCenter);
  }

  /*if (role == Qt::ForegroundRole) {
    switch (index.column()) {
      case CDDA_MODEL_COLUMN_ARTIST_INDEX :
			if (!isTrackArtistValid(index.row()+1)) return qVariantFromValue(QColor(Qt::gray));
			break;
      case CDDA_MODEL_COLUMN_TITLE_INDEX :
			if (!isTrackTitleValid(index.row()+1)) return qVariantFromValue(QColor(Qt::gray));
			break;
    }
  }*/

  if ((role == Qt::DisplayRole)
	|| (role == Qt::CheckStateRole && index.column()==CDDA_MODEL_COLUMN_RIP_INDEX)
	|| (role == CDDA_MODEL_INTERNAL_ROLE)
	|| (role == Qt::EditRole)) {

    switch (index.column()) {
      case CDDA_MODEL_COLUMN_RIP_INDEX :
			if (role == Qt::CheckStateRole) {
			  return isTrackInSelection(index.row()+1)?Qt::Checked:Qt::Unchecked;
			}
			break;
      case CDDA_MODEL_COLUMN_TRACK_INDEX :
			return index.row()+1+(trackOffset()>1?trackOffset():0);
      case CDDA_MODEL_COLUMN_LENGTH_INDEX :
			if (role==CDDA_MODEL_INTERNAL_ROLE)
			  return lengthOfTrack(index.row()+1);
			else
			  return QString("%1:%2").arg(lengthOfTrack(index.row()+1) / 60, 2, 10, QChar('0')).arg(lengthOfTrack(index.row()+1) % 60, 2, 10, QChar('0'));
      case CDDA_MODEL_COLUMN_ARTIST_INDEX :
			if (isAudioTrack(index.row()+1)) {
                          QString a = cd_info.track(index.row()).get(KCDDB::Artist).toString();
			  if (a.isEmpty()) return compact_disc->trackArtist(index.row()+1);
                          return a;
                        }
			break;
      case CDDA_MODEL_COLUMN_TITLE_INDEX :
			if (isAudioTrack(index.row()+1)) {
                          QString t = cd_info.track(index.row()).get(KCDDB::Title).toString();
                          if (t.isEmpty()) return compact_disc->trackTitle(index.row()+1);
			  return t;
                        }
			break;
      default : ;
    }

  }

  return QVariant();

}

bool CDDAModel::setData(const QModelIndex &index, const QVariant &value, int role) {

  if (compact_disc->isNoDisc() || compact_disc->discId()==0) {
    return FALSE;
  }

  if (!index.isValid()) {
    return FALSE;
  }

  if ((index.row() < 0) || (index.row() >= numOfTracks())) {
    return FALSE;
  }

  if (role == Qt::EditRole) {
    bool changed = FALSE;
    switch (index.column()) {
      case CDDA_MODEL_COLUMN_ARTIST_INDEX :
        if (value != cd_info.track(index.row()).get(KCDDB::Artist)) {
          cd_info.track(index.row()).set(KCDDB::Artist, value);
          changed = TRUE;
        }
        break;
      case CDDA_MODEL_COLUMN_TITLE_INDEX :
        if (value != cd_info.track(index.row()).get(KCDDB::Title)) {
          cd_info.track(index.row()).set(KCDDB::Title, value);
          changed = TRUE;
        }
        break;
      default : return FALSE;
    }
    if (changed) {
      emit dataChanged(index, index);
      modify();
    }
    return changed;
  }

  return FALSE;

}

QVariant CDDAModel::headerData(int section, Qt::Orientation orientation, int role) const {

  Q_UNUSED(orientation);

  if (orientation == Qt::Horizontal) {
    switch (role) {
      case Qt::DisplayRole :
        switch (section) {
          case CDDA_MODEL_COLUMN_RIP_INDEX : return CDDA_MODEL_COLUMN_RIP_LABEL;
          case CDDA_MODEL_COLUMN_TRACK_INDEX : return CDDA_MODEL_COLUMN_TRACK_LABEL;
          case CDDA_MODEL_COLUMN_LENGTH_INDEX : return CDDA_MODEL_COLUMN_LENGTH_LABEL;
          case CDDA_MODEL_COLUMN_ARTIST_INDEX : return CDDA_MODEL_COLUMN_ARTIST_LABEL;
          case CDDA_MODEL_COLUMN_TITLE_INDEX : return CDDA_MODEL_COLUMN_TITLE_LABEL;
          default : ;
        }
	break;
      case Qt::TextAlignmentRole :
        return Qt::AlignLeft;
      default : ;
    }
  } else if (orientation == Qt::Vertical) {
    if (role == Qt::DisplayRole) {
      return QVariant(section+1);
    }
  }

  return QVariant();

}

Qt::ItemFlags CDDAModel::flags(const QModelIndex &index) const {

  Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
  if ((index.column()==CDDA_MODEL_COLUMN_ARTIST_INDEX)
	|| (index.column()==CDDA_MODEL_COLUMN_TITLE_INDEX)) {
    flags |= Qt::ItemIsEditable;
  }
  if (!isAudioTrack(index.row()+1)) return Qt::ItemIsEnabled;
  return flags;

}

unsigned int CDDAModel::discid() const {
  return compact_disc->discId();
}

void CDDAModel::setArtist(const QString& a) {
  if (compact_disc->isNoDisc() || compact_disc->discId()==0) return;
  if (a != cd_info.get(KCDDB::Artist).toString()) {
    if (a.isEmpty()) {
      cd_info.set(KCDDB::Artist, compact_disc->discArtist());
    } else {
      cd_info.set(KCDDB::Artist, a);
    }
    modify();
    reset();
  }
}

const QString CDDAModel::artist() const {
  if (compact_disc->isNoDisc() || compact_disc->discId()==0) return QString();
  QString a = cd_info.get(KCDDB::Artist).toString();
  if (a.isEmpty()) return compact_disc->discArtist();
  return a;
}

void CDDAModel::setTitle(const QString& t) {
  if (compact_disc->isNoDisc() || compact_disc->discId()==0) return;
  if (t != cd_info.get(KCDDB::Title).toString()) {
    if (t.isEmpty()) {
      cd_info.set(KCDDB::Title, compact_disc->discTitle());
    } else {
      cd_info.set(KCDDB::Title, t);
    }
    modify();
    reset();
  }
}

const QString CDDAModel::title() const {
  if (compact_disc->isNoDisc() || compact_disc->discId()==0) return QString();
  QString t = cd_info.get(KCDDB::Title).toString();
  if (t.isEmpty()) return compact_disc->discTitle();
  return t;
}

void CDDAModel::setCategory(const QString& c) {
  if (compact_disc->isNoDisc() || compact_disc->discId()==0) return;
  if (c != cd_info.get(KCDDB::Category).toString()) {
    cd_info.set(KCDDB::Category, c);
    modify();
    reset();
  }
}

const QString CDDAModel::category() const {
  if (compact_disc->isNoDisc() || compact_disc->discId()==0) return QString();
  return cd_info.get(KCDDB::Category).toString();
}

void CDDAModel::setGenre(const QString& g) {
  if (compact_disc->isNoDisc() || compact_disc->discId()==0) return;
  if (g != cd_info.get(KCDDB::Genre).toString()) {
    cd_info.set(KCDDB::Genre, g);
    modify();
    reset();
  }
}

const QString CDDAModel::genre() const {
  if (compact_disc->isNoDisc() || compact_disc->discId()==0) return QString();
  return cd_info.get(KCDDB::Genre).toString();
}

void CDDAModel::setYear(const QString& year) {
  if (compact_disc->isNoDisc() || compact_disc->discId()==0) return;
  if (year != cd_info.get(KCDDB::Year).toString()) {
    cd_info.set(KCDDB::Year, year);
    modify();
    reset();
  }
}

const QString CDDAModel::year() const {
  if (compact_disc->isNoDisc() || compact_disc->discId()==0) return QString();
  return cd_info.get(KCDDB::Year).toString();
}

void CDDAModel::setExtendedData(const QStringList& e) {
  if (compact_disc->isNoDisc() || compact_disc->discId()==0) return;
  if (e != cd_info.get(KCDDB::Comment).toStringList()) {
    cd_info.set(KCDDB::Comment, e);
    modify();
    reset();
  }
}

const QStringList CDDAModel::extendedData() const {
  if (compact_disc->isNoDisc() || compact_disc->discId()==0) return QStringList();
  return cd_info.get(KCDDB::Comment).toStringList();
}

void CDDAModel::setCDNum(const int n) {
  if (compact_disc->isNoDisc() || compact_disc->discId()==0) return;
  if (n != cd_info.get("DNO").toInt()) {
    cd_info.set("DNO", n);
    modify();
    reset();
  }
}

int CDDAModel::cdNum() const {
  if (compact_disc->isNoDisc() || compact_disc->discId()==0) return -1;
  return cd_info.get("DNO").toInt();
}

void CDDAModel::setTrackOffset(const int n) {
  if (compact_disc->isNoDisc() || compact_disc->discId()==0) return;
  if (n != cd_info.get("DTRACKOFFSET").toInt()) {
    cd_info.set("DTRACKOFFSET", n);
    modify();
    reset();
  }
}

int CDDAModel::trackOffset() const {
  if (compact_disc->isNoDisc() || compact_disc->discId()==0) return 1;
  return cd_info.get("DTRACKOFFSET").toInt();
}

int CDDAModel::guessMultiCD(QString& newTitle) const {

  if (compact_disc->isNoDisc() || compact_disc->discId()==0) return -1;
  QString t = cd_info.get(KCDDB::Title).toString();
  QRegExp rx1("[\\(|\\[]* *([c|C][d|D]|[d|D][i|I][s|S][k|c|K|C]) *[0-9]* *[\\)|\\]]* *$");
  int i = rx1.indexIn(t);
  if (i>=0) {
    QString frac = t.mid(i);
    QRegExp rx2("(\\d+)");
    rx2.indexIn(frac);
    bool ok;
    int cdnum = rx2.cap(0).toInt(&ok);
    if (ok) {
      if (cdnum<0) return -1;
      if (cdnum==0) cdnum = 1;
      newTitle = t.left(i).trimmed();
      return cdnum;
    }
  }
  return -1;

}

void CDDAModel::setMultiCD(const bool multi) {
  if (compact_disc->isNoDisc() || compact_disc->discId()==0) return;
  if (multi != cd_info.get("DMULTICD").toBool()) {
    cd_info.set("DMULTICD", multi);
    modify();
    reset();
  }
}

bool CDDAModel::isMultiCD() const {
  if (compact_disc->isNoDisc() || compact_disc->discId()==0) return FALSE;
  return cd_info.get("DMULTICD").toBool();
}

void CDDAModel::setCustomData(const QString& type, const QVariant& data) {
  if (compact_disc->isNoDisc() || compact_disc->discId()==0) return;
  if (data != cd_info.get(type)) {
    cd_info.set(type, data);
    modify();
    reset();
  }
}

const QVariant CDDAModel::customData(const QString& type) const {
  if (compact_disc->isNoDisc() || compact_disc->discId()==0) return QVariant();
  return cd_info.get(type);
}

void CDDAModel::setCustomDataPerTrack(const int n, const QString& type, const QVariant& data) {
  if (compact_disc->isNoDisc() || compact_disc->discId()==0) return;
  if (data != cd_info.track(n).get(type)) {
    cd_info.track(n).set(type, data);
    modify();
    reset();
  }
}

const QVariant CDDAModel::getCustomDataPerTrack(const int n, const QString& type) {
  if (compact_disc->isNoDisc() || compact_disc->discId()==0) return QVariant();
  return cd_info.track(n).get(type);
}

const QImage CDDAModel::cover() const {
  return _cover;
}

void CDDAModel::setCover(const QImage& image) {
  _cover = image;
  reset();
}

void CDDAModel::clearCover() {
  _cover = QImage();
  reset();
}

bool CDDAModel::guessVarious() const {
  if (compact_disc->isNoDisc() || compact_disc->discId()==0) return FALSE;
  QString a;
  for (int i = 0; i < cd_info.numberOfTracks(); ++i) {
    if ((i > 0) && (cd_info.track(i).get(KCDDB::Artist).toString().toLower() != a.toLower())) return TRUE;
    a = cd_info.track(i).get(KCDDB::Artist).toString();
  }
  return FALSE;
}

void CDDAModel::setVarious(bool various) {
  if (compact_disc->isNoDisc() || compact_disc->discId()==0) return;
  if (various != cd_info.get("DVARIOUS").toBool()) {
    cd_info.set("DVARIOUS", various);
    modify();
  }
}

bool CDDAModel::isVarious() {
  if (compact_disc->isNoDisc() || compact_disc->discId()==0) return FALSE;
  return cd_info.get("DVARIOUS").toBool();
}

void CDDAModel::swapArtistAndTitleOfTracks() {
  if (compact_disc->isNoDisc() || compact_disc->discId()==0) return;
  for (int i = 0; i < cd_info.numberOfTracks(); ++i) {
    QVariant tmp = cd_info.track(i).get(KCDDB::Artist);
    cd_info.track(i).set(KCDDB::Artist, cd_info.track(i).get(KCDDB::Title));
    cd_info.track(i).set(KCDDB::Title, tmp);
  }
  modified = TRUE; emit cddbDataModified();
  reset();
}

void CDDAModel::swapArtistAndTitle() {
  if (compact_disc->isNoDisc() || compact_disc->discId()==0) return;
  QVariant tmp = cd_info.get(KCDDB::Title);
  cd_info.set(KCDDB::Title, cd_info.get(KCDDB::Artist));
  cd_info.set(KCDDB::Artist, tmp);
  modified = TRUE; emit cddbDataModified();
  reset();
}

void CDDAModel::splitTitleOfTracks(const QString& divider) {
  if (compact_disc->isNoDisc() || compact_disc->discId()==0) return;
  for (int i = 0; i < cd_info.numberOfTracks(); ++i) {
    int splitPos = cd_info.track(i).get(KCDDB::Title).toString().indexOf(divider);
    if (splitPos >= 0) {
      // split
      QString title = cd_info.track(i).get(KCDDB::Title).toString().mid(splitPos+divider.length());
      QString artist = cd_info.track(i).get(KCDDB::Title).toString().left(splitPos);
      cd_info.track(i).set(KCDDB::Artist, artist);
      cd_info.track(i).set(KCDDB::Title, title);
    }
  }
  modified = TRUE; emit cddbDataModified();
  reset();
}

void CDDAModel::capitalizeTracks() {
  if (compact_disc->isNoDisc() || compact_disc->discId()==0) return;
  for (int i = 0; i < cd_info.numberOfTracks(); ++i) {
    cd_info.track(i).set(KCDDB::Artist, capitalize(cd_info.track(i).get(KCDDB::Artist).toString()));
    cd_info.track(i).set(KCDDB::Title, capitalize(cd_info.track(i).get(KCDDB::Title).toString()));
  }
  modified = TRUE; emit cddbDataModified();
  reset();
}

void CDDAModel::capitalizeHeader() {
  if (compact_disc->isNoDisc() || compact_disc->discId()==0) return;
  cd_info.set(KCDDB::Artist, capitalize(cd_info.get(KCDDB::Artist).toString()));
  cd_info.set(KCDDB::Title, capitalize(cd_info.get(KCDDB::Title).toString()));
  modified = TRUE; emit cddbDataModified();
  reset();
}

void CDDAModel::setTitleArtistsFromHeader() {
  if (compact_disc->isNoDisc() || compact_disc->discId()==0) return;
  for (int i = 0; i < cd_info.numberOfTracks(); ++i) {
    cd_info.track(i).set(KCDDB::Artist, cd_info.get(KCDDB::Artist));
  }
  modified = TRUE; emit cddbDataModified();
  reset();
}

int CDDAModel::numOfTracks() const {
  return compact_disc->tracks();
}

int CDDAModel::numOfAudioTracks() const {
  int c = 0;
  for (int i = 1; i <= numOfTracks(); ++i) {
    if (isAudioTrack(i)) ++c;
  }
  return c;
}

int CDDAModel::numOfAudioTracksInSelection() const {
  return sel_tracks.count();
}

int CDDAModel::length() const {
  return compact_disc->discLength();
}

int CDDAModel::lengthOfAudioTracks() const {
  int c = 0;
  for (int i = 1; i <= numOfTracks(); ++i) {
    if (isAudioTrack(i)) c += lengthOfTrack(i);
  }
  return c;
}

int CDDAModel::lengthOfAudioTracksInSelection() const {
  QSet<int>::ConstIterator it(sel_tracks.begin()), end(sel_tracks.end());
  int l = 0;
  for (; it != end; ++it) {
    if (isAudioTrack(*it)) l += lengthOfTrack(*it);
  }
  return l;
}

int CDDAModel::lengthOfTrack(int n) const {
  return compact_disc->trackLength(n);
}

bool CDDAModel::isAudioTrack(int n) const {
  return compact_disc->isAudio(n);
}

void CDDAModel::clear() {
  cd_info.clear();
  clearCover();
  reset();
}

void CDDAModel::toggle(int row) {

  if (sel_tracks.contains(row+1)) {
    sel_tracks.remove(row+1);
  } else {
    sel_tracks.insert(row+1);
  }

  emit hasSelection(0 != sel_tracks.size());

}

bool CDDAModel::isTrackInSelection(int n) const {
  return sel_tracks.contains(n);
}

bool CDDAModel::isModified() const {
  return modified;
}

void CDDAModel::confirm() {
  modified = FALSE;
}

Error CDDAModel::lastError() const {
  return error;
}

CDDAModel::DriveStatus CDDAModel::driveStatus() const {
  return drive_status;
}

const QString CDDAModel::driveStatusString() const {
  switch (drive_status) {
    case DriveEmpty : return i18n("No disc in drive");
    case DriveReady : return i18n("Disc in drive");
    case DriveOpen : return i18n("Drive tray open");
    case DriveNotReady : return i18n("Drive not ready");
    case DriveError : return i18n("Drive error");
    default : ;
  }
  return i18n("No drive status available");
}

CDDAModel::DiscStatus CDDAModel::discStatus() const {
  return disc_status;
}

const QString CDDAModel::discStatusString() const {
  switch (disc_status) {
    case DiscPlaying : return i18n("Disc playing");
    case DiscPaused : return i18n("Disc paused");
    case DiscStopped : return i18n("Disc stopped");
    default : ;
  }
  return i18n("No disc status available");
}

CDDAModel::DiscType CDDAModel::discType() const {
  return disc_type;
}

const QString CDDAModel::discTypeString() const {
  switch (disc_type) {
    case DiscContainsAudioTracks : return i18n("Audio disc");
    case DiscContainsNoAudioTracks : return i18n("No audio disc");
    default : ;
  }
  return i18n("No disc type information available");
}

CDDAModel::DiscInfo CDDAModel::discInfo() const {
  return disc_info;
}

const QString CDDAModel::discInfoString() const {
  switch (disc_info) {
    case DiscCDTEXTInfo : return i18n("CD-Text");
    case DiscCDDBInfo : return i18n("CDDB");
    case DiscPhonenMetadataInfo : return i18n("Phonon Metadata");
    default : ;
  }
  return i18n("No disc information available");
}

void CDDAModel::lookupCDDB() {

  if (compact_disc->isNoDisc() || compact_disc->discId()==0) return;

  kDebug() << "lookupCDDB called";

  emit cddbLookupStarted();

  cddb->config().reparse();
  cddb->setBlockingMode(FALSE);
  cddb->lookup(compact_disc->discSignature());

}

void CDDAModel::submitCDDB() {

  if (compact_disc->isNoDisc() || compact_disc->discId()==0) return;

  kDebug() << "submitCDDB called";

  cddb->config().reparse();
  cddb->setBlockingMode(TRUE);
  cddb->submit(cd_info, compact_disc->discSignature());

  confirm();

  emit cddbDataSubmited(TRUE);

}

void CDDAModel::eject() {
  compact_disc->eject();
}

void CDDAModel::play(const unsigned int track) {
  compact_disc->playTrack(track);
}

void CDDAModel::playPos(const unsigned int position) {
  compact_disc->playPosition(position);
}

unsigned int CDDAModel::position() const {
  return compact_disc->trackPosition();
}

void CDDAModel::prev() {
  compact_disc->prev();
}

void CDDAModel::next() {
  compact_disc->next();
}

void CDDAModel::pause() {
  compact_disc->pause();
}

void CDDAModel::stop() {
  compact_disc->stop();
}

void CDDAModel::slot_disc_changed(unsigned int tracks) {

  kDebug() << "driveChanged (" << tracks << ")";

  clear();
  confirm();

  sel_tracks.clear();
  for (unsigned int i = 1; i <= tracks; ++i) {
    if (isAudioTrack(i)) sel_tracks.insert(i);
  }

  emit hasSelection(0!=sel_tracks.size());

  if (tracks > 0) disc_type = DiscContainsAudioTracks; else disc_type = DiscContainsNoAudioTracks;
  emit discChanged(disc_type);

}

void CDDAModel::slot_disc_information(KCompactDisc::DiscInfo info) {

  kDebug() << "infoChanged (" << info << ")";

  switch (info) {
    case KCompactDisc::Cdtext : disc_info = DiscCDTEXTInfo; break;
    case KCompactDisc::Cddb : disc_info = DiscCDDBInfo; break;
    case KCompactDisc::PhononMetadata : disc_info = DiscPhonenMetadataInfo; break;
  }

  set_default_values();
  setVarious(guessVarious());
  reset();

  emit discInfoChanged(disc_info);

}

void CDDAModel::slot_disc_status_changed(KCompactDisc::DiscStatus status) {

  kDebug() << "statusChanged (" << status << ")";

  DriveStatus ds = drive_status;

  switch (status) {

    case KCompactDisc::Playing : disc_status = DiscPlaying; drive_status = DriveReady; break;
    case KCompactDisc::Paused : disc_status = DiscPaused; drive_status = DriveReady; break;
    case KCompactDisc::Stopped : disc_status = DiscStopped; drive_status = DriveReady; break;

    case KCompactDisc::Ejected : disc_status = DiscNoStatus; drive_status = DriveOpen; break;
    case KCompactDisc::NoDisc : disc_status = DiscNoStatus; drive_status = DriveEmpty; break;
    case KCompactDisc::NotReady : disc_status = DiscNoStatus; drive_status = DriveNotReady; break;
    case KCompactDisc::Error : disc_status = DiscNoStatus; drive_status = DriveError; break;

    default : break;

  }

  if (disc_status==DiscNoStatus) {
    disc_info = DiscNoInfo;
    emit discInfoChanged(disc_info);
  }

  if (ds!=drive_status) emit driveStatusChanged(drive_status);
  emit discStatusChanged(disc_status);

  stop();

}

void CDDAModel::lookup_cddb_done(KCDDB::Result result) {

  if ((result != KCDDB::Success) && (result != KCDDB::MultipleRecordFound)) {
    error = Error(i18n("No entry found in CDDB."), i18n("This means no data found in the CDDB database. Please enter the data manually. Maybe try another CDDB server."), Error::ERROR, this);
    emit cddbLookupDone(FALSE);
    return;
  }

  KCDDB::CDInfo info = cddb->lookupResponse().first();
  if (cddb->lookupResponse().count() > 1) {
    KCDDB::CDInfoList cddb_info = cddb->lookupResponse();
    KCDDB::CDInfoList::iterator it;
    QStringList list;
    for ( it = cddb_info.begin(); it != cddb_info.end(); ++it) {
      list.append(QString("%1, %2, %3").arg((*it).get(KCDDB::Artist).toString())
          .arg((*it).get(KCDDB::Title).toString()).arg((*it).get(KCDDB::Genre).toString()));
    }

    bool ok = FALSE;
    QString res = KInputDialog::getItem(
      i18n("Select CDDB Entry"),
      i18n("Select a CDDB entry:"), list, 0, FALSE, &ok,
      NULL);

    if (ok) {
      // The user selected and item and pressed OK
      int c = 0;
      for (QStringList::Iterator it = list.begin(); it != list.end(); ++it) {
        if (*it==res) break;
        c++;
      }
      if (c < cddb_info.size()) info = cddb_info[c];
    } else {
      emit cddbLookupDone(TRUE);
      return;
      // user pressed Cancel
    }

  }

  cd_info = info;
  set_default_values();
  disc_info = DiscCDDBInfo;
  setVarious(guessVarious());
  QString newTitle;
  int cdnum = guessMultiCD(newTitle);
  if (cdnum > 0) {
    setMultiCD(TRUE);
    setCDNum(cdnum);
    setTitle(newTitle);
  }
  reset();

  emit cddbLookupDone(TRUE);

  emit discInfoChanged(disc_info);

}

const QString CDDAModel::capitalize(const QString &s) {

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

void CDDAModel::set_default_values() {

  if (cd_info.get(KCDDB::Year).toString().isEmpty()) cd_info.set(KCDDB::Year, QString("%1").arg(QDate::currentDate().year()));
  cd_info.set("DNO", 1);
  cd_info.set("DTRACKOFFSET", 1);
  cd_info.set("DMULTICD", FALSE);

}

void CDDAModel::modify() {

  modified = TRUE; emit cddbDataModified();
  if (disc_info==DiscNoInfo) { disc_info = DiscManualInfo; emit discInfoChanged(disc_info); }

}
