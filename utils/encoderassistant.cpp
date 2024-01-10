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

const QString EncoderAssistant::name(const EncoderAssistant::Encoder encoder) {

  switch (encoder) {

    case EncoderAssistant::LAME : return ENCODER_LAME_NAME;
    case EncoderAssistant::OGGENC : return ENCODER_OGGENC_NAME;
    case EncoderAssistant::FLAC : return ENCODER_FLAC_NAME;
    case EncoderAssistant::FAAC : return ENCODER_FAAC_NAME;
    case EncoderAssistant::WAVE : return ENCODER_WAVE_NAME;
    case EncoderAssistant::CUSTOM : return ENCODER_CUSTOM_NAME;
    default : return "";

  }

  return "";

}

bool EncoderAssistant::available(const EncoderAssistant::Encoder encoder) {

  switch (encoder) {

    case EncoderAssistant::LAME : return (KProcess::execute(ENCODER_LAME_BIN, QStringList() << ENCODER_LAME_VERSION_PARA)==0);
    case EncoderAssistant::OGGENC : return (KProcess::execute(ENCODER_OGGENC_BIN, QStringList() << ENCODER_OGGENC_VERSION_PARA)==0);
    case EncoderAssistant::FLAC : return (KProcess::execute(ENCODER_FLAC_BIN, QStringList() << ENCODER_FLAC_VERSION_PARA)==0);
    case EncoderAssistant::FAAC : return (KProcess::execute(ENCODER_FAAC_BIN, QStringList() << ENCODER_FAAC_VERSION_PARA)==1);
    case EncoderAssistant::WAVE : return (KProcess::execute(ENCODER_WAVE_BIN, QStringList() << ENCODER_WAVE_VERSION_PARA)==0);
    case EncoderAssistant::CUSTOM : return TRUE;
    default : return FALSE;

  }

  return FALSE;

}

const QString EncoderAssistant::version(const EncoderAssistant::Encoder encoder) {

  KProcess process;
  process.setOutputChannelMode(KProcess::SeparateChannels);
  process.setReadChannel(KProcess::StandardError);
  switch (encoder) {
    case EncoderAssistant::LAME : process.setShellCommand(QString(ENCODER_LAME_BIN)+" "+QString(ENCODER_LAME_VERSION_PARA)); break;
    case EncoderAssistant::OGGENC : process.setShellCommand(QString(ENCODER_OGGENC_BIN)+" "+QString(ENCODER_OGGENC_VERSION_PARA)); break;
    case EncoderAssistant::FLAC : process.setShellCommand(QString(ENCODER_FLAC_BIN)+" "+QString(ENCODER_FLAC_VERSION_PARA)); break;
    case EncoderAssistant::FAAC : process.setShellCommand(QString(ENCODER_FAAC_BIN)+" "+QString(ENCODER_FAAC_VERSION_PARA)); break;
    case EncoderAssistant::WAVE : return "";
    case EncoderAssistant::CUSTOM : return "";
    default : return "";
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

    case EncoderAssistant::LAME :
      if ((words.contains("version")) && (words.indexOf("version")+1<words.count())) return words[words.indexOf("version")+1];
      if (words.count()<2) return "";
      return words[words.count()-2];

    case EncoderAssistant::OGGENC :

    case EncoderAssistant::FLAC :
      return words.last();

    case EncoderAssistant::FAAC :
      if (list.count()<2) return "";
      words = list[1].split(" ");
      if (words.count()<2) return "";
      if ((words.contains("FAAC")) && (words.indexOf("FAAC")+1<words.count())) return words[words.indexOf("FAAC")+1];
      return words[1];

    case EncoderAssistant::WAVE :

    case EncoderAssistant::CUSTOM :

    default : ;

  }

  return "";

}

