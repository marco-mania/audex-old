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

#include "qencoderwrapper.h"

QEncoderWrapper::QEncoderWrapper(QObject* parent, QSettings *settings, QProfileModel *profile_model) : QObject(parent) {

  this->settings = settings;
  this->profile_model = profile_model;

  connect(&process, SIGNAL(readyReadStandardError()), this, SLOT(parseOutput()));
  connect(&process, SIGNAL(readyReadStandardOutput()), this, SLOT(parseOutput()));
  connect(&process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processFinished(int, QProcess::ExitStatus)));
  connect(&process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(processError(QProcess::ProcessError)));

  termination = FALSE;
  processing = 0;

}

QEncoderWrapper::~QEncoderWrapper() {

}

bool QEncoderWrapper::encode(int n,
	int cdno, int trackoffset,
	const QString& artist, const QString& album,
	const QString& tartist, const QString& ttitle,
	const QString& genre, const QString& date, const QString& ext, const QString& basepath,
	bool fat_compatible,
	const QString& input, const QString& output) {

  if (!processing) processing = 1; else return FALSE;
  termination = FALSE;

  QString command_mask = profile_model->data(profile_model->index(profile_model->currentProfileRow(), 4)).toString();
  if (command_mask.isEmpty()) { emit error(trUtf8("Command mask is empty.")); return FALSE; }
  int firstSpace = command_mask.indexOf(' ');
  encoder = command_mask.left(firstSpace);

  QMaskParser maskparser;
  QString command = maskparser.parseCommandMask(command_mask, input, output, n, cdno, trackoffset, artist, album, tartist, ttitle, date, genre, ext, basepath, fat_compatible);

  emit log(QString("(QEncoderWrapper) executing command \"%1\"").arg(command));
  process.start(command);

  processing_filename = output;

  emit info(trUtf8("Encoding track %1...").arg(n));

  return TRUE;

}

void QEncoderWrapper::cancel() {

  if (!processing) return;

  //we need to suppress normal error messages, because with a cancel the user known what he does
  termination = TRUE;
  process.terminate();

  bool delete_part_files = TRUE;
  if (settings) delete_part_files = settings->value("settings/delete_part_files", TRUE).toBool();

  if (delete_part_files) {
    QFile file(processing_filename);
    if (file.exists()) {
      file.remove();
      emit warning(trUtf8("Deleted partial file \"%1\".").arg(processing_filename.mid(processing_filename.lastIndexOf("/")+1)));
      emit log(QString("(QEncoderWrapper) deleted partial file \"%1\"").arg(processing_filename));
    }
  }

  emit error(trUtf8("User canceled encoding."));
  emit log("(QEncoderWrapper) Interrupt encoding.");

}

bool QEncoderWrapper::isProcessing() {
  return (processing>0);
}

const QStringList& QEncoderWrapper::protocol() {
  return _protocol;
}

void QEncoderWrapper::parseOutput() {

  QByteArray rawoutput = process.readAllStandardError();
  if (rawoutput.count() == 0) rawoutput = process.readAllStandardOutput();
  if (rawoutput.count() > 0) {
    QString output(rawoutput); QStringList list = output.trimmed().split("\n");
    _protocol << list;
    bool found = FALSE;
    for (int i=0; i<list.count(); i++) {
      if (list.at(i).contains('%')) {
        QString line = list.at(i);
        int startPos = line.indexOf(QRegExp("\\d+[,]?\\d*\\%"));
        if (startPos == -1) continue;
        QString p = line.mid(startPos);
        p = p.left(p.indexOf('%'));
        bool conversionSuccessful = FALSE;
        double percent = p.toDouble(&conversionSuccessful);
        if ((conversionSuccessful) && (percent >= 0) && (percent <= 100)) {
          emit progress((int)percent);
          found = TRUE;
        }
      }
    }
    if (!found) emit busy();
  }

}

void QEncoderWrapper::processFinished(int exitCode, QProcess::ExitStatus exitStatus) {
  processing = 0;
  if (termination) { emit finished(); return; }
  if ((exitStatus == QProcess::NormalExit) && (exitCode==0)) {
    emit info(trUtf8("Encoding OK (\"%1\").").arg(processing_filename));
  } else {
    emit error(trUtf8("An error occured while encoding file \"%1\".").arg(processing_filename),
	trUtf8("Please check your profile."));
  }
  emit finished();
  emit log("(QEncoderWrapper) encoding finished.");
}

void QEncoderWrapper::processError(QProcess::ProcessError err) {
  if (termination) return;
  switch (err) {
    case QProcess::FailedToStart :
      emit error(trUtf8("%1 failed to start.").arg(encoder), trUtf8("Either it is missing, or you may have insufficient permissions to invoke the program.")); break;
    case QProcess::Crashed :
      emit error(trUtf8("%1 crashed some time after starting successfully.").arg(encoder), trUtf8("Please check your profile.")); break;
    case QProcess::Timedout :
      emit error(trUtf8("%1 timed out. This should not happen.").arg(encoder), trUtf8("Please check your profile.")); break;
    case QProcess::WriteError :
      emit error(trUtf8("An error occurred when attempting to write to %1.").arg(encoder), trUtf8("For example, the process may not be running, or it may have closed its input channel.")); break;
    case QProcess::ReadError :
      emit error(trUtf8("An error occurred when attempting to read from %1.").arg(encoder), trUtf8("For example, the process may not be running.")); break;
    case QProcess::UnknownError :
      emit error(trUtf8("An unknown error occurred to %1. This should not happen.").arg(encoder), trUtf8("Please check your profile.")); break;
  }
  emit finished();
  emit log("(QEncoderWrapper) encoding finished.");
}
