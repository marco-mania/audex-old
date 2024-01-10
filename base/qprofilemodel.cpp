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

#include "qprofilemodel.h"

QProfileModel::QProfileModel(QObject *parent, QSettings *settings) {
  Q_UNUSED(parent);
  _settings = settings;
  current_profile_row = -1;
  revert();
}

QProfileModel::~QProfileModel() {
  clear();
}

int QProfileModel::rowCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return cache.count();
}

int QProfileModel::columnCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return Q_PROFILE_MODEL_COLUMN_COUNT;
}

QVariant QProfileModel::data(const QModelIndex &index, int role) const {

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
      case Q_PROFILE_MODEL_COLUMN_NAME_INDEX : return cache.at(index.row())->name();
      case Q_PROFILE_MODEL_COLUMN_MASK_INDEX : return cache.at(index.row())->filenameMask();
      case Q_PROFILE_MODEL_COLUMN_ARTISTLABEL_INDEX : return cache.at(index.row())->artistLabel();
      case Q_PROFILE_MODEL_COLUMN_TITLELABEL_INDEX : return cache.at(index.row())->titleLabel();
      case Q_PROFILE_MODEL_COLUMN_COMMAND_INDEX : return cache.at(index.row())->command();
      case Q_PROFILE_MODEL_COLUMN_EXTENSION_INDEX : return cache.at(index.row())->extension();
      case Q_PROFILE_MODEL_COLUMN_PROFILEDBINDEX_INDEX : return cache.at(index.row())->profileDBIndex();
      case Q_PROFILE_MODEL_COLUMN_FAT32COMPATIBLE_INDEX : return cache.at(index.row())->fat32Compatible();
      case Q_PROFILE_MODEL_COLUMN_SC_INDEX : return cache.at(index.row())->sc();
      case Q_PROFILE_MODEL_COLUMN_SC_SCALE_INDEX : return cache.at(index.row())->scScale();
      case Q_PROFILE_MODEL_COLUMN_SC_SIZE_INDEX : return cache.at(index.row())->scSize();
      case Q_PROFILE_MODEL_COLUMN_SC_FORMAT_INDEX : return cache.at(index.row())->scFormat();
      case Q_PROFILE_MODEL_COLUMN_SC_NAME_INDEX : return cache.at(index.row())->scName();
      case Q_PROFILE_MODEL_COLUMN_PL_INDEX : return cache.at(index.row())->pl();
      case Q_PROFILE_MODEL_COLUMN_PL_FORMAT_INDEX : return cache.at(index.row())->plFormat();
      case Q_PROFILE_MODEL_COLUMN_PL_NAME_INDEX : return cache.at(index.row())->plName();
      default : ;
    }
  }

  if (role == Qt::DecorationRole) {
    return QIcon(":/buttons/images/profile.png");
  }

  return QVariant();

}

