/* AUDEX CDDA EXTRACTOR
 * Copyright (C) 2007-2008 by Marco Nelles (marcomaniac@gmx.de)
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

#ifndef ENCODERWRAPPER_HEADER
#define ENCODERWRAPPER_HEADER

#include <QString>

#include <KDebug>
#include <KLocale>
#include <KProcess>

#include "maskparser.h"

class EncoderWrapper : public QObject {

  Q_OBJECT

public:
  EncoderWrapper(QObject* parent = 0, const QString& commandMask = "", const bool deleteFractionFiles = TRUE);
  ~EncoderWrapper();

  bool isProcessing();
  const QStringList& protocol();

public slots:
  bool encode(int n,
	int cdno, int trackoffset,
	const QString& artist, const QString& album,
	const QString& tartist, const QString& ttitle,
	const QString& genre, const QString& date, const QString& suffix, const QString& basepath,
	bool fat_compatible,
	const QString& input, const QString& output);
  void cancel();

private slots:
  void parseOutput();
  void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
  void processError(QProcess::ProcessError err);

signals:
  void progress(int percent_of_track);
  void finished();

  void error(const QString& message,
	const QString& details = QString());
  void warning(const QString& message);
  void info(const QString& message);

private:
  QString command_mask;
  bool delete_fraction_files;

  QString encoder;
  QStringList _protocol;

  QString processing_filename;

  bool termination;
  int processing;

  KProcess proc;

  int not_found_counter;

};

#endif
