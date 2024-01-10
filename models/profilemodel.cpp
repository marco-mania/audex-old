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

#include "profilemodel.h"

ProfileModel::ProfileModel(QObject *parent) {
  Q_UNUSED(parent);
  current_profile_row = -1;
  revert();
}

ProfileModel::~ProfileModel() {
  clear();
}

int ProfileModel::rowCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return cache.count();
}

int ProfileModel::columnCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return PROFILE_MODEL_COLUMN_COUNT;
}

QVariant ProfileModel::data(const QModelIndex &index, int role) const {

  if (!index.isValid()) {
    return QVariant();
  }

  if ((index.row() < 0) || (index.row() >= cache.count())) {
    return QVariant();
  }

  if (role == Qt::TextAlignmentRole) {
    return int(Qt::AlignLeft | Qt::AlignVCenter);
  }

  if ((role == Qt::DisplayRole) || (role == Qt::EditRole)) {
    switch (index.column()) {
      case PROFILE_MODEL_COLUMN_NAME_INDEX : return cache.at(index.row())[PROFILE_MODEL_NAME_KEY];
      case PROFILE_MODEL_COLUMN_MASK_INDEX : return cache.at(index.row())[PROFILE_MODEL_MASK_KEY];
      case PROFILE_MODEL_COLUMN_COMMAND_INDEX : return cache.at(index.row())[PROFILE_MODEL_COMMAND_KEY];
      case PROFILE_MODEL_COLUMN_SUFFIX_INDEX : return cache.at(index.row())[PROFILE_MODEL_SUFFIX_KEY];
      case PROFILE_MODEL_COLUMN_PROFILEINDEX_INDEX : return cache.at(index.row())[PROFILE_MODEL_PROFILEINDEX_KEY];
      case PROFILE_MODEL_COLUMN_FAT32COMPATIBLE_INDEX : return cache.at(index.row())[PROFILE_MODEL_FAT32COMPATIBLE_KEY];
      case PROFILE_MODEL_COLUMN_SC_INDEX : return cache.at(index.row())[PROFILE_MODEL_SC_KEY];
      case PROFILE_MODEL_COLUMN_SC_SCALE_INDEX : return cache.at(index.row())[PROFILE_MODEL_SC_SCALE_KEY];
      case PROFILE_MODEL_COLUMN_SC_SIZE_INDEX : return cache.at(index.row())[PROFILE_MODEL_SC_SIZE_KEY];
      case PROFILE_MODEL_COLUMN_SC_FORMAT_INDEX : return cache.at(index.row())[PROFILE_MODEL_SC_FORMAT_KEY];
      case PROFILE_MODEL_COLUMN_SC_NAME_INDEX : return cache.at(index.row())[PROFILE_MODEL_SC_NAME_KEY];
      case PROFILE_MODEL_COLUMN_PL_INDEX : return cache.at(index.row())[PROFILE_MODEL_PL_KEY];
      case PROFILE_MODEL_COLUMN_PL_FORMAT_INDEX : return cache.at(index.row())[PROFILE_MODEL_PL_FORMAT_KEY];
      case PROFILE_MODEL_COLUMN_PL_NAME_INDEX : return cache.at(index.row())[PROFILE_MODEL_PL_NAME_KEY];
      case PROFILE_MODEL_COLUMN_DISCID_INDEX : return cache.at(index.row())[PROFILE_MODEL_DISCID_KEY];
      case PROFILE_MODEL_COLUMN_INF_INDEX : return cache.at(index.row())[PROFILE_MODEL_INF_KEY];
      case PROFILE_MODEL_COLUMN_INF_TEXT_INDEX : return cache.at(index.row())[PROFILE_MODEL_INF_TEXT_KEY];
      case PROFILE_MODEL_COLUMN_INF_NAME_INDEX : return cache.at(index.row())[PROFILE_MODEL_INF_NAME_KEY];
      case PROFILE_MODEL_COLUMN_INF_SUFFIX_INDEX : return cache.at(index.row())[PROFILE_MODEL_INF_SUFFIX_KEY];
      case PROFILE_MODEL_COLUMN_HL_INDEX : return cache.at(index.row())[PROFILE_MODEL_HL_KEY];
      case PROFILE_MODEL_COLUMN_HL_FORMAT_INDEX : return cache.at(index.row())[PROFILE_MODEL_HL_FORMAT_KEY];
      case PROFILE_MODEL_COLUMN_HL_NAME_INDEX : return cache.at(index.row())[PROFILE_MODEL_HL_NAME_KEY];
      default : ;
    }
  }

  if (role == Qt::DecorationRole) {
    return KIcon("application-x-zerosize");
  }

  return QVariant();

}

