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

#ifndef QCDDADEVICE_HEADER
#define QCDDADEVICE_HEADER

#include <QtCore>

#include "qcddadrive.h"
#include "qcddaparanoia.h"

class QCDDADevice : public QObject {
  Q_OBJECT
public:
  QCDDADevice(QObject *parent = 0, const QString& device = "/dev/cdrom");
  ~QCDDADevice();

  bool setDevice(const QString& device = "");
  QString device() const;

  bool eject();
  bool isDiscInDrive();
  bool isEjectAble();
  bool isJukebox();
  int jukeboxSlots();
  void selectJukeboxSlot(int slot);
  bool canReportDriveStatus();

  QCDDADrive::DriveStatus driveStatus();
  QCDDADrive::DiscType discType();

  void setParanoiaMode(int mode); /* default: 3 */
  void setNeverSkip(bool b);
  void setMaxRetries(int m); /* default: 20 */

  qint16* paranoiaRead(void (*callback)(long, int));
  int paranoiaSeek(long sector, int mode);

  int firstSectorOfTrack(int track);
  int lastSectorOfTrack(int track);

  int numOfTracks();
  int numOfAudioTracks();

  int length();
  int numOfFrames(); //whole disc
  int lengthOfAudioTracks(); //length of all audio tracks
  int numOfFramesOfAudioTracks();
  /*sum skipped (because it is an audio track) frames,
    usually used to calculate overall percent*/
  int numOfSkippedFrames(int n = 100);

  int lengthOfTrack(int n);
  int numOfFramesOfTrack(int n);
  double sizeOfTrack(int n); //in MiB
  int frameOffsetOfTrack(int n);
  bool isAudioTrack(int n);

  quint32 discid();

signals:
  void driveStatusChanged(QCDDADrive::DriveStatus drivestatus);

  void log(const QString& message);
  void error(const QString& description,
	const QString& solution = QString());

private slots:
  void slot_drive_status_changed(QCDDADrive::DriveStatus drivestatus);

  void slot_log(const QString& message);
  void slot_error(const QString& description, const QString& solution);

private:
  QCDDADrive *drive;
  QCDDAParanoia *paranoia;

};

#endif
