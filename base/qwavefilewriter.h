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

#ifndef QWAVEFILEWRITER_HEADER
#define QWAVEFILEWRITER_HEADER

#include <QtCore>

class QWaveFileWriter : public QObject {
  Q_OBJECT
public:
  QWaveFileWriter();
  ~QWaveFileWriter();

  enum Endianess { BigEndian, LittleEndian };

  /**
   * open a new wave file.
   * closes any opened file.
   */
  bool open(const QString& filename);
  bool isOpen();
  QString filename() const;

  void setEndianess(const Endianess e);
  const Endianess endianess();

  /**
   * closes the file.
   * Length of the wave file will be written into the header.
   * If no data has been written to the file except the header
   * it will be removed.
   */
  void close();

public slots:
  /**
   * write 16bit samples to the file.
   */
  void write(const QByteArray& data);

signals:
  void error(const QString& errorstr);

private:
  void write_empty_header();
  void update_header();

  Endianess _endianess;

  QFile output_file;
  QDataStream output_stream;
  QString _filename;

  long data_written;

};

#endif
