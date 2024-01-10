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

#include "qcddaextractthread.h"

static QCDDAExtractThread* aet = 0;

void paranoiaCallback(long sector, int status)
{
  aet->createStatus(sector, status);
}

QCDDAExtractThread::QCDDAExtractThread(QObject* parent, QCDDADevice *device) : QThread(parent) {
  connect(device, SIGNAL(error(const QString&)), this, SLOT(s_error(const QString&)));
  this->device = device;
  overall_sectors_read = 0;
  paranoia_mode = 3;
  paranoia_retries = 20;
  never_skip = TRUE;
  track = 1;
  b_interrupt = FALSE;
  b_error = FALSE;
  read_error = FALSE;
}

QCDDAExtractThread::~QCDDAExtractThread() {
}

void QCDDAExtractThread::start() {
  QThread::start();
}

void QCDDAExtractThread::run() {

  if (!device) return;

  if (b_interrupt) return;

  b_interrupt = b_error = FALSE;
  first_sector = device->firstSectorOfTrack(track);
  last_sector = device->lastSectorOfTrack(track);

  if (first_sector < 0 || last_sector < 0) {
    emit info(trUtf8("Extracting finished."));
    return;
  }

  emit log("(QCDDAExtractThread) sectors to read: " + QString("%1").arg(last_sector-first_sector));

  //status variable
  last_read_sector = 0;
  overlap = 0;
  read_sectors = 0;

  //track length
  sectors_all = last_sector-first_sector;
  sectors_read = 0;

  device->setParanoiaMode(paranoia_mode);
  device->setNeverSkip(never_skip);
  device->setMaxRetries(paranoia_retries);

  device->paranoiaSeek(first_sector, SEEK_SET);
  current_sector = first_sector;

  emit info(trUtf8("Extracting track %1 (%2:%3)...").arg(track).arg((sectors_all / 75) / 60, 2, 10, QChar('0')).arg((sectors_all / 75) % 60, 2, 10, QChar('0')));
  extract_protocol.append(trUtf8("Start reading track %1 with %2 sectors").arg(track).arg(sectors_all));

  while (current_sector <= last_sector) {

    if (b_interrupt) {
      emit log("(QCDDAExtractThread) Interrupt reading.");
      break;
    }

    //let the global paranoia callback have access to this
    //to emit signals
    aet = this;

    int16_t* buf = device->paranoiaRead(paranoiaCallback);

    if (0 == buf) {

      emit log(QString("(QCDDAExtractThread) Unrecoverable error in paranoia_read (sector %1)").arg(current_sector));
      b_error = TRUE;
      break;

    } else {

      current_sector++;
      QByteArray a((char*)buf, CD_FRAMESIZE_RAW);
      emit output(a);
      a.clear();

      sectors_read++;
      overall_sectors_read++;
      emit progress((int)(100*((float)sectors_read / (float)sectors_all)), current_sector, overall_sectors_read);

    }

  }

  if (b_interrupt)
    emit error(trUtf8("User canceled extracting."));

  if (b_error)
    emit error(trUtf8("An error occured while extracting track %1.").arg(track));

  if ((!b_interrupt) && (!b_error)) emit info(trUtf8("Extracting OK (Track %1).").arg(track));

  emit log("(QCDDAExtractThread) reading finished.");
  extract_protocol.append(trUtf8("Reading finished"));

}

void QCDDAExtractThread::cancel() {
  b_interrupt = TRUE;
}

bool QCDDAExtractThread::isProcessing() {
  return !(b_interrupt || !isRunning());
}

const QStringList& QCDDAExtractThread::protocol() {
  return extract_protocol;
}

void QCDDAExtractThread::s_error(const QString& errorstr) {
  emit error(errorstr);
}