bool ProfileModel::setData(const QModelIndex &index, const QVariant &value, int role) {

  if (!index.isValid()) {
    return FALSE;
  }

  if ((index.row() < 0) || (index.row() >= cache.count())) {
    return FALSE;
  }

  if (role == Qt::EditRole) {

      switch (index.column()) {
        case PROFILE_MODEL_COLUMN_NAME_INDEX :
	  if (value.toString().isEmpty()) {
	    error = Error(
		i18n("Profile name must not be empty."),
		i18n("You've given no name for the profile. Please set one."),
		Error::ERROR,
		this);
	    return FALSE;
	  }
	  break;
        case PROFILE_MODEL_COLUMN_MASK_INDEX :
	  if (value.toString().isEmpty()) {
	    error = Error(
		i18n("Mask must not be empty."),
		i18n("You've given no mask for the profile. Please set one."),
		Error::ERROR,
		this);
	    return FALSE;
	  }
	  break;
        case PROFILE_MODEL_COLUMN_COMMAND_INDEX :
	  if (value.toString().isEmpty()) {
	    error = Error(
		i18n("Command must not be empty."),
		i18n("You've given no command for the profile. Please set one."),
		Error::ERROR,
		this);
	  return FALSE;
	}
	break;
        case PROFILE_MODEL_COLUMN_SUFFIX_INDEX :
	  if (value.toString().isEmpty()) {
	    error = Error(
		i18n("Suffix must not be empty."),
		i18n("You've given no filename suffix for the profile. Please set one."),
		Error::ERROR,
		this);
	    return FALSE;
	  }
	  break;
        case PROFILE_MODEL_COLUMN_PROFILEINDEX_INDEX : break;
        case PROFILE_MODEL_COLUMN_FAT32COMPATIBLE_INDEX : break;
        case PROFILE_MODEL_COLUMN_SC_INDEX : break;
        case PROFILE_MODEL_COLUMN_SC_SCALE_INDEX : break;
        case PROFILE_MODEL_COLUMN_SC_SIZE_INDEX : break;
        case PROFILE_MODEL_COLUMN_SC_FORMAT_INDEX :
	  if ((value.toString()!="JPEG") && (value.toString()!="JPG") && (value.toString()!="PNG") && (value.toString()!="TIFF") && (value.toString()!="BMP")) {
	    error = Error(
		i18n("The image file format is unknown."),
		i18n("Your given image file format is unknown. Please choose on of these formats: JPG/JPEG, PNG or BMP."),
		Error::ERROR,
		this);
	    return FALSE;
	  }
	  break;
        case PROFILE_MODEL_COLUMN_SC_NAME_INDEX :
	  if (value.toString().isEmpty()) {
	    error = Error(
		i18n("Cover name must not be empty."),
		i18n("You've given no name for the cover. Please set one."),
		Error::ERROR,
		this);
            return FALSE;
	  }
	  break;
        case PROFILE_MODEL_COLUMN_PL_INDEX : break;
        case PROFILE_MODEL_COLUMN_PL_FORMAT_INDEX :
	  if ((value.toString()!="M3U") && (value.toString()!="PLS") && (value.toString()!="XSPF")) {
	    error = Error(
		i18n("The playlist file format is unknown."),
		i18n("Your given playlist file format is unknown. Please choose on of these formats: M3U, PLS or XSPF."),
		Error::ERROR,
		this);
	    return FALSE;
	  }
	  break;
        case PROFILE_MODEL_COLUMN_PL_NAME_INDEX :
	  if (value.toString().isEmpty()) {
	    error = Error(
		i18n("Playlist name must not be empty."),
		i18n("You've given no name for the playlist. Please set one."),
		Error::ERROR,
		this);
	    return FALSE;
	  }
	  break;
	case PROFILE_MODEL_COLUMN_DISCID_INDEX : break;
	case PROFILE_MODEL_COLUMN_INF_INDEX : break;
	case PROFILE_MODEL_COLUMN_INF_TEXT_INDEX : break;
	case PROFILE_MODEL_COLUMN_INF_NAME_INDEX :
          if (value.toString().isEmpty()) {
	    error = Error(
		i18n("Info text name must not be empty."),
		i18n("You've given no name for the info text file. Please set one."),
		Error::ERROR,
		this);
	    return FALSE;
	  }
	  break;
	case PROFILE_MODEL_COLUMN_INF_SUFFIX_INDEX : break;
	  if (value.toString().isEmpty()) {
	    error = Error(
		i18n("Info text file name suffix must not be empty."),
		i18n("You've given no suffix for the info text file. Please set one."),
		Error::ERROR,
		this);
	    return FALSE;
	  }
	  break;
	case PROFILE_MODEL_COLUMN_HL_INDEX : break;
	case PROFILE_MODEL_COLUMN_HL_FORMAT_INDEX :
          if (value.toString()!="SFV") {
	    error = Error(
		i18n("The hashlist file format is unknown."),
		i18n("Your given hashlist file format is unknown. Please choose on of these formats: SFV."),
		Error::ERROR,
		this);
	    return FALSE;
	  }
	  break;
	case PROFILE_MODEL_COLUMN_HL_NAME_INDEX : break;
	  if (value.toString().isEmpty()) {
	    error = Error(
		i18n("Hashlist name must not be empty."),
		i18n("You've given no name for the hashlist. Please set one."),
		Error::ERROR,
		this);
	    return FALSE;
	  }
	  break;
        default : return FALSE;
      }

      //check if name is unique
      if (index.column()==PROFILE_MODEL_COLUMN_NAME_INDEX) {
        bool found = FALSE;
        for (int i = 0; i < cache.count(); ++i) {
          if (i==index.row()) continue;
          if (value.toString()==cache.at(i)[PROFILE_MODEL_NAME_KEY].toString()) { found = TRUE; break; }
        }
        if (found) {
          error = Error(
		i18n("Profile name already exists."),
		i18n("Your profile name %1 already exists in the set of profiles. Please choose an unique one.", value.toString()),
		Error::ERROR,
		this);
          return FALSE;
        }
      }

      //check if profile index is unique
      if ((index.column()==PROFILE_MODEL_COLUMN_PROFILEINDEX_INDEX) && (value.toInt()!=-1)) {
        bool found = FALSE;
        for (int i = 0; i < cache.count(); ++i) {
          if (i==index.row()) continue;
          if (value.toInt()==cache.at(i)[PROFILE_MODEL_PROFILEINDEX_KEY].toInt()) { found = TRUE; break; }
        }
        if (found) {
          error = Error(
		i18n("Profile index already exists."),
		i18n("Your profile index %1 already exists in the set of profiles. Please choose an unique one.", value.toInt()),
		Error::ERROR,
		this);
          return FALSE;
        }
      }

      switch (index.column()) {
        case PROFILE_MODEL_COLUMN_NAME_INDEX : cache[index.row()][PROFILE_MODEL_NAME_KEY] = value; break;
        case PROFILE_MODEL_COLUMN_MASK_INDEX : cache[index.row()][PROFILE_MODEL_MASK_KEY] = value; break;
        case PROFILE_MODEL_COLUMN_COMMAND_INDEX : cache[index.row()][PROFILE_MODEL_COMMAND_KEY] = value; break;
        case PROFILE_MODEL_COLUMN_SUFFIX_INDEX : cache[index.row()][PROFILE_MODEL_SUFFIX_KEY] = value; break;
        case PROFILE_MODEL_COLUMN_PROFILEINDEX_INDEX : cache[index.row()][PROFILE_MODEL_PROFILEINDEX_KEY] = value; break;
        case PROFILE_MODEL_COLUMN_FAT32COMPATIBLE_INDEX : cache[index.row()][PROFILE_MODEL_FAT32COMPATIBLE_KEY] = value; break;
        case PROFILE_MODEL_COLUMN_SC_INDEX : cache[index.row()][PROFILE_MODEL_SC_KEY] = value; break;
        case PROFILE_MODEL_COLUMN_SC_SCALE_INDEX : cache[index.row()][PROFILE_MODEL_SC_SCALE_KEY] = value; break;
        case PROFILE_MODEL_COLUMN_SC_SIZE_INDEX : cache[index.row()][PROFILE_MODEL_SC_SIZE_KEY] = value; break;
        case PROFILE_MODEL_COLUMN_SC_FORMAT_INDEX : cache[index.row()][PROFILE_MODEL_SC_FORMAT_KEY] = value; break;
        case PROFILE_MODEL_COLUMN_SC_NAME_INDEX : cache[index.row()][PROFILE_MODEL_SC_NAME_KEY] = value; break;
        case PROFILE_MODEL_COLUMN_PL_INDEX : cache[index.row()][PROFILE_MODEL_PL_KEY] = value; break;
        case PROFILE_MODEL_COLUMN_PL_FORMAT_INDEX : cache[index.row()][PROFILE_MODEL_PL_FORMAT_KEY] = value; break;
        case PROFILE_MODEL_COLUMN_PL_NAME_INDEX : cache[index.row()][PROFILE_MODEL_PL_NAME_KEY] = value; break;
	case PROFILE_MODEL_COLUMN_DISCID_INDEX : cache[index.row()][PROFILE_MODEL_DISCID_KEY] = value; break;
	case PROFILE_MODEL_COLUMN_INF_INDEX : cache[index.row()][PROFILE_MODEL_INF_KEY] = value; break;
	case PROFILE_MODEL_COLUMN_INF_TEXT_INDEX : cache[index.row()][PROFILE_MODEL_INF_TEXT_KEY] = value; break;
	case PROFILE_MODEL_COLUMN_INF_NAME_INDEX : cache[index.row()][PROFILE_MODEL_INF_NAME_KEY] = value; break;
	case PROFILE_MODEL_COLUMN_INF_SUFFIX_INDEX : cache[index.row()][PROFILE_MODEL_INF_SUFFIX_KEY] = value; break;
	case PROFILE_MODEL_COLUMN_HL_INDEX : cache[index.row()][PROFILE_MODEL_HL_KEY] = value; break;
	case PROFILE_MODEL_COLUMN_HL_FORMAT_INDEX : cache[index.row()][PROFILE_MODEL_HL_FORMAT_KEY] = value; break;
	case PROFILE_MODEL_COLUMN_HL_NAME_INDEX : cache[index.row()][PROFILE_MODEL_HL_NAME_KEY] = value; break;
        default : return FALSE;
      }
      return TRUE;
  }

  reset();

  return FALSE;

}

