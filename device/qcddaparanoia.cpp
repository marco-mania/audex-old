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

#include "qcddaparanoia.h"

/* some of this code in here is based on k3b 0.8.x sourcecode */

QCDDAParanoia::QCDDAParanoia(QObject *parent) : QObject(parent) {
  Q_UNUSED(parent);
  paranoia = NULL;
  paranoia_drive = NULL;
  setNeverSkip(true);
  setMaxRetries(20);
  setParanoiaMode(3);
}

QCDDAParanoia::~QCDDAParanoia() {
  _paranoia_free();
}

bool QCDDAParanoia::setDevice(const QString& device) {
  if ((device.isEmpty() && (_device.isEmpty()))) _device = "/dev/cdrom";
  if (!device.isEmpty()) _device = device;
  if (!_paranoia_init()) {
    //emit error(trUtf8("Internal device error."), trUtf8("Check your device. Is it really \"%1\"? If so also check your permissions on \"%1\".").arg(_device));
    return FALSE;
  }
  return TRUE;
}

QString QCDDAParanoia::device() const {
  return _device;
}

void QCDDAParanoia::setParanoiaMode(int mode) {

  mutex.lock();

  // from cdrdao 1.1.7
  paranoia_mode = PARANOIA_MODE_FULL^PARANOIA_MODE_NEVERSKIP;

  switch (mode) {
    case 0: paranoia_mode = PARANOIA_MODE_DISABLE; break;
    case 1: paranoia_mode |= PARANOIA_MODE_OVERLAP; paranoia_mode &= ~PARANOIA_MODE_VERIFY; break;
    case 2: paranoia_mode &= ~(PARANOIA_MODE_SCRATCH|PARANOIA_MODE_REPAIR); break;
  }

  if (paranoia_never_skip) paranoia_mode |= PARANOIA_MODE_NEVERSKIP;

  if (paranoia) paranoia_modeset(paranoia, paranoia_mode);

  mutex.unlock();

}

void QCDDAParanoia::setNeverSkip(bool b) {
  paranoia_never_skip = b;
  setParanoiaMode(paranoia_mode);
}

void QCDDAParanoia::setMaxRetries(int m) {
  paranoia_max_retries = m;
}

qint16* QCDDAParanoia::paranoiaRead(void (*callback)(long, int)) {
  if (paranoia) {
    mutex.lock();
    int16_t* data = paranoia_read_limited(paranoia, callback, paranoia_max_retries);
    mutex.unlock();
    return data;
  }
  return 0;
}

int QCDDAParanoia::paranoiaSeek(long sector, int mode) {
  if (paranoia) {
    mutex.lock();
    long pos = paranoia_seek(paranoia, sector, mode);
    mutex.unlock();
    return pos;
  }
  return -1;
}

int QCDDAParanoia::firstSectorOfTrack(int track) {
  if (paranoia_drive) {
    mutex.lock();
    long first_sector = cdda_track_firstsector(paranoia_drive, track);
    mutex.unlock();
    return first_sector;
  }
  return -1;
}

int QCDDAParanoia::lastSectorOfTrack(int track) {
  if (paranoia_drive) {
    mutex.lock();
    long last_sector = cdda_track_lastsector(paranoia_drive, track);
    mutex.unlock();
    return last_sector;
  }
  return -1;
}

int QCDDAParanoia::numOfTracks() {
  if (paranoia_drive) return (paranoia_drive->tracks<0)?0:paranoia_drive->tracks;
  return 0;
}

int QCDDAParanoia::numOfAudioTracks() {
  if (numOfTracks() > 0) {
    int j = 0;
    for (int i = 1; i <= numOfTracks(); i++) {
      if (isAudioTrack(i)) j++;
    }
    return j;
  }
  return 0;
}

int QCDDAParanoia::length() {
  return numOfFrames() / 75;
}

