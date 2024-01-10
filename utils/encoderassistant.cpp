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

#include "encoderassistant.h"

EncoderAssistant::EncoderAssistant(QObject* parent) : QObject(parent) {

}

EncoderAssistant::~EncoderAssistant() {

}

const QStringList EncoderAssistant::encoderList() {
  return QStringList() << OGGVORBIS << FLAC << MP3 << M4A << WAVE;
}

const QStringList EncoderAssistant::availableEncoderList() {

  QStringList list;

  if (encoderAvailable(EOggEnc)) list << OGGVORBIS;
  if (encoderAvailable(EFLAC)) list << FLAC;
  if (encoderAvailable(ELAME)) list << MP3;
  if (encoderAvailable(EFAAC)) list << M4A;
  if (encoderAvailable(EWAVE)) list << WAVE;

  return list;

}

const QStringList EncoderAssistant::availableEncoderListWithVersions() {

  QStringList list;

  QString item;
  if (encoderAvailable(EOggEnc)) {
    item = OGGVORBIS;
    QString version = encoderVersion(EOggEnc);
    if (!version.isEmpty()) {
      item += " (Version "+version+")";
    }
    list << item;
  }

  if (encoderAvailable(EFLAC)) {
    item = FLAC;
    QString version = encoderVersion(EFLAC);
    if (!version.isEmpty()) {
      item += " (Version "+version+")";
    }
    list << item;
  }

  if (encoderAvailable(ELAME)) {
    item = MP3;
    QString version = encoderVersion(ELAME);
    if (!version.isEmpty()) {
      item += " (Version "+version+")";
    }
    list << item;
  }

  if (encoderAvailable(EFAAC)) {
    item = M4A;
    QString version = encoderVersion(EFAAC);
    if (!version.isEmpty()) {
      item += " (Version "+version+")";
    }
    list << item;
  }

  if (encoderAvailable(EWAVE)) {
    item = WAVE;
    QString version = encoderVersion(EWAVE);
    if (!version.isEmpty()) {
      item += " (Version "+version+")";
    }
    list << item;
  }

  return list;

}

const QString EncoderAssistant::suffix(const EncoderAssistant::Encoder encoder) {

  switch (encoder) {
    case EOggEnc : return OGGVORBIS_SUFF;
    case EFLAC : return FLAC_SUFF;
    case ELAME : return MP3_SUFF;
    case EFAAC : return M4A_SUFF;
    case EWAVE : return WAVE_SUFF;
  }

  return "";

}

EncoderAssistant::Encoder EncoderAssistant::encoderByListText(const QString& text) {

  if (text.startsWith(OGGVORBIS)) return EOggEnc;
  if (text.startsWith(FLAC)) return EFLAC;
  if (text.startsWith(MP3)) return ELAME;
  if (text.startsWith(M4A)) return EFAAC;
  if (text.startsWith(WAVE)) return EWAVE;

  return EWAVE;

}

bool EncoderAssistant::encoderAvailable(const EncoderAssistant::Encoder encoder) {

  switch (encoder) {
    case EOggEnc : if (KProcess::execute(OGGVORBIS_BIN, QStringList() << OGGVORBIS_VER)==0) return TRUE; else return FALSE;
    case EFLAC : if (KProcess::execute(FLAC_BIN, QStringList() << FLAC_VER)==0) return TRUE; else return FALSE;
    case ELAME : if (KProcess::execute(MP3_BIN, QStringList() << MP3_VER)==0) return TRUE; else return FALSE;
    case EFAAC : if (KProcess::execute(M4A_BIN, QStringList() << M4A_VER)<=1) return TRUE; else return FALSE;
    case EWAVE : return TRUE;
  }

  return FALSE;

}

const QString EncoderAssistant::encoderVersion(const EncoderAssistant::Encoder encoder) {

  KProcess process;
  process.setOutputChannelMode(KProcess::SeparateChannels);
  process.setReadChannel(KProcess::StandardError);

  switch (encoder) {
    case EOggEnc : process.setShellCommand(QString(OGGVORBIS_BIN)+" "+QString(OGGVORBIS_VER)); break;
    case EFLAC : process.setShellCommand(QString(FLAC_BIN)+" "+QString(FLAC_VER)); break;
    case ELAME : process.setShellCommand(QString(MP3_BIN)+" "+QString(MP3_VER)); break;
    case EFAAC : process.setShellCommand(QString(M4A_BIN)+" "+QString(M4A_VER)); break;
    case EWAVE : return "";
  }
  process.start();
  if (!process.waitForFinished()) return "";
  QByteArray rawoutput = process.readAllStandardError();
  if (rawoutput.size() == 0) rawoutput = process.readAllStandardOutput();
  QString output(rawoutput);
  QStringList list = output.trimmed().split("\n");
  if (list.count()==0) return "";
  QStringList words = list[0].split(" ");
  if (words.count()==0) return "";

  switch (encoder) {
    case EOggEnc :
    case EFLAC :
      if (words.count()==0) return "";
      return words.last();
    case ELAME :
      if ((words.contains("version")) && (words.indexOf("version")+1<words.count())) return words[words.indexOf("version")+1];
      if (words.count()<2) return "";
      return words[words.count()-2];
    case EFAAC :
      if (list.count()<2) return "";
      words = list[1].split(" ");
      if (words.count()<2) return "";
      if ((words.contains("FAAC")) && (words.indexOf("FAAC")+1<words.count())) return words[words.indexOf("FAAC")+1];
      return words[1];
    case EWAVE : break;
  }

  return "";

}

const QString EncoderAssistant::encoderMask(const EncoderAssistant::Encoder encoder, const EncoderAssistant::Quality quality) {

  int q = 0;
  switch (encoder) {
    case EOggEnc :
      switch (quality) {
	case QMobile : q = 2; break;
	case QCompromise : q = 4; break;
	case QHQArchival : q = 6; break;
      }
      return QString(OGGVORBIS_CMD).arg(q);
    case EFLAC :
      return FLAC_CMD;
    case ELAME :
      switch (quality) {
	case QMobile : q = 6; break;
	case QCompromise : q = 4; break;
	case QHQArchival : q = 2; break;
      }
      return QString(MP3_CMD).arg(q);
    case EFAAC :
      switch (quality) {
	case QMobile : q = 75; break;
	case QCompromise : q = 100; break;
	case QHQArchival : q = 200; break;
      }
      return QString(M4A_CMD).arg(q);
    case EWAVE :
      return WAVE_CMD;
  }

  return "";

}

bool EncoderAssistant::encoderSupportQuality(const Encoder encoder) {

  switch (encoder) {
    case EOggEnc : return TRUE;
    case EFLAC : return FALSE;
    case ELAME : return TRUE;
    case EFAAC : return TRUE;
    case EWAVE : return FALSE;
  }

  return FALSE;

}
