/* AUDEX CDDA EXTRACTOR
 * Copyright (C) 2007-2008 by Marco Nelles (marcomaniac@gmx.de)
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
#include "hashlist.h"

Hashlist::Hashlist() {

}

const QStringList Hashlist::getSFV(const QStringList& filenames) {

  QStringList list;

  for (int i = 0; i < filenames.count(); ++i) {

    QFile file(filenames.at(i));
    if (!file.exists()) continue;
    if (!file.open(QFile::ReadOnly)) continue;

    int size = file.size();
    char buf[size];
    file.read(buf, size);
    CRC32 crc32;
    crc32.update(reinterpret_cast<const unsigned char*>(buf), size);
    QFileInfo info(filenames.at(i));
    list << info.fileName()+" "+QString("%1").arg(crc32.result(), 0, 16);

    file.close();

  }

  return list;

}