const QString EncoderAssistant::pattern(const EncoderAssistant::Encoder encoder, const Parameters& parameters) {

  switch (encoder) {

    case EncoderAssistant::LAME : {
      int preset = parameters.valueToInt(ENCODER_LAME_PRESET_KEY, ENCODER_LAME_PRESET);
      bool cbr = parameters.valueToBool(ENCODER_LAME_CBR_KEY);
      int bitrate = parameters.valueToInt(ENCODER_LAME_BITRATE_KEY, ENCODER_LAME_BITRATE);
      bool embed_cover = parameters.valueToBool(ENCODER_LAME_EMBED_COVER_KEY);
      QString cmd = ENCODER_LAME_BIN;
      switch (preset) {
        case 0 : cmd += " --preset medium"; break;
        case 1 : cmd += " --preset standard"; break;
        case 2 : cmd += " --preset extreme"; break;
        case 3 : cmd += " --preset insane"; break;
        case 4 : cmd += QString(" --preset")+(cbr?QString(" cbr"):QString())+QString(" %1").arg(bitrate); break;
        default : cmd += " --preset standard";
      }
      QString v = EncoderAssistant::version(EncoderAssistant::LAME);
      if ((v.startsWith("3.95")) || (v.startsWith("3.96")) || (v.startsWith("3.97"))) {
        cmd += QString(" --vbr-new");
      }
      if (embed_cover) {
        cmd += QString::fromUtf8(" ${"VAR_COVER_FILE" preparam=\"--ti \"}");
      }
      cmd += QString::fromUtf8(" --id3v2-only --ignore-tag-errors --tt \"$"VAR_TRACK_TITLE"\" --ta \"$"VAR_TRACK_ARTIST\
                           "\" --tl \"$"VAR_ALBUM_TITLE"\" --ty \"$"VAR_DATE"\" --tn $"VAR_TRACK_NO" --tg \"$"VAR_GENRE \
                           "\" $"VAR_INPUT_FILE" $"VAR_OUTPUT_FILE);
      return cmd;
    }

    case EncoderAssistant::OGGENC : {
      int quality = parameters.valueToInt(ENCODER_OGGENC_QUALITY_KEY, ENCODER_OGGENC_QUALITY);
      bool min_bitrate = parameters.valueToBool(ENCODER_OGGENC_MINBITRATE_KEY);
      int min_bitrate_value = parameters.valueToInt(ENCODER_OGGENC_MINBITRATE_VALUE_KEY, ENCODER_OGGENC_MINBITRATE_VALUE);
      bool max_bitrate = parameters.valueToBool(ENCODER_OGGENC_MAXBITRATE_KEY);
      int max_bitrate_value = parameters.valueToInt(ENCODER_OGGENC_MAXBITRATE_VALUE_KEY, ENCODER_OGGENC_MAXBITRATE_VALUE);
      QString cmd = ENCODER_OGGENC_BIN;
      cmd += QString(" -q %1").arg(quality);

      if (min_bitrate) {
        cmd += QString(" -m %1").arg(min_bitrate_value);
      }

      if (max_bitrate) {
        cmd += QString(" -M %1").arg(max_bitrate_value);
      }

      cmd += QString::fromUtf8(" -c \"Artist=$"VAR_TRACK_ARTIST"\" -c \"Title=$"VAR_TRACK_TITLE"\" -c \"Album=$"VAR_ALBUM_TITLE\
                               "\" -c \"Date=$"VAR_DATE"\" -c \"Tracknumber=$"VAR_TRACK_NO"\" -c \"Genre=$"VAR_GENRE\
                               "\" -o $"VAR_OUTPUT_FILE" $"VAR_INPUT_FILE);

      return cmd;
    }

    case EncoderAssistant::FLAC : {
      int compression = parameters.valueToInt(ENCODER_FLAC_COMPRESSION_KEY, ENCODER_FLAC_COMPRESSION);
      QString cmd = ENCODER_FLAC_BIN;
      cmd += QString(" -%1").arg(compression);
      cmd += QString::fromUtf8(" -T Artist=\"$"VAR_TRACK_ARTIST"\" -T Title=\"$"VAR_TRACK_TITLE"\" -T Album=\"$"VAR_ALBUM_TITLE\
                               "\" -T Date=\"$"VAR_DATE"\" -T Tracknumber=\"$"VAR_TRACK_NO"\" -T Genre=\"$"VAR_GENRE\
                               "\" -o $"VAR_OUTPUT_FILE" $"VAR_INPUT_FILE);
      return cmd;
    }

    case EncoderAssistant::FAAC : {
      int quality = parameters.valueToInt(ENCODER_FAAC_QUALITY_KEY, ENCODER_FAAC_QUALITY);
      QString cmd = ENCODER_FAAC_BIN;
      cmd += QString(" -q %1").arg(quality);
      cmd += QString::fromUtf8(" -w -s --title \"$"VAR_TRACK_TITLE"\" --artist \"$"VAR_TRACK_ARTIST"\" --album \"$"VAR_ALBUM_TITLE\
                               "\" --year \"$"VAR_DATE"\" --track $"VAR_TRACK_NO" --disc $cdno --genre \"$"VAR_GENRE\
                               "\" -o $"VAR_OUTPUT_FILE" $"VAR_INPUT_FILE);

      return cmd;
    }

    case EncoderAssistant::WAVE : {
      return QString(ENCODER_WAVE_BIN)+" $"VAR_INPUT_FILE" $"VAR_OUTPUT_FILE;
    }

    case EncoderAssistant::CUSTOM : {
      return parameters.value(ENCODER_CUSTOM_COMMAND_PATTERN_KEY, ENCODER_CUSTOM_COMMAND_PATTERN);
    }

    default : ;

  }

  return "";

}

