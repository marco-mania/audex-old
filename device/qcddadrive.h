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

#ifndef QCDDADRIVE_HEADER
#define QCDDADRIVE_HEADER

#include <QtCore>

extern "C" {
  #include <stdio.h>
  #include <stdlib.h>
  #include <fcntl.h>
  #include <stdarg.h>
  #include <errno.h>
  #include <netdb.h>
  #include <unistd.h>
  #include <string.h>
  #include <sys/ioctl.h>
  #include <sys/types.h>
  #include <linux/cdrom.h>
}

class QCDDADrive : public QObject {
  Q_OBJECT
public:
  QCDDADrive(QObject *parent = 0);
  ~QCDDADrive();

  bool setDevice(const QString& device);
  QString device() const;

  bool eject();
  bool isDiscInDrive();
  bool isEjectAble();
  bool isJukebox();
  int jukeboxSlots();
  void selectJukeboxSlot(int slot);
  bool canReportDriveStatus();

  enum DriveStatus { dsNull, dsDriveEmpty, dsDriveReady, dsDriveOpen, dsDriveNotReady, dsNoInfo, dsUnknown };
  QCDDADrive::DriveStatus driveStatus();
  enum DiscType { dtNull, dtAudio, dtMixed, dtXA22, dtXA21, dtData, dtNoInfo, dtUnknown };
  QCDDADrive::DiscType discType();

signals:
  void driveStatusChanged(QCDDADrive::DriveStatus drivestatus);

  void log(const QString& message);
  void error(const QString& description,
	const QString& solution = QString());

private slots:
  void refresh_drive_status();

private:
  QString _device;

  QTimer *timer;
  QMutex mutex;

  QCDDADrive::DriveStatus drive_status;
  QCDDADrive::DriveStatus keep_drive_status;

  void refresh_drive_capabilities();
  int drive_capabilities;

};

#endif
