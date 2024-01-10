/* AUDEX CDDA EXTRACTOR
 * Copyright (C) 2007-2011 Marco Nelles (audex@maniatek.com)
 * <http://kde.maniatek.com/audex>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "patternparser.h"

SaxHandler::SaxHandler() : QXmlDefaultHandler() {
  trackno = 1;
  cdno = 0;
  trackoffset = 0;
  fat32compatible = FALSE;
  discid = 0;
  size = 0;
  length = 0;
  nooftracks = 0;
  is_filename_pattern = FALSE;
  is_command_pattern = FALSE;
  is_simple_pattern = FALSE;
  is_text_pattern = FALSE;
  cover = NULL;
}

SaxHandler::~SaxHandler() {

}

bool SaxHandler::startElement(const QString& namespaceURI, const QString &localName,
		     const QString& qName, const QXmlAttributes& atts) {

  Q_UNUSED(namespaceURI);
  Q_UNUSED(localName);

  if (qName == VAR_FILENAME_PATTERN) { is_filename_pattern = TRUE; return TRUE; }
  if (qName == VAR_COMMAND_PATTERN) { is_command_pattern = TRUE; return TRUE; }
  if (qName == VAR_SIMPLE_PATTERN) { is_simple_pattern = TRUE; return TRUE; }
  if (qName == VAR_TEXT_PATTERN) { is_text_pattern = TRUE; return TRUE; }

  p_element.clear();

  if (qName == VAR_ALBUM_ARTIST) {
    if ((is_filename_pattern) || (is_simple_pattern)) {
      QString s;
      if ((fat32compatible) || (IS_TRUE(atts.value("fat32compatible")))) s = make_fat32_compatible(artist); else s = make_compatible(artist);
      if ((replacespaceswithunderscores) || (IS_TRUE(atts.value("underscores")))) s = replace_spaces_with_underscores(s);
      if (IS_TRUE(atts.value("replace_char_list"))) s = replace_char_list(atts, s);
      if (IS_TRUE(atts.value("lowercase"))) s = s.toLower();
        else if (IS_TRUE(atts.value("uppercase"))) s = s.toUpper();
      p_element += s;
    } else if (is_command_pattern) {
      p_element += make_compatible_2(artist);
    } else {
      p_element += artist;
    }
  }
  if (qName == VAR_ALBUM_TITLE) {
    if ((is_filename_pattern) || (is_simple_pattern)) {
      QString s;
      if ((fat32compatible) || (IS_TRUE(atts.value("fat32compatible")))) s = make_fat32_compatible(title); else s = make_compatible(title);
      if ((replacespaceswithunderscores) || (IS_TRUE(atts.value("underscores")))) s = replace_spaces_with_underscores(s);
      if (IS_TRUE(atts.value("replace_char_list"))) s = replace_char_list(atts, s);
      if (IS_TRUE(atts.value("lowercase"))) s = s.toLower();
        else if (IS_TRUE(atts.value("uppercase"))) s = s.toUpper();
      p_element += s;
    } else if (is_command_pattern) {
      p_element += make_compatible_2(title);
    } else {
      p_element += title;
    }
  }
  if (qName == VAR_DATE) {
    if ((is_filename_pattern) || (is_simple_pattern)) {
      QString s;
      if ((fat32compatible) || (IS_TRUE(atts.value("fat32compatible")))) s = make_fat32_compatible(date); else s = make_compatible(date);
      if ((replacespaceswithunderscores) || (IS_TRUE(atts.value("underscores")))) s = replace_spaces_with_underscores(s);
      if (IS_TRUE(atts.value("replace_char_list"))) s = replace_char_list(atts, s);
      if (IS_TRUE(atts.value("lowercase"))) s = s.toLower();
        else if (IS_TRUE(atts.value("uppercase"))) s = s.toUpper();
      p_element += s;
    } else if (is_command_pattern) {
      p_element += make_compatible_2(date);
    } else {
      p_element += date;
    }
  }
  if (qName == VAR_GENRE) {
    if ((is_filename_pattern) || (is_simple_pattern)) {
      QString s;
      if ((fat32compatible) || (IS_TRUE(atts.value("fat32compatible")))) s = make_fat32_compatible(genre); else s = make_compatible(genre);
      if ((replacespaceswithunderscores) || (IS_TRUE(atts.value("underscores")))) s = replace_spaces_with_underscores(s);
      if (IS_TRUE(atts.value("replace_char_list"))) s = replace_char_list(atts, s);
      if (IS_TRUE(atts.value("lowercase"))) s = s.toLower();
        else if (IS_TRUE(atts.value("uppercase"))) s = s.toUpper();
      p_element += s;
    } else if (is_command_pattern) {
      p_element += make_compatible_2(genre);
    } else {
      p_element += genre;
    }
  }

  if ((is_filename_pattern) || (is_command_pattern) || (is_simple_pattern)) {
    if (qName == VAR_CDNO) {
      if (cdno > 0) {
        bool ok;
        int l = atts.value("length").toInt(&ok);
        QChar fc = '0';
        if (!atts.value("fillchar").isEmpty()) fc = atts.value("fillchar").at(0);
        if (ok)
          p_element += QString("%1").arg(cdno, l, 10, fc);
        else
          p_element += QString("%1").arg(cdno);
      }
    }
  }
  if ((is_filename_pattern) || (is_command_pattern)) {
    if (qName == VAR_TRACK_ARTIST) {
      if (is_filename_pattern) {
        QString s;
        if ((fat32compatible) || (IS_TRUE(atts.value("fat32compatible")))) s = make_fat32_compatible(tartist); else s = make_compatible(tartist);
        if ((replacespaceswithunderscores) || (IS_TRUE(atts.value("underscores")))) s = replace_spaces_with_underscores(s);
        if (IS_TRUE(atts.value("lowercase"))) s = s.toLower();
          else if (IS_TRUE(atts.value("uppercase"))) s = s.toUpper();
        if (IS_TRUE(atts.value("replace_char_list"))) s = replace_char_list(atts, s);
        p_element += s;
      } else if (is_command_pattern) {
        p_element += make_compatible_2(tartist);
      } else {
        p_element += tartist;
      }
    }
    if (qName == VAR_TRACK_TITLE) {
      if (is_filename_pattern) {
        QString s;
        if ((fat32compatible) || (IS_TRUE(atts.value("fat32compatible")))) s = make_fat32_compatible(ttitle); else s = make_compatible(ttitle);
        if ((replacespaceswithunderscores) || (IS_TRUE(atts.value("underscores")))) s = replace_spaces_with_underscores(s);
        if (IS_TRUE(atts.value("replace_char_list"))) s = replace_char_list(atts, s);
        if (IS_TRUE(atts.value("lowercase"))) s = s.toLower();
          else if (IS_TRUE(atts.value("uppercase"))) s = s.toUpper();
        p_element += s;
      } else if (is_command_pattern) {
        p_element += make_compatible_2(ttitle);
      } else {
        p_element += ttitle;
      }
    }
    if (qName == VAR_TRACK_NO) {
      int t;
      if (trackoffset>1) t = trackno+trackoffset; else t = trackno;
      bool ok;
      int l = atts.value("length").toInt(&ok);
      QChar fc = '0';
      if (!atts.value("fillchar").isEmpty()) fc = atts.value("fillchar").at(0);
      if (ok)
        p_element += QString("%1").arg(t, l, 10, fc);
      else
        p_element += QString("%1").arg(t);
    }
    if (qName == VAR_SUFFIX) p_element += suffix;
  }
  if (is_command_pattern) {
    if (qName == VAR_INPUT_FILE) p_element += "\""+input+"\"";
    if (qName == VAR_OUTPUT_FILE) p_element += "\""+output+"\"";
    if (qName == VAR_COVER_FILE) {

      QString format = STANDARD_EMBED_COVER_FORMAT;
      if (!atts.value("format").isEmpty()) format = atts.value("format");

      if ((cover) && (!cover->supportedFormats().contains(format.toAscii().toLower()))) format = STANDARD_EMBED_COVER_FORMAT;

      QString filename;
      bool stop = FALSE;
      if (demomode) {

        filename = tmppath+"/audexcover.123."+format.toLower();

      } else {

        int x = -1;
        int y = -1;
        bool ok;
        if (!atts.value("x").isEmpty()) x = atts.value("x").toInt(&ok);
        if (!ok) x = -1;
        if (!atts.value("y").isEmpty()) y = atts.value("y").toInt(&ok);
        if (!ok) y = -1;

        QByteArray ba = QCryptographicHash::hash(QString(artist+title+date+QString("%1").arg(x*y)+format).toUtf8(), QCryptographicHash::Md5);
        QString mda5 = ba.toHex();

        PID pid;
        QString tmp_dir = tmppath;
        if (tmp_dir.right(1) != "/") tmp_dir += "/";
        tmp_dir += "audex."+QString("%1").arg(pid.getPID());
        kDebug() << "Temporary folder in use:" << tmp_dir;
        QDir *dir = new QDir(tmp_dir);
        if (!dir->exists()) {
          if (!dir->mkpath(tmp_dir)) {
            kDebug() << "Unable to create temporary folder " << tmp_dir << ". No temporary cover file will be saved. Please check.";
            stop = TRUE;
          }
        }

        if (!stop) filename = tmp_dir+"/cover."+QString("%1").arg(mda5)+"."+format.toLower();

        QFileInfo finfo(filename);
        if ((!finfo.exists()) && (!stop)) {

          bool success;
          if ((!cover) || ((cover) && (cover->isEmpty()))) {
	    if (IS_TRUE(atts.value("usenocover"))) {
	      QImage c = QImage(KStandardDirs::locate("data", QString("audex/images/nocover.png")));
	      if ((x != -1) && (y != -1)) {
                c = c.scaled(x, y, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
              }
              success = c.save(filename, format.toAscii());
	    } else {
              stop = TRUE;
	    }
	  } else {
            success = cover->save(filename, QSize(x, y));
	  }

	  if (!stop) {

	    if (!success) {
              kDebug() << "WARNING! Could not create temporary cover file" << filename;
	    } else {
              kDebug() << "Successfully created temporary cover file" << filename << "(" << QFile(filename).size() / 1024 << "KiB)";
	    }

	  }

	}

      }

      if (!stop) {
        p_element = "\""+filename+"\"";
      }

    }

  }
  if (is_text_pattern) {
    if (qName == VAR_CD_SIZE) {
      QChar iec;
      if (!atts.value("iec").isEmpty()) iec = atts.value("iec").at(0);
      if ((iec!='b') && (iec!='k') && (iec!='m') && (iec!='g')) iec = 'm';
      bool ok;
      int p = atts.value("precision").toInt(&ok);
      if (!ok) p = 2;
      if (iec=='b') p_element += QString("%1").arg(size, 0, 'f', p);
        else if (iec=='k') p_element += QString("%1").arg(size / 1024.0f, 0, 'f', p);
          else if (iec=='m') p_element += QString("%1").arg(size / (1024.0f*1024.0f), 0, 'f', p);
            else if (iec=='g') p_element += QString("%1").arg(size / (1024.0f*1024.0f*1024.0f), 0, 'f', p);
    }
    if (qName == VAR_CD_LENGTH) p_element += QString("%1:%2").arg(length / 60, 2, 10, QChar('0')).arg(length % 60, 2, 10, QChar('0'));
    if (qName == VAR_CD_NO_OF_TRACKS) p_element += QString("%1").arg(nooftracks);
    if (qName == VAR_TODAY) {
      QString format;
      if (!atts.value("format").isEmpty()) format = atts.value("format");
      if (format.isEmpty()) {
        p_element += QString("%1").arg(QDate::currentDate().toString());
      } else {
        p_element += QString("%1").arg(QDate::currentDate().toString(format));
      }
    }
    if (qName == VAR_NOW) {
      QString format;
      if (!atts.value("format").isEmpty()) format = atts.value("format");
      if (format.isEmpty()) {
        p_element += QString("%1").arg(QDateTime::currentDateTime().toString());
      } else {
        p_element += QString("%1").arg(QDateTime::currentDateTime().toString(format));
      }
    }
    if (qName == VAR_LINEBREAK) p_element += "\n";
    if (qName == VAR_DISCID) {
      bool ok;
      int base = atts.value("base").toInt(&ok);
      if (!ok) base = 16;
      p_element += QString("%1").arg(discid, 0, base);
    }
  }
  
  
  if (!p_element.isEmpty()) {
  
    QString pre = atts.value("pre");
    QString post = atts.value("post");
    p_text += pre+p_element+post;
  
  }

  return TRUE;

}

bool SaxHandler::endElement(const QString& namespaceURI, const QString& localName, const QString& qName) {

  Q_UNUSED(namespaceURI);
  Q_UNUSED(localName);

  if (qName == VAR_FILENAME_PATTERN) { is_filename_pattern = FALSE; p_text.replace("//", "/"); p_text = p_text.simplified(); return TRUE; }
  if (qName == VAR_COMMAND_PATTERN) { is_command_pattern = FALSE; p_text.replace("//", "/"); p_text = p_text.simplified(); return TRUE; }
  if (qName == VAR_SIMPLE_PATTERN) { is_simple_pattern = FALSE; p_text.replace("//", "/"); p_text = p_text.simplified()+"."+suffix; return TRUE; }
  if (qName == VAR_TEXT_PATTERN) { is_text_pattern = FALSE; return TRUE; }

  return TRUE;

}

bool SaxHandler::characters(const QString& ch) {
  p_text += ch;
  return TRUE;
}

bool SaxHandler::fatalError(const QXmlParseException& exception) {
  kDebug() << QString("XML pattern parse error: Column %1 (%2)").arg(exception.columnNumber()).arg(exception.message());
  return FALSE;
}

const QString SaxHandler::make_compatible(const QString& string) {
  QString s = string;
  s.replace("/", "_");
  s.replace("\"", "'");
  s.replace("\\", "_");
  return s;
}

const QString SaxHandler::make_compatible_2(const QString& string) {
  QString s = string;
  s.replace("\"", "\\\"");
  return s;
}

//remove \ / : * ? " < > |
const QString SaxHandler::make_fat32_compatible(const QString& string) {
  QString s = string;
  s.replace("\\", "_");
  s.replace("/", "_");
  s.replace(":", "_");
  s.replace("*", "_");
  s.replace("?", "_");
  s.replace("<", "_");
  s.replace(">", "_");
  s.replace("|", "_");
  return s;
}

const QString SaxHandler::replace_spaces_with_underscores(const QString& string) {
  QString s = string;
  s.replace(" ", "_");
  return s;
}

const QString SaxHandler::replace_char_list(const QXmlAttributes& atts, const QString& string) {

  int i;
  QString from, to, result;

  kDebug() << "starting replacement for: " << string;

  from = atts.value("replace_char_list_from");
  to = atts.value("replace_char_list_to");

  if (from.count() != to.count()) {
    kDebug() << "Could not replace if list lenght are not equal";
    return string;
  }

  result = string;
  for (i=0; i < from.count(); i++) {
    result.replace(from.at(i), to.at(i));
  }

  kDebug() << "finished: " << result;

  return result;

}

PatternParser::PatternParser(QObject *parent) : QObject(parent) {
  Q_UNUSED(parent);
}

PatternParser::~PatternParser() {

}

const QString PatternParser::parseFilenamePattern(const QString& pattern,
	int trackno, int cdno, int trackoffset,
	const QString& artist, const QString& title,
	const QString& tartist, const QString& ttitle,
	const QString& date, const QString& genre, const QString& suffix,
	bool fat32compatible, bool replacespaceswithunderscores) {

  SaxHandler handler;
  handler.setTrackNo(trackno);
  handler.setCDNo(cdno);
  handler.setTrackOffset(trackoffset);
  handler.setArtist(artist);
  handler.setTitle(title);
  handler.setTrackArtist(tartist);
  handler.setTrackTitle(ttitle);
  handler.setDate(date);
  handler.setGenre(genre);
  handler.setSuffix(suffix);
  handler.setFAT32Compatible(fat32compatible);
  handler.setReplaceSpacesWithUnderscores(replacespaceswithunderscores);

  QXmlInputSource inputSource;
  inputSource.setData("<filenamepattern>"+p_xmlize_pattern(pattern)+"</filenamepattern>");
  QXmlSimpleReader reader;
  reader.setContentHandler(&handler);
  reader.setErrorHandler(&handler);
  reader.parse(inputSource);

  return handler.text();

}

const QString PatternParser::parseCommandPattern(const QString& pattern,
	const QString& input, const QString& output,
	int trackno, int cdno, int trackoffset,
	const QString& artist, const QString& title,
	const QString& tartist, const QString& ttitle,
	const QString& date, const QString& genre, const QString& suffix, CachedImage *cover,
	bool fatcompatible, const QString& tmppath, const bool demomode) {

  SaxHandler handler;
  handler.setInputFile(input);
  handler.setOutputFile(output);
  handler.setTrackNo(trackno);
  handler.setCDNo(cdno);
  handler.setTrackOffset(trackoffset);
  handler.setArtist(artist);
  handler.setTitle(title);
  handler.setTrackArtist(tartist);
  handler.setTrackTitle(ttitle);
  handler.setDate(date);
  handler.setGenre(genre);
  handler.setSuffix(suffix);
  handler.setCover(cover);
  handler.setFAT32Compatible(fatcompatible);
  handler.setTMPPath(tmppath);
  handler.setDemoMode(demomode);
  
  QXmlInputSource inputSource;
  inputSource.setData("<commandpattern>"+p_xmlize_pattern(pattern)+"</commandpattern>");
  QXmlSimpleReader reader;
  reader.setContentHandler(&handler);
  reader.setErrorHandler(&handler);
  reader.parse(inputSource);

  return handler.text();

}

const QString PatternParser::parseSimplePattern(const QString& pattern,
	int cdno, const QString& artist, const QString& title,
	const QString& date, const QString& genre, const QString& suffix,
	bool fat32compatible) {

  SaxHandler handler;
  handler.setCDNo(cdno);
  handler.setArtist(artist);
  handler.setTitle(title);
  handler.setDate(date);
  handler.setGenre(genre);
  handler.setSuffix(suffix);
  handler.setFAT32Compatible(fat32compatible);
  handler.setReplaceSpacesWithUnderscores(FALSE);

  QXmlInputSource inputSource;
  inputSource.setData("<simplepattern>"+p_xmlize_pattern(pattern)+"</simplepattern>");
  QXmlSimpleReader reader;
  reader.setContentHandler(&handler);
  reader.setErrorHandler(&handler);
  reader.parse(inputSource);

  return handler.text();

}

void PatternParser::parseInfoText(QStringList& text,
	const QString& artist, const QString& title,
	const QString& date, const QString& genre,
	const quint32 discid, const qreal size, const int length, const int nooftracks) {

  SaxHandler handler;
  handler.setArtist(artist);
  handler.setTitle(title);
  handler.setDate(date);
  handler.setGenre(genre);
  handler.setDiscid(discid);
  handler.setSize(size);
  handler.setLength(length);
  handler.setNoOfTracks(nooftracks);

  QXmlInputSource inputSource;
  inputSource.setData("<textpattern>"+p_xmlize_pattern(text.join("\n"))+"</textpattern>");
  QXmlSimpleReader reader;
  reader.setContentHandler(&handler);
  reader.setErrorHandler(&handler);
  reader.parse(inputSource);

  text = handler.text().split('\n');

}

const QString PatternParser::p_xmlize_pattern(const QString& pattern) {

  QString newpattern;

  QString name;
  QString attr;
  int s = 0;
  for (int i = 0; i < pattern.length(); ++i) {

     if (pattern[i] == '&') {
       newpattern += "&amp;";
       continue;
     }
    
     switch (s) {

       //outside var
       case 0 :
         if (pattern[i] == '$') {
	   name.clear();
           s = 1;
	   continue;
	 }
	 break;

       //inside var
       case 1 :
         if (pattern[i] == '{') {
	   s = 3;
	 } else if (pattern[i] == '$') {
           newpattern += '$';
	   s = 0;
	 } else {
           s = 2;
	   name += pattern[i];
	 }
	 continue;

       //inside simple var
       case 2 :
         if (!pattern[i].isLetter()) {
	   if (!name.trimmed().isEmpty()) newpattern += "<"+name+" />";
	   name.clear();
           s = 0;
	   if (pattern[i] == '$') {
	     name.clear();
             s = 1;
	     continue;
	   } else {
	     newpattern += pattern[i];
	   }
	   continue;
	 }
	 name += pattern[i];
         continue;

       //inside extended var
       case 3 :
         if (pattern[i] == '}') {
	   if (!name.trimmed().isEmpty()) newpattern += "<"+name+" />";
	   name.clear();
           s = 0;
	   continue;
	 }
	 name += pattern[i];
         continue;

     }

     newpattern += pattern[i];

  }

  //rest at the end?
  if ((s==2) && (!name.trimmed().isEmpty())) newpattern += "<"+name+" />";

  return newpattern;

}