QVariant ProfileModel::headerData(int section, Qt::Orientation orientation, int role) const {

  if (orientation == Qt::Vertical) return QVariant();

  if (role == Qt::DisplayRole) {
    switch (section) {
      case PROFILE_MODEL_COLUMN_NAME_INDEX : return PROFILE_MODEL_COLUMN_NAME_LABEL;
      case PROFILE_MODEL_COLUMN_MASK_INDEX : return PROFILE_MODEL_COLUMN_MASK_LABEL;
      case PROFILE_MODEL_COLUMN_COMMAND_INDEX : return PROFILE_MODEL_COLUMN_COMMAND_LABEL;
      case PROFILE_MODEL_COLUMN_SUFFIX_INDEX : return PROFILE_MODEL_COLUMN_SUFFIX_LABEL;
      case PROFILE_MODEL_COLUMN_PROFILEINDEX_INDEX : return PROFILE_MODEL_COLUMN_PROFILEINDEX_LABEL;
      case PROFILE_MODEL_COLUMN_FAT32COMPATIBLE_INDEX : return PROFILE_MODEL_COLUMN_FAT32COMPATIBLE_LABEL;
      case PROFILE_MODEL_COLUMN_SC_INDEX : return PROFILE_MODEL_COLUMN_SC_LABEL;
      case PROFILE_MODEL_COLUMN_SC_SCALE_INDEX : return PROFILE_MODEL_COLUMN_SC_SCALE_LABEL;
      case PROFILE_MODEL_COLUMN_SC_SIZE_INDEX : return PROFILE_MODEL_COLUMN_SC_SIZE_LABEL;
      case PROFILE_MODEL_COLUMN_SC_FORMAT_INDEX : return PROFILE_MODEL_COLUMN_SC_FORMAT_LABEL;
      case PROFILE_MODEL_COLUMN_SC_NAME_INDEX : return PROFILE_MODEL_COLUMN_SC_NAME_LABEL;
      case PROFILE_MODEL_COLUMN_PL_INDEX : return PROFILE_MODEL_COLUMN_PL_LABEL;
      case PROFILE_MODEL_COLUMN_PL_FORMAT_INDEX : return PROFILE_MODEL_COLUMN_PL_FORMAT_LABEL;
      case PROFILE_MODEL_COLUMN_PL_NAME_INDEX : return PROFILE_MODEL_COLUMN_PL_NAME_LABEL;
      case PROFILE_MODEL_COLUMN_DISCID_INDEX : return PROFILE_MODEL_COLUMN_DISCID_LABEL;
      case PROFILE_MODEL_COLUMN_INF_INDEX : return PROFILE_MODEL_COLUMN_INF_LABEL;
      case PROFILE_MODEL_COLUMN_INF_TEXT_INDEX : return PROFILE_MODEL_COLUMN_INF_TEXT_LABEL;
      case PROFILE_MODEL_COLUMN_INF_NAME_INDEX : return PROFILE_MODEL_COLUMN_INF_NAME_LABEL;
      case PROFILE_MODEL_COLUMN_INF_SUFFIX_INDEX : return PROFILE_MODEL_COLUMN_INF_SUFFIX_LABEL;
      case PROFILE_MODEL_COLUMN_HL_INDEX : return PROFILE_MODEL_COLUMN_HL_LABEL;
      case PROFILE_MODEL_COLUMN_HL_FORMAT_INDEX : return PROFILE_MODEL_COLUMN_HL_FORMAT_LABEL;
      case PROFILE_MODEL_COLUMN_HL_NAME_INDEX : return PROFILE_MODEL_COLUMN_HL_NAME_LABEL;
      default : ;
    }
  }

  return QVariant();

}

