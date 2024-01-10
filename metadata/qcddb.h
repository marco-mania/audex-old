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

/** Connect a CDDB/FreeDB server and fetch the metadata of an audio cd.
  * It's a full implementation of CDDB protocol level 1,2,3,4,5 and 6.
  *
  *  @author Marco Nelles
  *  @date 2007-08-21
  *
  *  @version 0.2
  *  Some minor bugfixes.
  *
  *  @version 0.1
  *  First working release.
  */

#ifndef QCDDB_HEADER
#define QCDDB_HEADER

#include <QtCore>
#include <QtNetwork>

#include "qcddametadata.h"

#define TCP_MAX_LINE_SIZE		256

/** This list is used to calculate the CDDB disc id.
  * Insert the start frames ((minute*60 + seconds)*75+frames)
  * of all tracks, followed by the last frame of the disc. The
  * last frame is the start frame of the leadout track.
  */
typedef QList<quint32> TrackOffsetList;

class QCDDB_Protocol : public QObject {
  Q_OBJECT
public:
  QCDDB_Protocol(QObject *parent = 0);
  ~QCDDB_Protocol();

  void setProtocolLevel(int l) { _protocol_level = l; }
  int protocolLevel() { return _protocol_level; }

  void setHost(const QString& host) { _host = host; }
  QString host() const { return _host; }
  void setPort(const quint16 port) { _port = port; }
  quint16 port() const { return _port; }

  void setTimeout(int msecs) { _timeout = msecs; }
  int timeout() { return _timeout; }

  QString serverhost() const { return _serverhost; }
  QString serverversion() const { return _serverversion; }
  bool serverReadOnly() { return _server_readonly; }

  void setProxyType(QNetworkProxy::ProxyType type) { proxy.setType(type); emit proxySettingsChanged(); }
  QNetworkProxy::ProxyType proxyType() const { return proxy.type(); }
  void setProxyHost(const QString& host) { proxy.setHostName(host); emit proxySettingsChanged(); }
  QString proxyHost() const { return proxy.hostName(); }
  void setProxyPort(const quint16 port) { proxy.setPort(port); emit proxySettingsChanged(); }
  quint16 proxyPort() const { return proxy.port(); }
  void setProxyUsername(const QString& username) { proxy.setUser(username); emit proxySettingsChanged(); }
  QString proxyUsername() const { return proxy.user(); }
  void setProxyPassword(const QString& password) { proxy.setPassword(password); emit proxySettingsChanged(); }
  QString proxyPassword() const { return proxy.password(); }

  QCDDA_HeaderMetaDataList queryResults() const { return query_results; }
  QCDDA_MetaData readResult() const { return read_result; }
  void clearQueryResults() { query_results.clear(); }
  void clearReadResult() { read_result.clear(); }

  void setLocalCachePath(const QString& path) { _local_cache_path = path; }
  QString localCachePath() const { return _local_cache_path; }

  bool readFromLocalCache(const TrackOffsetList& trackOffsetList);
  void writeToLocalCache(const QCDDA_MetaData& metadata);

signals:
  void queryDone(int matchcount);
  void readDone();
  void submitDone();

  void proxySettingsChanged();

  void log(const QString& message);
  void error(const QString& description,
	const QString& solution = QString());
  void warning(const QString& description);
  void info(const QString& description);

protected:
  QString build_query_command(const TrackOffsetList& trackOffsetList) const;
  QString build_read_command(int no = 0) const;

  enum ParseResult {
    OK,
    OK_BUT_MULTIPLE_MATCHES,
    OK_BUT_RECEIVING_MORE_LINES_NOW,
    FAILED,
    FAILED_NO_ENTRY_FOUND
  };

  ParseResult parse_greeting_headline(const QString& line);
  ParseResult parse_handshake_headline(const QString& line);
  ParseResult parse_protocol_headline(const QString& line);
  ParseResult parse_query_headline(const QString& line);
  ParseResult parse_read_headline(const QString& line);
  ParseResult parse_write_headline(const QString& line, bool first);

