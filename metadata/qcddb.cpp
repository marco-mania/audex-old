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

#include "qcddb.h"

QCDDB_Protocol::QCDDB_Protocol(QObject *parent) : QObject(parent) {
  Q_UNUSED(parent);
  _protocol_level = 1;
  _host = "freedb.org";
  _server_readonly = TRUE;
  _timeout = 3000;
  proxy.setType(QNetworkProxy::NoProxy);
  _local_cache_path = "~/.cddb/";
}

QCDDB_Protocol::~QCDDB_Protocol() {
}

bool QCDDB_Protocol::readFromLocalCache(const TrackOffsetList& trackOffsetList) {
  if (_local_cache_path.isEmpty()) return FALSE;
  QDir dir(_local_cache_path.replace("~", QDir::homePath()));
  QFileInfoList list = dir.entryInfoList();
  for (int i = 0; i < list.count(); i++) {
    if (!list[i].isDir()) continue;
    QFile file(QString("%1/%2").arg(list[i].absoluteFilePath()).arg(discid(trackOffsetList), 0, 16));
    if (file.exists()) {
      QStringList buffer;
      if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return FALSE;
      while (!file.atEnd()) { buffer.append(QString::fromUtf8(file.readLine().data())); }
      read_result.importFromCDDB(buffer);
      read_result.headerData().set(QCDDA_HeaderMetaData::Category, list[i].baseName());
      file.close();
      emit log("(QCDDB_Protocol) Read cddb entry from local cache (" + file.fileName() + ")");
      return TRUE;
    }
  }
  return FALSE;
}

void QCDDB_Protocol::writeToLocalCache(const QCDDA_MetaData& metadata) {
  if (_local_cache_path.isEmpty()) return;
  if (!metadata.isValid()) return;
  QString path = QString("%1/%2").arg(_local_cache_path.replace("~", QDir::homePath())).arg(metadata.headerData().get(QCDDA_HeaderMetaData::Category).toString());
  QString filepath = QString("%1/%2").arg(path).arg(metadata.headerData().get(QCDDA_HeaderMetaData::Discid).toUInt(), 0, 16);
  QDir dir(path); if (!dir.exists()) dir.mkpath(path);
  QFile file(filepath);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;
  emit log("(QCDDB_Protocol) Write cddb entry to local cache (" + file.fileName() + ")");
  file.write(metadata.exportToCDDB(FALSE).toUtf8()+"\n");
  file.close();
}

QString QCDDB_Protocol::build_query_command(const TrackOffsetList& trackOffsetList) const {
  QString command = QString("cddb query %1 %2")
	.arg(QString("%1").arg(discid(trackOffsetList), 0, 16))
	.arg(trackOffsetList.count()-1);
  for (int i = 0; i < trackOffsetList.count()-1; i++)
    command += QString(" %1").arg(trackOffsetList[i]);
  command += QString(" %1").arg(trackOffsetList.last() / 75);
  return command;
}

QString QCDDB_Protocol::build_read_command(int no) const {
  return QString("cddb read %1 %2")
	.arg(query_results[no].get(QCDDA_HeaderMetaData::Category).toString())
	.arg(QString("%1").arg(query_results[no].get(QCDDA_HeaderMetaData::Discid).toUInt(), 0, 16));
}

QCDDB_Protocol::ParseResult QCDDB_Protocol::parse_greeting_headline(const QString& line) {
  emit log("(QCDDB_Protocol) Parsing server greeting response: " + line);
  uint serverStatus = status_code(line);
  if (serverStatus == 200) {
    emit log("(QCDDB_Protocol) Server greeting response: read-write");
    QStringList tokenList = line.split(' ', QString::SkipEmptyParts);
    _serverhost = tokenList[1];
    _serverversion = tokenList[4];
    _server_readonly = FALSE;
    return OK;
  } else if (serverStatus == 201) {
    emit log("(QCDDB_Protocol) Server greeting response: read-only");
    emit warning(trUtf8("Connection to CDDB/FreeDB server is readonly."));
    _server_readonly = TRUE;
    return OK;
  } else if (serverStatus == 432) {
    emit log("(QCDDB_Protocol) Server greeting response: permission denied");
    emit error(trUtf8("For some unknown reason CDDB/FreeDB server reported permission denied."),
	trUtf8("Please try another server, wait some time or enter data manually."));
    return FAILED;
  } else if (serverStatus == 433) {
    emit log("(QCDDB_Protocol) Server greeting response: too many users connected");
    emit error(trUtf8("CDDB/FreeDB server reported too many users connected. Server is working to capacity."),
	trUtf8("Please try another server, wait some time before try again or enter data manually."));
    return FAILED;
  } else if (serverStatus == 434) {
    emit log("(QCDDB_Protocol) Server greeting response: system load too high");
    emit error(trUtf8("CDDB/FreeDB server reported system load too high. Server is working to capacity."),
	trUtf8("Please try another server, wait some time before try again or enter data manually."));
    return FAILED;
  } else {
    emit log("(QCDDB_Protocol) Server greeting: unknown error");
    emit error(trUtf8("CDDB/FreeDB unspecified server error. This is a critical error."),
	trUtf8("Please try another server, wait some time and make a bug report. Also you can enter data manually of course."));
    return FAILED;
  }
}

