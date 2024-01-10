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

#include "maskparser.h"

SaxHandler::SaxHandler() : QXmlDefaultHandler() {
  trackno = 1;
  cdno = 0;
  trackoffset = 0;
  fat32compatible = FALSE;
  discid = 0;
  size = 0;
  length = 0;
  nooftracks = 0;
  is_filename_mask = FALSE;
  is_command_mask = FALSE;
  is_simple_mask = FALSE;
  is_text_mask = FALSE;
}

SaxHandler::~SaxHandler() {

}

bool SaxHandler::startElement(const QString& namespaceURI, const QString &localName,
		     const QString& qName, const QXmlAttributes& atts) {

  Q_UNUSED(namespaceURI);
  Q_UNUSED(localName);

  if (qName == TAG_FILENAME_MASK) { is_filename_mask = TRUE; return TRUE; }
  if (qName == TAG_COMMAND_MASK) { is_command_mask = TRUE; return TRUE; }
  if (qName == TAG_SIMPLE_MASK) { is_simple_mask = TRUE; return TRUE; }
  if (qName == TAG_TEXT_MASK) { is_text_mask = TRUE; return TRUE; }

  if (qName == TAG_ALBUM_ARTIST) _text += artist;
  if (qName == TAG_ALBUM_TITLE) _text += title;
  if (qName == TAG_DATE) _text += date;
  if (qName == TAG_GENRE) _text += genre;

  if ((is_filename_mask) || (is_command_mask) || (is_simple_mask)) {
    if (qName == TAG_CDNO) {
      if (cdno > 0) {
        bool ok;
        int l = atts.value("length").toInt(&ok);
        QChar fc = '0';
        if (!atts.value("fillchar").isEmpty()) fc = atts.value("fillchar").at(0);
        if (ok)
          _text += QString("%1").arg(cdno, l, 10, fc);
        else
          _text += QString("%1").arg(cdno);
      }
    }
  }
  if ((is_filename_mask) || (is_command_mask)) {
    if (qName == TAG_HOME_PATH) _text += QDir::homePath();
    if (qName == TAG_TRACK_ARTIST) _text += tartist;
    if (qName == TAG_TRACK_TITLE) _text += ttitle;
    if (qName == TAG_TRACK_NO) {
      int t;
      if (trackoffset>1) t = trackno+trackoffset; else t = trackno;
      bool ok;
      int l = atts.value("length").toInt(&ok);
      QChar fc = '0';
      if (!atts.value("fillchar").isEmpty()) fc = atts.value("fillchar").at(0);
      if (ok)
        _text += QString("%1").arg(t, l, 10, fc);
      else
        _text += QString("%1").arg(t);
    }
    if (qName == TAG_BASE_PATH) _text += basepath;
    if (qName == TAG_SUFFIX) _text += suffix;
  }
  if (is_command_mask) {
    if (qName == TAG_INPUT_FILE) _text += "\""+input+"\"";
    if (qName == TAG_OUTPUT_FILE) _text += "\""+output+"\"";
    if (qName == TAG_COVER_FILE) {

      QString format = "JPEG";
      if (!atts.value("format").isEmpty()) format = atts.value("format");

      if ((format.toLower() != "jpg") &&
          (format.toLower() != "jpeg") &&
	  (format.toLower() != "png") &&
	  (format.toLower() != "gif")) format = "JPG";

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
        kDebug() << "Temporary directory in use:" << tmp_dir;
        QDir *dir = new QDir(tmp_dir);
        if (!dir->exists()) {
          if (!dir->mkpath(tmp_dir)) {
            kDebug() << "Unable to create temporary directory " << tmp_dir << ". No temporary cover file will be saved. Please check.";
            stop = TRUE;
          }
        }

        if (!stop) filename = tmp_dir+"/cover."+QString("%1").arg(mda5)+"."+format.toLower();

        if ((!QFile::exists(filename)) && (!stop)) {

          QImage c;
          if (cover.isNull()) {
	    if (IS_TRUE(atts.value("usenocover"))) {
	      c = QImage(KStandardDirs::locate("data", QString("audex/images/nocover.png")));
	    } else {
              stop = TRUE;
	    }
	  } else {
            c = cover;
	  }

	  if (!stop) {

            if ((x != -1) && (y != -1)) {
              c = c.scaled(x, y, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            }

	    if (!c.save(filename)) {
              kDebug() << "WARNING! Could not create temporary cover file" << filename;
	    } else {
              kDebug() << "Successfully created temporary cover file" << filename << "(" << QFile(filename).size() / 1024 << "KiB)";
	    }

	  }

	}
      
      }

      if (!stop) {
        QString preparam = atts.value("preparam");
        QString postparam = atts.value("postparam");
        _text += preparam+"\""+filename+"\""+postparam;
      }
 
    }
 
  }
  if (is_text_mask) {
    if (qName == TAG_CD_SIZE) {
      QChar iec;
      if (!atts.value("iec").isEmpty()) iec = atts.value("iec").at(0);
      if ((iec!='b') && (iec!='k') && (iec!='m') && (iec!='g')) iec = 'm';
      bool ok;
      int p = atts.value("precision").toInt(&ok);
      if (!ok) p = 2;
      if (iec=='b') _text += QString("%1").arg(size, 0, 'f', p);
        else if (iec=='k') _text += QString("%1").arg(size / 1024.0f, 0, 'f', p);
          else if (iec=='m') _text += QString("%1").arg(size / (1024.0f*1024.0f), 0, 'f', p);
            else if (iec=='g') _text += QString("%1").arg(size / (1024.0f*1024.0f*1024.0f), 0, 'f', p);
    }
    if (qName == TAG_CD_LENGTH) _text += QString("%1:%2").arg(length / 60, 2, 10, QChar('0')).arg(length % 60, 2, 10, QChar('0'));
    if (qName == TAG_CD_NO_OF_TRACKS) _text += QString("%1").arg(nooftracks);
    if (qName == TAG_TODAY) {
      QString format;
      if (!atts.value("format").isEmpty()) format = atts.value("format");
      if (format.isEmpty()) {
        _text += QString("%1").arg(QDate::currentDate().toString());
      } else {
        _text += QString("%1").arg(QDate::currentDate().toString(format));
      }
    }
    if (qName == TAG_NOW) {
      QString format;
      if (!atts.value("format").isEmpty()) format = atts.value("format");
      if (format.isEmpty()) {
        _text += QString("%1").arg(QDateTime::currentDateTime().toString());
      } else {
        _text += QString("%1").arg(QDateTime::currentDateTime().toString(format));
      }
    }
    if (qName == TAG_LINEBREAK) _text += "\n";
    if (qName == TAG_DISCID) {
      bool ok;
      int base = atts.value("base").toInt(&ok);
      if (!ok) base = 16;
      _text += QString("%1").arg(discid, 0, base);
    }
  }


  return TRUE;

}

