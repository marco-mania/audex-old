#include "qcddadevice.h"

QCDDADevice::QCDDADevice(QObject *parent, const QString& device) : QObject(parent) {

  Q_UNUSED(parent);

  drive = new QCDDADrive(this);
  if (!drive) {
    emit log("(QCDDADevice) unable to create drive class. low mem?");
    emit error(trUtf8("Internal device error."), trUtf8("Check your device and make a bug report."));
    return;
  }
  connect(drive, SIGNAL(driveStatusChanged(QCDDADrive::DriveStatus)), this, SLOT(slot_drive_status_changed(QCDDADrive::DriveStatus)));
  connect(drive, SIGNAL(error(const QString&, const QString&)), this, SLOT(slot_error(const QString&, const QString&)));
  connect(drive, SIGNAL(log(const QString&)), this, SLOT(slot_log(const QString&)));
  drive->setDevice(device);

  paranoia = new QCDDAParanoia(this);
  if (!paranoia) {
    emit log("(QCDDADevice) unable to create paranoia class. low mem?");
    emit error(trUtf8("Internal device error."), trUtf8("Check your device and make a bug report."));
    return;
  }
  connect(paranoia, SIGNAL(error(const QString&, const QString&)), this, SLOT(slot_error(const QString&, const QString&)));
  connect(paranoia, SIGNAL(log(const QString&)), this, SLOT(slot_log(const QString&)));
  paranoia->setDevice(device);

}

QCDDADevice::~QCDDADevice() {
  if (drive) delete drive;
  if (paranoia) delete paranoia;
}

bool QCDDADevice::setDevice(const QString& device) {
  bool ok1 = FALSE;
  if (drive) ok1 = drive->setDevice(device);
  bool ok2 = FALSE;
  if (paranoia && ok1) ok2 = paranoia->setDevice(device);
  return (ok1 && ok2);
}

QString QCDDADevice::device() const {
  if (drive) return drive->device();
  if (paranoia) return paranoia->device();
  return QString();
}

bool QCDDADevice::eject() {
  if (drive) return drive->eject();
  return FALSE;
}

bool QCDDADevice::isDiscInDrive() {
  if (drive) return drive->isDiscInDrive();
  return FALSE;
}

bool QCDDADevice::isEjectAble() {
  if (drive) return drive->isEjectAble();
  return FALSE;
}

bool QCDDADevice::isJukebox() {
  if (drive) return drive->isJukebox();
  return FALSE;
}

int QCDDADevice::jukeboxSlots() {
  if (drive) return drive->jukeboxSlots();
  return 0;
}

void QCDDADevice::selectJukeboxSlot(int slot) {
  if (drive) drive->selectJukeboxSlot(slot);
}

bool QCDDADevice::canReportDriveStatus() {
  if (drive) return drive->canReportDriveStatus();
  return FALSE;
}

QCDDADrive::DriveStatus QCDDADevice::driveStatus() {
  if (drive) return drive->driveStatus();
  return QCDDADrive::dsNull;
}

QCDDADrive::DiscType QCDDADevice::discType() {
  if (drive) return drive->discType();
  return QCDDADrive::dtNull;
}

void QCDDADevice::setParanoiaMode(int mode) {
  if (paranoia) paranoia->setParanoiaMode(mode);
}

void QCDDADevice::setNeverSkip(bool b) {
  if (paranoia) paranoia->setNeverSkip(b);
}

void QCDDADevice::setMaxRetries(int m) {
  if (paranoia) paranoia->setMaxRetries(m);
}

qint16* QCDDADevice::paranoiaRead(void (*callback)(long, int)) {
  if (paranoia) return paranoia->paranoiaRead(callback);
  return 0;
}

int QCDDADevice::paranoiaSeek(long sector, int mode) {
  if (paranoia) return paranoia->paranoiaSeek(sector, mode);
  return 0;
}

int QCDDADevice::firstSectorOfTrack(int track) {
  if (paranoia) return paranoia->firstSectorOfTrack(track);
  return 0;
}

int QCDDADevice::lastSectorOfTrack(int track) {
  if (paranoia) return paranoia->lastSectorOfTrack(track);
  return 0;
}

int QCDDADevice::numOfTracks() {
  if (paranoia) return paranoia->numOfTracks();
  return 0;
}

int QCDDADevice::numOfAudioTracks() {
  if (paranoia) return paranoia->numOfAudioTracks();
  return 0;
}

int QCDDADevice::length() {
  if (paranoia) return paranoia->length();
  return 0;
}

int QCDDADevice::numOfFrames() {
  if (paranoia) return paranoia->numOfFrames();
  return 0;
}

int QCDDADevice::lengthOfAudioTracks() {
  if (paranoia) return paranoia->lengthOfAudioTracks();
  return 0;
}

int QCDDADevice::numOfFramesOfAudioTracks() {
  if (paranoia) return paranoia->numOfFramesOfAudioTracks();
  return 0;
}

int QCDDADevice::numOfSkippedFrames(int n) {
  if (paranoia) return paranoia->numOfSkippedFrames(n);
  return 0;
}

int QCDDADevice::lengthOfTrack(int n) {
  if (paranoia) return paranoia->lengthOfTrack(n);
  return 0;
}

int QCDDADevice::numOfFramesOfTrack(int n) {
  if (paranoia) return paranoia->numOfFramesOfTrack(n);
  return 0;
}

double QCDDADevice::sizeOfTrack(int n) {
  if (paranoia) return paranoia->sizeOfTrack(n);
  return 0.0f;
}

int QCDDADevice::frameOffsetOfTrack(int n) {
  if (paranoia) return paranoia->frameOffsetOfTrack(n);
  return 0;
}

bool QCDDADevice::isAudioTrack(int n) {
  if (paranoia) return paranoia->isAudioTrack(n);
  return TRUE;
}

quint32 QCDDADevice::discid() {
  if (paranoia) return paranoia->discid();
  return 0;
}

void QCDDADevice::slot_drive_status_changed(QCDDADrive::DriveStatus drivestatus) {
  if (paranoia) paranoia->reset();
  emit driveStatusChanged(drivestatus);
}

void QCDDADevice::slot_log(const QString& message) {
  emit log(message);
}

void QCDDADevice::slot_error(const QString& description, const QString& solution) {
  emit error(description, solution);
}