void QCDDAExtractThread::createStatus(long sector, int status) {

  sector /= CD_FRAMESIZE_RAW/2;

  switch (status) {
  case -1:
    break;
  case -2:
    break;
  case PARANOIA_CB_READ:
    //no problem
    //does only this mean that the sector has been read?
    last_read_sector = sector;  //this seems to be rather useless
    read_sectors++;
    read_error = FALSE;
    break;
  case PARANOIA_CB_VERIFY:
    //qDebug() << "(QCDDAExtractThread) verify.";
    break;
  case PARANOIA_CB_FIXUP_EDGE:
    emit log("(QCDDAExtractThread) fixup edge.");
    extract_protocol.append(trUtf8("fixup edge (absolute sector %1, relative sector %2, track time pos %3:%4)").arg(sector).arg(current_sector).arg((current_sector/75)/60, 2, 10, QChar('0')).arg((current_sector/75)%60, 2, 10, QChar('0')));
    break;
  case PARANOIA_CB_FIXUP_ATOM:
    emit log("(QCDDAExtractThread) fixup atom.");
    extract_protocol.append(trUtf8("fixup atom (absolute sector %1, relative sector %2, track time pos %3:%4)").arg(sector).arg(current_sector).arg((current_sector/75)/60, 2, 10, QChar('0')).arg((current_sector/75)%60, 2, 10, QChar('0')));
    break;
  case PARANOIA_CB_SCRATCH:
    //scratch detected
    emit log("(QCDDAExtractThread) scratch detected.");
    extract_protocol.append(trUtf8("scratch detected (absolute sector %1, relative sector %2, track time pos %3:%4)").arg(sector).arg(current_sector).arg((current_sector/75)/60, 2, 10, QChar('0')).arg((current_sector/75)%60, 2, 10, QChar('0')));
    break;
  case PARANOIA_CB_REPAIR:
    emit log("(QCDDAExtractThread) repair.");
    extract_protocol.append(trUtf8("repair (absolute sector %1, relative sector %2, track time pos %3:%4)").arg(sector).arg(current_sector).arg((current_sector/75)/60, 2, 10, QChar('0')).arg((current_sector/75)%60, 2, 10, QChar('0')));
    break;
  case PARANOIA_CB_SKIP:
    //skipped sector
    emit log("(QCDDAExtractThread) skip.");
    extract_protocol.append(trUtf8("skip (absolute sector %1, relative sector %2, track time pos %3:%4)").arg(sector).arg(current_sector).arg((current_sector/75)/60, 2, 10, QChar('0')).arg((current_sector/75)%60, 2, 10, QChar('0')));
    break;
  case PARANOIA_CB_DRIFT:
    emit log("(QCDDAExtractThread) drift.");
    extract_protocol.append(trUtf8("drift (absolute sector %1, relative sector %2, track time pos %3:%4)").arg(sector).arg(current_sector).arg((current_sector/75)/60, 2, 10, QChar('0')).arg((current_sector/75)%60, 2, 10, QChar('0')));
    break;
  case PARANOIA_CB_BACKOFF:
    emit log("(QCDDAExtractThread) backoff.");
    extract_protocol.append(trUtf8("backoff (absolute sector %1, relative sector %2, track time pos %3:%4)").arg(sector).arg(current_sector).arg((current_sector/75)/60, 2, 10, QChar('0')).arg((current_sector/75)%60, 2, 10, QChar('0')));
    break;
  case PARANOIA_CB_OVERLAP:
    //sector does not seem to contain the current
    //sector but the amount of overlapped data
    //qDebug() << "(QCDDAExtractThread) overlap.";
    overlap = sector;
    break;
  case PARANOIA_CB_FIXUP_DROPPED:
    emit log("(QCDDAExtractThread) fixup dropped.");
    extract_protocol.append(trUtf8("fixup dropped (absolute sector %1, relative sector %2, track time pos %3:%4)").arg(sector).arg(current_sector).arg((last_sector-first_sector/75)/60, 2, 10, QChar('0')).arg((last_sector-first_sector/75)%60, 2, 10, QChar('0')));
    break;
  case PARANOIA_CB_FIXUP_DUPED:
    emit log("(QCDDAExtractThread) fixup duped.");
    extract_protocol.append(trUtf8("fixup duped (absolute sector %1, relative sector %2, track time pos %3:%4)").arg(sector).arg(current_sector).arg((last_sector-first_sector/75)/60, 2, 10, QChar('0')).arg((last_sector-first_sector/75)%60, 2, 10, QChar('0')));
    break;
  case PARANOIA_CB_READERR:
    emit log("(QCDDAExtractThread) readerr.");
    if (!read_error) { read_error = TRUE; warning(trUtf8("Read error detected (absolute sector %1, relative sector %2, track time pos %3:%4).").arg(sector).arg(current_sector).arg((last_sector-first_sector/75)/60, 2, 10, QChar('0')).arg((last_sector-first_sector/75)%60, 2, 10, QChar('0'))); }
    extract_protocol.append(trUtf8("READ ERROR (absolute sector %1, relative sector %2, track time pos %3:%4)").arg(sector).arg(current_sector).arg((last_sector-first_sector/75)/60, 2, 10, QChar('0')).arg((last_sector-first_sector/75)%60, 2, 10, QChar('0')));
    break;
  }

}