QCDDB_Protocol::ParseResult QCDDB_Protocol::parse_handshake_headline(const QString& line) {
  emit log("(QCDDB_Protocol) Parsing server handshake response: " + line);
  uint serverStatus = status_code(line);
  if (serverStatus == 200) {
    emit log("(QCDDB_Protocol) handshake successful");
    return OK;
  } else if (serverStatus == 431) {
    emit log("(QCDDB_Protocol) handshake not successful");
    emit error(trUtf8("CDDB/FreeDB server handshake not successful. This means a connection to the CDDB/FreeDB server is not possible."),
	trUtf8("Please try another protocol, another server or make a bug report. Also you can enter data manually of course."));
    return FAILED;
  } else if (serverStatus == 402) {
    emit log("(QCDDB_Protocol) handshake already done");
    return OK;
  } else {
    emit log("(QCDDB_Protocol) unknown handshake error");
    emit error(trUtf8("CDDB/FreeDB server handshake not successful. This seems to be a server problem. A connection to the CDDB/FreeDB server is not possible."),
	trUtf8("Please try another protocol, another server or make a bug report. Also you can enter data manually of course."));
    return FAILED;
  }
}

QCDDB_Protocol::ParseResult QCDDB_Protocol::parse_protocol_headline(const QString& line) {
  emit log("(QCDDB_Protocol) Parsing server protocol response: " + line);
  uint serverStatus = status_code(line);
  if (serverStatus == 200) {
    emit log("(QCDDB_Protocol) protocol command response:" + line.mid(5));
    return OK;
  } else if (serverStatus == 201) {
    int spacePos = line.lastIndexOf(' ');
    bool ok;
    int pl = line.mid(spacePos+1).toUInt(&ok);
    if (!ok) {
      emit log("(QCDDB_Protocol) protocol level response parsing failed");
      emit warning(trUtf8("CDDB/FreeDB cannot be set to protocol level. Server response error."));
      return OK;
    } else {
      emit log("(QCDDB_Protocol) protocol level set successful");
      _protocol_level = pl;
      return OK;
    }
  } else if (serverStatus == 501) {
    emit log("(QCDDB_Protocol) illegal protocol level");
    emit error(trUtf8("CDDB/FreeDB cannot be set to an illegal protocol level."),
	trUtf8("Please try another protocol or make a bug report."));
    return FAILED;
  } else if (serverStatus == 502) {
    emit log("(QCDDB_Protocol) protocol level already set");
    emit warning(trUtf8("CDDB/FreeDB server already set to protocol level."));
    return OK;
  } else {
    emit log("(QCDDB_Protocol) unknown error during protocol setting");
    emit error(trUtf8("CDDB/FreeDB server error during setting protocol level."),
	trUtf8("Please try another protocol or make a bug report."));
    return FAILED;
  }
}

QCDDB_Protocol::ParseResult QCDDB_Protocol::parse_query_headline(const QString& line) {
  emit log("(QCDDB_Protocol) Parsing server query response: " + line);
  uint serverStatus = status_code(line);
  if (serverStatus == 200) {
    emit log("(QCDDB_Protocol) found exact match");
    return OK;
  } else if (serverStatus == 210) {
    emit log("(QCDDB_Protocol) found exact match, receiving and parsing list");
    return OK_BUT_MULTIPLE_MATCHES;
  } else if (serverStatus == 211) {
    emit log("(QCDDB_Protocol) found inexact match, receiving and parsing list");
    return OK_BUT_MULTIPLE_MATCHES;
  } else if (serverStatus == 202) {
    emit log("(QCDDB_Protocol) no match found");
    emit error(trUtf8("CDDB/FreeDB server reports no match found for current cd."),
	trUtf8("You can try another server or enter data manually. Please don't forget to submit."));
    return FAILED_NO_ENTRY_FOUND;
  } else if (serverStatus == 403) {
    emit log("(QCDDB_Protocol) database entry corrupt");
    emit error(trUtf8("CDDB/FreeDB server reports corrupted database entry."),
	trUtf8("You can try another server or enter data manually. Please don't forget to submit."));
    return FAILED;
  } else if (serverStatus == 409) {
    emit log("(QCDDB_Protocol) no handshake done");
    emit error(trUtf8("CDDB/FreeDB server internal missing handshake error."),
	trUtf8("Please try another server or protocol and make a bug report."));
    return FAILED;
  } else {
    emit log("(QCDDB_Protocol) unknown query error");
    emit error(trUtf8("CDDB/FreeDB reports an unspecified error."),
	trUtf8("Please try another server or protocol and make a bug report."));
    return FAILED;
  }
}

QCDDB_Protocol::ParseResult QCDDB_Protocol::parse_read_headline(const QString& line) {
  emit log("(QCDDB_Protocol) Parsing server read response: " + line);
  uint serverStatus = status_code(line);
  if (serverStatus == 210) {
    emit log("(QCDDB_Protocol) ok, reading and parsing cddb entry");
    return OK;
  } else if (serverStatus == 401) {
    emit log("(QCDDB_Protocol) entry not found");
    return FAILED_NO_ENTRY_FOUND;
  } else if (serverStatus == 402) {
    emit log("(QCDDB_Protocol) server error");
    emit error(trUtf8("CDDB/FreeDB server reports an error."),
	trUtf8("You can try another server, protocol or enter data manually."));
    return FAILED;
  } else if (serverStatus == 403) {
    emit log("(QCDDB_Protocol) database entry corrupt");
    emit error(trUtf8("CDDB/FreeDB server reports corrupted database entry."),
	trUtf8("You can try another server or enter data manually. Please don't forget to submit."));
    return FAILED;
  } else if (serverStatus == 409) {
    emit log("(QCDDB_Protocol) no handshake done");
    emit error(trUtf8("CDDB/FreeDB server internal missing handshake error."),
	trUtf8("Please try another server or protocol and make a bug report."));
    return FAILED;
  } else {
    emit log("(QCDDB_Protocol) unknown read error");
    emit error(trUtf8("CDDB/FreeDB reports an unspecified error."),
	trUtf8("Please try another server or protocol and make a bug report."));
    return FAILED;
  }
}