bool SaxHandler::endElement(const QString& namespaceURI, const QString& localName, const QString& qName) {

  Q_UNUSED(namespaceURI);
  Q_UNUSED(localName);

  if (qName == TAG_FILENAME_MASK) { is_filename_mask = FALSE; _text.replace("//", "/"); _text = _text.simplified(); return TRUE; }
  if (qName == TAG_COMMAND_MASK) { is_command_mask = FALSE; _text.replace("//", "/"); _text = _text.simplified(); return TRUE; }
  if (qName == TAG_SIMPLE_MASK) { is_simple_mask = FALSE; _text.replace("//", "/"); _text = _text.simplified()+"."+suffix; return TRUE; }
  if (qName == TAG_TEXT_MASK) { is_text_mask = FALSE; return TRUE; }

  return TRUE;

}

bool SaxHandler::characters(const QString& ch) {
  _text += ch;
  return TRUE;
}

bool SaxHandler::fatalError(const QXmlParseException& exception) {
  kDebug() << QString("XML mask parse error: Column %1 (%2)").arg(exception.columnNumber()).arg(exception.message());
  return FALSE;
}

void SaxHandler::make_compatible(QString& string) {
  string.replace("/", "\\/");
  string.replace("\"", "\\\"");
}

//remove \ / : * ? " < > |
void SaxHandler::make_fat32_compatible(QString& string) {
  string.replace("\\", "_");
  string.replace("/", "_");
  string.replace(":", "_");
  string.replace("*", "_");
  string.replace("?", "_");
  string.replace("<", "_");
  string.replace(">", "_");
  string.replace("|", "_");
}

