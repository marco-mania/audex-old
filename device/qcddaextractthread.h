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

#ifndef QCDDAEXTRACTTHREAD_HEADER
#define QCDDAEXTRACTTHREAD_HEADER

#include <QtCore>

#include "qcddadevice.h"

class QCDDAExtractThread : public QThread {
  Q_OBJECT
public:
  QCDDAExtractThread(QObject* parent = 0, QCDDADevice *device = 0);
  ~QCDDAExtractThread();

public slots:
  void start();
  void cancel();
  void setParanoiaMode(int mode) { paranoia_mode = mode; }
  void setMaxRetries(int r) { paranoia_retries = r; }
  void setNeverSkip(bool b) { never_skip = b; }
  void setTrackToRip(unsigned int t) { track = t; }
  void skipTrack(unsigned int t) { overall_sectors_read += device->numOfFramesOfTrack(t); }

  bool isProcessing();

  const QStringList& protocol();

private slots:
  void s_error(const QString& errorstr);

signals:
  void output(const QByteArray&);

  void progress(const int percent, const int sector, const int sectorsOverall);

  void log(const QString& message);
  void error(const QString& description,
	const QString& solution = QString());
  void warning(const QString& description);
  void info(const QString& description);

protected:
  /** reimplemented from QThread. Does the work */
  void run();

private:
  QCDDADevice *device;

  long first_sector;
  long current_sector;
  long last_sector;
  unsigned long sectors_read;
  unsigned long overall_sectors_read;
  unsigned long sectors_all;

  int paranoia_mode;
  int paranoia_retries;
  bool never_skip;

  unsigned int track;

  bool b_interrupt;
  bool b_error;

  QStringList extract_protocol;

  void createStatus(long, int);

  //status variables
  long last_read_sector;
  long overlap;
  long read_sectors;

  bool read_error;

  //this friend function will call createStatus(long, int)
  friend void paranoiaCallback(long, int);

};

#endif
