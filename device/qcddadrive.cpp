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

#include "qcddadrive.h"

QCDDADrive::QCDDADrive(QObject *parent) : QObject(parent) {
  Q_UNUSED(parent);
  drive_status = dsNoInfo;
  timer = new QTimer(this);
  if (!timer) {
    emit log("(QCDDADrive) could not create timer class.");
    emit error(trUtf8("Internal panic error."), trUtf8("Check your hardware and make a bug report."));
    emit driveStatusChanged(dsDriveNotReady);
    return;
  }
  connect(timer, SIGNAL(timeout()), this, SLOT(refresh_drive_status()));
  timer->start(500);
}

QCDDADrive::~QCDDADrive() {
  delete timer;
}

bool QCDDADrive::setDevice(const QString& device) {
  mutex.lock();
  if (!timer) {
    emit log("(QCDDADrive) timer class does not exist.");
    emit error(trUtf8("Internal panic error."), trUtf8("Check your hardware and make a bug report."));
    emit driveStatusChanged(dsDriveNotReady);
    mutex.unlock();
    return FALSE;
  }
  if (device.isEmpty() && (_device.isEmpty())) _device = "/dev/cdrom";
  if (!device.isEmpty()) _device = device;
  keep_drive_status = dsNull;
  refresh_drive_capabilities();
  mutex.unlock();
  return TRUE;
}

QString QCDDADrive::device() const {
  return _device;
}

bool QCDDADrive::eject() {

  if (_device.isEmpty()) return FALSE;
  if (!isEjectAble()) return FALSE;

  if (drive_status==dsDriveOpen) {
    int drive = open(_device.toAscii().data(), O_RDONLY | O_NONBLOCK);
    if (drive<0) {
      emit log("(QCDDADrive) unable to init drive (10). Hardware problem? Wrong device node?");
      emit error(trUtf8("Internal device error."), trUtf8("Check your device and make a bug report."));
      emit driveStatusChanged(dsDriveNotReady);
      return FALSE;
    }
    if (ioctl(drive, CDROMCLOSETRAY, 0)) {
      emit error(trUtf8("Error while closing the tray."), trUtf8("Check if your device has the capabilities to close. Please make a bug report."));
      return FALSE;
    }
    close(drive);
    return TRUE;
  } else {
    int drive = open(_device.toAscii().data(), O_RDONLY | O_NONBLOCK);
    if (drive<0) {
      emit log("(QCDDADrive) unable to init drive (20). Hardware problem? Wrong device node?");
      emit error(trUtf8("Internal device error."), trUtf8("Check your device and make a bug report."));
      emit driveStatusChanged(dsDriveNotReady);
      return FALSE;
    }
    if (ioctl(drive, CDROMEJECT, 0)) {
      emit error(trUtf8("Error while opening the tray."), trUtf8("Check if your device has the capabilities to eject. Please make a bug report."));
      return FALSE;
    }
    close(drive);
    return TRUE;
  }

}

bool QCDDADrive::isDiscInDrive() {
  return (drive_status==dsDriveReady);
}

bool QCDDADrive::isEjectAble() {
  return (drive_capabilities & CDC_OPEN_TRAY);
}

bool QCDDADrive::isJukebox() {
  return (drive_capabilities & CDC_SELECT_DISC);
}

int QCDDADrive::jukeboxSlots() {
  if (_device.isEmpty()) return 0;
  int drive = open(_device.toAscii().data(), O_RDONLY | O_NONBLOCK);
  if (drive<0) {
    emit log("(QCDDADrive) unable to init drive (30). Hardware problem? Wrong device node?");
    emit error(trUtf8("Internal device error."), trUtf8("Check your device and make a bug report."));
    emit driveStatusChanged(dsDriveNotReady);
    _device.clear();
    return 0;
  }
  int ret = ioctl(drive, CDROM_CHANGER_NSLOTS, 0);
  close(drive);
  return ret;
}

void QCDDADrive::selectJukeboxSlot(int slot) {
  if (isJukebox()) {
    int drive = open(_device.toAscii().data(), O_RDONLY | O_NONBLOCK);
    if (drive<0) {
      emit log("(QCDDADrive) unable to init drive (40). Hardware problem? Wrong device node?");
      emit error(trUtf8("Internal device error."), trUtf8("Check your device and make a bug report."));
      emit driveStatusChanged(dsDriveNotReady);
      _device.clear();
      return;
    }
    ioctl(drive, CDROM_SELECT_DISC, slot);
    close(drive);
    refresh_drive_capabilities();
  }
}

