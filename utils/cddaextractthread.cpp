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

#include "cddaextractthread.h"

static CDDAExtractThread* aet = 0;

void paranoiaCallback(long sector, int status) {
  aet->createStatus(sector, status);
}

CDDAExtractThread::CDDAExtractThread(QObject* parent, const QString& device) : QThread(parent) {

  paranoia = new CDDAParanoia(this);
  if (!paranoia) {
    kDebug() << "Unable to create paranoia class. low mem?";
    emit error(i18n("Internal device error."), i18n("Check your device and make a bug report."));
    return;
  }
  connect(paranoia, SIGNAL(error(const QString&, const QString&)), this, SLOT(slot_error(const QString&, const QString&)));
  paranoia->setDevice(device);

  overall_sectors_read = 0;
  paranoia_mode = 3;
  paranoia_retries = 20;
  never_skip = TRUE;
  track = 1;
  b_interrupt = FALSE;
  b_error = FALSE;
  read_error = FALSE;

}

CDDAExtractThread::~CDDAExtractThread() {

  delete paranoia;

}

void CDDAExtractThread::start() {
  QThread::start();
}

void CDDAExtractThread::run() {

  if (!paranoia) return;

  if (b_interrupt) return;

  b_interrupt = b_error = FALSE;
  first_sector = paranoia->firstSectorOfTrack(track);
  last_sector = paranoia->lastSectorOfTrack(track);

  if (first_sector < 0 || last_sector < 0) {
    emit info(i18n("Extracting finished."));
    return;
  }

  kDebug() << "Sectors to read: " << QString("%1").arg(last_sector-first_sector);

  //status variable
  last_read_sector = 0;
  overlap = 0;
  read_sectors = 0;

  //track length
  sectors_all = last_sector-first_sector;
  sectors_read = 0;

  paranoia->setParanoiaMode(paranoia_mode);
  paranoia->setNeverSkip(never_skip);
  paranoia->setMaxRetries(paranoia_retries);

  paranoia->paranoiaSeek(first_sector, SEEK_SET);
  current_sector = first_sector;

  {
    QString min = QString("%1").arg((sectors_all / 75) / 60, 2, 10, QChar('0'));
    QString sec = QString("%1").arg((sectors_all / 75) % 60, 2, 10, QChar('0'));
    emit info(i18n("Ripping track %1 (%2:%3)...", track, min, sec));
  }
  extract_protocol.append(i18n("Start reading track %1 with %2 sectors", track, sectors_all));

  while (current_sector <= last_sector) {

    if (b_interrupt) {
      kDebug() << "Interrupt reading.";
      break;
    }

    //let the global paranoia callback have access to this
    //to emit signals
    aet = this;

    int16_t* buf = paranoia->paranoiaRead(paranoiaCallback);

    if (0 == buf) {

      kDebug() << "Unrecoverable error in paranoia_read (sector " << current_sector << ")";
      b_error = TRUE;
      break;

    } else {

      current_sector++;
      QByteArray a((char*)buf, CD_FRAMESIZE_RAW);
      emit output(a);
      a.clear();

      sectors_read++;
      overall_sectors_read++;
      float fraction = 0.0f;
      if (sectors_all>0) fraction = (float)sectors_read / (float)sectors_all;
      emit progress((int)(100.0f*fraction), current_sector, overall_sectors_read);

    }

  }

  if (b_interrupt)
    emit error(i18n("User canceled extracting."));

  if (b_error)
    emit error(i18n("An error occured while ripping track %1.", track));

  if ((!b_interrupt) && (!b_error)) emit info(i18n("Ripping OK (Track %1).", track));

  kDebug () << "Reading finished.";
  extract_protocol.append(i18n("Reading finished"));

}

void CDDAExtractThread::cancel() {
  b_interrupt = TRUE;
}

bool CDDAExtractThread::isProcessing() {
  return !(b_interrupt || !isRunning());
}

const QStringList& CDDAExtractThread::protocol() {
  return extract_protocol;
}

void CDDAExtractThread::slot_error(const QString& message, const QString& details) {
  emit error(message, details);
}

void CDDAExtractThread::createStatus(long sector, int status) {

  sector /= CD_FRAMESIZE_RAW/2;
  QString tp_min = QString("%1").arg((current_sector/75)/60, 2, 10, QChar('0'));
  QString tp_sec = QString("%1").arg((current_sector/75)%60, 2, 10, QChar('0'));

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
    //kDebug() << "verify.";
    break;
  case PARANOIA_CB_FIXUP_EDGE:
    kDebug() << "fixup edge.";
    extract_protocol.append(i18n("fixup edge (absolute sector %1, relative sector %2, track time pos %3:%4)", sector, current_sector, tp_min, tp_sec));
    break;
  case PARANOIA_CB_FIXUP_ATOM:
    kDebug() << "fixup atom.";
    extract_protocol.append(i18n("fixup atom (absolute sector %1, relative sector %2, track time pos %3:%4)", sector, current_sector, tp_min, tp_sec));
    break;
  case PARANOIA_CB_SCRATCH:
    //scratch detected
    kDebug() << "scratch detected.";
    extract_protocol.append(i18n("scratch detected (absolute sector %1, relative sector %2, track time pos %3:%4)", sector, current_sector, tp_min, tp_sec));
    break;
  case PARANOIA_CB_REPAIR:
    kDebug() << "repair.";
    extract_protocol.append(i18n("repair (absolute sector %1, relative sector %2, track time pos %3:%4)", sector, current_sector, tp_min, tp_sec));
    break;
  case PARANOIA_CB_SKIP:
    //skipped sector
    kDebug() << "skip.";
    extract_protocol.append(i18n("skip (absolute sector %1, relative sector %2, track time pos %3:%4)", sector, current_sector, tp_min, tp_sec));
    break;
  case PARANOIA_CB_DRIFT:
    kDebug() << "drift.";
    extract_protocol.append(i18n("drift (absolute sector %1, relative sector %2, track time pos %3:%4)", sector, current_sector, tp_min, tp_sec));
    break;
  case PARANOIA_CB_BACKOFF:
    kDebug() << "backoff.";
    extract_protocol.append(i18n("backoff (absolute sector %1, relative sector %2, track time pos %3:%4)", sector, current_sector, tp_min, tp_sec));
    break;
  case PARANOIA_CB_OVERLAP:
    //sector does not seem to contain the current
    //sector but the amount of overlapped data
    //kDebug() << "overlap.";
    overlap = sector;
    break;
  case PARANOIA_CB_FIXUP_DROPPED:
    kDebug() << "fixup dropped.";
    extract_protocol.append(i18n("fixup dropped (absolute sector %1, relative sector %2, track time pos %3:%4)", sector, current_sector, tp_min, tp_sec));
    break;
  case PARANOIA_CB_FIXUP_DUPED:
    kDebug() << "fixup duped.";
    extract_protocol.append(i18n("fixup duped (absolute sector %1, relative sector %2, track time pos %3:%4)", sector, current_sector, tp_min, tp_sec));
    break;
  case PARANOIA_CB_READERR:
    kDebug() << "readerr.";
    if (!read_error) { read_error = TRUE; warning(i18n("Read error detected (absolute sector %1, relative sector %2, track time pos %3:%4)", sector, current_sector, tp_min, tp_sec)); }
    extract_protocol.append(i18n("READ ERROR (absolute sector %1, relative sector %2, track time pos %3:%4)", sector, current_sector, tp_min, tp_sec));
    break;
  }

}
