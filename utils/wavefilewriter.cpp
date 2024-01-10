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
#include "wavefilewriter.h"

WaveFileWriter::WaveFileWriter() {
  data_written = 0;
  _endianess = LittleEndian;
}

WaveFileWriter::~WaveFileWriter() {
  close();
}

bool WaveFileWriter::open(const QString& filename) {

  close();

  data_written = 0;

  output_file.setFileName(filename);
  if (output_file.open(QIODevice::WriteOnly)) {
    _filename = filename;
    write_empty_header();
    return true;
  } else {
    return false;
  }

}

bool WaveFileWriter::isOpen() {
  return output_file.isOpen();
}

QString WaveFileWriter::filename() const {
  return _filename;
}

void WaveFileWriter::setEndianess(const Endianess e) {
  _endianess = e;
}

WaveFileWriter::Endianess WaveFileWriter::endianess() {
  return _endianess;
}

void WaveFileWriter::close() {

  if (isOpen()) {
    if (data_written) {
      //update wave header
      update_header();
      output_file.close();
    } else {
      output_file.close();
      output_file.remove();
    }
  }
  _filename = QString::null;

}

void WaveFileWriter::write(const QByteArray& data) {

  int len = data.size();
  if (isOpen()) {
    if (_endianess == LittleEndian) {
      qint64 ret = output_file.write(data);
      if (ret==-1) {
        emit error(output_file.errorString());
        return;
      }
    } else {
      if (data.size() % 2 > 0) {
        kDebug() << "Data length is not a multible of 2! Cannot write data.";
        return;
      }
      //we need to swap the bytes
      char* buffer = new char[len];
      for (int i = 0; i < len-1; i += 2) {
        buffer[i] = data.data()[i+1];
        buffer[i+1] = data.data()[i];
      }
      output_file.write(buffer, len);
      delete [] buffer;
    }
    data_written += len;
  }

}

void WaveFileWriter::write_empty_header() {
  static const char riffHeader[] = {
    0x52, 0x49, 0x46, 0x46, // 0  "RIFF"
    0x00, 0x00, 0x00, 0x00, // 4  wavSize
    0x57, 0x41, 0x56, 0x45, // 8  "WAVE"
    0x66, 0x6d, 0x74, 0x20, // 12 "fmt "
    0x10, 0x00, 0x00, 0x00, // 16
    0x01, 0x00, 0x02, 0x00, // 20
    0x44, 0xac, 0x00, 0x00, // 24
    0x10, 0xb1, 0x02, 0x00, // 28
    0x04, 0x00, 0x10, 0x00, // 32
    0x64, 0x61, 0x74, 0x61, // 36 "data"
    0x00, 0x00, 0x00, 0x00  // 40 byteCount
  };
  output_file.write(riffHeader, 44);
}

void WaveFileWriter::update_header() {
  if (isOpen()) {
    output_file.flush();

    char c[4];
    qint32 wavSize = data_written+44-8;

    // jump to the wavSize position in the header

    output_file.seek(4);
    c[0] = (wavSize   >> 0 ) & 0xff;
    c[1] = (wavSize   >> 8 ) & 0xff;
    c[2] = (wavSize   >> 16) & 0xff;
    c[3] = (wavSize   >> 24) & 0xff;
    output_file.write(c, 4);

    output_file.seek(40);
    c[0] = (data_written   >> 0 ) & 0xff;
    c[1] = (data_written   >> 8 ) & 0xff;
    c[2] = (data_written   >> 16) & 0xff;
    c[3] = (data_written   >> 24) & 0xff;
    output_file.write(c, 4);

    // jump back to the end
    output_file.seek(output_file.size());
  }
}