bool QCDDADrive::canReportDriveStatus() {
  return (drive_capabilities & CDC_DRIVE_STATUS);
}

QCDDADrive::DriveStatus QCDDADrive::driveStatus() {
  return drive_status;
}

QCDDADrive::DiscType QCDDADrive::discType() {
  if (_device.isEmpty()) return dtNoInfo;
  int drive = open(_device.toAscii().data(), O_RDONLY | O_NONBLOCK);
  if (drive<0) {
    emit log("(QCDDADrive) unable to init drive (50). Hardware problem? Wrong device node?");
    emit error(trUtf8("Internal device error."), trUtf8("Check your device and make a bug report."));
    emit driveStatusChanged(dsDriveNotReady);
    _device.clear();
    return dtNoInfo;
  }
  int status = ioctl(drive, CDROM_DISC_STATUS, 0);
  close(drive);
  switch (status) {
    case CDS_NO_INFO : return dtNoInfo;
    case CDS_AUDIO : return dtAudio;
    case CDS_MIXED : return dtMixed;
    case CDS_XA_2_2 : return dtXA22;
    case CDS_XA_2_1 : return dtXA21;
    case CDS_DATA_1 : return dtData;
    default : return dtUnknown;
  }
}

void QCDDADrive::refresh_drive_status() {
  if (!mutex.tryLock()) { return; }
  if (_device.isEmpty()) { mutex.unlock(); return; }
  int drive = open(_device.toAscii().data(), O_RDONLY | O_NONBLOCK);
  if (drive<0) {
    emit log("(QCDDADrive) unable to init drive (60). Hardware problem? Wrong device node?");
    emit error(trUtf8("Internal device error."), trUtf8("Check your device. Is it really \"%1\"? If so also check your permissions on \"%1\".").arg(_device));
    emit driveStatusChanged(dsDriveNotReady);
    _device.clear();
    mutex.unlock();
    return;
  }
  int status = ioctl(drive, CDROM_DRIVE_STATUS, 0);
  close(drive);
  switch (status) {
    case CDS_NO_INFO : drive_status = dsNoInfo; break;
    case CDS_NO_DISC : drive_status = dsDriveEmpty; break;
    case CDS_TRAY_OPEN : drive_status = dsDriveOpen; break;
    case CDS_DRIVE_NOT_READY : drive_status = dsDriveNotReady; break;
    case CDS_DISC_OK : drive_status = dsDriveReady; break;
    default : drive_status = dsUnknown; break;
  }
  if (drive_status != keep_drive_status) {
    switch (drive_status) {
      case dsNoInfo : emit log("(QCDDAParanoia) (status) no info about drive status"); emit driveStatusChanged(dsNoInfo); break;
      case dsDriveEmpty : emit log("(QCDDAParanoia) (status) drive empty"); emit driveStatusChanged(dsDriveEmpty); break;
      case dsDriveOpen : emit log("(QCDDAParanoia) (status) drive open"); emit driveStatusChanged(dsDriveOpen); break;
      case dsDriveNotReady : emit log("(QCDDAParanoia) (status) drive not ready"); emit driveStatusChanged(dsDriveNotReady); break;
      case dsDriveReady : emit log("(QCDDAParanoia) (status) drive ready"); emit driveStatusChanged(dsDriveReady); break;
      default : emit log("(QCDDAParanoia) (status) unknown drive status"); break;
    }
    keep_drive_status = drive_status;
  }
  mutex.unlock();
}

void QCDDADrive::refresh_drive_capabilities() {
  if (_device.isEmpty()) {
    drive_capabilities = 0;
    return;
  }
  int drive = open(_device.toAscii().data(), O_RDONLY | O_NONBLOCK);
  if (drive<0) {
    emit log("(QCDDADrive) unable to init drive (70). Hardware problem? Wrong device node?");
    emit error(trUtf8("Internal device error."), trUtf8("Check your device. Is it really %1? If so also check your permissions on \"%1\"").arg(_device));
    emit driveStatusChanged(dsDriveNotReady);
    _device.clear();
    return;
  }
  drive_capabilities = ioctl(drive, CDROM_GET_CAPABILITY, 0);
  close(drive);
}
