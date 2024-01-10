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

/* Some of this code is inspired by amarok 1.4.7
 * (C) 2004 Mark Kretschmann <markey@web.de>
 * (C) 2004 Stefan Bogner <bochi@online.ms>
 * (C) 2004 Max Howell
*/

#include "coverfetcher.h"

CoverFetcher::CoverFetcher(QObject *parent) : QObject(parent) {
  Q_UNUSED(parent);
  _status = NOS;
  f_i = 0;
}

CoverFetcher::~CoverFetcher() {
  clear_covers();
}

void CoverFetcher::startFetch(const QString& searchstring, const int fetchNo) {

  if (_status!=NOS) return;

  if (fetchNo == 0) return;

  fetch_no = fetchNo;

  // Static license Key. Thanks hydrogen ;-)
  const QString LICENSE("11ZKJS8X1ETSTJ6MT802");

  QString ss = searchstring;
  ss.replace("&", "");

  QString tld;
  if (locale == "us")
    tld = "com";
  else if( locale =="uk" )
    tld = "co.uk";
  else
    tld = locale;

  QString url;
  url = "http://ecs.amazonaws." + tld
        + "/onca/xml?Service=AWSECommerceService&Version=2007-10-29&Operation=ItemSearch&AssociateTag=webservices-20&AWSAccessKeyId=" + LICENSE
        + "&Keywords=" + KUrl::toPercentEncoding(ss, "/")
        + "&SearchIndex=Music&ResponseGroup=Small,Images";

  kDebug() << "searching covers (" << url << ")...";

  _status = SEARCHING; emit statusChanged(SEARCHING);

  job = KIO::storedGet(url);
  connect(job, SIGNAL(result(KJob*)), SLOT(fetched_xml_data(KJob*)));

}

QImage CoverFetcher::cover(int index) {
  if ((index < 0) || (index >= covers.count())) return QImage();
  return *covers[index];
}

QString CoverFetcher::caption(int index) {
  if ((index < 0) || (index >= covers.count())) return QString();
  return cover_names[index];
}

void CoverFetcher::setLocale(const QString &locale) {

  QString l = locale;
  if (locale=="0") l = "us";
  if (locale=="1") l = "de";
  if (locale=="2") l = "fr";
  if (locale=="3") l = "jp";
  if (locale=="4") l = "ca";

  if ((l != "fr") && (l != "de") && (l != "jp") && (l != "uk") && (l != "ca"))
    this->locale = "us";
  else
    this->locale = l;

}

void CoverFetcher::setLocale(const int locale) {
  setLocale(QString("%1").arg(locale));
}

void CoverFetcher::fetched_xml_data(KJob* job) {

  QByteArray buffer;

  if (job && job->error()) {
    kDebug() << "There was an error communicating with Amazon.";
    emit error(i18n("There was an error communicating with Amazon."), i18n("Try again later. Otherwise make a bug report."));
    _status = NOS; emit statusChanged(NOS);
    return;
  }
  if (job) {
    KIO::StoredTransferJob* const storedJob = static_cast<KIO::StoredTransferJob*>(job);
    buffer = storedJob->data();
  }

  if (buffer.count()==0) {
    kDebug() << "amazon server: empty response";
    emit error(i18n("amazon server: Empty response."),
	i18n("Try again later. Make a bug report."));
    _status = NOS;  emit statusChanged(NOS);
    return;
  }

  switch (_status) {

      case SEARCHING : {
        kDebug() << "searching finished.";
	//kDebug() << QString::fromUtf8(buffer.data());
        parse_xml_respone(QString::fromUtf8(buffer.data()));
        _status = NOS; emit statusChanged(NOS);
        fetch_covers();
      } break;

      case FETCHING : {
        kDebug() << "cover fetched.";
        QImage *image = new QImage();
        image->loadFromData(buffer);
        if ((image->height()>1) || (image->width()>1)) {
          covers.append(image); int f_i_p = f_i+1;
          emit fetched(*image, cover_names[f_i], f_i_p);
        }
	if (((fetch_no > -1) && (++f_i==fetch_no)) || (cover_urls.count()==0)) {
          _status = NOS; emit statusChanged(NOS);
          f_i = 0;
          emit allFetched();
	} else {
          fetch_covers();
        }
      } break;

      case NOS : break;

  }


}

void CoverFetcher::parse_xml_respone(const QString& xml) {

  cover_asins.clear();
  amazon_urls.clear();
  cover_urls.clear();
  cover_names.clear();

  QDomDocument doc;
  if (!doc.setContent(xml)) {
    kDebug() << "amazon server: xml response failed";
    emit error(i18n("The XML obtained from Amazon is invalid."),
	i18n("Try again later. Make a bug report."));
    return;
  }

  // the url for the Amazon product info page
  const QDomNodeList list = doc.documentElement().namedItem("Items").childNodes();

  for (int i = 0; i < list.count(); ++i) {
    QDomNode n = list.item( i );
    if (n.isElement() && n.nodeName() == "IsValid") {
      if (n.toElement().text() == "False") {
        kDebug() << "amazon server: xml response failed";
        emit error(i18n("The XML obtained from Amazon is invalid."),
		i18n("Try again later. Make a bug report."));
        return;
      }
    } else if (list.item(i).nodeName() == "Item") {
      const QDomNode node = list.item(i);
      parse_item_node(node);
    }
  }

}

void CoverFetcher::parse_item_node(const QDomNode& node) {

  QDomNode it = node.firstChild();

  QString size = "LargeImage";

  while (!it.isNull()) {
    if (it.isElement()) {
      QDomElement e = it.toElement();
      if (e.tagName() == "ASIN") {
	cover_asins.append(e.text());
      } else if (e.tagName() == "DetailPageURL") {
        amazon_urls.append(e.text());
      } else if (e.tagName() == size) {
        QDomNode subIt = e.firstChild();
        while (!subIt.isNull()) {
          if (subIt.isElement()) {
            QDomElement subE = subIt.toElement();
            if (subE.tagName() == "URL") {
              cover_urls.append(subE.text());
              break;
            }
          }
          subIt = subIt.nextSibling();
        }
      } else if (e.tagName() == "ItemAttributes") {
        QDomNodeList nodes = e.childNodes();
        QDomNode iter;
        QString artist;
        QString album;
        for (int i = 0; i < nodes.count(); ++i) {
          iter = nodes.item(i);
          if (iter.isElement()) {
            if (iter.nodeName() == "Artist") {
              artist = iter.toElement().text();
            } else if (iter.nodeName() == "Title") {
              album = iter.toElement().text();
            }
          }
        }
        cover_names.append(QString(artist + " - " + album));
      }
    }
    it = it.nextSibling();
  }

}

bool CoverFetcher::fetch_covers() {

  if (cover_urls.count()==0) {
    emit nothingFetched();
    return FALSE;
  }

  kDebug() << "fetching cover...";
  _status = FETCHING; emit statusChanged(FETCHING);

  job = KIO::storedGet(KUrl(cover_urls.takeFirst()));
  connect(job, SIGNAL(result(KJob*)), SLOT(fetched_xml_data(KJob*)));

  return TRUE;

}
