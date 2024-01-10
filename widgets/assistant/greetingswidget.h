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

#ifndef GREETINGSWIDGET_H
#define GREETINGSWIDGET_H

#include <QWidget>

#include <KDebug>
#include <KDialog>

#include "ui_greetingswidgetUI.h"

class greetingsWidgetUI : public QWidget, public Ui::GreetingsWidgetUI {
public:
  explicit greetingsWidgetUI(QWidget *parent) : QWidget(parent) {
    setupUi(this);
  }
};

class greetingsWidget : public greetingsWidgetUI {
  Q_OBJECT
public:
  explicit greetingsWidget(QWidget *parent = 0);
  ~greetingsWidget();
};

#endif