bool ProfileModel::removeRows(int row, int count, const QModelIndex& parent) {

  Q_UNUSED(parent);

  if ((row < 0) || (row >= cache.count())) {
    return FALSE;
  }

  if (count <= 0) {
    return FALSE;
  }

  int c;
  if (row+count > cache.count()) {
    c = cache.count();
  } else {
    c = row+count;
  }

  for (int i = row; i < c; ++i) cache.removeAt(i);

  reset();

  if (cache.count()==0) {
    current_profile_row = -1;
    emit currentProfileChanged(-1);
  } else if (current_profile_row>=cache.count()) {
    current_profile_row = cache.count()-1;
    emit currentProfileChanged(current_profile_row);
  } else {
    emit currentProfileChanged(current_profile_row);
  }

  emit profilesRemovedOrInserted();

  return TRUE;

}

bool ProfileModel::insertRows(int row, int count, const QModelIndex& parent) {

  Q_UNUSED(parent);

  if ((row < 0) || (row > cache.count())) {
    return FALSE;
  }

  if (count <= 0) {
    return FALSE;
  }

  if (row == cache.count()) {
    for (int i = 0; i < count; ++i) cache.append(newProfile());
  } else {
    for (int i = row; i < count; ++i) cache.insert(i, newProfile());
  }

  reset();

  if (current_profile_row==-1) {
    current_profile_row = 0;
    emit currentProfileChanged(0);
  } else {
    emit currentProfileChanged(current_profile_row);
  }

  emit profilesRemovedOrInserted();

  return TRUE;

}