QCDDB_Protocol::ParseResult QCDDB_Protocol::parse_write_headline(const QString& line, bool first) {
  emit log("(QCDDB_Protocol) Parsing server write response: " + line);
  uint serverStatus = status_code(line);
  if (first) {
    if (serverStatus == 320) {
      emit log("(QCDDB_TCPTransport) ok, writing new db entry...");
      return OK_BUT_RECEIVING_MORE_LINES_NOW;
    } else if (serverStatus == 401) {
      emit log("(QCDDB_TCPTransport) Server submitting: permission denied");
      emit error(trUtf8("CDDB/FreeDB server reported permission denied."),
	trUtf8("Please try to submit with another protocol."));
      return FAILED;
    } else if (serverStatus == 402) {
      emit log("(QCDDB_TCPTransport) Server file system full/file access failed");
      emit error(trUtf8("CDDB/FreeDB server reported system error. Please wait for some time."));
      return FAILED;
    } else if (serverStatus == 409) {
      emit log("(QCDDB_Protocol) no handshake done");
      emit error(trUtf8("CDDB/FreeDB server internal missing handshake error."),
	trUtf8("Please try another protocol and make a bug report."));
      return FAILED;
    } else {
      emit log("(QCDDB_Protocol) unknown submit error");
      emit error(trUtf8("CDDB/FreeDB reports an unspecified error."),
	trUtf8("Please make a bug report."));
      return FAILED;
    }
  } else {
    if (serverStatus == 200) {
      emit log("(QCDDB_TCPTransport) ok, new cddb entry accepted by server");
      return OK;
    } else if (serverStatus == 501) {
      emit log("(QCDDB_TCPTransport) Server submitting: entry rejected (" + line.mid(4) + ")");
      emit error(trUtf8("CDDB/FreeDB server has rejected your entry. %1.").arg(line.mid(4)),
	trUtf8("Please check existing entry in CDDB/FreeDB. Maybe this is a bug."));
      return FAILED;
    } else {
      emit log("(QCDDB_TCPTransport) unknown submit error: " + line.mid(4));
      emit error(trUtf8("CDDB/FreeDB server reports an unknown error."),
	trUtf8("Please make a bug report."));
      return FAILED;
    }
  }
}

quint32 QCDDB_Protocol::discid(const TrackOffsetList& list) const {
  // Taken from version by Michael Matz in kio_audiocd.
  quint32 id = 0;
  int numTracks = list.count()-1;
  for (int i = numTracks-1; i >= 0; i--) {
    int n = list[i]/75;
    while (n > 0) {
      id += n % 10;
      n /= 10;
    }
  }
  unsigned int l = list[numTracks] / 75;
  l -= list[0] / 75;
  id = ((id % 255) << 24) | (l << 8) | numTracks;
  return id;
}

quint32 QCDDB_Protocol::status_code(const QString& line) {
  QStringList tokenList = line.split(' ', QString::SkipEmptyParts);
  uint serverStatus = tokenList[0].toUInt();
  return serverStatus;
}

void QCDDB_Protocol::append_query_result(const QString& line) {
  if (line.length()==0) return;
  QCDDA_HeaderMetaData headerdata;
  QStringList match = line.split(' ', QString::SkipEmptyParts);
  headerdata.set(QCDDA_HeaderMetaData::Category, match[0]);
  match.removeAt(0);
  bool ok;
  uint discid = match[0].toUInt(&ok, 16);
  if (!ok) return;
  headerdata.set(QCDDA_HeaderMetaData::Discid, discid);
  match.removeAt(0);
  QString dtitle = match.join(" ").trimmed();
  int slashPos = dtitle.indexOf(" / ");
  if (slashPos == -1) {
    headerdata.set(QCDDA_HeaderMetaData::Artist, dtitle);
    headerdata.set(QCDDA_HeaderMetaData::Title, dtitle);
  } else {
    headerdata.set(QCDDA_HeaderMetaData::Artist, dtitle.left(slashPos).trimmed());
    headerdata.set(QCDDA_HeaderMetaData::Title, dtitle.mid(slashPos + 3).trimmed());
  }
  query_results.append(headerdata);
}

QString QCDDB_Protocol::cddb_entry(const QCDDA_MetaData& metadata) {

  QString data;
  data += "# xmcd\n";
  data += "#\n";

  if (metadata.offsetsValid()) {
    data += "# Track frame offsets:\n";
    for (int i = 1; i <= metadata.trackCount(); i++)
      data += QString("#\t%1\n").arg(metadata.track(i).get(QCDDA_TrackMetaData::TrackOffset).toInt());
    if (metadata.get(QCDDA_MetaData::Length).toInt() > 0)
      data += QString("#\n# Disc length: %1 seconds\n#\n").arg(metadata.get(QCDDA_MetaData::Length).toInt());
  }

  data += metadata.exportToCDDB(TRUE) + "\n";
  return data;

}

//=================================================================================================
//=================================================================================================

QCDDB_TCPTransport::QCDDB_TCPTransport(QObject *parent) : QCDDB_Protocol(parent) {
  Q_UNUSED(parent);
  tcp_socket = new QTcpSocket(this);
  if (!tcp_socket) {
    emit log("(QCDDB_TCPTransport) could not create tcp socket.");
    emit error(trUtf8("Internal panic error."), trUtf8("Check your hardware and make a bug report."));
    return;
  }
  connect(tcp_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slot_error(QAbstractSocket::SocketError)));
  connect(tcp_socket, SIGNAL(readyRead()), this, SLOT(slot_read_response()));
  connect(tcp_socket, SIGNAL(connected()), this, SLOT(slot_connected()));
  connect(tcp_socket, SIGNAL(disconnected()), this, SLOT(slot_disconnected()));
  connect(tcp_socket, SIGNAL(proxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*)), this, SLOT(proxy_authentication_required(const QNetworkProxy&, QAuthenticator*)));
  connect(this, SIGNAL(proxySettingsChanged()), this, SLOT(slot_proxy_settings_changed()));
  _port = 8880;
  _status = NOS;
  _connected = FALSE;
  _handshaked = FALSE;
  multiple_lines = FALSE;
  first_line = TRUE;
}

