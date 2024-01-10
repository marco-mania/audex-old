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

#ifndef WAIT_HEADER
#define WAIT_HEADER

#include <QtCore>

class Wait : public QThread {
protected:
  Wait(int time) : QThread() { Q_UNUSED(time); }
  void run() {}
public:
  static void sleep(int time) { QThread::sleep(time); }
  static void msleep(int time) { QThread::msleep(time); }
  static void usleep(int time) { QThread::usleep( time); }
};

#endif
