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

// Some of this code is inspired by amarok 1.4.7
// (C) 2004 Mark Kretschmann <markey@web.de>
// (C) 2004 Stefan Bogner <bochi@online.ms>
// (C) 2004 Max Howell

#include "qcoverfetcher.h"

QCoverFetcher::QCoverFetcher(QObject *parent) : QObject(parent) {
  Q_UNUSED(parent);
  http = new QHttp(this);
  if (!http) {
    emit log("(QCoverFetcher) could not create http class.");
    emit error(trUtf8("Internal panic error."), trUtf8("Check your hardware and make a bug report."));
    return;
  }
  connect(http, SIGNAL(requestFinished(int, bool)), this, SLOT(slot_request_finished(int, bool)));
  connect(http, SIGNAL(proxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*)), this, SLOT(slot_proxy_authentication_required(const QNetworkProxy&, QAuthenticator*)));
  _status = NOS;
  f_i = 0;
  proxy.setType(QNetworkProxy::NoProxy);
}

QCoverFetcher::~QCoverFetcher() {
  clear_covers();
  delete http;
}

void QCoverFetcher::startFetch(const QString& searchstring) {

  if (!http) return;
  if (_status!=NOS) return;

  // Static license Key. Thanks muesli ;-)
  const QString LICENSE("D1URM11J3F2CEH");
  //const QString LICENSE("D226ELUJMCPJMC");

  QString musicMode = "music";
  QString host = "xml.amazon.com";
  if (locale == "fr") {
    musicMode = "music-fr";
    host = "webservices.amazon.fr";
  } else if (locale == "de") {
    host = "webservices.amazon.de";
  } else if (locale == "jp") {
    musicMode = "music-jp";
    host = "xml.amazon.co.jp";
  } else if (locale == "ca") {
    musicMode = "music-ca";
  }

  //qDebug() << host;

  http->setHost(host, 80);

  // changed to type=lite because it makes less traffic
  QString url = "/onca/xml3?t=webservices-20&dev-t=" + LICENSE
      + "&KeywordSearch=" + QString(QUrl::toPercentEncoding(searchstring, "/"))
      + "&mode=" + musicMode
      + "&type=lite&locale=" + locale
      + "&page=1&f=xml";

  //qDebug() << url;

  emit log("(QCoverFetcher) searching...");

  _status = SEARCHING; emit statusChanged(SEARCHING);

  _transactionid = http->get(url);

}

QImage QCoverFetcher::cover(int index) {
  if ((index < 0) || (index >= covers.count())) return QImage();
  return *covers[index];
}

QString QCoverFetcher::caption(int index) {
  if ((index < 0) || (index >= covers.count())) return QString();
  return captions[index];
}

void QCoverFetcher::setLocale(const QString &locale) {

  if ((locale != "fr") && (locale != "de") && (locale != "jp") && (locale != "uk") && (locale != "ca"))
    this->locale = "us";
  else
    this->locale = locale;

}

