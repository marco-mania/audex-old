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

#ifndef QPROFILEMODEL_HEADER
#define QPROFILEMODEL_HEADER

#include <QtCore>
#include <QtGui>

#define Q_PROFILE_MODEL_COLUMN_COUNT			16

#define Q_PROFILE_MODEL_COLUMN_NAME_LABEL		trUtf8("Name")
#define Q_PROFILE_MODEL_COLUMN_MASK_LABEL		trUtf8("Mask")
#define Q_PROFILE_MODEL_COLUMN_ARTISTLABEL_LABEL	trUtf8("Artist Label")
#define Q_PROFILE_MODEL_COLUMN_TITLELABEL_LABEL		trUtf8("Title Label")
#define Q_PROFILE_MODEL_COLUMN_COMMAND_LABEL		trUtf8("Command")
#define Q_PROFILE_MODEL_COLUMN_EXTENSION_LABEL		trUtf8("Extension")
#define Q_PROFILE_MODEL_COLUMN_PROFILEDBINDEX_LABEL	trUtf8("Profile DB Index")
#define Q_PROFILE_MODEL_COLUMN_FAT32COMPATIBLE_LABEL	trUtf8("FAT32 Compatible Filenames")
#define Q_PROFILE_MODEL_COLUMN_SC_LABEL			trUtf8("Store cover");
#define Q_PROFILE_MODEL_COLUMN_SC_SCALE_LABEL		trUtf8("Scale cover")
#define Q_PROFILE_MODEL_COLUMN_SC_SIZE_LABEL		trUtf8("Size of cover")
#define Q_PROFILE_MODEL_COLUMN_SC_FORMAT_LABEL		trUtf8("Format of the cover to store")
#define Q_PROFILE_MODEL_COLUMN_SC_NAME_LABEL		trUtf8("Name of the cover to store")
#define Q_PROFILE_MODEL_COLUMN_PL_LABEL			trUtf8("Playlist")
#define Q_PROFILE_MODEL_COLUMN_PL_FORMAT_LABEL		trUtf8("Playlist format")
#define Q_PROFILE_MODEL_COLUMN_PL_NAME_LABEL		trUtf8("Playlist name")

#define Q_PROFILE_MODEL_COLUMN_NAME_INDEX		0
#define Q_PROFILE_MODEL_COLUMN_MASK_INDEX		1
#define Q_PROFILE_MODEL_COLUMN_ARTISTLABEL_INDEX	2
#define Q_PROFILE_MODEL_COLUMN_TITLELABEL_INDEX		3
#define Q_PROFILE_MODEL_COLUMN_COMMAND_INDEX		4
#define Q_PROFILE_MODEL_COLUMN_EXTENSION_INDEX		5
#define Q_PROFILE_MODEL_COLUMN_PROFILEDBINDEX_INDEX	6
#define Q_PROFILE_MODEL_COLUMN_FAT32COMPATIBLE_INDEX	7
#define Q_PROFILE_MODEL_COLUMN_SC_INDEX			8
#define Q_PROFILE_MODEL_COLUMN_SC_SCALE_INDEX		9
#define Q_PROFILE_MODEL_COLUMN_SC_SIZE_INDEX		10
#define Q_PROFILE_MODEL_COLUMN_SC_FORMAT_INDEX		11
#define Q_PROFILE_MODEL_COLUMN_SC_NAME_INDEX		12
#define Q_PROFILE_MODEL_COLUMN_PL_INDEX			13
#define Q_PROFILE_MODEL_COLUMN_PL_FORMAT_INDEX		14
#define Q_PROFILE_MODEL_COLUMN_PL_NAME_INDEX		15


class QProfile : public QObject {
  Q_OBJECT
public:
  QProfile() {
    _name = "";
    _mask = "";
    _artist_label = "";
    _title_label = "";
    _command = "";
    _profile_db_index = -1;
    _fat32_compatible = FALSE;
    _sc = FALSE;
    _sc_scale = FALSE;
    _sc_size = QSize(300, 300);
    _sc_format = "JPEG";
    _sc_name = "cover";
    _pl = FALSE;
    _pl_format = "M3U";
    _pl_name = "playlist";
  }
  ~QProfile() { }
  void setName(const QString& n) { _name = n; }
  QString name() const { return _name; }
  void setFilenameMask(const QString& m) { _mask = m; }
  QString filenameMask() const { return _mask; }
  void setArtistLabel(const QString& l) { _artist_label = l; }
  QString artistLabel() const { return _artist_label; }
  void setTitleLabel(const QString& l) { _title_label = l; }
  QString titleLabel() const { return _title_label; }
  void setCommand(const QString& c) { _command = c; }
  QString command() const { return _command; }
  void setExtension(const QString& e) { _extension = e; }
  QString extension() const { return _extension; }
  void setProfileDBIndex(const int db_index) { _profile_db_index = db_index; }
  int profileDBIndex() const { return _profile_db_index; }
  void setFAT32Compatible(const bool fat32) { _fat32_compatible = fat32; }
  bool fat32Compatible() const { return _fat32_compatible; }
  void setSC(const bool sc) { _sc = sc; }
  bool sc() const { return _sc; }
  void setSCScale(const bool scale) { _sc_scale = scale; }
  bool scScale() const { return _sc_scale; }
  void setSCSize(const QSize& s) { _sc_size = s; }
  QSize scSize() const { return _sc_size; }
  void setSCFormat(const QString& f) { _sc_format = f; }
  QString scFormat() const { return _sc_format; }
  void setSCName(const QString& n) { _sc_name = n; }
  QString scName() const { return _sc_name; }
  void setPL(const bool pl) { _pl = pl; }
  bool pl() const { return _pl; }
  void setPLFormat(const QString& f) { _pl_format = f; }
  QString plFormat() const { return _pl_format; }
  void setPLName(const QString& n) { _pl_name = n; }
  QString plName() const { return _pl_name; }
private:
  QString _name;
  QString _mask;
  QString _artist_label;
  QString _title_label;
  QString _command;
  QString _extension;
  int _profile_db_index;
  bool _fat32_compatible;
  int _paranoia_mode;
  bool _sc;
  bool _sc_scale;
  QSize _sc_size;
  QString _sc_format;
  QString _sc_name;
  bool _pl;
  QString _pl_format;
  QString _pl_name;
};

/** qt4 audex profile model **/
class QProfileModel : public QAbstractTableModel {
  Q_OBJECT
public:
  QProfileModel(QObject *parent = 0, QSettings *settings = 0);
  ~QProfileModel();

  int rowCount(const QModelIndex &parent = QModelIndex()) const;
  int columnCount(const QModelIndex &parent = QModelIndex()) const;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());
  bool insertRows(int row, int count, const QModelIndex & parent = QModelIndex());

  int currentProfileRow();

  void clear();

public slots:
  bool submit();
  void revert();

  void copy();

  void saveProfilesToFile(const QString& filename);
  void loadProfilesFromFile(const QString& filename);

  void setCurrentProfileRow(int row);

signals:
  void currentProfileChanged(const int row);

  void log(const QString& message);
  void error(const QString& description,
	const QString& solution = QString());
  void warning(const QString& description);
  void info(const QString& description);

private:
  QList<QProfile*> cache;
  QSettings *_settings;
  int current_profile_row;

  void _new_name(QString& name);
  bool save(QSettings *settings);
  void load(QSettings *settings);

};

#endif
