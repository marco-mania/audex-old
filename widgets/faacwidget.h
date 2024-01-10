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

#ifndef FAACWIDGET_H
#define FAACWIDGET_H

#include <QWidget>
#include <QDir>

#include <KDebug>
#include <KDialog>

#include "utils/error.h"
#include "utils/parameters.h"
#include "utils/encoderassistant.h"

#include "ui_faacwidgetUI.h"

class faacWidgetUI : public QWidget, public Ui::FAACWidgetUI {
public:
  explicit faacWidgetUI(QWidget *parent) : QWidget(parent) {
    setupUi(this);
  }
};

class faacWidget : public faacWidgetUI {
  Q_OBJECT
public:
  explicit faacWidget(Parameters *parameters, QWidget *parent = 0);
  ~faacWidget();
  Error lastError() const { return error; }
  inline bool isChanged() const { return changed; }
public slots:
  bool save();
signals:
  void triggerChanged();
private slots:
  void quality_changed_by_slider(int quality);
  void quality_changed_by_spinbox(int quality);
  void trigger_changed();
private:
  Parameters *parameters;
  Error error;
  bool changed;
};

#endif