Parameters EncoderAssistant::stdParameters(const Encoder encoder, const Quality quality) {

  Parameters parameters;

  switch (encoder) {

    case EncoderAssistant::LAME :

      switch (quality) {

        case NORMAL :
          parameters.setValue(ENCODER_LAME_PRESET_KEY, ENCODER_LAME_PRESET);
          parameters.setValue(ENCODER_LAME_EMBED_COVER_KEY, ENCODER_LAME_EMBED_COVER);
          parameters.setValue(ENCODER_LAME_BITRATE_KEY, ENCODER_LAME_BITRATE);
          parameters.setValue(ENCODER_LAME_EMBED_COVER_KEY, ENCODER_LAME_EMBED_COVER);
	  break;

        case MOBILE :
          parameters.setValue(ENCODER_LAME_PRESET_KEY, ENCODER_LAME_PRESET_M);
          parameters.setValue(ENCODER_LAME_EMBED_COVER_KEY, ENCODER_LAME_EMBED_COVER_M);
          parameters.setValue(ENCODER_LAME_BITRATE_KEY, ENCODER_LAME_BITRATE_M);
          parameters.setValue(ENCODER_LAME_EMBED_COVER_KEY, ENCODER_LAME_EMBED_COVER_M);
	  break;

	case EXTREME :
          parameters.setValue(ENCODER_LAME_PRESET_KEY, ENCODER_LAME_PRESET_X);
          parameters.setValue(ENCODER_LAME_EMBED_COVER_KEY, ENCODER_LAME_EMBED_COVER_X);
          parameters.setValue(ENCODER_LAME_BITRATE_KEY, ENCODER_LAME_BITRATE_X);
          parameters.setValue(ENCODER_LAME_EMBED_COVER_KEY, ENCODER_LAME_EMBED_COVER_X);
	  break;

      }

      break;

    case EncoderAssistant::OGGENC :

      switch (quality) {

        case NORMAL :
          parameters.setValue(ENCODER_OGGENC_QUALITY_KEY, ENCODER_OGGENC_QUALITY);
          parameters.setValue(ENCODER_OGGENC_MINBITRATE_KEY, ENCODER_OGGENC_MINBITRATE);
          parameters.setValue(ENCODER_OGGENC_MINBITRATE_VALUE_KEY, ENCODER_OGGENC_MINBITRATE_VALUE);
          parameters.setValue(ENCODER_OGGENC_MAXBITRATE_KEY, ENCODER_OGGENC_MAXBITRATE);
          parameters.setValue(ENCODER_OGGENC_MAXBITRATE_VALUE_KEY, ENCODER_OGGENC_MAXBITRATE_VALUE);
	  break;

        case MOBILE :
          parameters.setValue(ENCODER_OGGENC_QUALITY_KEY, ENCODER_OGGENC_QUALITY_M);
          parameters.setValue(ENCODER_OGGENC_MINBITRATE_KEY, ENCODER_OGGENC_MINBITRATE_M);
          parameters.setValue(ENCODER_OGGENC_MINBITRATE_VALUE_KEY, ENCODER_OGGENC_MINBITRATE_VALUE_M);
          parameters.setValue(ENCODER_OGGENC_MAXBITRATE_KEY, ENCODER_OGGENC_MAXBITRATE_M);
          parameters.setValue(ENCODER_OGGENC_MAXBITRATE_VALUE_KEY, ENCODER_OGGENC_MAXBITRATE_VALUE_M);
	  break;

	case EXTREME :
	  parameters.setValue(ENCODER_OGGENC_QUALITY_KEY, ENCODER_OGGENC_QUALITY_X);
          parameters.setValue(ENCODER_OGGENC_MINBITRATE_KEY, ENCODER_OGGENC_MINBITRATE_X);
          parameters.setValue(ENCODER_OGGENC_MINBITRATE_VALUE_KEY, ENCODER_OGGENC_MINBITRATE_VALUE_X);
          parameters.setValue(ENCODER_OGGENC_MAXBITRATE_KEY, ENCODER_OGGENC_MAXBITRATE_X);
          parameters.setValue(ENCODER_OGGENC_MAXBITRATE_VALUE_KEY, ENCODER_OGGENC_MAXBITRATE_VALUE_X);
	  break;

      }

      break;

    case EncoderAssistant::FLAC :

      parameters.setValue(ENCODER_FLAC_COMPRESSION_KEY, ENCODER_FLAC_COMPRESSION);

      break;

    case EncoderAssistant::FAAC :

      switch (quality) {

        case NORMAL :
	  parameters.setValue(ENCODER_FAAC_QUALITY_KEY, ENCODER_FAAC_QUALITY);
	  break;

        case MOBILE :
	  parameters.setValue(ENCODER_FAAC_QUALITY_KEY, ENCODER_FAAC_QUALITY_M);
	  break;

	case EXTREME :
	  parameters.setValue(ENCODER_FAAC_QUALITY_KEY, ENCODER_FAAC_QUALITY_X);
	  break;

      }

      break;

    case EncoderAssistant::WAVE :

    case EncoderAssistant::CUSTOM :

    default : ;

  }

  return parameters;

}

