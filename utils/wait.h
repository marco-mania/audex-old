/* AUDEX CDDA EXTRACTOR
 * Copyright (C) 2007-2011 Marco Nelles (audex@maniatek.com)
 * <http://kde.maniatek.com/audex>
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

#ifndef WAIT_HEADER
#define WAIT_HEADER

#include <QThread>

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
