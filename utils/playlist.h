/* AUDEX CDDA EXTRACTOR
 * Copyright (C) 2007-2009 Marco Nelles (audex@maniatek.de)
 * <http://opensource.maniatek.de/audex>
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

#ifndef PLAYLIST_HEADER
#define PLAYLIST_HEADER

#include <QString>
#include <QStringList>
#include <QDomDocument>

#include <KDebug>
#include <KLocale>

#include "../models/cddamodel.h"

class Playlist : public QObject {
  Q_OBJECT
public:
  Playlist(const QStringList& filenames, CDDAModel *model);
  ~Playlist();

  QStringList asM3U(bool absolutePath = FALSE, bool merge = FALSE) const;
  QStringList asPLS(bool absolutePath = FALSE) const;
  QStringList asXSPF() const;

private:
  QStringList filenamelist;
  CDDAModel *model;

};

#endif
