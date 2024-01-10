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

#ifndef ENCODERASSISTANT_H
#define ENCODERASSISTANT_H

#include <QString>
#include <QStringList>

#include <KDebug>
#include <KLocale>
#include <KProcess>

#define OGGVORBIS	"Ogg Vorbis"
#define FLAC		"FLAC"
#define MP3		"MP3"
#define M4A		"M4A (AAC)"
#define WAVE		"RIFF WAVE"

#define OGGVORBIS_BIN	"oggenc"
#define FLAC_BIN	"flac"
#define MP3_BIN		"lame"
#define M4A_BIN		"faac"
#define WAVE_BIN	"mv"

#define OGGVORBIS_VER	"-v"
#define FLAC_VER	"-v"
#define MP3_VER		"--version"
#define M4A_VER		""
#define WAVE_VER	""

#define OGGVORBIS_SUFF	"ogg"
#define FLAC_SUFF	"flac"
#define MP3_SUFF	"mp3"
#define M4A_SUFF	"m4a"
#define WAVE_SUFF	"wav"

#define OGGVORBIS_CMD	"oggenc -q %1 -c \"Artist=$tartist\" -c \"Title=$ttitle\" -c \"Album=$title\" -c \"Date=$date\" -c \"Tracknumber=$trackno\" -c \"Genre=$genre\" -o $o $i"
#define FLAC_CMD	"flac -T=Artist=\"$tartist\" -T=Title=\"$ttitle\" -T=Album=\"$title\" -T=Date=\"$date\" -T=Tracknumber=\"$trackno\" -T=Genre=\"$genre\" -o $o $i"
#define MP3_CMD		"lame -V %1 --id3v2-only --ignore-tag-errors --tt \"$ttitle\" --ta \"$tartist\" --tl \"$title\" --ty \"$date\" --tn $trackno --tg \"$genre\" ${cover format=\"JPG\" x=\"300\" y=\"300\" preparam=\"--ti \"} $i $o"
#define M4A_CMD		"faac -q %1 -w --title \"$ttitle\" --artist \"$tartist\" --album \"$title\" --year \"$date\" --track $trackno --disc $cdno --genre \"$genre\" -o $o $i"
#define WAVE_CMD	"mv $i $o"

class EncoderAssistant : public QObject {

  Q_OBJECT

public:
  EncoderAssistant(QObject* parent = 0);
  ~EncoderAssistant();

  enum Quality {
    QMobile,
    QCompromise,
    QHQArchival
  };

  enum Encoder {
    EOggEnc,
    EFLAC,
    ELAME,
    EFAAC,
    EWAVE
  };

  const QStringList encoderList();
  const QStringList availableEncoderList();
  const QStringList availableEncoderListWithVersions();

  const QString suffix(const Encoder encoder = EOggEnc);
  EncoderAssistant::Encoder encoderByListText(const QString& text = OGGVORBIS);

  bool encoderAvailable(const Encoder encoder = EOggEnc);
  const QString encoderVersion(const Encoder encoder = EOggEnc);
  const QString encoderMask(const Encoder encoder = EOggEnc, const Quality quality = QCompromise);
  bool encoderSupportQuality(const Encoder encoder = EOggEnc);

signals:
  void error(const QString& message,
	const QString& details = QString());
  void warning(const QString& message);
  void info(const QString& message);

};

#endif
