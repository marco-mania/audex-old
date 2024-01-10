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

#ifndef QENCODERWRAPPER_HEADER
#define QENCODERWRAPPER_HEADER

#include <QtCore>

#include "../base/qprofilemodel.h"
#include "qmaskparser.h"

class QEncoderWrapper : public QObject {

  Q_OBJECT

public:
  QEncoderWrapper(QObject* parent, QSettings *settings, QProfileModel *profile_model);
  ~QEncoderWrapper();

public slots:
  bool encode(int n,
	int cdno, int trackoffset,
	const QString& artist, const QString& album,
	const QString& tartist, const QString& ttitle,
	const QString& genre, const QString& date, const QString& ext, const QString& basepath,
	bool fat_compatible,
	const QString& input, const QString& output);
  void cancel();

  bool isProcessing();
  const QStringList& protocol();

private slots:
  void parseOutput();
  void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
  void processError(QProcess::ProcessError err);

signals:
  void progress(int percent_of_track);
  void busy(); //available if no progress indicator found
  void finished();

  void log(const QString& message);
  void error(const QString& description,
	const QString& solution = QString());
  void warning(const QString& description);
  void info(const QString& description);

private:
  QSettings *settings;
  QProfileModel *profile_model;

  QString encoder;
  QStringList _protocol;

  QString processing_filename;

  bool termination;
  int processing;

  QProcess process;

};

#endif