const QMap<int,QString> EncoderAssistant::encoderList() {

  QMap<int,QString> encoders;

  encoders[(int)EncoderAssistant::LAME] = ENCODER_LAME_NAME;
  encoders[(int)EncoderAssistant::OGGENC] = ENCODER_OGGENC_NAME;
  encoders[(int)EncoderAssistant::FLAC] = ENCODER_FLAC_NAME;
  encoders[(int)EncoderAssistant::FAAC] = ENCODER_FAAC_NAME;
  encoders[(int)EncoderAssistant::WAVE] = ENCODER_WAVE_NAME;
  encoders[(int)EncoderAssistant::CUSTOM] = ENCODER_CUSTOM_NAME;

  return encoders;

}

const QMap<int,QString> EncoderAssistant::availableEncoderNameList() {

  QMap<int,QString> encoders;

  if (EncoderAssistant::available(EncoderAssistant::LAME)) encoders[(int)EncoderAssistant::LAME] = ENCODER_LAME_NAME;
  if (EncoderAssistant::available(EncoderAssistant::OGGENC)) encoders[(int)EncoderAssistant::OGGENC] = ENCODER_OGGENC_NAME;
  if (EncoderAssistant::available(EncoderAssistant::FLAC)) encoders[(int)EncoderAssistant::FLAC] = ENCODER_FLAC_NAME;
  if (EncoderAssistant::available(EncoderAssistant::FAAC)) encoders[(int)EncoderAssistant::FAAC] = ENCODER_FAAC_NAME;
  if (EncoderAssistant::available(EncoderAssistant::WAVE)) encoders[(int)EncoderAssistant::WAVE] = ENCODER_WAVE_NAME;
  if (EncoderAssistant::available(EncoderAssistant::CUSTOM)) encoders[(int)EncoderAssistant::CUSTOM] = ENCODER_CUSTOM_NAME;

  return encoders;

}

const QMap<int,QString> EncoderAssistant::availableEncoderNameListWithVersions() {

  QMap<int,QString> encoders;

  if (EncoderAssistant::available(EncoderAssistant::LAME)) encoders[(int)EncoderAssistant::LAME] = ENCODER_LAME_NAME+" "+version(LAME);
  if (EncoderAssistant::available(EncoderAssistant::OGGENC)) encoders[(int)EncoderAssistant::OGGENC] = ENCODER_OGGENC_NAME+" "+version(OGGENC);
  if (EncoderAssistant::available(EncoderAssistant::FLAC)) encoders[(int)EncoderAssistant::FLAC] = ENCODER_FLAC_NAME+" "+version(FLAC);
  if (EncoderAssistant::available(EncoderAssistant::FAAC)) encoders[(int)EncoderAssistant::FAAC] = ENCODER_FAAC_NAME+" "+version(FAAC);
  if (EncoderAssistant::available(EncoderAssistant::WAVE)) encoders[(int)EncoderAssistant::WAVE] = ENCODER_WAVE_NAME+" "+version(WAVE);
  if (EncoderAssistant::available(EncoderAssistant::CUSTOM)) encoders[(int)EncoderAssistant::CUSTOM] = ENCODER_CUSTOM_NAME+" "+version(CUSTOM);

  return encoders;

}