bool QProfileModel::setData(const QModelIndex &index, const QVariant &value, int role) {

  if (!index.isValid()) {
    return false;
  }

  if ((index.row() < 0) || (index.row() >= cache.count())) {
    return false;
  }

  if (role == Qt::EditRole) {

      switch (index.column()) {
        case Q_PROFILE_MODEL_COLUMN_NAME_INDEX : if (value.toString().isEmpty()) { emit error(trUtf8("Profile name must not be empty."), trUtf8("Please define one.")); return false; } break;
        case Q_PROFILE_MODEL_COLUMN_MASK_INDEX : if (value.toString().isEmpty()) { emit error(trUtf8("Profile mask must not be empty."), trUtf8("Please define one.")); return false; } break;
        case Q_PROFILE_MODEL_COLUMN_ARTISTLABEL_INDEX : if (value.toString().isEmpty()) { emit error(trUtf8("Profile artist label must not be empty."), trUtf8("Please define one.")); return false; } break;
        case Q_PROFILE_MODEL_COLUMN_TITLELABEL_INDEX : if (value.toString().isEmpty()) { emit error(trUtf8("Profile title label must not be empty."), trUtf8("Please define one.")); return false; } break;
        case Q_PROFILE_MODEL_COLUMN_COMMAND_INDEX : if (value.toString().isEmpty()) { emit error(trUtf8("Profile command must not be empty."), trUtf8("Please define one.")); return false; } break;
        case Q_PROFILE_MODEL_COLUMN_EXTENSION_INDEX : if (value.toString().isEmpty()) { emit error(trUtf8("Profile extension must not be empty."), trUtf8("Please define one.")); return false; } break;
        case Q_PROFILE_MODEL_COLUMN_PROFILEDBINDEX_INDEX : break;
        case Q_PROFILE_MODEL_COLUMN_FAT32COMPATIBLE_INDEX : break;
        case Q_PROFILE_MODEL_COLUMN_SC_INDEX : break;
        case Q_PROFILE_MODEL_COLUMN_SC_SCALE_INDEX : break;
        case Q_PROFILE_MODEL_COLUMN_SC_SIZE_INDEX : break;
        case Q_PROFILE_MODEL_COLUMN_SC_FORMAT_INDEX : if ((value.toString()!="JPEG") && (value.toString()!="JPG") && (value.toString()!="PNG") && (value.toString()!="TIFF") && (value.toString()!="BMP")) { emit error(trUtf8("Please choose on of these formats: JPG/JPEG, PNG, TIFF or BMP."), trUtf8("Please select one proper format.")); return FALSE; } break;
        case Q_PROFILE_MODEL_COLUMN_SC_NAME_INDEX : if (value.toString().isEmpty()) { emit error(trUtf8("Cover name must not be empty."), trUtf8("Please define one.")); return false; } break;
        case Q_PROFILE_MODEL_COLUMN_PL_INDEX : break;
        case Q_PROFILE_MODEL_COLUMN_PL_FORMAT_INDEX : if ((value.toString()!="M3U") && (value.toString()!="PLS") && (value.toString()!="XSPF")) { emit error(trUtf8("Please choose on of these formats: M3U, PLS or XSPF."), trUtf8("Please select one proper format.")); return FALSE; } break;
        case Q_PROFILE_MODEL_COLUMN_PL_NAME_INDEX : if (value.toString().isEmpty()) { emit error(trUtf8("Playlist name must not be empty."), trUtf8("Please define one.")); return false; } break;
        default : return FALSE;
      }

      //check if name is unique
      if (index.column()==Q_PROFILE_MODEL_COLUMN_NAME_INDEX) {
        bool found = FALSE;
        for (int i = 0; i < cache.count(); i++) {
          if (i==index.row()) continue;
          if (value.toString()==cache.at(i)->name()) { found = TRUE; break; }
        }
        if (found) {
          emit error(trUtf8("Profile name %1 already exists.").arg(value.toString()), trUtf8("Please choose another one."));
          return FALSE;
        }
      }

      switch (index.column()) {
        case Q_PROFILE_MODEL_COLUMN_NAME_INDEX : cache.at(index.row())->setName(value.toString()); break;
        case Q_PROFILE_MODEL_COLUMN_MASK_INDEX : cache.at(index.row())->setFilenameMask(value.toString()); break;
        case Q_PROFILE_MODEL_COLUMN_ARTISTLABEL_INDEX : cache.at(index.row())->setArtistLabel(value.toString()); break;
        case Q_PROFILE_MODEL_COLUMN_TITLELABEL_INDEX : cache.at(index.row())->setTitleLabel(value.toString()); break;
        case Q_PROFILE_MODEL_COLUMN_COMMAND_INDEX : cache.at(index.row())->setCommand(value.toString()); break;
        case Q_PROFILE_MODEL_COLUMN_EXTENSION_INDEX : cache.at(index.row())->setExtension(value.toString()); break;
        case Q_PROFILE_MODEL_COLUMN_PROFILEDBINDEX_INDEX : cache.at(index.row())->setProfileDBIndex(value.toInt()); break;
        case Q_PROFILE_MODEL_COLUMN_FAT32COMPATIBLE_INDEX : cache.at(index.row())->setFAT32Compatible(value.toBool()); break;
        case Q_PROFILE_MODEL_COLUMN_SC_INDEX : cache.at(index.row())->setSC(value.toBool()); break;
        case Q_PROFILE_MODEL_COLUMN_SC_SCALE_INDEX : cache.at(index.row())->setSCScale(value.toBool()); break;
        case Q_PROFILE_MODEL_COLUMN_SC_SIZE_INDEX : cache.at(index.row())->setSCSize(value.toSize()); break;
        case Q_PROFILE_MODEL_COLUMN_SC_FORMAT_INDEX : cache.at(index.row())->setSCFormat(value.toString()); break;
        case Q_PROFILE_MODEL_COLUMN_SC_NAME_INDEX : cache.at(index.row())->setSCName(value.toString()); break;
        case Q_PROFILE_MODEL_COLUMN_PL_INDEX : cache.at(index.row())->setPL(value.toBool()); break;
        case Q_PROFILE_MODEL_COLUMN_PL_FORMAT_INDEX : cache.at(index.row())->setPLFormat(value.toString()); break;
        case Q_PROFILE_MODEL_COLUMN_PL_NAME_INDEX : cache.at(index.row())->setPLName(value.toString()); break;
        default : return FALSE;
      }
      return TRUE;
  }

  reset();

  return FALSE;

}