QCDDB_TCPTransport::~QCDDB_TCPTransport() {
  delete tcp_socket;
}

void QCDDB_TCPTransport::conn() {

  if (!tcp_socket) return;
  if (_connected) return;

  emit log("(QCDDB_TCPTransport) connecting...");

  tcp_socket->abort();
  _status = CONNECTING;
  tcp_socket->connectToHost(_host, _port);

}

void QCDDB_TCPTransport::disconn() {

  if (!tcp_socket) return;
  if (!_connected) return;

  emit log("(QCDDB_TCPTransport) disconnecting...");

  _status = DISCONNECTING;
  tcp_socket->disconnectFromHost();

}

void QCDDB_TCPTransport::handshake() {

  if (!_connected) return;
  if (_handshaked) return;
  if (_status != NOS) return;

  emit log("(QCDDB_TCPTransport) handshaking...");

  _status = HANDSHAKING;
  writeLine(build_handshake_command());

}

void QCDDB_TCPTransport::protocol(int protocol) {

  if (!_connected) return;
  if (_status != NOS) return;

  emit log("(QCDDB_TCPTransport) protocol setting...");

  _status = PROTOCOLING;
  writeLine(build_protocol_command(protocol));

}

void QCDDB_TCPTransport::query(const TrackOffsetList& trackOffsetList) {

  if (!_connected) return;
  if (!_handshaked) return;
  if (_status != NOS) return;
  if (trackOffsetList.count()==0) return;

  emit log("(QCDDB_TCPTransport) querying...");

  _status = QUERYING;
  writeLine(build_query_command(trackOffsetList));

}

void QCDDB_TCPTransport::read(int no) {

  if (!_connected) return;
  if (!_handshaked) return;
  if (_status != NOS) return;
  if ((no < 0) || (no >= query_results.count())) {
    emit log("(QCDDB_TCPTransport) query before read first.");
    return;
  }

  emit log("(QCDDB_TCPTransport) reading...");

  last_read_no = no;

  _status = READING;
  writeLine(build_read_command(no));

}

void QCDDB_TCPTransport::submit(const QCDDA_MetaData& metadata) {

  if (!_connected) return;
  if (!_handshaked) return;
  if (_status != NOS) return;
  if (_server_readonly) {
    emit log("(QCDDB_TCPTransport) submit failed. server is read only.");
    emit error(trUtf8("CDDB/FreeDB server does not allow submitting via the CDDB-protocol."),
                                    trUtf8("Please try submitting with another protocol."));
    return;
  }
  if (!metadata.isValid()) {
    emit log("(QCDDB_TCPTransport) Metadata is not valid.");
    return;
  }

  emit log("(QCDDB_TCPTransport) submitting...");

  write_buffer = cddb_entry(metadata);
  write_buffer.append(".\n");

  _status = WRITING;
  writeLine(build_write_command(metadata));

}

void QCDDB_TCPTransport::slot_read_response() {

  switch (_status) {

    case CONNECTING : {
      if (!tcp_socket->canReadLine()) return;
      QString line;
      if (_protocol_level > 5)
        line = QString::fromUtf8(tcp_socket->readLine(TCP_MAX_LINE_SIZE).data()).trimmed();
      else
        line = QString::fromAscii(tcp_socket->readLine(TCP_MAX_LINE_SIZE).data()).trimmed();
      ParseResult result = parse_greeting_headline(line);
      if (result == OK) {
        _status = NOS;
        emit connectionDone();
      } else {
        _status = NOS;
        disconn();
      }
    } break;

    case HANDSHAKING : {
      if (!_connected) return;
      if (!tcp_socket->canReadLine()) return;
      QString line;
      if (_protocol_level > 5)
        line = QString::fromUtf8(tcp_socket->readLine(TCP_MAX_LINE_SIZE).data()).trimmed();
      else
        line = QString::fromAscii(tcp_socket->readLine(TCP_MAX_LINE_SIZE).data()).trimmed();
      ParseResult result = parse_handshake_headline(line);
      if (result == OK) {
        _status = NOS;
        _handshaked = TRUE;
        emit handshakeDone();
      } else {
        _status = NOS;
        disconn();
      }
    } break;

    case PROTOCOLING : {
      if (!_connected) return;
      if (!tcp_socket->canReadLine()) return;
      QString line;
      if (_protocol_level > 5)
        line = QString::fromUtf8(tcp_socket->readLine(TCP_MAX_LINE_SIZE).data()).trimmed();
      else
        line = QString::fromAscii(tcp_socket->readLine(TCP_MAX_LINE_SIZE).data()).trimmed();
      ParseResult result = parse_protocol_headline(line);
      if (result == OK) {
        _status = NOS;
        emit protocolDone();
      } else {
        _status = NOS;
      }
    } break;

    case QUERYING : {
      if (!_connected) return;
      if (!tcp_socket->canReadLine()) return;

      if (!multiple_lines) {
        QString line;
        if (_protocol_level > 5)
          line = QString::fromUtf8(tcp_socket->readLine(TCP_MAX_LINE_SIZE).data()).trimmed();
        else
          line = QString::fromAscii(tcp_socket->readLine(TCP_MAX_LINE_SIZE).data()).trimmed();
        multiple_lines = FALSE;
        ParseResult result = parse_query_headline(line);
        if (result==OK) {
          query_results.clear();
          append_query_result(line.mid(4));
          _status = NOS;
          emit queryDone(1);
        } else if (result==OK_BUT_MULTIPLE_MATCHES) {
          multiple_lines = TRUE;
        } else {
          _status = NOS;
        }
      }
      if (multiple_lines) {
        while (tcp_socket->canReadLine()) {
          QString _line;
          if (_protocol_level > 5)
            _line = QString::fromUtf8(tcp_socket->readLine(TCP_MAX_LINE_SIZE).data()).trimmed();
          else
            _line = QString::fromAscii(tcp_socket->readLine(TCP_MAX_LINE_SIZE).data()).trimmed();
          if (_line==".") { multiple_lines = FALSE; _status = NOS; emit queryDone(query_results.count()); return; }
          append_query_result(_line);
        }
      }
    } break;

    case READING : {
      if (!_connected) return;
      if (!tcp_socket->canReadLine()) return;

      if (!multiple_lines) {
        QString line;
        if (_protocol_level > 5)
          line = QString::fromUtf8(tcp_socket->readLine(TCP_MAX_LINE_SIZE).data()).trimmed();
        else
          line = QString::fromAscii(tcp_socket->readLine(TCP_MAX_LINE_SIZE).data()).trimmed();
        multiple_lines = FALSE;
        ParseResult result = parse_read_headline(line);
        if (result==OK) {
          multiple_lines = TRUE;
        } else {
          _status = NOS;
        }
      }
      if (multiple_lines) {
        while (tcp_socket->canReadLine()) {
          QString _line;
          if (_protocol_level > 5)
            _line = QString::fromUtf8(tcp_socket->readLine(TCP_MAX_LINE_SIZE).data()).trimmed();
          else
            _line = QString::fromAscii(tcp_socket->readLine(TCP_MAX_LINE_SIZE).data()).trimmed();
          if (_line==".") {
            read_result.importFromCDDB(read_buffer);
            read_result.headerData().set(QCDDA_HeaderMetaData::Category, query_results[last_read_no].get(QCDDA_HeaderMetaData::Category));
            multiple_lines = FALSE;
            _status = NOS;
            emit readDone();
            return;
          }
          read_buffer.append(_line);
        }
      }
    } break;

    case WRITING : {
      if (!_connected) return;
      if (!tcp_socket->canReadLine()) return;
      QString line;
      if (_protocol_level > 5)
        line = QString::fromUtf8(tcp_socket->readLine(TCP_MAX_LINE_SIZE).data()).trimmed();
      else
        line = QString::fromAscii(tcp_socket->readLine(TCP_MAX_LINE_SIZE).data()).trimmed();

      ParseResult result = parse_write_headline(line, first_line);
      if (result==OK_BUT_RECEIVING_MORE_LINES_NOW) {
        first_line = FALSE;
        if (_protocol_level > 5)
          tcp_socket->write(write_buffer.toUtf8());
        else
          tcp_socket->write(write_buffer.toLatin1());
      } else if (result==OK) {
        first_line = TRUE;
        _status = NOS;
        emit submitDone();
      } else {
        first_line = TRUE;
        _status = NOS;
      }
    } break;

    case DISCONNECTING : break;

    case NOS : break;

  }

}

