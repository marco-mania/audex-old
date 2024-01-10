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

#ifndef COVERBROWSERWIDGET_H
#define COVERBROWSERWIDGET_H

#include <QPixmap>
#include <QImage>

#include <KDialog>

#include "../utils/coverfetcher.h"

#include "ui_coverbrowserwidgetUI.h"

class CoverBrowserDialog : public KDialog {

  Q_OBJECT

public:
  CoverBrowserDialog(QImage *cover, const QString& locale = "us", QWidget *parent = 0);
  CoverBrowserDialog(QImage *cover, const int locale = 0, QWidget *parent = 0);
  ~CoverBrowserDialog();

public slots:
  void fetch(const QString& searchstring);

protected slots:
  virtual void slotButtonClicked(int button);

private slots:
  void select_this(QListWidgetItem* item);

  void enable_select_button();

  void add_item(const QImage& cover, const QString& caption, int no);
  void all_fetched();
  void nothing_fetched();

private:
  Ui::CoverBrowserWidgetUI ui;
  CoverFetcher cover_fetcher;
  QImage *cover;

  void setup(QImage *cover, const QString& locale);

};

#endif
