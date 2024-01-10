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

#ifndef CDDAHEADERWIDGET_H
#define CDDAHEADERWIDGET_H

#include <cmath>

#include <QDebug>
#include <QTimer>
#include <QWidget>
#include <QImage>
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QDesktopServices>
#include <QFontMetrics>
#include <QFont>

#include <KAction>
#include <KActionCollection>
#include <KFileDialog>
#include <KMenu>
#include <KLocale>
#include <KDebug>
#include <KStandardDirs>
#include <KUrl>

#include "preferences.h"

#include "../models/cddamodel.h"
#include "../dialogs/coverbrowserdialog.h"
#include "../dialogs/cddaheaderdatadialog.h"
#include "../utils/coverfetcher.h"

// fixed point defines
#define FP_BITS 10
#define FP_FACTOR (1 << FP_BITS)

enum FadeStyle {
    NoFade,
    FadeDown,
    FadeRight,
    FadeUp,
    FadeLeft
};

enum MirrorStyle {
    NoMirror,
    MirrorOverX,
    MirrorOverY
};

class CDDAHeaderWidget : public QWidget {

  Q_OBJECT

public:

  explicit CDDAHeaderWidget(CDDAModel *cddaModel, QWidget* parent = 0, const int coverSize = 128, const int padding = 20);
  ~CDDAHeaderWidget();
  QSize sizeHint() const;
  void setCover(const QImage& cover);

  bool isEnabled() const;

public slots:
  void setEnabled(bool enabled);

  void amazonAuto();

signals:
  void headerDataChanged();

  void coverUp();
  void coverDown();

protected:

  void paintEvent(QPaintEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void mousePressEvent(QMouseEvent *event);

private slots:

  void update();
  void trigger_repaint();
  void cover_is_down();

  void amazon();
  void load();
  void save();
  void view_cover();
  void remove();

  void edit_data();
  void wikipedia();

  void set_cover(const QImage& cover, const QString& caption, int no);

  void context_menu(const QPoint& point);

private:

  CDDAModel *cdda_model;
  KActionCollection *action_collection;
  int cover_size;
  int padding;

  QImage cover;
  QImage cover_holding;

  QTimer timer;
  bool animation_up;
  bool animation_down;
  qreal scale_factor;
  bool scale_up;
  bool scale_down;
  qreal opacity_factor;
  bool fade_in;
  bool fade_out;

  QRectF cover_rect;
  bool cursor_on_cover;

  QRectF link1_rect;
  bool cursor_on_link1;

  QRectF link2_rect;
  bool cursor_on_link2;

  void setup_actions();

  bool enabled;

  CoverFetcher cover_fetcher;

};

#endif