void QCDDB_TCPTransport::slot_connected() {
  _connected = TRUE;
  emit log("(QCDDB_TCPTransport) server connected: " + tcp_socket->peerName());
}

void QCDDB_TCPTransport::slot_disconnected() {
  _status = NOS;
  _connected = FALSE;
  _handshaked = FALSE;
  _server_readonly = TRUE;
  multiple_lines = FALSE;
  _serverhost.clear();
  _serverversion.clear();
  emit disconnectionDone();
  emit log("(QCDDB_TCPTransport) server disconnected");
}

void QCDDB_TCPTransport::slot_proxy_settings_changed() {
  tcp_socket->setProxy(proxy);
}

void QCDDB_TCPTransport::proxy_authentication_required(const QNetworkProxy& proxy, QAuthenticator *authenticator) {
  emit proxyAuthenticationRequired(proxy, authenticator);
}

void QCDDB_TCPTransport::slot_error(QAbstractSocket::SocketError socketError) {
  switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
      break;
    case QAbstractSocket::HostNotFoundError:
      emit log("(QCDDB_TCPTransport) host not found");
      emit error(trUtf8("The host was not found."),
	trUtf8("Please check the host name and port settings."));
      break;
    case QAbstractSocket::ConnectionRefusedError:
      emit log("(QCDDB_TCPTransport) connection refused");
      emit error(trUtf8("The connection was refused by the peer."),
	trUtf8("Make sure the cddb/freedb server is running, "
	"and check that the host name and port "
	"settings are correct."));
      break;
    default:
      emit log("(QCDDB_TCPTransport) error: "+tcp_socket->errorString());
      emit error(trUtf8("The following error occurred: %1.").arg(tcp_socket->errorString()),
	trUtf8("Maybe make a bug report."));
      break;
  }
  _connected = FALSE;
  _handshaked = FALSE;
}

QString QCDDB_TCPTransport::build_handshake_command() const {
  QString username = "anonymous";
  return QString("cddb hello %1 %2 %3 %4")
	.arg(username)
	.arg(QHostInfo::localHostName())
	.arg(QCDDB_CLIENTNAME)
	.arg(QCDDB_CLIENTVERSION);
}
QString QCDDB_TCPTransport::build_protocol_command(int protocol) const {
  QString command;
  if (protocol > 0) {
    command = QString("proto %1")
	.arg(protocol);
  } else {
    command = QString("proto %1")
	.arg(_protocol_level);
  }
  return command;
}

QString QCDDB_TCPTransport::build_write_command(const QCDDA_MetaData& metadata) {
  return QString("cddb write %1 %2")
	.arg(QString("%1").arg(metadata.headerData().get(QCDDA_HeaderMetaData::Discid).toUInt(), 0, 16))
	.arg(metadata.headerData().get(QCDDA_HeaderMetaData::Category).toString());
}

void QCDDB_TCPTransport::writeLine(const QString& line) {
  if (_protocol_level > 5) {
    QByteArray buf(line.toUtf8());
    buf.append('\n');
    tcp_socket->write(buf);
  } else {
    QByteArray buf(line.toAscii());
    buf.append('\n');
    tcp_socket->write(buf);
  }
}

