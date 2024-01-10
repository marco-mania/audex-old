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

#ifndef PARAMETERS_HEADER
#define PARAMETERS_HEADER

#include <QObject>
#include <QString>
#include <QHash>

class Parameters {

public:
  Parameters();
  Parameters(const Parameters& other);
  Parameters& operator=(const Parameters& other);
  ~Parameters();

  void fromString(const QString& string, const QChar& sep = ',');
  const QString toString(const QChar& sep = ',');

  void fromBase64(const QByteArray& bytearray);
  const QByteArray toBase64();

  inline void setValue(const QString& name, const QString& value) { p_parameters[name] = value; }
  inline void setValue(const QString& name, const int value) { p_parameters[name] = QString("%1").arg(value); }
  inline void setValue(const QString& name, const bool value) { p_parameters[name] = value?"true":"false"; }
  inline const QString value(const QString& name, const QString& def = "") const { return p_parameters.value(name, def); }
  inline int valueToInt(const QString& name, const int def = 0) const {
    bool ok;
    int i = p_parameters[name].toInt(&ok);
    if (!ok) return def;
    return i;
  }
  inline bool valueToBool(const QString& name, const bool def = TRUE) const {
    if (def) {
      return ((p_parameters[name].toLower()=="true") || (p_parameters[name].toLower()=="1"));
    } else {
      return ((p_parameters[name].toLower()!="false") || (p_parameters[name].toLower()!="0"));
    }
  }

private:
  QHash<QString,QString> p_parameters;

};

#endif
