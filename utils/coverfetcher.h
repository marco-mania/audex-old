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

#ifndef COVERFETCHER_HEADER
#define COVERFETCHER_HEADER

#include <QtXml>
#include <QObject>

#include <KDebug>
#include <KLocale>
#include <KUrl>
#include <KIO/Job>
#include <KIO/SimpleJob>
#include <KIO/TransferJob>

class CoverFetcher : public QObject {
  Q_OBJECT
public:
  CoverFetcher(QObject *parent = 0);
  ~CoverFetcher();

  void startFetch(const QString& searchstring);

  QImage cover(int index);
  QString caption(int index);
  int count() { return cover_names.count(); }

  //us = International; fr = France; de = Germany; jp = Japan; uk = United Kingdom; ca = Canada
  void setLocale(const QString &locale);

  enum Status {
    NOS,
    SEARCHING,
    FETCHING
  };

signals:
  void fetched(const QImage& cover, const QString& caption, int no);
  void allFetched();
  void nothingFetched();

  void statusChanged(Status status);

  void error(const QString& description,
	const QString& solution = QString());
  void warning(const QString& description);
  void info(const QString& description);

private slots:
  void fetched_xml_data(KJob* job);

private:
  QStringList cover_asins;
  QStringList amazon_urls;
  QStringList cover_urls;
  QStringList cover_names;
  QList<QImage*> covers;
  void clear_covers() { for (int i = 0; i < covers.count(); i++) delete covers[i]; covers.clear(); }

  KIO::TransferJob* job;

  QString locale;

  Status _status;

  int f_i;

  void parse_xml_respone(const QString& xml);
  void parse_item_node(const QDomNode& node);
  bool fetch_covers();

};

#endif