QVariant QProfileModel::headerData(int section, Qt::Orientation orientation, int role) const {

  if (orientation == Qt::Vertical) return QVariant();

  if (role == Qt::DisplayRole) {
    switch (section) {
      case Q_PROFILE_MODEL_COLUMN_NAME_INDEX : return Q_PROFILE_MODEL_COLUMN_NAME_LABEL;
      case Q_PROFILE_MODEL_COLUMN_MASK_INDEX : return Q_PROFILE_MODEL_COLUMN_MASK_LABEL;
      case Q_PROFILE_MODEL_COLUMN_ARTISTLABEL_INDEX : return Q_PROFILE_MODEL_COLUMN_ARTISTLABEL_LABEL;
      case Q_PROFILE_MODEL_COLUMN_TITLELABEL_INDEX : return Q_PROFILE_MODEL_COLUMN_TITLELABEL_LABEL;
      case Q_PROFILE_MODEL_COLUMN_COMMAND_INDEX : return Q_PROFILE_MODEL_COLUMN_COMMAND_LABEL;
      case Q_PROFILE_MODEL_COLUMN_EXTENSION_INDEX : return Q_PROFILE_MODEL_COLUMN_EXTENSION_LABEL;
      case Q_PROFILE_MODEL_COLUMN_PROFILEDBINDEX_INDEX : return Q_PROFILE_MODEL_COLUMN_PROFILEDBINDEX_LABEL;
      case Q_PROFILE_MODEL_COLUMN_FAT32COMPATIBLE_INDEX : return Q_PROFILE_MODEL_COLUMN_FAT32COMPATIBLE_LABEL;
      case Q_PROFILE_MODEL_COLUMN_SC_INDEX : return Q_PROFILE_MODEL_COLUMN_SC_LABEL;
      case Q_PROFILE_MODEL_COLUMN_SC_SCALE_INDEX : return Q_PROFILE_MODEL_COLUMN_SC_SCALE_LABEL;
      case Q_PROFILE_MODEL_COLUMN_SC_SIZE_INDEX : return Q_PROFILE_MODEL_COLUMN_SC_SIZE_LABEL;
      case Q_PROFILE_MODEL_COLUMN_SC_FORMAT_INDEX : return Q_PROFILE_MODEL_COLUMN_SC_FORMAT_LABEL;
      case Q_PROFILE_MODEL_COLUMN_SC_NAME_INDEX : return Q_PROFILE_MODEL_COLUMN_SC_NAME_LABEL;
      case Q_PROFILE_MODEL_COLUMN_PL_INDEX : return Q_PROFILE_MODEL_COLUMN_PL_LABEL;
      case Q_PROFILE_MODEL_COLUMN_PL_FORMAT_INDEX : return Q_PROFILE_MODEL_COLUMN_PL_FORMAT_LABEL;
      case Q_PROFILE_MODEL_COLUMN_PL_NAME_INDEX : return Q_PROFILE_MODEL_COLUMN_PL_NAME_LABEL;
      default : ;
    }
  }

  return QVariant();

}

bool QProfileModel::removeRows(int row, int count, const QModelIndex& parent) {

  Q_UNUSED(parent);

  if ((row < 0) || (row >= cache.count())) {
    return false;
  }

  if (count <= 0) {
    return false;
  }

  if (row+count > cache.count()) {
    count = cache.count()-row;
  }

  for (int i = row; i < cache.count(); i++) { delete cache.at(i); cache.removeAt(i); }

  reset();

  return true;

}

bool QProfileModel::insertRows(int row, int count, const QModelIndex& parent) {

  Q_UNUSED(parent);

  if ((row < 0) || (row > cache.count())) {
    return false;
  }

  if (count <= 0) {
    return false;
  }

  if (row == cache.count()) {
    for (int i = 0; i < count; i++) { QProfile *p = new QProfile(); cache.append(p); }
  } else {
    for (int i = row; i < count; i++) { QProfile *p = new QProfile(); cache.insert(i, p); }
  }

  reset();

  return true;

}

void QProfileModel::setCurrentProfileRow(int row) {
  if ((row < 0) || (row >= cache.count())) return;
  current_profile_row = row;
  emit currentProfileChanged(row);
}


int QProfileModel::currentProfileRow() {
  if (cache.count()==0) return -1;
  return current_profile_row;
}

void QProfileModel::clear() {
  for (int i = 0; i < cache.count(); i++) delete cache.at(i);
}