void ProfileModel::setCurrentProfileRow(int row) {
  if (cache.count()==0) row = -1; else if ((row < 0) || (row >= cache.count())) row = 0;
  if (row != current_profile_row) {
    current_profile_row = row;
  }
}

int ProfileModel::currentProfileRow() const {
  if (cache.count()==0) return -1;
  return current_profile_row;
}

void ProfileModel::clear() {
  cache.clear();
  if (current_profile_row != -1) {
    current_profile_row = -1;
    emit currentProfileChanged(-1);
  }
}

bool ProfileModel::nameExists(const QString& name) const {

  for (int j = 0; j < cache.count(); ++j)
    if (name==cache.at(j)[PROFILE_MODEL_NAME_KEY].toString()) return TRUE;

  return FALSE;

}

int ProfileModel::profileIndexMax() const {

  int index = -1;

  for (int j = 0; j < cache.count(); ++j) {
    int i = cache.at(j)[PROFILE_MODEL_PROFILEINDEX_KEY].toInt();
    if (index<i) index = i;
  }

  return index;

}

Error ProfileModel::lastError() const {
  return error;
}

void ProfileModel::commit() {
  KConfig config;
  save(&config);
}

const Profile ProfileModel::newProfile(const QString& name, const QString& mask, const QString& command, const QString& suffix,
		const int profileIndex, const bool fat32Compatible,
		const bool storeCover, const bool storeCoverScale, const QSize& storeCoverSize,
		const QString& storeCoverFormat, const QString& storeCoverName,
		const bool playlist, const QString& playlistFormat, const QString& playlistName,
		const bool discid, const bool inf, const QStringList& infoText, const QString& infoName, const QString& infoSuffix,
		const bool hashlist, const QString& hashlistFormat, const QString& hashlistName) {

  Profile p;

  p[PROFILE_MODEL_NAME_KEY] = name;
  p[PROFILE_MODEL_MASK_KEY] = mask;
  p[PROFILE_MODEL_COMMAND_KEY] = command;
  p[PROFILE_MODEL_SUFFIX_KEY] = suffix;
  p[PROFILE_MODEL_PROFILEINDEX_KEY] = profileIndex;
  p[PROFILE_MODEL_FAT32COMPATIBLE_KEY] = fat32Compatible;
  p[PROFILE_MODEL_SC_KEY] = storeCover;
  p[PROFILE_MODEL_SC_SCALE_KEY] = storeCoverScale;
  p[PROFILE_MODEL_SC_SIZE_KEY] = storeCoverSize;
  p[PROFILE_MODEL_SC_FORMAT_KEY] = storeCoverFormat;
  p[PROFILE_MODEL_SC_NAME_KEY] = storeCoverName;
  p[PROFILE_MODEL_PL_KEY] = playlist;
  p[PROFILE_MODEL_PL_FORMAT_KEY] = playlistFormat;
  p[PROFILE_MODEL_PL_NAME_KEY] = playlistName;
  p[PROFILE_MODEL_DISCID_KEY] = discid;
  p[PROFILE_MODEL_INF_KEY] = inf;
  p[PROFILE_MODEL_INF_TEXT_KEY] = infoText;
  p[PROFILE_MODEL_INF_NAME_KEY] = infoName;
  p[PROFILE_MODEL_INF_SUFFIX_KEY] = infoSuffix;
  p[PROFILE_MODEL_HL_KEY] = hashlist;
  p[PROFILE_MODEL_HL_FORMAT_KEY] = hashlistFormat;
  p[PROFILE_MODEL_HL_NAME_KEY] = hashlistName;

  return p;

}

