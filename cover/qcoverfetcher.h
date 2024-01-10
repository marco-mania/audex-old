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

#ifndef QCOVERFETCHER_HEADER
#define QCOVERFETCHER_HEADER

#include <QtGui>
#include <QtNetwork>
#include <QtXml>

class QCoverFetcher : public QObject {
  Q_OBJECT
public:
  QCoverFetcher(QObject *parent = 0);
  ~QCoverFetcher();

  void startFetch(const QString& searchstring);

  QImage cover(int index);
  QString caption(int index);
  int count() { return captions.count(); }

  //us = International; fr = France; de = Germany; jp = Japan; uk = United Kingdom; ca = Canada
  void setLocale(const QString &locale);

  enum Status {
    NOS,
    SEARCHING,
    FETCHING
  };

  void setProxyType(QNetworkProxy::ProxyType type) { proxy.setType(type); http->setProxy(proxy); }
  QNetworkProxy::ProxyType proxyType() const { return proxy.type(); }
  void setProxyHost(const QString& host) { proxy.setHostName(host); http->setProxy(proxy); }
  QString proxyHost() const { return proxy.hostName(); }
  void setProxyPort(const quint16 port) { proxy.setPort(port); http->setProxy(proxy); }
  quint16 proxyPort() const { return proxy.port(); }
  void setProxyUsername(const QString& username) { proxy.setUser(username); http->setProxy(proxy); }
  QString proxyUsername() const { return proxy.user(); }
  void setProxyPassword(const QString& password) { proxy.setPassword(password); http->setProxy(proxy); }
  QString proxyPassword() const { return proxy.password(); }

signals:
  void fetched(const QImage& cover, const QString& caption, const QString& date, int no);
  void allFetched();
  void nothingFetched();

  void statusChanged(Status status);

  void proxyAuthenticationRequired(const QNetworkProxy& proxy, QAuthenticator *authenticator);

  void log(const QString& message);
  void error(const QString& description,
	const QString& solution = QString());
  void warning(const QString& description);
  void info(const QString& description);

private slots:
  void slot_request_finished(int id, bool error);

  void slot_proxy_authentication_required(const QNetworkProxy& proxy, QAuthenticator *authenticator);

private:
  QHttp *http;

  QStringList cover_urls;
  QList<QImage*> covers;
  void clear_covers() { for (int i = 0; i < covers.count(); i++) delete covers[i]; covers.clear(); }
  QStringList captions;
  QStringList dates;

  QString locale;

  Status _status;

  int _transactionid;

  int f_i;

  QNetworkProxy proxy;

  void parseXMLRespone(const QString& xml);
  bool fetchCovers();

};

#endif