//=================================================================================================
//=================================================================================================

QCDDB_HTTPTransport::QCDDB_HTTPTransport(QObject *parent) : QCDDB_Protocol(parent) {
  Q_UNUSED(parent);
  http = new QHttp(this);
  if (!http) {
    emit log("(QCDDB_TCPTransport) could not create http class.");
    emit error(trUtf8("Internal panic error."), trUtf8("Check your hardware and make a bug report."));
    return;
  }
  connect(http, SIGNAL(requestFinished(int, bool)), this, SLOT(slot_request_finished(int, bool)));
  connect(http, SIGNAL(proxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*)), this, SLOT(slot_proxy_authentication_required(const QNetworkProxy&, QAuthenticator*)));
  connect(this, SIGNAL(proxySettingsChanged()), this, SLOT(slot_proxy_settings_changed()));
  _port = 80;
  _http_path_query = "/~cddb/cddb.cgi";
  _http_path_submit = "/~cddb/submit.cgi";
  _email_address = "anonymous@anonymous.de";
  _status = NOS;
  _protocol_level = 6;
}

QCDDB_HTTPTransport::~QCDDB_HTTPTransport() {
  delete http;
}

void QCDDB_HTTPTransport::query(const TrackOffsetList& trackOffsetList) {

  if (!http) return;
  if (_status!=NOS) return;
  if (trackOffsetList.count()==0) return;

  http->setHost(_host, _port);

  QString username = "anonymous";

  QString url = QString("/%1?cmd=%2&hello=%3+%4+%5+%6&proto=%7")
        .arg(trim_slashes(_http_path_query)) //1
	.arg(build_query_command(trackOffsetList).replace(' ', '+')) //2
	.arg(username) //3
	.arg(QHostInfo::localHostName()) //4
	.arg(QCDDB_CLIENTNAME) //5
	.arg(QCDDB_CLIENTVERSION) //6
	.arg(_protocol_level); //7

  emit log("(QCDDB_HTTPTransport) querying...");

  _status = QUERYING;
  _transactionid = http->get(url);

}

void QCDDB_HTTPTransport::read(int no) {

  if (!http) return;
  if (_status!=NOS) return;
  if ((no < 0) || (no >= query_results.count())) {
    emit log("(QCDDB_HTTPTransport) query before read first.");
    return;
  }

  http->setHost(_host, _port);

  QString username = "anonymous";

  QString url = QString("/%1?cmd=%2&hello=%3+%4+%5+%6&proto=%7")
        .arg(trim_slashes(_http_path_query)) //1
	.arg(build_read_command(no).replace(' ', '+')) //2
	.arg(username) //3
	.arg(QHostInfo::localHostName()) //4
	.arg(QCDDB_CLIENTNAME) //5
	.arg(QCDDB_CLIENTVERSION) //6
	.arg(_protocol_level);

  last_read_no = no;

  emit log("(QCDDB_HTTPTransport) reading...");

  _status = READING;
  _transactionid = http->get(url);

}

void QCDDB_HTTPTransport::submit(const QCDDA_MetaData& metadata) {

  if (!http) return;
  if (_status!=NOS) return;
  if (!metadata.isValid()) {
    emit log("(QCDDB_HTTPTransport) Metadata is not valid.");
    emit error(trUtf8("The CD information is invalid."),
	trUtf8("Please make sure there are all information. Standard values are not accepted."));
    return;
  }

  QHttpRequestHeader header("POST", _http_path_submit);
  header.setContentType("text/plain");
  header.setValue("Category", metadata.headerData().get(QCDDA_HeaderMetaData::Category).toString());
  header.setValue("Discid", QString("%1").arg(metadata.headerData().get(QCDDA_HeaderMetaData::Discid).toUInt(), 0, 16));
  header.setValue("User-Email", _email_address);
  //header.setValue("Submit-Mode", "test");
  header.setValue("Submit-Mode", "submit");
  header.setValue("Charset", "UTF-8");
  header.setValue("Host", _serverhost);

  QString write_buffer = cddb_entry(metadata);
  header.setContentLength(write_buffer.size());

  emit log("(QCDDB_HTTPTransport) submiting...");

  http->setHost(_host, _port);

  _status = WRITING;
  _transactionid = http->request(header, write_buffer.toUtf8());

}