bool QProfileModel::submit() {
  return save(_settings);
}

bool QProfileModel::save(QSettings *settings) {
  if (!settings) return false;
  settings->beginGroup("profiles");
  settings->remove("");
  settings->endGroup();
  settings->beginWriteArray("profiles");
  for (int i = 0; i < cache.count(); i++) {
    settings->setArrayIndex(i);
    settings->setValue("name", cache.at(i)->name());
    settings->setValue("mask", cache.at(i)->filenameMask());
    settings->setValue("artist_label", cache.at(i)->artistLabel());
    settings->setValue("title_label", cache.at(i)->titleLabel());
    settings->setValue("command", cache.at(i)->command());
    settings->setValue("extension", cache.at(i)->extension());
    settings->setValue("profile_db_index", cache.at(i)->profileDBIndex());
    settings->setValue("fat32_compatible", cache.at(i)->fat32Compatible());
    settings->setValue("sc", cache.at(i)->sc());
    settings->setValue("sc_scale", cache.at(i)->scScale());
    settings->setValue("sc_size", cache.at(i)->scSize());
    settings->setValue("sc_format", cache.at(i)->scFormat());
    settings->setValue("sc_name", cache.at(i)->scName());
    settings->setValue("pl", cache.at(i)->pl());
    settings->setValue("pl_format", cache.at(i)->plFormat());
    settings->setValue("pl_name", cache.at(i)->plName());
  }
  settings->endArray();
  return true;
}

void QProfileModel::_new_name(QString& name) {
  for (int j = 0; j < cache.count(); j++) {
    if (name==cache.at(j)->name()) {
      name = "_"+name;
      _new_name(name);
      return;
    }
  }
}

void QProfileModel::load(QSettings *settings) {
  if (!settings) return;
  int size = settings->beginReadArray("profiles");
  for (int i = 0; i < size; i++) {
    settings->setArrayIndex(i);
    QProfile *p = new QProfile();
    QString name = settings->value("name", "Profile").toString();
    _new_name(name); p->setName(name);
    p->setFilenameMask(settings->value("mask", "~/${artist}/${artist} [${date}] ${album}/${trackno} - ${artist} - ${title}.${extension}").toString());
    p->setArtistLabel(settings->value("artist_label", "Artist").toString());
    p->setTitleLabel(settings->value("title_label", "Title").toString());
    p->setCommand(settings->value("command", "oggenc -q 6 -o $o $i").toString());
    p->setExtension(settings->value("extension", "ogg").toString());
    p->setProfileDBIndex(settings->value("profile_db_index", -1).toInt());
    p->setFAT32Compatible(settings->value("fat32_compatible", FALSE).toBool());
    p->setSC(settings->value("sc", FALSE).toBool());
    p->setSCScale(settings->value("sc_scale", FALSE).toBool());
    p->setSCSize(settings->value("sc_size", QSize(300, 300)).toSize());
    p->setSCFormat(settings->value("sc_format", "JPG").toString());
    p->setSCName(settings->value("sc_name", "cover").toString());
    p->setPL(settings->value("pl", FALSE).toBool());
    p->setPLFormat(settings->value("pl_format", "M3U").toString());
    p->setPLName(settings->value("pl_name", "playlist").toString());
    cache.append(p);
  }
  settings->endArray();
  reset();
}

void QProfileModel::revert() {
  clear();
  load(_settings);
}

void QProfileModel::copy() {
  int row = currentProfileRow();
  if (row==-1) return;
  QProfile *current = cache[row];
  QProfile *p = new QProfile();

  QString name = current->name();
  _new_name(name); p->setName(name);
  p->setFilenameMask(current->filenameMask());
  p->setArtistLabel(current->artistLabel());
  p->setTitleLabel(current->titleLabel());
  p->setCommand(current->command());
  p->setExtension(current->extension());
  p->setProfileDBIndex(current->profileDBIndex());
  p->setFAT32Compatible(current->fat32Compatible());
  p->setSCScale(current->scScale());
  p->setSCSize(current->scSize());
  p->setSCFormat(current->scFormat());
  p->setSCName(current->scName());
  cache.append(p);
  reset();
}

void QProfileModel::saveProfilesToFile(const QString& filename) {
  QSettings ini(filename, QSettings::IniFormat, this);
  if (!save(&ini)) {
    emit error(trUtf8("Error while saving profiles in file %1.").arg(filename), trUtf8("Try another filename. Check the path (write-access?) and your drive."));
  }
}

void QProfileModel::loadProfilesFromFile(const QString& filename) {
  QSettings ini(filename, QSettings::IniFormat, this);
  load(&ini);
}
