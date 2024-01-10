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

#ifndef PROFILEMODEL_HEADER
#define PROFILEMODEL_HEADER

#include <QAbstractTableModel>
#include <QVariant>
#include <QString>

#include <KDebug>
#include <KLocale>
#include <KIcon>
#include <KConfig>
#include <KConfigGroup>
#include <KGlobal>
#include <KMessageBox>

#include "../utils/error.h"

#define PROFILE_MODEL_COLUMN_COUNT			22

#define PROFILE_MODEL_COLUMN_NAME_LABEL			i18n("Name")
#define PROFILE_MODEL_COLUMN_MASK_LABEL			i18n("Mask")
#define PROFILE_MODEL_COLUMN_COMMAND_LABEL		i18n("Command")
#define PROFILE_MODEL_COLUMN_SUFFIX_LABEL		i18n("Suffix")
#define PROFILE_MODEL_COLUMN_PROFILEINDEX_LABEL		i18n("Profile Index")
#define PROFILE_MODEL_COLUMN_FAT32COMPATIBLE_LABEL	i18n("FAT32 Compatible Filenames")
#define PROFILE_MODEL_COLUMN_SC_LABEL			i18n("Store cover");
#define PROFILE_MODEL_COLUMN_SC_SCALE_LABEL		i18n("Scale cover")
#define PROFILE_MODEL_COLUMN_SC_SIZE_LABEL		i18n("Size of cover")
#define PROFILE_MODEL_COLUMN_SC_FORMAT_LABEL		i18n("Format of the cover to store")
#define PROFILE_MODEL_COLUMN_SC_NAME_LABEL		i18n("Name of the cover to store")
#define PROFILE_MODEL_COLUMN_PL_LABEL			i18n("Playlist")
#define PROFILE_MODEL_COLUMN_PL_FORMAT_LABEL		i18n("Playlist format")
#define PROFILE_MODEL_COLUMN_PL_NAME_LABEL		i18n("Playlist name")
#define PROFILE_MODEL_COLUMN_DISCID_LABEL		i18n("Store discid")
#define PROFILE_MODEL_COLUMN_INF_LABEL			i18n("Store infos")
#define PROFILE_MODEL_COLUMN_INF_TEXT_LABEL		i18n("Info text")
#define PROFILE_MODEL_COLUMN_INF_NAME_LABEL		i18n("Info name")
#define PROFILE_MODEL_COLUMN_INF_SUFFIX_LABEL		i18n("Info suffix")
#define PROFILE_MODEL_COLUMN_HL_LABEL			i18n("Store hashlist")
#define PROFILE_MODEL_COLUMN_HL_FORMAT_LABEL		i18n("Hashlist format")
#define PROFILE_MODEL_COLUMN_HL_NAME_LABEL		i18n("Hashlist name")

#define PROFILE_MODEL_COLUMN_NAME_INDEX			0
#define PROFILE_MODEL_COLUMN_MASK_INDEX			1
#define PROFILE_MODEL_COLUMN_COMMAND_INDEX		2
#define PROFILE_MODEL_COLUMN_SUFFIX_INDEX		3
#define PROFILE_MODEL_COLUMN_PROFILEINDEX_INDEX		4
#define PROFILE_MODEL_COLUMN_FAT32COMPATIBLE_INDEX	5
#define PROFILE_MODEL_COLUMN_SC_INDEX			6
#define PROFILE_MODEL_COLUMN_SC_SCALE_INDEX		7
#define PROFILE_MODEL_COLUMN_SC_SIZE_INDEX		8
#define PROFILE_MODEL_COLUMN_SC_FORMAT_INDEX		9
#define PROFILE_MODEL_COLUMN_SC_NAME_INDEX		10
#define PROFILE_MODEL_COLUMN_PL_INDEX			11
#define PROFILE_MODEL_COLUMN_PL_FORMAT_INDEX		12
#define PROFILE_MODEL_COLUMN_PL_NAME_INDEX		13
#define PROFILE_MODEL_COLUMN_DISCID_INDEX		14
#define PROFILE_MODEL_COLUMN_INF_INDEX			15
#define PROFILE_MODEL_COLUMN_INF_TEXT_INDEX		16
#define PROFILE_MODEL_COLUMN_INF_NAME_INDEX		17
#define PROFILE_MODEL_COLUMN_INF_SUFFIX_INDEX		18
#define PROFILE_MODEL_COLUMN_HL_INDEX			19
#define PROFILE_MODEL_COLUMN_HL_FORMAT_INDEX		20
#define PROFILE_MODEL_COLUMN_HL_NAME_INDEX		21