void QCDDB_HTTPTransport::slot_request_finished(int id, bool err) {

  if (_status==NOS) return;

  if (err) {
    switch (http->error()) {
      case QHttp::NoError : break; //???
      case QHttp::HostNotFound :
        emit log("(QCDDB_HTTPTransport) The host name lookup for CDDB/FreeDB server failed.");
        emit error(trUtf8("The host name lookup for CDDB/FreeDB server failed."),
		trUtf8("Check CDDB/FreeDB server hostname."));
        break;
      case QHttp::ConnectionRefused :
        emit log("(QCDDB_HTTPTransport) The CDDB/FreeDB server refused the http connection.");
        emit error(trUtf8("The CDDB/FreeDB server refused the http connection."),
		trUtf8("Please check all settings, then try another server."));
        break;
      case QHttp::UnexpectedClose :
        emit log("(QCDDB_HTTPTransport) The CDDB/FreeDB server closed the http connection unexpectedly.");
        emit error(trUtf8("The CDDB/FreeDB server closed the http connection unexpectedly."),
		trUtf8("Please check all settings, then try another server."));
        break;
      case QHttp::InvalidResponseHeader :
        emit log("(QCDDB_HTTPTransport) The CDDB/FreeDB server sent an invalid http response header.");
        emit error(trUtf8("The CDDB/FreeDB server sent an invalid http response header."),
		trUtf8("Please try another server."));
        break;
      case QHttp::WrongContentLength :
        emit log("(QCDDB_HTTPTransport) The client could not read the content correctly because an error with respect to the content length occurred.");
        emit error(trUtf8("The client could not read the content correctly because an error with respect to the content length occurred."),
		trUtf8("Please try another server. Make a bug report."));
        break;
      case QHttp::Aborted :
        emit log("(QCDDB_HTTPTransport) The request was aborted.");
        emit error(trUtf8("The request was aborted."), trUtf8("Please try again."));
        break;
      case QHttp::ProxyAuthenticationRequiredError :
        emit log("(QCDDB_HTTPTransport) The proxy server requires authentication to establish a connection.");
        emit error(trUtf8("The proxy server requires authentication to establish a connection."),
		trUtf8("Please check your proxy settings."));
        break;
      case QHttp::AuthenticationRequiredError :
        emit log("(QCDDB_HTTPTransport) The CDDB/FreeDB server requires http authentication to complete the request.");
        emit error(trUtf8("The CDDB/FreeDB server requires http authentication to complete the request."),
		trUtf8("Please try another server."));
        break;
      case QHttp::UnknownError :
        emit log("(QCDDB_HTTPTransport) CDDB/FreeDB server: Unknown error");
        emit error(trUtf8("CDDB/FreeDB server: Unknown error"),
		trUtf8("Please try another server. Make a bug report."));
        break;
    }
    _transactionid = 0;
    return;
  }

  if (id == _transactionid) {

    QStringList buffer;
    if (_protocol_level > 5)
      buffer = QString::fromUtf8(http->readAll().data()).replace("\r", "").split('\n', QString::SkipEmptyParts);
    else
      buffer = QString::fromLatin1(http->readAll().data()).replace("\r", "").split('\n', QString::SkipEmptyParts);

    /*qDebug() << "BUFFER::";
    for (int i = 0; i < buffer.count(); i++)
      qDebug() << buffer[i];*/

    if (buffer.count()==0) {
      emit log("(QCDDB_HTTPTransport) CDDB/FreeDB server: empty response");
      emit error(trUtf8("CDDB/FreeDB server: Empty response."),
	trUtf8("Try again later or make a bug report."));
      _transactionid = 0;
      _status = NOS;
      return;
    }

    switch (_status) {

      case QUERYING : {
        ParseResult result = parse_query_headline(buffer[0]);
        query_results.clear();
        if (result == OK) {
          append_query_result(buffer[0].mid(4));
          _status = NOS;
          _transactionid = 0;
          emit queryDone(1);
        } else if (result == OK_BUT_MULTIPLE_MATCHES) {
          for (int i = 1; i < buffer.count(); i++) {
            if (buffer[i] != ".") {
              append_query_result(buffer[i]);
            } else break;
          }
          _status = NOS;
          _transactionid = 0;
          emit queryDone(query_results.count());
        } else {
          _transactionid = 0;
          _status = NOS;
        }
      } break;

      case READING : {
        ParseResult result = parse_read_headline(buffer[0]);
        if (result == OK) {
          buffer.removeAt(0);
          read_result.importFromCDDB(buffer);
          if (!read_result.headerData().get(QCDDA_HeaderMetaData::Discid).isValid())
            read_result.headerData().set(QCDDA_HeaderMetaData::Discid, query_results[last_read_no].get(QCDDA_HeaderMetaData::Discid));
          read_result.headerData().set(QCDDA_HeaderMetaData::Category, query_results[last_read_no].get(QCDDA_HeaderMetaData::Category));
          _status = NOS;
          _transactionid = 0;
          emit readDone();
        } else {
          _transactionid = 0;
          _status = NOS;
        }
      } break;

      case WRITING : {
        ParseResult result = parse_write_headline(buffer[0], FALSE);
        if ((result==OK_BUT_RECEIVING_MORE_LINES_NOW) || (result==OK)) {
          _status = NOS;
          emit submitDone();
        } else {
          _status = NOS;
        }
      } break;

      case NOS : break;

    }

  }

}

void QCDDB_HTTPTransport::slot_proxy_authentication_required(const QNetworkProxy& proxy, QAuthenticator *authenticator) {
  emit proxyAuthenticationRequired(proxy, authenticator);
}

void QCDDB_HTTPTransport::slot_proxy_settings_changed() {
  http->setProxy(proxy);
}

//=================================================================================================
//=================================================================================================

QCDDB::QCDDB(QObject *parent) : QObject(parent) {
  tcp = new QCDDB_TCPTransport();
  connect(tcp, SIGNAL(connectionDone()), this, SLOT(slot_tcp_connection_done()));
  connect(tcp, SIGNAL(handshakeDone()), this, SLOT(slot_tcp_handshake_done()));
  connect(tcp, SIGNAL(protocolDone()), this, SLOT(slot_tcp_protocol_done()));
  connect(tcp, SIGNAL(queryDone(int)), this, SLOT(slot_query_done(int)));
  connect(tcp, SIGNAL(readDone()), this, SLOT(slot_read_done()));
  connect(tcp, SIGNAL(submitDone()), this, SLOT(slot_submit_done()));
  connect(tcp, SIGNAL(proxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*)), this, SLOT(slot_proxy_authentication_required(const QNetworkProxy&, QAuthenticator*)));
  connect(tcp, SIGNAL(log(const QString&)), this, SLOT(slot_log(const QString&)));
  connect(tcp, SIGNAL(error(const QString&, const QString&)), this, SLOT(slot_error(const QString&, const QString&)));
  connect(tcp, SIGNAL(warning(const QString&)), this, SLOT(slot_warn(const QString&)));
  connect(tcp, SIGNAL(info(const QString&)), this, SLOT(slot_info(const QString&)));
  http = new QCDDB_HTTPTransport();
  connect(http, SIGNAL(queryDone(int)), this, SLOT(slot_query_done(int)));
  connect(http, SIGNAL(readDone()), this, SLOT(slot_read_done()));
  connect(http, SIGNAL(submitDone()), this, SLOT(slot_submit_done()));
  connect(http, SIGNAL(proxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*)), this, SLOT(slot_proxy_authentication_required(const QNetworkProxy&, QAuthenticator*)));
  connect(http, SIGNAL(log(const QString&)), this, SLOT(slot_log(const QString&)));
  connect(http, SIGNAL(error(const QString&, const QString&)), this, SLOT(slot_error(const QString&, const QString&)));
  connect(http, SIGNAL(warning(const QString&)), this, SLOT(slot_warn(const QString&)));
  connect(http, SIGNAL(info(const QString&)), this, SLOT(slot_info(const QString&)));
  _transport = HTTP;
  _enable_local_cache = TRUE;
  _enable_remote = TRUE;
  _in_local_cache = FALSE;
  to_go = -1;
}