void ProfileModel::_new_name(QString& name) {
  for (int j = 0; j < cache.count(); ++j) {
    if (name==cache.at(j)[PROFILE_MODEL_NAME_KEY].toString()) {
      name = "_"+name;
      _new_name(name);
      return;
    }
  }
}

void ProfileModel::revert() {
  clear();
  KConfig config;
  load(&config);
}

void ProfileModel::copy(const int profileRow) {

  if ((profileRow<0) || (profileRow>=rowCount())) return;

  int row = profileRow;
  Profile p = cache[row];

  QString name = cache[row][PROFILE_MODEL_NAME_KEY].toString();
  _new_name(name);
  p[PROFILE_MODEL_NAME_KEY] = name;

  cache.append(p);
  reset();

}

bool ProfileModel::saveProfilesToFile(const QString& filename) {
  KConfig config(filename);
  save(&config);
  return TRUE;
}

bool ProfileModel::loadProfilesFromFile(const QString& filename) {
  KConfig config(filename);
  load(&config);
  return TRUE;
}

void ProfileModel::save(KConfig *config) {
  KConfigGroup profilesGroup(config, "Profiles");
  profilesGroup.deleteGroup();
  profilesGroup.writeEntry("Count", cache.count());
  for (int i = 0; i < cache.count(); ++i) {
    KConfigGroup subGroup(&profilesGroup, QString("Profile %1").arg(i));
    subGroup.writeEntry(PROFILE_MODEL_NAME_KEY, cache[i][PROFILE_MODEL_NAME_KEY]);
    subGroup.writeEntry(PROFILE_MODEL_MASK_KEY, cache[i][PROFILE_MODEL_MASK_KEY]);
    subGroup.writeEntry(PROFILE_MODEL_COMMAND_KEY, cache[i][PROFILE_MODEL_COMMAND_KEY]);
    subGroup.writeEntry(PROFILE_MODEL_SUFFIX_KEY, cache[i][PROFILE_MODEL_SUFFIX_KEY]);
    subGroup.writeEntry(PROFILE_MODEL_PROFILEINDEX_KEY, cache[i][PROFILE_MODEL_PROFILEINDEX_KEY]);
    subGroup.writeEntry(PROFILE_MODEL_FAT32COMPATIBLE_KEY, cache[i][PROFILE_MODEL_FAT32COMPATIBLE_KEY]);
    subGroup.writeEntry(PROFILE_MODEL_SC_KEY, cache[i][PROFILE_MODEL_SC_KEY]);
    subGroup.writeEntry(PROFILE_MODEL_SC_SCALE_KEY, cache[i][PROFILE_MODEL_SC_SCALE_KEY]);
    subGroup.writeEntry(PROFILE_MODEL_SC_SIZE_KEY, cache[i][PROFILE_MODEL_SC_SIZE_KEY]);
    subGroup.writeEntry(PROFILE_MODEL_SC_FORMAT_KEY, cache[i][PROFILE_MODEL_SC_FORMAT_KEY]);
    subGroup.writeEntry(PROFILE_MODEL_SC_NAME_KEY, cache[i][PROFILE_MODEL_SC_NAME_KEY]);
    subGroup.writeEntry(PROFILE_MODEL_PL_KEY, cache[i][PROFILE_MODEL_PL_KEY]);
    subGroup.writeEntry(PROFILE_MODEL_PL_FORMAT_KEY, cache[i][PROFILE_MODEL_PL_FORMAT_KEY]);
    subGroup.writeEntry(PROFILE_MODEL_PL_NAME_KEY, cache[i][PROFILE_MODEL_PL_NAME_KEY]);
    subGroup.writeEntry(PROFILE_MODEL_DISCID_KEY, cache[i][PROFILE_MODEL_DISCID_KEY]);
    subGroup.writeEntry(PROFILE_MODEL_INF_KEY, cache[i][PROFILE_MODEL_INF_KEY]);
    subGroup.writeEntry(PROFILE_MODEL_INF_TEXT_KEY, cache[i][PROFILE_MODEL_INF_TEXT_KEY]);
    subGroup.writeEntry(PROFILE_MODEL_INF_NAME_KEY, cache[i][PROFILE_MODEL_INF_NAME_KEY]);
    subGroup.writeEntry(PROFILE_MODEL_INF_SUFFIX_KEY, cache[i][PROFILE_MODEL_INF_SUFFIX_KEY]);
    subGroup.writeEntry(PROFILE_MODEL_HL_KEY, cache[i][PROFILE_MODEL_HL_KEY]);
    subGroup.writeEntry(PROFILE_MODEL_HL_FORMAT_KEY, cache[i][PROFILE_MODEL_HL_FORMAT_KEY]);
    subGroup.writeEntry(PROFILE_MODEL_HL_NAME_KEY, cache[i][PROFILE_MODEL_HL_NAME_KEY]);
  }
  if (cache.count()>0) profilesGroup.config()->sync();
}

