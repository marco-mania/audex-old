/* AUDEX CDDA EXTRACTOR
 * Copyright (C) 2007-2013 Marco Nelles (audex@maniatek.com)
 * <http://kde.maniatek.com/audex>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "profilemodel.h"

ProfileModel::ProfileModel(QObject *parent) {
  Q_UNUSED(parent);
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
  return PROFILE_MODEL_COLUMN_NUM;
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

      case PROFILE_MODEL_COLUMN_PROFILEINDEX_INDEX : return cache.at(index.row())[PROFILE_MODEL_PROFILEINDEX_KEY];
      case PROFILE_MODEL_COLUMN_NAME_INDEX : return cache.at(index.row())[PROFILE_MODEL_NAME_KEY];
      case PROFILE_MODEL_COLUMN_ICON_INDEX : return cache.at(index.row())[PROFILE_MODEL_ICON_KEY];

      case PROFILE_MODEL_COLUMN_ENCODER_SELECTED_INDEX : return cache.at(index.row())[PROFILE_MODEL_ENCODER_SELECTED_KEY];

      case PROFILE_MODEL_COLUMN_PATTERN_INDEX : return cache.at(index.row())[PROFILE_MODEL_PATTERN_KEY];
      case PROFILE_MODEL_COLUMN_FAT32COMPATIBLE_INDEX : return cache.at(index.row())[PROFILE_MODEL_FAT32COMPATIBLE_KEY];
      case PROFILE_MODEL_COLUMN_UNDERSCORE_INDEX : return cache.at(index.row())[PROFILE_MODEL_UNDERSCORE_KEY];
      case PROFILE_MODEL_COLUMN_2DIGITSTRACKNUM_INDEX : return cache.at(index.row())[PROFILE_MODEL_2DIGITSTRACKNUM_KEY];
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
      case PROFILE_MODEL_COLUMN_CUE_INDEX : return cache.at(index.row())[PROFILE_MODEL_CUE_KEY];
      case PROFILE_MODEL_COLUMN_CUE_NAME_INDEX : return cache.at(index.row())[PROFILE_MODEL_CUE_NAME_KEY];
      case PROFILE_MODEL_COLUMN_SF_INDEX : return cache.at(index.row())[PROFILE_MODEL_SF_KEY];
      case PROFILE_MODEL_COLUMN_SF_NAME_INDEX : return cache.at(index.row())[PROFILE_MODEL_SF_NAME_KEY];
      case PROFILE_MODEL_COLUMN_ENCODER_LAME_PARAMETERS_INDEX : return cache.at(index.row())[PROFILE_MODEL_COLUMN_ENCODER_LAME_PARAMETERS_KEY];
      case PROFILE_MODEL_COLUMN_ENCODER_OGGENC_PARAMETERS_INDEX : return cache.at(index.row())[PROFILE_MODEL_COLUMN_ENCODER_OGGENC_PARAMETERS_KEY];
      case PROFILE_MODEL_COLUMN_ENCODER_FLAC_PARAMETERS_INDEX : return cache.at(index.row())[PROFILE_MODEL_COLUMN_ENCODER_FLAC_PARAMETERS_KEY];
      case PROFILE_MODEL_COLUMN_ENCODER_FAAC_PARAMETERS_INDEX : return cache.at(index.row())[PROFILE_MODEL_COLUMN_ENCODER_FAAC_PARAMETERS_KEY];
      case PROFILE_MODEL_COLUMN_ENCODER_WAVE_PARAMETERS_INDEX : return cache.at(index.row())[PROFILE_MODEL_COLUMN_ENCODER_WAVE_PARAMETERS_KEY];
      case PROFILE_MODEL_COLUMN_ENCODER_CUSTOM_PARAMETERS_INDEX : return cache.at(index.row())[PROFILE_MODEL_COLUMN_ENCODER_CUSTOM_PARAMETERS_KEY];

    }
  }

  if (role == Qt::DecorationRole) {
    QString iconName(cache.at(index.row())[PROFILE_MODEL_ICON_KEY].toString());

    if(!iconName.isEmpty()) {
      KIcon icon(iconName);
      if(!icon.isNull()) {
        return icon;
      }
    }

    return KIcon(DEFAULT_ICON);
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

        case PROFILE_MODEL_COLUMN_PROFILEINDEX_INDEX : break;
        case PROFILE_MODEL_COLUMN_ICON_INDEX : break;

        case PROFILE_MODEL_COLUMN_NAME_INDEX :
	  if (value.toString().isEmpty()) {
	    error = Error(
		i18n("Profile name must not be empty."),
		i18n("You have given no name for the profile. Please set one."),
		Error::ERROR,
		this);
	    return FALSE;
	  }
	  break;
        case PROFILE_MODEL_COLUMN_ENCODER_SELECTED_INDEX :
	  if (value.toInt()==-1) {
	    error = Error(
		i18n("Profile encoder is not defined."),
		i18n("You have given no encoder for the profile. Please set one."),
		Error::ERROR,
		this);
	    return FALSE;
	  }
	  break;
	case PROFILE_MODEL_COLUMN_PATTERN_INDEX :
	  if (value.toString().isEmpty()) {
            error = Error(
		i18n("Profile filename pattern is not defined."),
		i18n("You have given no filename pattern for the profile. Please set one."),
		Error::ERROR,
		this);
	    return FALSE;
	  }
	  break;
        case PROFILE_MODEL_COLUMN_FAT32COMPATIBLE_INDEX : break;
	case PROFILE_MODEL_COLUMN_UNDERSCORE_INDEX : break;
	case PROFILE_MODEL_COLUMN_2DIGITSTRACKNUM_INDEX : break;
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
		i18n("You have given no name for the cover. Please set one."),
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
		i18n("You have given no name for the playlist. Please set one."),
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
		i18n("You have given no name for the info text file. Please set one."),
		Error::ERROR,
		this);
	    return FALSE;
	  }
	  break;
	case PROFILE_MODEL_COLUMN_INF_SUFFIX_INDEX : break;
	  if (value.toString().isEmpty()) {
	    error = Error(
		i18n("Info text file name suffix must not be empty."),
		i18n("You have given no suffix for the info text file. Please set one."),
		Error::ERROR,
		this);
	    return FALSE;
	  }
	  break;
	case PROFILE_MODEL_COLUMN_HL_INDEX : break;
	case PROFILE_MODEL_COLUMN_HL_FORMAT_INDEX :
          if ((value.toString() != "SFV") && (value.toString() != "MD5")) {
	    error = Error(
		i18n("The hashlist file format is unknown."),
		i18n("Your given hashlist file format is unknown. Please choose on of these formats: SFV, MD5."),
		Error::ERROR,
		this);
	    return FALSE;
	  }
	  break;
	case PROFILE_MODEL_COLUMN_HL_NAME_INDEX : break;
	  if (value.toString().isEmpty()) {
	    error = Error(
		i18n("Hashlist name must not be empty."),
		i18n("You have given no name for the hashlist. Please set one."),
		Error::ERROR,
		this);
	    return FALSE;
	  }
	  break;
	case PROFILE_MODEL_COLUMN_CUE_INDEX : break;
	case PROFILE_MODEL_COLUMN_CUE_NAME_INDEX : break;
	  if (value.toString().isEmpty()) {
	    error = Error(
		i18n("Cue filename name must not be empty."),
		i18n("You have given no name for the cue sheet. Please set one."),
		Error::ERROR,
		this);
	    return FALSE;
	  }
	  break;
	case PROFILE_MODEL_COLUMN_SF_INDEX : break;
	case PROFILE_MODEL_COLUMN_SF_NAME_INDEX : break;
	  if (value.toString().isEmpty()) {
	    error = Error(
		i18n("Filename name must not be empty."),
		i18n("You have given no name for the single audio file. Please set one."),
		Error::ERROR,
		this);
	    return FALSE;
	  }
	  break;
	case PROFILE_MODEL_COLUMN_ENCODER_LAME_PARAMETERS_INDEX : break;
        case PROFILE_MODEL_COLUMN_ENCODER_OGGENC_PARAMETERS_INDEX : break;
        case PROFILE_MODEL_COLUMN_ENCODER_FLAC_PARAMETERS_INDEX : break;
        case PROFILE_MODEL_COLUMN_ENCODER_FAAC_PARAMETERS_INDEX : break;
        case PROFILE_MODEL_COLUMN_ENCODER_WAVE_PARAMETERS_INDEX : break;
        case PROFILE_MODEL_COLUMN_ENCODER_CUSTOM_PARAMETERS_INDEX : break;
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
		i18n("Your profile name %1 already exists in the set of profiles. Please choose a unique one.", value.toString()),
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
		i18n("Your profile index %1 already exists in the set of profiles. Please choose a unique one.", value.toInt()),
		Error::ERROR,
		this);
          return FALSE;
        }
      }

      switch (index.column()) {

        case PROFILE_MODEL_COLUMN_PROFILEINDEX_INDEX : cache[index.row()][PROFILE_MODEL_PROFILEINDEX_KEY] = value; break;
        case PROFILE_MODEL_COLUMN_NAME_INDEX : cache[index.row()][PROFILE_MODEL_NAME_KEY] = value; break;
        case PROFILE_MODEL_COLUMN_ICON_INDEX : cache[index.row()][PROFILE_MODEL_ICON_KEY] = value; break;

        case PROFILE_MODEL_COLUMN_ENCODER_SELECTED_INDEX : cache[index.row()][PROFILE_MODEL_ENCODER_SELECTED_KEY] = value; break;

        case PROFILE_MODEL_COLUMN_PATTERN_INDEX : cache[index.row()][PROFILE_MODEL_PATTERN_KEY] = value; break;
        case PROFILE_MODEL_COLUMN_FAT32COMPATIBLE_INDEX : cache[index.row()][PROFILE_MODEL_FAT32COMPATIBLE_KEY] = value; break;
	case PROFILE_MODEL_COLUMN_UNDERSCORE_INDEX : cache[index.row()][PROFILE_MODEL_UNDERSCORE_KEY] = value; break;
	case PROFILE_MODEL_COLUMN_2DIGITSTRACKNUM_INDEX : cache[index.row()][PROFILE_MODEL_2DIGITSTRACKNUM_KEY] = value; break;
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
	case PROFILE_MODEL_COLUMN_CUE_INDEX : cache[index.row()][PROFILE_MODEL_CUE_KEY] = value; break;
	case PROFILE_MODEL_COLUMN_CUE_NAME_INDEX : cache[index.row()][PROFILE_MODEL_CUE_NAME_KEY] = value; break;
	case PROFILE_MODEL_COLUMN_SF_INDEX : cache[index.row()][PROFILE_MODEL_SF_KEY] = value; break;
	case PROFILE_MODEL_COLUMN_SF_NAME_INDEX : cache[index.row()][PROFILE_MODEL_SF_NAME_KEY] = value; break;

	case PROFILE_MODEL_COLUMN_ENCODER_LAME_PARAMETERS_INDEX : cache[index.row()][PROFILE_MODEL_COLUMN_ENCODER_LAME_PARAMETERS_KEY] = value; break;
	case PROFILE_MODEL_COLUMN_ENCODER_OGGENC_PARAMETERS_INDEX : cache[index.row()][PROFILE_MODEL_COLUMN_ENCODER_OGGENC_PARAMETERS_KEY] = value; break;
	case PROFILE_MODEL_COLUMN_ENCODER_FLAC_PARAMETERS_INDEX : cache[index.row()][PROFILE_MODEL_COLUMN_ENCODER_FLAC_PARAMETERS_KEY] = value; break;
	case PROFILE_MODEL_COLUMN_ENCODER_FAAC_PARAMETERS_INDEX : cache[index.row()][PROFILE_MODEL_COLUMN_ENCODER_FAAC_PARAMETERS_KEY] = value; break;
	case PROFILE_MODEL_COLUMN_ENCODER_WAVE_PARAMETERS_INDEX : cache[index.row()][PROFILE_MODEL_COLUMN_ENCODER_WAVE_PARAMETERS_KEY] = value; break;
	case PROFILE_MODEL_COLUMN_ENCODER_CUSTOM_PARAMETERS_INDEX : cache[index.row()][PROFILE_MODEL_COLUMN_ENCODER_CUSTOM_PARAMETERS_KEY] = value; break;

        default : break;

      }
      return TRUE;
  }

  reset();

  error = Error(
		i18n("Unknown error. No index found in profile model."),
		i18n("This is an internal error. Please report."),
		Error::ERROR,
		this);

  return FALSE;

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

  for (int i = row; i < c; ++i) {
    cache.removeAt(i);
  }

  reset();

  //update current profile index. maybe current has been deleted?
  setCurrentProfileIndex(current_profile_index);

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

  bool wasEmpty = (cache.count()==0);

  if (row == cache.count()) {
    for (int i = 0; i < count; ++i) cache.append(newProfile());
  } else {
    for (int i = row; i < count; ++i) cache.insert(i, newProfile());
  }

  reset();

  if (wasEmpty) {
    //set first profile as current index
    setCurrentProfileIndex(cache.at(0)[PROFILE_MODEL_PROFILEINDEX_KEY].toInt());
  }

  emit profilesRemovedOrInserted();

  return TRUE;

}

void ProfileModel::setCurrentProfileIndex(int profile_index) {
  int pi = profile_index;
  if (cache.count()==0) {
    pi = -1;
  } else if (!indexExists(profile_index)) {
    //set first profile as current index
    pi = cache.at(0)[PROFILE_MODEL_PROFILEINDEX_KEY].toInt();
  }
  if (pi != current_profile_index) {
    current_profile_index = pi;
    KConfig config;
    KConfigGroup profilesGroup(&config, "Profiles");
    profilesGroup.writeEntry("Standard", pi);
    profilesGroup.config()->sync();
    emit currentProfileIndexChanged(pi);
  }
}

int ProfileModel::setRowAsCurrentProfileIndex(int row) {
  int i = cache.at(row).value(PROFILE_MODEL_PROFILEINDEX_KEY, -1).toInt();
  setCurrentProfileIndex(i);
  return i;
}

int ProfileModel::currentProfileIndex() const {
  if (cache.count()==0) return -1;
  return current_profile_index;
}

int ProfileModel::currentProfileRow() const {
  return getRowByIndex(current_profile_index);
}

int ProfileModel::getRowByIndex(int profile_index) const {
  for (int i = 0; i < cache.count(); ++i)
    if (profile_index==cache.at(i)[PROFILE_MODEL_PROFILEINDEX_KEY].toInt()) return i;
  return -1;
}

void ProfileModel::clear() {
  cache.clear();
  current_profile_index = -1;
}

bool ProfileModel::nameExists(const QString& name) const {

  for (int j = 0; j < cache.count(); ++j)
    if (name==cache.at(j)[PROFILE_MODEL_NAME_KEY].toString()) return TRUE;

  return FALSE;

}

bool ProfileModel::indexExists(int profile_index) const {

  for (int j = 0; j < cache.count(); ++j)
    if (profile_index==cache.at(j)[PROFILE_MODEL_PROFILEINDEX_KEY].toInt()) return TRUE;

  return FALSE;

}

int ProfileModel::getNewIndex() const {

  QSet<int> indexes;
  QList<Profile>::ConstIterator it(cache.begin()), end(cache.end());

  for (; it != end; ++it) {
    indexes.insert((*it)[PROFILE_MODEL_PROFILEINDEX_KEY].toInt());
  }
  for (int i = 0; i < INT_MAX; ++i) {
    if (!indexes.contains(i)) return i;
  }
  return -1;

}

static bool lessThan(const Profile &p1, const Profile &p2) {
  return (QString::localeAwareCompare(p1[PROFILE_MODEL_NAME_KEY].toString(), p2[PROFILE_MODEL_NAME_KEY].toString()) < 0);
}


void ProfileModel::sortItems() {
  qSort(cache.begin(), cache.end(), lessThan);
  reset();
  emit profilesRemovedOrInserted();
}

void ProfileModel::autoCreate() {

  bool flag = FALSE;

  bool wasEmpty = (cache.count()==0);

  if (EncoderAssistant::available(EncoderAssistant::LAME)) {

    if (!nameExists(EncoderAssistant::name(EncoderAssistant::LAME) + LABEL_MOBILE_QUALITY)) {
      Profile p = newProfile();
      p[PROFILE_MODEL_NAME_KEY] = EncoderAssistant::name(EncoderAssistant::LAME) + LABEL_MOBILE_QUALITY;
      p[PROFILE_MODEL_ICON_KEY] = EncoderAssistant::icon(EncoderAssistant::LAME);
      p[PROFILE_MODEL_ENCODER_SELECTED_KEY] = (int)EncoderAssistant::LAME;
      p[PROFILE_MODEL_COLUMN_ENCODER_LAME_PARAMETERS_KEY] = EncoderAssistant::stdParameters(EncoderAssistant::LAME, EncoderAssistant::MOBILE).toString();
      cache.append(p);
      flag = TRUE;
    }

    if (!nameExists(EncoderAssistant::name(EncoderAssistant::LAME) + LABEL_NORMAL_QUALITY)) {
      Profile p = newProfile();
      p[PROFILE_MODEL_NAME_KEY] = EncoderAssistant::name(EncoderAssistant::LAME) + LABEL_NORMAL_QUALITY;
      p[PROFILE_MODEL_ICON_KEY] = EncoderAssistant::icon(EncoderAssistant::LAME);
      p[PROFILE_MODEL_ENCODER_SELECTED_KEY] = (int)EncoderAssistant::LAME;
      p[PROFILE_MODEL_COLUMN_ENCODER_LAME_PARAMETERS_KEY] = EncoderAssistant::stdParameters(EncoderAssistant::LAME, EncoderAssistant::NORMAL).toString();
      cache.append(p);
      flag = TRUE;
    }

    if (!nameExists(EncoderAssistant::name(EncoderAssistant::LAME) + LABEL_EXTREME_QUALITY)) {
      Profile p = newProfile();
      p[PROFILE_MODEL_NAME_KEY] = EncoderAssistant::name(EncoderAssistant::LAME) + LABEL_EXTREME_QUALITY;
      p[PROFILE_MODEL_ICON_KEY] = EncoderAssistant::icon(EncoderAssistant::LAME);
      p[PROFILE_MODEL_ENCODER_SELECTED_KEY] = (int)EncoderAssistant::LAME;
      p[PROFILE_MODEL_COLUMN_ENCODER_LAME_PARAMETERS_KEY] = EncoderAssistant::stdParameters(EncoderAssistant::LAME, EncoderAssistant::EXTREME).toString();;
      cache.append(p);
      flag = TRUE;
    }

  }

  if (EncoderAssistant::available(EncoderAssistant::OGGENC)) {

    if (!nameExists(EncoderAssistant::name(EncoderAssistant::OGGENC) + LABEL_MOBILE_QUALITY)) {
      Profile p = newProfile();
      p[PROFILE_MODEL_NAME_KEY] = EncoderAssistant::name(EncoderAssistant::OGGENC) + LABEL_MOBILE_QUALITY;
      p[PROFILE_MODEL_ICON_KEY] = EncoderAssistant::icon(EncoderAssistant::OGGENC);
      p[PROFILE_MODEL_ENCODER_SELECTED_KEY] = (int)EncoderAssistant::OGGENC;
      p[PROFILE_MODEL_COLUMN_ENCODER_OGGENC_PARAMETERS_KEY] = EncoderAssistant::stdParameters(EncoderAssistant::OGGENC, EncoderAssistant::MOBILE).toString();
      cache.append(p);
      flag = TRUE;
    }

    if (!nameExists(EncoderAssistant::name(EncoderAssistant::OGGENC) + LABEL_NORMAL_QUALITY)) {
      Profile p = newProfile();
      p[PROFILE_MODEL_NAME_KEY] = EncoderAssistant::name(EncoderAssistant::OGGENC) + LABEL_NORMAL_QUALITY;
      p[PROFILE_MODEL_ICON_KEY] = EncoderAssistant::icon(EncoderAssistant::OGGENC);
      p[PROFILE_MODEL_ENCODER_SELECTED_KEY] = (int)EncoderAssistant::OGGENC;
      p[PROFILE_MODEL_COLUMN_ENCODER_OGGENC_PARAMETERS_KEY] = EncoderAssistant::stdParameters(EncoderAssistant::OGGENC, EncoderAssistant::NORMAL).toString();
      cache.append(p);
      flag = TRUE;
    }

    if (!nameExists(EncoderAssistant::name(EncoderAssistant::OGGENC) + LABEL_EXTREME_QUALITY)) {
      Profile p = newProfile();
      p[PROFILE_MODEL_NAME_KEY] = EncoderAssistant::name(EncoderAssistant::OGGENC) + LABEL_EXTREME_QUALITY;
      p[PROFILE_MODEL_ICON_KEY] = EncoderAssistant::icon(EncoderAssistant::OGGENC);
      p[PROFILE_MODEL_ENCODER_SELECTED_KEY] = (int)EncoderAssistant::OGGENC;
      p[PROFILE_MODEL_COLUMN_ENCODER_OGGENC_PARAMETERS_KEY] = EncoderAssistant::stdParameters(EncoderAssistant::OGGENC, EncoderAssistant::EXTREME).toString();
      cache.append(p);
      flag = TRUE;
    }

  }

  if ((!nameExists(EncoderAssistant::name(EncoderAssistant::FLAC))) && (EncoderAssistant::available(EncoderAssistant::FLAC))) {
    Profile p = newProfile();
    p[PROFILE_MODEL_NAME_KEY] = EncoderAssistant::name(EncoderAssistant::FLAC);
    p[PROFILE_MODEL_ICON_KEY] = EncoderAssistant::icon(EncoderAssistant::FLAC);
    p[PROFILE_MODEL_ENCODER_SELECTED_KEY] = (int)EncoderAssistant::FLAC;
    cache.append(p);
    flag = TRUE;
  }

  if (EncoderAssistant::available(EncoderAssistant::FAAC)) {

    if (!nameExists(EncoderAssistant::name(EncoderAssistant::FAAC) + LABEL_MOBILE_QUALITY)) {
      Profile p = newProfile();
      p[PROFILE_MODEL_NAME_KEY] = EncoderAssistant::name(EncoderAssistant::FAAC) + LABEL_MOBILE_QUALITY;
      p[PROFILE_MODEL_ICON_KEY] = EncoderAssistant::icon(EncoderAssistant::FAAC);
      p[PROFILE_MODEL_ENCODER_SELECTED_KEY] = (int)EncoderAssistant::FAAC;
      p[PROFILE_MODEL_COLUMN_ENCODER_FAAC_PARAMETERS_KEY] = EncoderAssistant::stdParameters(EncoderAssistant::FAAC, EncoderAssistant::MOBILE).toString();
      cache.append(p);
      flag = TRUE;
    }

    if (!nameExists(EncoderAssistant::name(EncoderAssistant::FAAC) + LABEL_NORMAL_QUALITY)) {
      Profile p = newProfile();
      p[PROFILE_MODEL_NAME_KEY] = EncoderAssistant::name(EncoderAssistant::FAAC) + LABEL_NORMAL_QUALITY;
      p[PROFILE_MODEL_ICON_KEY] = EncoderAssistant::icon(EncoderAssistant::FAAC);
      p[PROFILE_MODEL_ENCODER_SELECTED_KEY] = (int)EncoderAssistant::FAAC;
      p[PROFILE_MODEL_COLUMN_ENCODER_FAAC_PARAMETERS_KEY] = EncoderAssistant::stdParameters(EncoderAssistant::FAAC, EncoderAssistant::NORMAL).toString();
      cache.append(p);
      flag = TRUE;
    }

    if (!nameExists(EncoderAssistant::name(EncoderAssistant::FAAC) + LABEL_EXTREME_QUALITY)) {
      Profile p = newProfile();
      p[PROFILE_MODEL_NAME_KEY] = EncoderAssistant::name(EncoderAssistant::FAAC) + LABEL_EXTREME_QUALITY;
      p[PROFILE_MODEL_ICON_KEY] = EncoderAssistant::icon(EncoderAssistant::FAAC);
      p[PROFILE_MODEL_ENCODER_SELECTED_KEY] = (int)EncoderAssistant::FAAC;
      p[PROFILE_MODEL_COLUMN_ENCODER_FAAC_PARAMETERS_KEY] = EncoderAssistant::stdParameters(EncoderAssistant::FAAC, EncoderAssistant::EXTREME).toString();
      cache.append(p);
      flag = TRUE;
    }

  }

  if ((!nameExists(EncoderAssistant::name(EncoderAssistant::WAVE))) && (EncoderAssistant::available(EncoderAssistant::WAVE))) {
    Profile p = newProfile();
    p[PROFILE_MODEL_NAME_KEY] = EncoderAssistant::name(EncoderAssistant::WAVE);
    p[PROFILE_MODEL_ICON_KEY] = EncoderAssistant::icon(EncoderAssistant::WAVE);
    p[PROFILE_MODEL_ENCODER_SELECTED_KEY] = (int)EncoderAssistant::WAVE;
    cache.append(p);
    flag = TRUE;
  }

  if (flag) {
    sortItems();
    if (wasEmpty) {
      //set first profile as current index
      setCurrentProfileIndex(cache.at(0)[PROFILE_MODEL_PROFILEINDEX_KEY].toInt());
    }
  }

  commit();

}

EncoderAssistant::Encoder ProfileModel::getSelectedEncoderFromCurrentIndex() {

  return (EncoderAssistant::Encoder)data(index(currentProfileRow(), PROFILE_MODEL_COLUMN_ENCODER_SELECTED_INDEX)).toInt();

}

const Parameters ProfileModel::getSelectedEncoderParametersFromCurrentIndex() {

  Parameters parameters;

  EncoderAssistant::Encoder encoder = getSelectedEncoderFromCurrentIndex();
  //what parameters does the encoder start with?
  switch (encoder) {
    case EncoderAssistant::LAME : parameters.fromString(data(index(currentProfileRow(), PROFILE_MODEL_COLUMN_ENCODER_LAME_PARAMETERS_INDEX)).toString()); break;
    case EncoderAssistant::OGGENC : parameters.fromString(data(index(currentProfileRow(), PROFILE_MODEL_COLUMN_ENCODER_OGGENC_PARAMETERS_INDEX)).toString()); break;
    case EncoderAssistant::FLAC : parameters.fromString(data(index(currentProfileRow(), PROFILE_MODEL_COLUMN_ENCODER_FLAC_PARAMETERS_INDEX)).toString()); break;
    case EncoderAssistant::FAAC : parameters.fromString(data(index(currentProfileRow(), PROFILE_MODEL_COLUMN_ENCODER_FAAC_PARAMETERS_INDEX)).toString()); break;
    case EncoderAssistant::WAVE : parameters.fromString(data(index(currentProfileRow(), PROFILE_MODEL_COLUMN_ENCODER_WAVE_PARAMETERS_INDEX)).toString()); break;
    case EncoderAssistant::CUSTOM : parameters.fromString(data(index(currentProfileRow(), PROFILE_MODEL_COLUMN_ENCODER_CUSTOM_PARAMETERS_INDEX)).toString()); break;
    case EncoderAssistant::NUM : ;
  }

  return parameters;

}

const QString ProfileModel::getSelectedEncoderPatternFromCurrentIndex() {

  EncoderAssistant::Encoder encoder = getSelectedEncoderFromCurrentIndex();
  Parameters parameters = getSelectedEncoderParametersFromCurrentIndex();

  return EncoderAssistant::pattern(encoder, parameters);

}

const QString ProfileModel::getSelectedEncoderSuffixFromCurrentIndex() {

  EncoderAssistant::Encoder encoder = getSelectedEncoderFromCurrentIndex();
  Parameters parameters = getSelectedEncoderParametersFromCurrentIndex();

  switch (encoder) {

    case EncoderAssistant::LAME : return parameters.value(ENCODER_LAME_SUFFIX_KEY, ENCODER_LAME_SUFFIX);
    case EncoderAssistant::OGGENC : return parameters.value(ENCODER_OGGENC_SUFFIX_KEY, ENCODER_OGGENC_SUFFIX);
    case EncoderAssistant::FLAC : return parameters.value(ENCODER_FLAC_SUFFIX_KEY, ENCODER_FLAC_SUFFIX);
    case EncoderAssistant::FAAC : return parameters.value(ENCODER_FAAC_SUFFIX_KEY, ENCODER_FAAC_SUFFIX);
    case EncoderAssistant::WAVE : return parameters.value(ENCODER_WAVE_SUFFIX_KEY, ENCODER_WAVE_SUFFIX);
    case EncoderAssistant::CUSTOM : return parameters.value(ENCODER_CUSTOM_SUFFIX_KEY, ENCODER_CUSTOM_SUFFIX);
    case EncoderAssistant::NUM : return "";

  }

  return "";

}

const QString ProfileModel::getSelectedEncoderNameAndVersion() {
 
  EncoderAssistant::Encoder encoder = getSelectedEncoderFromCurrentIndex();
  
  return QString("%1 %2").arg(EncoderAssistant::encoderName(encoder)).arg(EncoderAssistant::version(encoder));
  
}

Error ProfileModel::lastError() const {
  return error;
}

void ProfileModel::commit() {
  KConfig config;
  save(&config);
}

const Profile ProfileModel::newProfile() {

  Profile p;

  p[PROFILE_MODEL_PROFILEINDEX_KEY] = getNewIndex();
  p[PROFILE_MODEL_NAME_KEY] = DEFAULT_NAME;

  p[PROFILE_MODEL_ENCODER_SELECTED_KEY] = DEFAULT_ENCODER_SELECTED;

  p[PROFILE_MODEL_PATTERN_KEY] = DEFAULT_PATTERN;

  p[PROFILE_MODEL_FAT32COMPATIBLE_KEY] = DEFAULT_FAT32;
  p[PROFILE_MODEL_UNDERSCORE_KEY] = DEFAULT_UNDERSCORE;
  p[PROFILE_MODEL_2DIGITSTRACKNUM_KEY] = DEFAULT_2DIGITSTRACKNUM;

  p[PROFILE_MODEL_SC_KEY] = DEFAULT_SC;
  p[PROFILE_MODEL_SC_SCALE_KEY] = DEFAULT_SC_SCALE;
  p[PROFILE_MODEL_SC_SIZE_KEY] = DEFAULT_SC_SIZE;
  p[PROFILE_MODEL_SC_FORMAT_KEY] = DEFAULT_SC_FORMAT;
  p[PROFILE_MODEL_SC_NAME_KEY] = DEFAULT_SC_NAME;
  p[PROFILE_MODEL_PL_KEY] = DEFAULT_PL;
  p[PROFILE_MODEL_PL_FORMAT_KEY] = DEFAULT_PL_FORMAT;
  p[PROFILE_MODEL_PL_NAME_KEY] = DEFAULT_PL_NAME;
  p[PROFILE_MODEL_DISCID_KEY] = DEFAULT_DISCID;
  p[PROFILE_MODEL_INF_KEY] = DEFAULT_INF;
  p[PROFILE_MODEL_INF_TEXT_KEY] = DEFAULT_INF_TEXT;
  p[PROFILE_MODEL_INF_NAME_KEY] = DEFAULT_INF_NAME;
  p[PROFILE_MODEL_INF_SUFFIX_KEY] = DEFAULT_INF_SUFFIX;
  p[PROFILE_MODEL_HL_KEY] = DEFAULT_HL;
  p[PROFILE_MODEL_HL_FORMAT_KEY] = DEFAULT_HL_FORMAT;
  p[PROFILE_MODEL_HL_NAME_KEY] = DEFAULT_HL_NAME;
  p[PROFILE_MODEL_CUE_KEY] = DEFAULT_CUE;
  p[PROFILE_MODEL_CUE_NAME_KEY] = DEFAULT_CUE_NAME;
  p[PROFILE_MODEL_SF_KEY] = DEFAULT_SF;
  p[PROFILE_MODEL_SF_NAME_KEY] = DEFAULT_SF_NAME;

  p[PROFILE_MODEL_COLUMN_ENCODER_LAME_PARAMETERS_KEY] = DEFAULT_ENCODER_PARAMETERS;
  p[PROFILE_MODEL_COLUMN_ENCODER_OGGENC_PARAMETERS_KEY] = DEFAULT_ENCODER_PARAMETERS;
  p[PROFILE_MODEL_COLUMN_ENCODER_FLAC_PARAMETERS_KEY] = DEFAULT_ENCODER_PARAMETERS;
  p[PROFILE_MODEL_COLUMN_ENCODER_FAAC_PARAMETERS_KEY] = DEFAULT_ENCODER_PARAMETERS;
  p[PROFILE_MODEL_COLUMN_ENCODER_WAVE_PARAMETERS_KEY] = DEFAULT_ENCODER_PARAMETERS;
  p[PROFILE_MODEL_COLUMN_ENCODER_CUSTOM_PARAMETERS_KEY] = DEFAULT_ENCODER_PARAMETERS;

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

int ProfileModel::copy(const int profileRow) {

  if ((profileRow < 0) || (profileRow >= rowCount())) return -1;

  int key = getNewIndex();
  Profile p = cache[profileRow];

  QString name = cache[profileRow][PROFILE_MODEL_NAME_KEY].toString();
  _new_name(name);
  p[PROFILE_MODEL_NAME_KEY] = name;
  p[PROFILE_MODEL_PROFILEINDEX_KEY] = key;
  cache.append(p);

  reset();
  emit profilesRemovedOrInserted();

  return key;

}

bool ProfileModel::saveProfilesToFile(const QString& filename) {
  KConfig config(filename);
  save(&config);
  return TRUE;
}

bool ProfileModel::loadProfilesFromFile(const QString& filename) {
  KConfig config(filename);
  load(&config);
  reset();
  commit();
  return TRUE;
}

void ProfileModel::save(KConfig *config) {

  KConfigGroup profilesGroup(config, "Profiles");
  profilesGroup.deleteGroup();
  profilesGroup.writeEntry("Standard", current_profile_index);
  profilesGroup.writeEntry("Count", cache.count());

  for (int i = 0; i < cache.count(); ++i) {

    KConfigGroup subGroup(&profilesGroup, QString("Profile %1").arg(i));

    subGroup.writeEntry(PROFILE_MODEL_PROFILEINDEX_KEY, cache[i][PROFILE_MODEL_PROFILEINDEX_KEY]);
    subGroup.writeEntry(PROFILE_MODEL_NAME_KEY, cache[i][PROFILE_MODEL_NAME_KEY]);
    subGroup.writeEntry(PROFILE_MODEL_ICON_KEY, cache[i][PROFILE_MODEL_ICON_KEY]);

    subGroup.writeEntry(PROFILE_MODEL_ENCODER_SELECTED_KEY, cache[i][PROFILE_MODEL_ENCODER_SELECTED_KEY]);

    subGroup.writeEntry(PROFILE_MODEL_PATTERN_KEY, cache[i][PROFILE_MODEL_PATTERN_KEY]);
    subGroup.writeEntry(PROFILE_MODEL_FAT32COMPATIBLE_KEY, cache[i][PROFILE_MODEL_FAT32COMPATIBLE_KEY]);
    subGroup.writeEntry(PROFILE_MODEL_UNDERSCORE_KEY, cache[i][PROFILE_MODEL_UNDERSCORE_KEY]);
    subGroup.writeEntry(PROFILE_MODEL_2DIGITSTRACKNUM_KEY, cache[i][PROFILE_MODEL_2DIGITSTRACKNUM_KEY]);

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
    subGroup.writeEntry(PROFILE_MODEL_CUE_KEY, cache[i][PROFILE_MODEL_CUE_KEY]);
    subGroup.writeEntry(PROFILE_MODEL_CUE_NAME_KEY, cache[i][PROFILE_MODEL_CUE_NAME_KEY]);
    subGroup.writeEntry(PROFILE_MODEL_SF_KEY, cache[i][PROFILE_MODEL_SF_KEY]);
    subGroup.writeEntry(PROFILE_MODEL_SF_NAME_KEY, cache[i][PROFILE_MODEL_SF_NAME_KEY]);

    subGroup.writeEntry(PROFILE_MODEL_COLUMN_ENCODER_LAME_PARAMETERS_KEY, cache[i][PROFILE_MODEL_COLUMN_ENCODER_LAME_PARAMETERS_KEY]);
    subGroup.writeEntry(PROFILE_MODEL_COLUMN_ENCODER_OGGENC_PARAMETERS_KEY, cache[i][PROFILE_MODEL_COLUMN_ENCODER_OGGENC_PARAMETERS_KEY]);
    subGroup.writeEntry(PROFILE_MODEL_COLUMN_ENCODER_FLAC_PARAMETERS_KEY, cache[i][PROFILE_MODEL_COLUMN_ENCODER_FLAC_PARAMETERS_KEY]);
    subGroup.writeEntry(PROFILE_MODEL_COLUMN_ENCODER_FAAC_PARAMETERS_KEY, cache[i][PROFILE_MODEL_COLUMN_ENCODER_FAAC_PARAMETERS_KEY]);
    subGroup.writeEntry(PROFILE_MODEL_COLUMN_ENCODER_WAVE_PARAMETERS_KEY, cache[i][PROFILE_MODEL_COLUMN_ENCODER_WAVE_PARAMETERS_KEY]);
    subGroup.writeEntry(PROFILE_MODEL_COLUMN_ENCODER_CUSTOM_PARAMETERS_KEY, cache[i][PROFILE_MODEL_COLUMN_ENCODER_CUSTOM_PARAMETERS_KEY]);

  }

  if (cache.count()>0) profilesGroup.config()->sync();

}

void ProfileModel::load(KConfig *config) {

  KConfigGroup profilesGroup(config, "Profiles");
  clear();
  current_profile_index = profilesGroup.readEntry("Standard", -1);
  int profileCount = profilesGroup.readEntry("Count", QVariant(0)).toInt();

  for (int i = 0; i < profileCount; ++i) {

    Profile p;
    KConfigGroup subGroup(&profilesGroup, QString("Profile %1").arg(i));

    p[PROFILE_MODEL_PROFILEINDEX_KEY] = subGroup.readEntry(PROFILE_MODEL_PROFILEINDEX_KEY, DEFAULT_PROFILEINDEX);
    p[PROFILE_MODEL_NAME_KEY] = subGroup.readEntry(PROFILE_MODEL_NAME_KEY, DEFAULT_NAME);
    p[PROFILE_MODEL_ICON_KEY] = subGroup.readEntry(PROFILE_MODEL_ICON_KEY, DEFAULT_ICON);

    p[PROFILE_MODEL_ENCODER_SELECTED_KEY] = subGroup.readEntry(PROFILE_MODEL_ENCODER_SELECTED_KEY, DEFAULT_ENCODER_SELECTED);

    p[PROFILE_MODEL_PATTERN_KEY] = subGroup.readEntry(PROFILE_MODEL_PATTERN_KEY, DEFAULT_PATTERN);
    p[PROFILE_MODEL_FAT32COMPATIBLE_KEY] = subGroup.readEntry(PROFILE_MODEL_FAT32COMPATIBLE_KEY, DEFAULT_FAT32);
    p[PROFILE_MODEL_UNDERSCORE_KEY] = subGroup.readEntry(PROFILE_MODEL_UNDERSCORE_KEY, DEFAULT_UNDERSCORE);
    p[PROFILE_MODEL_2DIGITSTRACKNUM_KEY] = subGroup.readEntry(PROFILE_MODEL_2DIGITSTRACKNUM_KEY, DEFAULT_2DIGITSTRACKNUM);

    p[PROFILE_MODEL_SC_KEY] = subGroup.readEntry(PROFILE_MODEL_SC_KEY, DEFAULT_SC);
    p[PROFILE_MODEL_SC_SCALE_KEY] = subGroup.readEntry(PROFILE_MODEL_SC_SCALE_KEY, DEFAULT_SC_SCALE);
    p[PROFILE_MODEL_SC_SIZE_KEY] = subGroup.readEntry(PROFILE_MODEL_SC_SIZE_KEY, DEFAULT_SC_SIZE);
    p[PROFILE_MODEL_SC_FORMAT_KEY] = subGroup.readEntry(PROFILE_MODEL_SC_FORMAT_KEY, DEFAULT_SC_FORMAT);
    p[PROFILE_MODEL_SC_NAME_KEY] = subGroup.readEntry(PROFILE_MODEL_SC_NAME_KEY, DEFAULT_SC_NAME);
    p[PROFILE_MODEL_PL_KEY] = subGroup.readEntry(PROFILE_MODEL_PL_KEY, DEFAULT_PL);
    p[PROFILE_MODEL_PL_FORMAT_KEY] = subGroup.readEntry(PROFILE_MODEL_PL_FORMAT_KEY, DEFAULT_PL_NAME);
    p[PROFILE_MODEL_PL_NAME_KEY] = subGroup.readEntry(PROFILE_MODEL_PL_NAME_KEY, DEFAULT_PL_NAME);
    p[PROFILE_MODEL_DISCID_KEY] = subGroup.readEntry(PROFILE_MODEL_DISCID_KEY, DEFAULT_DISCID);
    p[PROFILE_MODEL_INF_KEY] = subGroup.readEntry(PROFILE_MODEL_INF_KEY, DEFAULT_INF);
    p[PROFILE_MODEL_INF_TEXT_KEY] = subGroup.readEntry(PROFILE_MODEL_INF_TEXT_KEY, DEFAULT_INF_TEXT);
    p[PROFILE_MODEL_INF_NAME_KEY] = subGroup.readEntry(PROFILE_MODEL_INF_NAME_KEY, DEFAULT_INF_NAME);
    p[PROFILE_MODEL_INF_SUFFIX_KEY] = subGroup.readEntry(PROFILE_MODEL_INF_SUFFIX_KEY, DEFAULT_INF_SUFFIX);
    p[PROFILE_MODEL_HL_KEY] = subGroup.readEntry(PROFILE_MODEL_HL_KEY, DEFAULT_HL);
    p[PROFILE_MODEL_HL_FORMAT_KEY] = subGroup.readEntry(PROFILE_MODEL_HL_FORMAT_KEY, DEFAULT_HL_FORMAT);
    p[PROFILE_MODEL_HL_NAME_KEY] = subGroup.readEntry(PROFILE_MODEL_HL_NAME_KEY, DEFAULT_HL_NAME);
    p[PROFILE_MODEL_CUE_KEY] = subGroup.readEntry(PROFILE_MODEL_CUE_KEY, DEFAULT_CUE);
    p[PROFILE_MODEL_CUE_NAME_KEY] = subGroup.readEntry(PROFILE_MODEL_CUE_NAME_KEY, DEFAULT_CUE_NAME);
    p[PROFILE_MODEL_SF_KEY] = subGroup.readEntry(PROFILE_MODEL_SF_KEY, DEFAULT_SF);
    p[PROFILE_MODEL_SF_NAME_KEY] = subGroup.readEntry(PROFILE_MODEL_SF_NAME_KEY, DEFAULT_SF_NAME);

    p[PROFILE_MODEL_COLUMN_ENCODER_LAME_PARAMETERS_KEY] = subGroup.readEntry(PROFILE_MODEL_COLUMN_ENCODER_LAME_PARAMETERS_KEY, DEFAULT_ENCODER_PARAMETERS);
    p[PROFILE_MODEL_COLUMN_ENCODER_OGGENC_PARAMETERS_KEY] = subGroup.readEntry(PROFILE_MODEL_COLUMN_ENCODER_OGGENC_PARAMETERS_KEY, DEFAULT_ENCODER_PARAMETERS);
    p[PROFILE_MODEL_COLUMN_ENCODER_FLAC_PARAMETERS_KEY] = subGroup.readEntry(PROFILE_MODEL_COLUMN_ENCODER_FLAC_PARAMETERS_KEY, DEFAULT_ENCODER_PARAMETERS);
    p[PROFILE_MODEL_COLUMN_ENCODER_FAAC_PARAMETERS_KEY] = subGroup.readEntry(PROFILE_MODEL_COLUMN_ENCODER_FAAC_PARAMETERS_KEY, DEFAULT_ENCODER_PARAMETERS);
    p[PROFILE_MODEL_COLUMN_ENCODER_WAVE_PARAMETERS_KEY] = subGroup.readEntry(PROFILE_MODEL_COLUMN_ENCODER_WAVE_PARAMETERS_KEY, DEFAULT_ENCODER_PARAMETERS);
    p[PROFILE_MODEL_COLUMN_ENCODER_CUSTOM_PARAMETERS_KEY] = subGroup.readEntry(PROFILE_MODEL_COLUMN_ENCODER_CUSTOM_PARAMETERS_KEY, DEFAULT_ENCODER_PARAMETERS);

    cache.append(p);

  }

  if (profileCount>0) {
    if (current_profile_index==-1) current_profile_index = 0;
    emit profilesRemovedOrInserted();
  }

}