QCDDB::~QCDDB() {
  if (tcp) delete tcp;
  if (http) delete http;
}

void QCDDB::query(const TrackOffsetList& trackOffsetList) {

  if (_enable_local_cache) {
    switch (_transport) {
      case CDDBP : _in_local_cache = tcp->readFromLocalCache(trackOffsetList); break;
      case HTTP : _in_local_cache = http->readFromLocalCache(trackOffsetList); break;
    }
    if (_in_local_cache) {
      switch (_transport) {
        case CDDBP : tcp->clearQueryResults(); break;
        case HTTP : http->clearQueryResults(); break;
      }
      slot_query_done(1);
      return;
    } else if (!_enable_remote) {
      emit error(trUtf8("No match found in local cache."),
	trUtf8("Please try to fetch information from a CDDB/FreeDB server."));
      return;
    }
  }

  if (_enable_remote) {
    emit busy(TRUE, 0);
    switch (_transport) {
      case CDDBP :
        tcp->clearQueryResults();
        if (!tcp->connected())
          { to_go = 0; to_go_trackoffsetlist = trackOffsetList; tcp->conn(); }
        else
          tcp->query(trackOffsetList);
        break;
      case HTTP :
        http->clearQueryResults();
        http->query(trackOffsetList);
        break;
    }
  }

}

void QCDDB::read(int no) {

  if (_in_local_cache) {
    slot_read_done();
    return;
  }

  if (_enable_remote) {
    emit busy(TRUE, 0);
    switch (_transport) {
      case CDDBP :
        if (!tcp->connected())
          { to_go = 1; to_go_no = no; tcp->conn(); }
        else
          tcp->read(no);
        break;
      case HTTP :
        http->read(no);
        break;
    }
  }

}

void QCDDB::submit(const QCDDA_MetaData& metadata) {
  if (_enable_remote) {
    emit busy(TRUE, 1);
    switch (_transport) {
      case CDDBP :
        if (!tcp->connected())
          { to_go = 2; to_go_metadata = metadata; tcp->conn(); }
        else
          tcp->submit(metadata);
        break;
      case HTTP :
        http->submit(metadata);
        break;
    }
  }
  if (_enable_local_cache) {
    switch (_transport) {
      case CDDBP :
        tcp->writeToLocalCache(metadata);
        break;
      case HTTP :
        http->writeToLocalCache(metadata);
        break;
    }
  }
}

void QCDDB::submitOnlyToCache(const QCDDA_MetaData& metadata) {
  if (_enable_local_cache) {
    switch (_transport) {
      case CDDBP :
        tcp->writeToLocalCache(metadata);
        break;
      case HTTP :
        http->writeToLocalCache(metadata);
        break;
    }
  }
}

void QCDDB::slot_tcp_connection_done() {
  emit info(trUtf8("Connection successfully done."));
  tcp->handshake();
}

void QCDDB::slot_tcp_handshake_done() {
  emit info(trUtf8("Handshake successfully done."));
  tcp->protocol();
}

void QCDDB::slot_tcp_protocol_done() {
  emit info(trUtf8("Protocol selection successfully done."));
  if (to_go==0)
    tcp->query(to_go_trackoffsetlist);
  else if (to_go==1)
    tcp->read(to_go_no);
  else if (to_go==2)
    tcp->submit(to_go_metadata);
  else
    emit busy(FALSE, 0);
  to_go = -1;
}

void QCDDB::slot_query_done(int matchcount) {
  if (_in_local_cache) {
    emit info(trUtf8("Query successfully done. Found match in local cache."));
  } else {
    emit busy(FALSE, 0);
    emit info(trUtf8("Query successfully done. Found %1 matches.").arg(matchcount));
  }
  emit queryDone(matchcount);
}

void QCDDB::slot_read_done() {
  emit busy(FALSE, 0);
  emit info(trUtf8("Read successfully done."));
  emit readDone();
  if (_enable_local_cache && !_in_local_cache) {
    switch (_transport) {
      case CDDBP : tcp->writeToLocalCache(tcp->readResult()); break;
      case HTTP : http->writeToLocalCache(http->readResult()); break;
    }
  }
  if (_transport==CDDBP) tcp->disconn();
}

void QCDDB::slot_submit_done() {
  emit busy(FALSE, 1);
  emit info(trUtf8("Submit successfully done."));
  emit submitDone();
  if (_transport==CDDBP) tcp->disconn();
}

void QCDDB::slot_proxy_authentication_required(const QNetworkProxy& proxy, QAuthenticator *authenticator) {
  emit proxyAuthenticationRequired(proxy, authenticator);
}

void QCDDB::slot_log(const QString& message) {
  emit log(message);
}

void QCDDB::slot_error(const QString& description, const QString& solution) {
  emit busy(FALSE, 0);
  emit error(description, solution);
  if (_transport==CDDBP) tcp->disconn();
}

void QCDDB::slot_warn(const QString& description) {
  emit warning(description);
}

void QCDDB::slot_info(const QString& description) {
  emit info(description);
}
