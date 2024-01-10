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

#ifndef OGGENCWIDGET_H
#define OGGENCWIDGET_H

#include <QWidget>
#include <QDir>

#include <KDebug>
#include <KDialog>

#include "utils/error.h"
#include "utils/parameters.h"
#include "utils/encoderassistant.h"

#include "ui_oggencwidgetUI.h"

class oggencWidgetUI : public QWidget, public Ui::OggEncWidgetUI {
public:
  explicit oggencWidgetUI(QWidget *parent) : QWidget(parent) {
    setupUi(this);
  }
};

class oggencWidget : public oggencWidgetUI {
  Q_OBJECT
public:
  explicit oggencWidget(Parameters *parameters, QWidget *parent = 0);
  ~oggencWidget();
  inline Error lastError() const { return error; }
  inline bool isChanged() const { return changed; }
public slots:
  bool save();
signals:
  void triggerChanged();
private slots:
  void quality_changed_by_slider(int quality);
  void quality_changed_by_spinbox(int quality);
  void set_bitrate(int quality);
  void enable_min_bitrate(bool enable);
  void enable_max_bitrate(bool enable);
  void trigger_changed();
private:
  Parameters *parameters;
  Error error;
  bool changed;
};

#endif
