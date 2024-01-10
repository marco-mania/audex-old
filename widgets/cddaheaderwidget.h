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

#ifndef CDDAHEADERWIDGET_H
#define CDDAHEADERWIDGET_H

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

static inline QImage mirrorImage(const QImage &img, MirrorStyle mirrorStyle = MirrorOverX, FadeStyle fadeStyle = FadeDown) {
/****************************************************************************
**
** Copyright (C) 2007-2007 Trolltech ASA. All rights reserved.
**
** This function is part of the Graphics Dojo project on Trolltech Labs.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

  QImage tmpImage = img;
  if (mirrorStyle != NoMirror) tmpImage = tmpImage.mirrored(mirrorStyle == MirrorOverY, mirrorStyle == MirrorOverX);

  if (fadeStyle != NoFade) {

    QPoint p1, p2;

    if (fadeStyle == FadeDown)
      p2.setY(tmpImage.height());
    else if (fadeStyle == FadeUp)
      p1.setY(tmpImage.height());
    else if (fadeStyle == FadeRight)
      p2.setX(tmpImage.width());
    else if (fadeStyle == FadeLeft)
      p1.setX(tmpImage.width());

    QLinearGradient gradient(p1, p2);
    gradient.setColorAt(0, Qt::white);
    gradient.setColorAt(0.2, QColor(0, 0, 0, 20));
    gradient.setColorAt(1, Qt::transparent);

    QPainter p(&tmpImage);
    p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    p.fillRect(0, 0, tmpImage.width(), tmpImage.height(), gradient);
    p.end();

  }

  return tmpImage;

}

static inline QImage fadeImage(const QImage &img, float val, const QColor &color) {

  /* This function is part of the KDE libraries
  Copyright (C) 1998, 1999, 2001, 2002 Daniel M. Duley <mosfet@interaccess.com>
  (C) 1998, 1999 Christian Tibirna <ctibirna@total.net>
  (C) 1998, 1999 Dirk Mueller <mueller@kde.org> */

  QImage tmpImage = img;

  if (tmpImage.width() == 0 || tmpImage.height() == 0) return tmpImage;

  // We don't handle bitmaps
  if (tmpImage.depth() == 1) return tmpImage;

  unsigned char tbl[256];
  for (int i = 0; i < 256; ++i) tbl[i] = (int)(val * i + 0.5);

  int red = color.red();
  int green = color.green();
  int blue = color.blue();

  QRgb col;
  int r, g, b, cr, cg, cb;

  if (tmpImage.depth() <= 8) {
  // pseudo color
    for (int i=0; i<tmpImage.numColors(); i++) {
      col = tmpImage.color(i);
      cr = qRed(col); cg = qGreen(col); cb = qBlue(col);
      if (cr > red)
        r = cr - tbl[cr-red];
      else
        r = cr + tbl[red-cr];
      if (cg > green)
        g = cg - tbl[cg-green];
      else
        g = cg + tbl[green-cg];
      if (cb > blue)
        b = cb - tbl[cb-blue];
      else
        b = cb + tbl[blue-cb];
        tmpImage.setColor(i, qRgba(r, g, b, qAlpha(col)));

    }

  } else {

    // truecolor
    for (int y=0; y<tmpImage.height(); ++y) {
      QRgb *data = (QRgb*)tmpImage.scanLine(y);
      for (int x=0; x<tmpImage.width(); ++x) {
        col = *data;
        cr = qRed(col); cg = qGreen(col); cb = qBlue(col);
        if (cr > red)
          r = cr - tbl[cr-red];
        else
          r = cr + tbl[red-cr];
        if (cg > green)
          g = cg - tbl[cg-green];
        else
          g = cg + tbl[green-cg];
        if (cb > blue)
          b = cb - tbl[cb-blue];
        else
          b = cb + tbl[blue-cb];
        *data++ = qRgba(r, g, b, qAlpha(col));
      }
    }

  }

  return tmpImage;

}

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

};

#endif