#define PROFILE_MODEL_NAME_KEY				"name"
#define PROFILE_MODEL_MASK_KEY				"mask"
#define PROFILE_MODEL_COMMAND_KEY			"command"
#define PROFILE_MODEL_SUFFIX_KEY			"suffix"
#define PROFILE_MODEL_PROFILEINDEX_KEY			"profile_key"
#define PROFILE_MODEL_FAT32COMPATIBLE_KEY		"fat32_compatible"
#define PROFILE_MODEL_SC_KEY				"sc"
#define PROFILE_MODEL_SC_SCALE_KEY			"sc_scale"
#define PROFILE_MODEL_SC_SIZE_KEY			"sc_size"
#define PROFILE_MODEL_SC_FORMAT_KEY			"sc_format"
#define PROFILE_MODEL_SC_NAME_KEY			"sc_name"
#define PROFILE_MODEL_PL_KEY				"pl"
#define PROFILE_MODEL_PL_FORMAT_KEY			"pl_format"
#define PROFILE_MODEL_PL_NAME_KEY			"pl_name"
#define PROFILE_MODEL_DISCID_KEY			"discid"
#define PROFILE_MODEL_INF_KEY				"inf"
#define PROFILE_MODEL_INF_TEXT_KEY			"inf_text"
#define PROFILE_MODEL_INF_NAME_KEY			"inf_name"
#define PROFILE_MODEL_INF_SUFFIX_KEY			"inf_suffix"
#define PROFILE_MODEL_HL_KEY				"hl"
#define PROFILE_MODEL_HL_FORMAT_KEY			"hl_format"
#define PROFILE_MODEL_HL_NAME_KEY			"hl_name"

typedef QMap<QString,QVariant> Profile;

/** audex profile model **/
class ProfileModel : public QAbstractTableModel {
  Q_OBJECT
public:
  ProfileModel(QObject *parent = 0);
  ~ProfileModel();

  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  int columnCount(const QModelIndex &parent = QModelIndex()) const;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());
  bool insertRows(int row, int count, const QModelIndex & parent = QModelIndex());

  int currentProfileRow() const;

  void clear();

  bool nameExists(const QString& name) const;
  int profileIndexMax() const;

  Error lastError() const;

public slots:
  void commit();
  void revert();

  void copy(const int profileRow);

  bool saveProfilesToFile(const QString& filename);
  bool loadProfilesFromFile(const QString& filename);

  void setCurrentProfileRow(int row);

signals:
  void currentProfileChanged(const int row);

  void profilesRemovedOrInserted();

private:
  const Profile newProfile(const QString& name = "", const QString& mask = "", const QString& command = "", const QString& suffix = "",
		const int profileIndex = -1, const bool fat32Compatible = FALSE,
		const bool storeCover = FALSE, const bool storeCoverScale = FALSE, const QSize& storeCoverSize = QSize(300, 300),
		const QString& storeCoverFormat = "JPEG", const QString& storeCoverName = "cover",
		const bool playlist = FALSE, const QString& playlistFormat = "M3U", const QString& playlistName = "playlist",
		const bool discid = FALSE, const bool inf = FALSE, const QStringList& infoText = QStringList(), const QString& infoName = "info", const QString& infoSuffix = "nfo",
		const bool hashlist = FALSE, const QString& hashlistFormat = "SFV", const QString& hashlistName = "checksums");
  QList<Profile> cache;
  int current_profile_row;

  Error error;

  void _new_name(QString& name);

  void save(KConfig *config);
  void load(KConfig *config);

};

#endif