  QCDDA_HeaderMetaDataList query_results;
  QCDDA_MetaData read_result;

  QString _local_cache_path;

  int _protocol_level;

  QString _host;
  quint16 _port;

  int _timeout;
  QString _serverhost;
  QString _serverversion;
  bool _server_readonly;

  QNetworkProxy proxy;

  quint32 discid(const TrackOffsetList& list) const;

  quint32 status_code(const QString& line);

  void append_query_result(const QString& line);

  QString cddb_entry(const QCDDA_MetaData& metadata);

};

class QCDDB_TCPTransport : public QCDDB_Protocol {
  Q_OBJECT
public:
  QCDDB_TCPTransport(QObject *parent = 0);
  ~QCDDB_TCPTransport();

  bool connected() { return _connected; }
  bool handshaked() { return _handshaked; }

public slots:
  void conn();
  void disconn();
  void handshake();
  void protocol(int protocol = 0);
  void query(const TrackOffsetList& trackOffsetList);
  void read(int no = 0);
  void submit(const QCDDA_MetaData& metadata);

signals:
  void connectionDone();
  void disconnectionDone();
  void handshakeDone();
  void protocolDone();

  void proxyAuthenticationRequired(const QNetworkProxy& proxy, QAuthenticator *authenticator);

protected slots:
  void slot_read_response();
  void slot_connected();
  void slot_disconnected();

  void slot_proxy_settings_changed();
  void proxy_authentication_required(const QNetworkProxy& proxy, QAuthenticator *authenticator);

  void slot_error(QAbstractSocket::SocketError socketError);

protected:
  QString build_handshake_command() const;
  QString build_protocol_command(int protocol = 0) const;
  QString build_write_command(const QCDDA_MetaData& metadata);

  QTcpSocket *tcp_socket;

  enum Status {
    NOS,
    CONNECTING,
    DISCONNECTING,
    HANDSHAKING,
    PROTOCOLING,
    QUERYING,
    READING,
    WRITING
  };
  Status _status;

  bool _connected;
  bool _handshaked;

  bool multiple_lines;
  bool first_line;
  int last_read_no;
  QStringList read_buffer;
  QString write_buffer;

  void writeLine(const QString& line);

};

class QCDDB_HTTPTransport : public QCDDB_Protocol {
  Q_OBJECT
public:
  QCDDB_HTTPTransport(QObject *parent = 0);
  ~QCDDB_HTTPTransport();

  void setHTTPPathQuery(const QString& path) { _http_path_query = path; }
  QString httpPathQuery() const { return _http_path_query; }
  void setHTTPPathSubmit(const QString& path) { _http_path_submit = path; }
  QString httpPathSubmit() const { return _http_path_submit; }

  void setEmailAddress(const QString& email) { _email_address = email; }
  QString emailAddress() const { return _email_address; }

public slots:
  void query(const TrackOffsetList& trackOffsetList);
  void read(int no = 0);
  void submit(const QCDDA_MetaData& metadata);

signals:
  void proxyAuthenticationRequired(const QNetworkProxy& proxy, QAuthenticator *authenticator);

protected slots:
  void slot_request_finished(int id, bool error);

  void slot_proxy_settings_changed();
  void slot_proxy_authentication_required(const QNetworkProxy& proxy, QAuthenticator *authenticator);

protected:
  QHttp *http;

  QString _http_path_query;
  QString _http_path_submit;

  QString _email_address;

  enum Status {
    NOS,
    QUERYING,
    READING,
    WRITING
  };
  Status _status;

  int last_read_no;

  int _transactionid;

  QString trim_slashes(const QString& line) const {
    QString l = line;
    while (l.startsWith('/')) l = l.mid(1);
    while (l.endsWith('/')) l = l.left(l.length()-1);
    return l;
  }

};

class QCDDB : public QObject {
  Q_OBJECT
public:
  QCDDB(QObject *parent = 0);
  ~QCDDB();