MaskParser::MaskParser(QObject *parent) : QObject(parent) {
  Q_UNUSED(parent);
}

MaskParser::~MaskParser() {

}

QString MaskParser::parseFilenameMask(const QString& mask,
	int trackno, int cdno, int trackoffset,
	const QString& artist, const QString& title,
	const QString& tartist, const QString& ttitle,
	const QString& date, const QString& genre, const QString& suffix, const QString& basepath,
	bool fat32compatible) {

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
  handler.setBasepath(basepath);
  handler.setFAT32Compatible(fat32compatible);

  QXmlInputSource inputSource;
  inputSource.setData("<filenamemask>"+p_xmlize_mask(mask)+"</filenamemask>");
  QXmlSimpleReader reader;
  reader.setContentHandler(&handler);
  reader.setErrorHandler(&handler);
  reader.parse(inputSource);

  return handler.text();

}

QString MaskParser::parseCommandMask(const QString& mask,
	const QString& input, const QString& output,
	int trackno, int cdno, int trackoffset,
	const QString& artist, const QString& title,
	const QString& tartist, const QString& ttitle,
	const QString& date, const QString& genre, const QString& suffix, const QImage& cover, const QString& basepath,
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
  handler.setBasepath(basepath);
  handler.setFAT32Compatible(fatcompatible);
  handler.setTMPPath(tmppath);
  handler.setDemoMode(demomode);

  QXmlInputSource inputSource;
  inputSource.setData("<commandmask>"+p_xmlize_mask(mask)+"</commandmask>");
  QXmlSimpleReader reader;
  reader.setContentHandler(&handler);
  reader.setErrorHandler(&handler);
  reader.parse(inputSource);

  return handler.text();

}

QString MaskParser::parseSimpleMask(const QString& mask,
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

  QXmlInputSource inputSource;
  inputSource.setData("<simplemask>"+p_xmlize_mask(mask)+"</simplemask>");
  QXmlSimpleReader reader;
  reader.setContentHandler(&handler);
  reader.setErrorHandler(&handler);
  reader.parse(inputSource);

  return handler.text();

}

void MaskParser::parseInfoText(QStringList& text,
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
  inputSource.setData("<textmask>"+p_xmlize_mask(text.join("\n"))+"</textmask>");
  QXmlSimpleReader reader;
  reader.setContentHandler(&handler);
  reader.setErrorHandler(&handler);
  reader.parse(inputSource);

  text = handler.text().split('\n');

}

const QString MaskParser::p_xmlize_mask(const QString& mask) {

  QString newmask;

  QString name;
  QString attr;
  int s = 0;
  for (int i = 0; i < mask.length(); ++i) {

     switch (s) {

       //outside var
       case 0 :
         if (mask[i] == '$') {
	   name.clear();
           s = 1;
	   continue;
	 }
	 break;

       //inside var
       case 1 :
         if (mask[i] == '{') {
	   s = 3;
	 } else if (mask[i] == '$') {
           newmask += '$';
	   s = 0;
	 } else {
           s = 2;
	   name += mask[i];
	 }
	 continue;

       //inside simple var
       case 2 :
         if (!mask[i].isLetter()) {
	   if (!name.trimmed().isEmpty()) newmask += "<"+name+" />";
	   name.clear();
           s = 0;
	   if (mask[i] == '$') {
	     name.clear();
             s = 1;
	     continue;
	   } else {
	     newmask += mask[i];
	   }
	   continue;
	 }
	 name += mask[i];
         continue;

       //inside extended var
       case 3 :
         if (mask[i] == '}') {
	   if (!name.trimmed().isEmpty()) newmask += "<"+name+" />";
	   name.clear();
           s = 0;
	   continue;
	 }
	 name += mask[i];
         continue;

     }

     newmask += mask[i];

  }

  //rest at the end?
  if ((s==2) && (!name.trimmed().isEmpty())) newmask += "<"+name+" />";

  return newmask;

}