int QCDDAParanoia::numOfFrames() {
  if (numOfTracks() > 0) {
    if (paranoia_drive) return cdda_disc_lastsector(paranoia_drive);
  }
  return 0;
}

int QCDDAParanoia::lengthOfAudioTracks() {
  return numOfFramesOfAudioTracks() / 75;
}

int QCDDAParanoia::numOfFramesOfAudioTracks() {
  if (numOfTracks() > 0) {
    int frames = 0;
    for (int i = 1; i <= numOfTracks(); i++) {
      if (isAudioTrack(i)) frames += numOfFramesOfTrack(i);
    }
    return frames;
  }
  return 0;
}

int QCDDAParanoia::numOfSkippedFrames(int n) {
  if (numOfTracks() > 0) {
    if (n < 1) n = 1;
    if (n > numOfTracks()) n = numOfTracks();
    int frames = 0;
    for (int i = 1; i < n; i++) {
      if (!isAudioTrack(i)) frames += numOfFramesOfTrack(i);
    }
    return frames;
  }
  return 0;
}

int QCDDAParanoia::lengthOfTrack(int n) {
  if (numOfTracks() > 0) {
    return numOfFramesOfTrack(n) / 75;
  }
  return 0;
}

int QCDDAParanoia::numOfFramesOfTrack(int n) {

  if (numOfTracks() > 0) {

    if (n < 1) n = 1;
    if (n > numOfTracks()) n = numOfTracks();

    if (n == numOfTracks()) {
      return numOfFrames()-paranoia_drive->disc_toc[n-1].dwStartSector;
    } else {
      return paranoia_drive->disc_toc[n].dwStartSector-paranoia_drive->disc_toc[n-1].dwStartSector;
    }

  }
  return 0;

}

double QCDDAParanoia::sizeOfTrack(int n) {
  if (numOfTracks() > 0) {
    double frame_size = (double)(numOfFramesOfTrack(n));
    if (isAudioTrack(n)) {
      return (frame_size*2352.0f)/(1024.0f*1024.0f);
    } else {
      return (frame_size*2048.0f)/(1024.0f*1024.0f);
    }
  }
  return 0.0f;
}

int QCDDAParanoia::frameOffsetOfTrack(int n) {
  if (numOfTracks() > 0) {
    return paranoia_drive->disc_toc[n-1].dwStartSector;
  }
  return 0;
}

bool QCDDAParanoia::isAudioTrack(int n) {
  if (paranoia_drive) return IS_AUDIO(paranoia_drive, n-1);
  return TRUE;
}

quint32 QCDDAParanoia::discid() {
  quint32 cksum = 0;
  for (int i = 1; i <= numOfTracks(); i++) {
    cksum += checksum((frameOffsetOfTrack(i)+150) / 75);
  }
  return ((cksum % 0xff) << 24 | length() << 8 | numOfTracks());
}

void QCDDAParanoia::reset() {
  _paranoia_init();
}

bool QCDDAParanoia::_paranoia_init() {

  mutex.lock();

  _paranoia_free();

  paranoia_drive = cdda_identify(_device.toAscii().data(), 0, 0);
  if (paranoia_drive == 0) {
    mutex.unlock();
    emit log("(QCDDAParanoia) failed to find device.");
    return FALSE;
  }

  //cdda_cdda_verbose_set(_drive, 1, 1);

  cdda_open(paranoia_drive);
  paranoia = paranoia_init(paranoia_drive);
  if (paranoia == 0) {
    _paranoia_free();
    mutex.unlock();
    emit log("(QCDDAParanoia) failed to init device");
    return FALSE;
  }

  mutex.unlock();
  return TRUE;

}

void QCDDAParanoia::_paranoia_free() {
  //mutex.lock();
  if (paranoia) {
    paranoia_free(paranoia);
    paranoia = 0;
  }
  if (paranoia_drive) {
    cdda_close(paranoia_drive);
    paranoia_drive = 0;
  }
  //mutex.unlock();
}