  enum Transport {
    CDDBP,
    HTTP
  };
  /** Select protocol to communicate with a CDDB/FreeDB server.
    *  @param[in] transport The transport
    *  @see transport()
    */
  void setTransport(const Transport transport) {
    if (transport == _transport) return;
    _transport = transport;
    clear();
    _in_local_cache = FALSE;
  }
  /** Returns the actual selected transport.
    *  @return The actual transport
    *  @see setTransport(const Transport transport)
    */
  Transport transport() const { return _transport; }

  /** Enable/Disable local cache. The local cache is a directory of your choice (default ~/.cddb)
    * which stores the cddb database entries per file named by the disc id.
    *  @param[in] enable A boolean
    *  @see localCacheEnabled()
    *  @see setLocalCachePath(const QString& path)
    *  @see localCachePath()
    */
  void enableLocalCache(bool enable) { _enable_local_cache = enable; }
  /** Returns whether the local cache capabilities are activated.
    *  @return a boolean
    *  @see localCacheEnabled()
    *  @see setLocalCachePath(const QString& path)
    *  @see localCachePath()
    */
  bool localCacheEnabled() { return _enable_local_cache; }

  /** Enable/Disable remote access to an external CDDB/FreeDB server. Use this method if you want
    * to use <b>only</b> local cache.
    *  @param[in] enable A boolean
    *  @see remoteEnabled()
    */
  void enableRemote(bool enable) { _enable_remote = enable; }
  /** Returns whether the connection capabilities to a CDDB/FreeDB server are activated.
    *  @return A boolean
    *  @see enableRemote(bool enable)
    */
  bool remoteEnabled() { return _enable_remote; }

  /**GENERAL**********************************************************************/
  /** Set the protocol level. In most cases this will be 6, which is also the default.
    * The main difference between 6 and minor protocol versions is the utf-8 capabilitie of
    * version 6.
    *  @param[in] l a number between 1 and 6
    *  @see protocolLevel()
    */
  void setProtocolLevel(int l) {
    tcp->setProtocolLevel(l);
    http->setProtocolLevel(l);
  }
  /** Returns the protocol level. The default is 6.
    *  @return an integer between 1 and 6
    *  @see setProtocolLevel(int l)
    */
  int protocolLevel() {
    switch (_transport) {
      case CDDBP : return tcp->protocolLevel();
      case HTTP : return http->protocolLevel();
    }
  }

  /** Set the host of the CDDB/FreeDB server. The default is "freedb.org".
    *  @param[in] host A string
    *  @see host()
    */
  void setHost(const QString& host) {
    tcp->setHost(host);
    http->setHost(host);
  }
  /** Returns the current host. The default is "freedb.org".
    *  @return A string
    *  @see setHost(const QString& host)
    */
  QString host() const {
    switch (_transport) {
      case CDDBP : return tcp->host();
      case HTTP : return http->host();
    }
  }
  void setPort(const quint16 port) {
    tcp->setPort(port);
    http->setPort(port);
  }
  quint16 port() const {
    switch (_transport) {
      case CDDBP : return tcp->port();
      case HTTP : return http->port();
    }
  }

  void setTimeout(int msecs) {
    tcp->setTimeout(msecs);
    http->setTimeout(msecs);
  }
  int timeout() {
    switch (_transport) {
      case CDDBP : return tcp->timeout();
      case HTTP : return http->timeout();
    }
  }

  QString serverhost() const {
    switch (_transport) {
      case CDDBP : return tcp->serverhost();
      case HTTP : return http->serverhost();
    }
  }
  QString serverversion() const {
    switch (_transport) {
      case CDDBP : return tcp->serverversion();
      case HTTP : return http->serverversion();
    }
  }
  bool serverReadOnly() {
    switch (_transport) {
      case CDDBP : return tcp->serverReadOnly();
      case HTTP : return http->serverReadOnly();
    }
  }