void QCoverFetcher::slot_request_finished(int id, bool err) {

  if (err) {
    switch (http->error()) {
      case QHttp::NoError : break; //???
      case QHttp::HostNotFound :
        emit log("(QCoverFetcher) The host name lookup for amazon server failed.");
        emit error(trUtf8("The host name lookup for amazon server failed."),
		trUtf8("Check amazon.com."));
        break;
      case QHttp::ConnectionRefused :
        emit log("(QCoverFetcher) The amazon server refused the http connection.");
        emit error(trUtf8("The amazon server refused the http connection."),
		trUtf8("Please check all settings, then try again."));
        break;
      case QHttp::UnexpectedClose :
        emit log("(QCoverFetcher) The amazon server closed the http connection unexpectedly.");
        emit error(trUtf8("The amazon server closed the http connection unexpectedly."),
		trUtf8("Please check all settings, then try again."));
        break;
      case QHttp::InvalidResponseHeader :
        emit log("(QCoverFetcher) The amazon server sent an invalid http response header.");
        emit error(trUtf8("The amazon server sent an invalid http response header."),
		trUtf8("Please try again later."));
        break;
      case QHttp::WrongContentLength :
        emit log("(QCoverFetcher) The client could not read the content correctly because an error with respect to the content length occurred.");
        emit error(trUtf8("The client could not read the content correctly because an error with respect to the content length occurred."),
		trUtf8("Please try again later. Make a bug report."));
        break;
      case QHttp::Aborted :
        emit log("(QCoverFetcher) The request was aborted.");
        emit error(trUtf8("The request was aborted."), trUtf8("Please try again."));
        break;
      case QHttp::ProxyAuthenticationRequiredError :
        emit log("(QCoverFetcher) The proxy server requires authentication to establish a connection.");
        emit error(trUtf8("The proxy server requires authentication to establish a connection."),
		trUtf8("Please check your proxy settings."));
        break;
      case QHttp::AuthenticationRequiredError :
        emit log("(QCoverFetcher) The amazon server requires http authentication to complete the request.");
        emit error(trUtf8("The amazon server requires http authentication to complete the request."),
		trUtf8("Please try again later. Make a bug report."));
        break;
      case QHttp::UnknownError :
        emit log("(QCoverFetcher) amazon server: Unknown error");
        emit error(trUtf8("amazon server: Unknown error"),
		trUtf8("Please try again later. Make a bug report."));
        break;
    }
    _transactionid = 0;
    return;
  }

  if (id == _transactionid) {

    QByteArray buffer = http->readAll();
    //qDebug() << buffer;

    if (buffer.count()==0) {
      emit log("(QCoverFetcher) amazon server: empty response");
      emit error(trUtf8("amazon server: Empty response."),
	trUtf8("Try again later. Make a bug report."));
      _transactionid = 0;
      _status = NOS;  emit statusChanged(NOS);
      return;
    }

    switch (_status) {

      case SEARCHING : {
        emit log("(QCoverFetcher) searching finished.");
        parseXMLRespone(QString::fromUtf8(buffer.data()));
        _status = NOS; emit statusChanged(NOS);
        fetchCovers();
      } break;

      case FETCHING : {
        emit log("(QCoverFetcher) cover fetched.");
        QImage *image = new QImage();
        *image = QImage::fromData(buffer);
        if ((image->height()>1) || (image->width()>1)) {
          covers.append(image); int f_i_p = f_i+1;
          emit fetched(*image, captions[f_i], dates[f_i], f_i_p);
        }
        f_i++;
        if (cover_urls.count()>0) {
          fetchCovers();
        } else {
          _status = NOS; emit statusChanged(NOS);
          f_i = 0;
          emit allFetched();
        }
      } break;

      case NOS : break;

    }

  }

}

void QCoverFetcher::slot_proxy_authentication_required(const QNetworkProxy& proxy, QAuthenticator *authenticator) {
  emit proxyAuthenticationRequired(proxy, authenticator);
}

void QCoverFetcher::parseXMLRespone(const QString& xml) {

  QDomDocument doc;
  if (!doc.setContent(xml)) {
    emit log("(QCoverFetcher) amazon server: xml response failed");
      emit error(trUtf8("The XML obtained from Amazon is invalid."),
	trUtf8("Try again later. Make a bug report."));
    return;
  }

  const QDomNode details = doc.documentElement().namedItem("Details");

  cover_urls.clear();
  clear_covers();
  captions.clear();
  dates.clear();

  for (QDomNode node = details; !node.isNull(); node = node.nextSibling()) {

    QString coverUrl = node.namedItem("ImageUrlLarge").firstChild().toText().nodeValue();
    QString name = node.namedItem("ProductName").firstChild().toText().nodeValue();
    QString date = node.namedItem("ReleaseDate").firstChild().toText().nodeValue();

    const QDomNode artists = node.namedItem("Artists");
    // in most cases, Amazon only sends one Artist in Artists
    QString artist = "";
    if (!artists.isNull()) artist = artists.namedItem("Artist").firstChild().toText().nodeValue();

    qDebug() << "name:" << name << " artist:" << artist << " url:" << coverUrl << endl;

    if (!coverUrl.isEmpty()) {
      cover_urls.append(coverUrl);
      if (artist.isEmpty()) {
        captions.append(name);
      } else {
        captions.append(artist + " - " + name);
      }
      dates.append(date);
    }

  }

}

bool QCoverFetcher::fetchCovers() {
  if (!http) return FALSE;
  if (cover_urls.count()==0) {
    emit nothingFetched();
    return FALSE;
  }

  QUrl url(cover_urls.takeFirst());
  http->setHost(url.host(), 80);
  emit log("(QCoverFetcher) fetching cover...");
  _status = FETCHING; emit statusChanged(FETCHING);
  _transactionid = http->get(url.path());

  return TRUE;

}