void ProfileModel::load(KConfig *config) {
  KConfigGroup profilesGroup(config, "Profiles");
  clear();
  int profileCount = profilesGroup.readEntry("Count", QVariant(0)).toInt();
  for (int i = 0; i < profileCount; ++i) {
    Profile p;
    KConfigGroup subGroup(&profilesGroup, QString("Profile %1").arg(i));
    p[PROFILE_MODEL_NAME_KEY] = subGroup.readEntry(PROFILE_MODEL_NAME_KEY, "");
    p[PROFILE_MODEL_MASK_KEY] = subGroup.readEntry(PROFILE_MODEL_MASK_KEY, "");
    p[PROFILE_MODEL_COMMAND_KEY] = subGroup.readEntry(PROFILE_MODEL_COMMAND_KEY, "");
    p[PROFILE_MODEL_SUFFIX_KEY] = subGroup.readEntry(PROFILE_MODEL_SUFFIX_KEY, "");
    p[PROFILE_MODEL_PROFILEINDEX_KEY] = subGroup.readEntry(PROFILE_MODEL_PROFILEINDEX_KEY, -1);
    p[PROFILE_MODEL_FAT32COMPATIBLE_KEY] = subGroup.readEntry(PROFILE_MODEL_FAT32COMPATIBLE_KEY, FALSE);
    p[PROFILE_MODEL_SC_KEY] = subGroup.readEntry(PROFILE_MODEL_SC_KEY, FALSE);
    p[PROFILE_MODEL_SC_SCALE_KEY] = subGroup.readEntry(PROFILE_MODEL_SC_SCALE_KEY, FALSE);
    p[PROFILE_MODEL_SC_SIZE_KEY] = subGroup.readEntry(PROFILE_MODEL_SC_SIZE_KEY, QSize(300, 300));
    p[PROFILE_MODEL_SC_FORMAT_KEY] = subGroup.readEntry(PROFILE_MODEL_SC_FORMAT_KEY, "JPG");
    p[PROFILE_MODEL_SC_NAME_KEY] = subGroup.readEntry(PROFILE_MODEL_SC_NAME_KEY, "cover");
    p[PROFILE_MODEL_PL_KEY] = subGroup.readEntry(PROFILE_MODEL_PL_KEY, FALSE);
    p[PROFILE_MODEL_PL_FORMAT_KEY] = subGroup.readEntry(PROFILE_MODEL_PL_FORMAT_KEY, "M3U");
    p[PROFILE_MODEL_PL_NAME_KEY] = subGroup.readEntry(PROFILE_MODEL_PL_NAME_KEY, "playlist");
    p[PROFILE_MODEL_DISCID_KEY] = subGroup.readEntry(PROFILE_MODEL_DISCID_KEY, FALSE);
    p[PROFILE_MODEL_INF_KEY] = subGroup.readEntry(PROFILE_MODEL_INF_KEY, FALSE);
    p[PROFILE_MODEL_INF_TEXT_KEY] = subGroup.readEntry(PROFILE_MODEL_INF_TEXT_KEY, QStringList());
    p[PROFILE_MODEL_INF_NAME_KEY] = subGroup.readEntry(PROFILE_MODEL_INF_NAME_KEY, "info");
    p[PROFILE_MODEL_INF_SUFFIX_KEY] = subGroup.readEntry(PROFILE_MODEL_INF_SUFFIX_KEY, "nfo");
    p[PROFILE_MODEL_HL_KEY] = subGroup.readEntry(PROFILE_MODEL_HL_KEY, FALSE);
    p[PROFILE_MODEL_HL_FORMAT_KEY] = subGroup.readEntry(PROFILE_MODEL_HL_FORMAT_KEY, "SFV");
    p[PROFILE_MODEL_HL_NAME_KEY] = subGroup.readEntry(PROFILE_MODEL_HL_NAME_KEY, "checksums");
    cache.append(p);
  }
  if (profileCount>0) {
    if (current_profile_row==-1) {
      current_profile_row = 0;
      emit currentProfileChanged(0);
    }
    emit profilesRemovedOrInserted();
  }
}