  void setProxyType(QNetworkProxy::ProxyType type) {
    tcp->setProxyType(type);
    http->setProxyType(type);
  }
  QNetworkProxy::ProxyType proxyType() {
    switch (_transport) {
      case CDDBP : return tcp->proxyType();
      case HTTP : return http->proxyType();
    }
  }
  void setProxyHost(const QString& host) {
    tcp->setProxyHost(host);
    http->setProxyHost(host);
  }
  QString proxyHost() const {
    switch (_transport) {
      case CDDBP : return tcp->proxyHost();
      case HTTP : return http->proxyHost();
    }
  }
  void setProxyPort(const quint16 port) {
    tcp->setProxyPort(port);
    http->setProxyPort(port);
  }
  quint16 proxyPort() const {
    switch (_transport) {
      case CDDBP : return tcp->proxyPort();
      case HTTP : return http->proxyPort();
    }
  }
  void setProxyUsername(const QString& username) {
    tcp->setProxyUsername(username);
    http->setProxyUsername(username);
  }
  QString proxyUsername() const {
    switch (_transport) {
      case CDDBP : return tcp->proxyUsername();
      case HTTP : return http->proxyUsername();
    }
  }
  void setProxyPassword(const QString& password) {
    tcp->setProxyPassword(password);
    http->setProxyPassword(password);
  }
  QString proxyPassword() const {
    switch (_transport) {
      case CDDBP : return tcp->proxyPassword();
      case HTTP : return http->proxyPassword();
    }
  }

  void setLocalCachePath(const QString& path) {
    tcp->setLocalCachePath(path);
    http->setLocalCachePath(path);
  }
  QString localCachePath() const {
    switch (_transport) {
      case CDDBP : return tcp->localCachePath();
      case HTTP : return http->localCachePath();
    }
  }

  /**HTTP RELATED**************************************************************/
  void setHTTPPathQuery(const QString& path) { http->setHTTPPathQuery(path); }
  QString httpPathQuery() const { return http->httpPathQuery(); }
  void setHTTPPathSubmit(const QString& path) { http->setHTTPPathSubmit(path); }
  QString httpPathSubmit() const { return http->httpPathSubmit(); }
  void setEmailAddress(const QString& email) { http->setEmailAddress(email); }
  QString emailAddress() const { return http->emailAddress(); }

  QCDDA_HeaderMetaDataList queryResults() const {
    switch (_transport) {
      case CDDBP : return tcp->queryResults();
      case HTTP : return http->queryResults();
    }
    return QCDDA_HeaderMetaDataList();
  }
  QCDDA_MetaData readResult() const {
    switch (_transport) {
      case CDDBP : return tcp->readResult();
      case HTTP : return http->readResult();
    }
    return QCDDA_MetaData();
  }

  void clear() {
    tcp->clearQueryResults();
    tcp->clearReadResult();
    http->clearQueryResults();
    http->clearReadResult();
  }

public slots:
  void query(const TrackOffsetList& trackOffsetList);
  void read(int no = 0);
  void submit(const QCDDA_MetaData& metadata);
  void submitOnlyToCache(const QCDDA_MetaData& metadata);

signals:
  void busy(bool indicator, int kind); //kind==0: recv, kind==1: send

  void queryDone(int matchcount);
  void readDone();
  void submitDone();

  void proxyAuthenticationRequired(const QNetworkProxy& proxy, QAuthenticator *authenticator);

  void log(const QString& message);
  void error(const QString& description,
	const QString& solution = QString());
  void warning(const QString& description);
  void info(const QString& description);

private slots:
  void slot_tcp_connection_done();
  void slot_tcp_handshake_done();
  void slot_tcp_protocol_done();
  void slot_query_done(int matchcount);
  void slot_read_done();
  void slot_submit_done();
  void slot_proxy_authentication_required(const QNetworkProxy& proxy, QAuthenticator *authenticator);
  void slot_log(const QString& message);
  void slot_error(const QString& description, const QString& solution);
  void slot_warn(const QString& description);
  void slot_info(const QString& description);

private:
  QCDDB_TCPTransport *tcp;
  QCDDB_HTTPTransport *http;

  Transport _transport;
  bool _enable_local_cache;
  bool _enable_remote;

  bool _in_local_cache;

  int to_go;
  TrackOffsetList to_go_trackoffsetlist;
  int to_go_no;
  QCDDA_MetaData to_go_metadata;

};

#endif
