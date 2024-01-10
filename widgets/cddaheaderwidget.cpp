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

#include "cddaheaderwidget.h"

CDDAHeaderWidget ::CDDAHeaderWidget(CDDAModel *cddaModel, QWidget* parent, const int coverSize, const int padding) : QWidget(parent) {

  cdda_model = cddaModel;
  if (!cdda_model) {
    kDebug() << "CDDAModel is NULL!";
    return;
  }
  connect(cdda_model, SIGNAL(modelReset()), this, SLOT(update()));

  setup_actions();

  this->cover_size = coverSize;

  this->padding = padding;

  animation_up = FALSE;
  animation_down = FALSE;
  scale_up = FALSE;
  scale_down = FALSE;
  fade_in = FALSE;
  fade_out = FALSE;
  scale_factor = 1.0;
  opacity_factor = 1.0;

  setMouseTracking(TRUE);
  cursor_on_cover = FALSE;
  cursor_on_link1 = FALSE;
  cursor_on_link2 = FALSE;

  connect(this, SIGNAL(coverDown()), this, SLOT(cover_is_down()));
  connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(context_menu(const QPoint&)));

  setContextMenuPolicy(Qt::CustomContextMenu);

  timer.setInterval(40);
  connect(&timer, SIGNAL(timeout()), this, SLOT(trigger_repaint()));

  setMinimumSize(QSize(500, (int)(cover_size*1.6)));

  update();

}

CDDAHeaderWidget::~CDDAHeaderWidget() {
  delete action_collection;
}

QSize CDDAHeaderWidget::sizeHint() const {
  return QSize(500, (int)(cover_size*1.6));
}

void CDDAHeaderWidget::setCover(const QImage& cover) {

  if (this->cover.isNull()) {
    this->cover = cover.convertToFormat(QImage::Format_ARGB32_Premultiplied);
    animation_up = TRUE;
    fade_in = TRUE;
    scale_factor = 0.7;
    opacity_factor = 0.0;
  } else {
    this->cover_holding = cover.convertToFormat(QImage::Format_ARGB32_Premultiplied);
    animation_down = TRUE;
    scale_down = TRUE;
    scale_factor = 1.0;
    opacity_factor = 1.0;
  }

  timer.start();

}

bool CDDAHeaderWidget::isEnabled() const {
  return enabled;
}

void CDDAHeaderWidget::setEnabled(bool enabled) {
  this->enabled = enabled;
  repaint();
}

void CDDAHeaderWidget::paintEvent(QPaintEvent *event) {

  Q_UNUSED(event);

  QPainter p;

  p.begin(this);
  p.fillRect(rect(), Qt::black);

  if (enabled) {

    QImage scaled_cover = cover.scaled((int)(scale_factor*cover_size), (int)(scale_factor*cover_size), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    QImage faded_cover = fadeImage(scaled_cover, 1-opacity_factor, QColor(Qt::black));

    p.drawImage((cover_size/2-(scaled_cover.width()/2))+padding, (cover_size/2-(scaled_cover.height()/2))+padding, faded_cover);
    cover_rect = QRectF(padding, padding, scaled_cover.width(), scaled_cover.height());

    QImage mirror_img = mirrorImage(faded_cover);
    //we shear it a along the x axis to produce the nice leaning to the side effect. (valid only for MirrorOverX)
    mirror_img = mirror_img.transformed(QMatrix().shear(0.2, 0.0), Qt::SmoothTransformation);

    p.drawImage((cover_size/2-(scaled_cover.width()/2))+padding, (cover_size/2-(scaled_cover.height()/2))+padding+mirror_img.height()+8, mirror_img);

    p.setPen(QPen(Qt::white));

    QFont font;
    font.setPixelSize(18);
    font.setBold(TRUE);
    p.setFont(font);
    p.drawText(60+cover_size, padding+font.pixelSize(), cdda_model->artist());

    font.setPixelSize(12);
    font.setBold(TRUE);
    font.setItalic(TRUE);
    p.setFont(font);
    p.drawText(60+cover_size, padding+25+font.pixelSize(), cdda_model->title());

    font.setBold(FALSE);
    font.setItalic(FALSE);
    p.setFont(font);

    int i = 55;
    if (!cdda_model->year().isEmpty()) {
      p.drawText(60+cover_size, padding+i+font.pixelSize(), i18n("Release Year: "));
      p.drawText(180+cover_size, padding+i+font.pixelSize(), cdda_model->year());
      i += 14;
    }
    if (!cdda_model->genre().isEmpty()) {
      p.drawText(60+cover_size, padding+i+font.pixelSize(), i18n("Genre: "));
      p.drawText(180+cover_size, padding+i+font.pixelSize(), cdda_model->genre());
      i += 14;
    }
    if (cdda_model->isMultiCD()) {
      p.drawText(60+cover_size, padding+i+font.pixelSize(), i18n("CD Number: "));
      p.drawText(180+cover_size, padding+i+font.pixelSize(), QString("%1").arg(cdda_model->cdNum()));
      i += 14;
    }

    font.setUnderline(TRUE);
    p.setFont(font);

    //links

    QFontMetrics fm(font);
    QString link1 = i18n("Edit Data");
    QString link2 = i18n("Wikipedia");
    p.drawText(60+cover_size, padding+cover_size-12, fm.width(link1), 16, Qt::TextSingleLine, link1);
    p.drawText(68+fm.width(link1)+cover_size, padding+cover_size-12, fm.width(link2), 16, Qt::TextSingleLine, link2);
    link1_rect = QRectF(60+cover_size, padding+cover_size-12, fm.width(link1), 12);
    link2_rect = QRectF(68+fm.width(link1)+cover_size, padding+cover_size-12, fm.width(link2), 12);

  } else { //disabled

    QFont font;
    font.setPixelSize(18);
    font.setBold(TRUE);
    font.setItalic(TRUE);
    p.setFont(font);
    p.drawText(rect(), Qt::AlignCenter | Qt::AlignVCenter, i18n("No CD in drive"));

  }

  p.end();

}

void CDDAHeaderWidget::mouseMoveEvent(QMouseEvent *event) {

  if (cover_rect.contains(event->pos())) {

    if (!cursor_on_cover) { QApplication::setOverrideCursor(QCursor(Qt::PointingHandCursor)); cursor_on_cover = TRUE; }

  } else if (link1_rect.contains(event->pos())) {

    if (!cursor_on_link1) { QApplication::setOverrideCursor(QCursor(Qt::PointingHandCursor)); cursor_on_link1 = TRUE; }

  } else if (link2_rect.contains(event->pos())) {

    if (!cursor_on_link2) { QApplication::setOverrideCursor(QCursor(Qt::PointingHandCursor)); cursor_on_link2 = TRUE; }

  } else {

    if (cursor_on_cover) {
      QApplication::restoreOverrideCursor();
      cursor_on_cover = FALSE;
    } else if (cursor_on_link1) {
      QApplication::restoreOverrideCursor();
      cursor_on_link1 = FALSE;
    } else if (cursor_on_link2) {
      QApplication::restoreOverrideCursor();
      cursor_on_link2 = FALSE;
    }

  }

}

void CDDAHeaderWidget::mousePressEvent(QMouseEvent *event) {
  if (event->button()==Qt::LeftButton) {
    if (cursor_on_cover) {
      if (cdda_model->cover().isNull()) amazon(); else view_cover();
    }
    if (cursor_on_link1) edit_data();
    if (cursor_on_link2) wikipedia();
  }
}

void CDDAHeaderWidget::update() {

  action_collection->action("fetch")->setEnabled(cdda_model->discInfo()!=CDDAModel::DiscNoInfo);

  QImage tmp;
  if (cdda_model->cover().isNull()) {
    tmp = QImage(KStandardDirs::locate("data", QString("audex/images/nocover.png")));
    tmp.setText("", "nocover");
    action_collection->action("save")->setEnabled(FALSE);
    action_collection->action("view")->setEnabled(FALSE);
    action_collection->action("remove")->setEnabled(FALSE);
  } else {
    tmp = cdda_model->cover();
    tmp.setText("", QString("%1").arg(tmp.cacheKey()));
    action_collection->action("save")->setEnabled(TRUE);
    action_collection->action("view")->setEnabled(TRUE);
    action_collection->action("remove")->setEnabled(TRUE);
  }

  if (tmp.text() != cover.text()) setCover(tmp);

  repaint();

}

void CDDAHeaderWidget::trigger_repaint() {

  if (animation_down) {

    if (scale_down) {
      scale_factor -= 0.08;
      if (qFuzzyCompare(scale_factor, .7) || scale_factor < .7) {
        scale_down = FALSE;
	fade_out = TRUE;
      }
    } else if (fade_out) {
      opacity_factor -= 0.16;
      if (qFuzzyCompare(opacity_factor, 0) || opacity_factor < 0) {
        opacity_factor = 0;
	fade_out = FALSE;
	animation_down = FALSE;
	timer.stop();
	emit coverDown();
      }
    }

  } else if (animation_up) {

    if (fade_in) {
      opacity_factor += 0.16;
      if (qFuzzyCompare(opacity_factor, 1) || opacity_factor > 1) {
        opacity_factor = 1;
	fade_in = FALSE;
	scale_up = TRUE;
      }
    } else if (scale_up) {
      scale_factor += 0.08;
      if (qFuzzyCompare(scale_factor, 1) || scale_factor > 1) {
        scale_up = FALSE;
	animation_up = FALSE;
	timer.stop();
	emit coverUp();
      }
    }

  }

  repaint();

}

void CDDAHeaderWidget::cover_is_down() {
  this->cover = cover_holding;
  animation_up = TRUE;
  fade_in = TRUE;
  scale_factor = .7;
  opacity_factor = 0.0;
  timer.start();
}

void CDDAHeaderWidget::amazon() {

  if (cdda_model->discInfo()==CDDAModel::DiscNoInfo) return;

  QApplication::restoreOverrideCursor();
  cursor_on_cover = FALSE;

  QImage cover;
  CoverBrowserDialog *dialog = new CoverBrowserDialog(&cover, Preferences::amazonLocale(), this);

  dialog->fetch(QString("%1 - %2").arg(cdda_model->artist()).arg(cdda_model->title()));

  if (dialog->exec()==QDialog::Accepted) {
    cdda_model->setCover(cover);
    delete dialog;
    update();
  } else {
    delete dialog;
  }

}

void CDDAHeaderWidget::load() {
  QString cover = KFileDialog::getOpenFileName(KUrl(QDir::homePath()), "image/png image/jpeg image/jpg image/gif image/bmp", this, i18n("Load cover"));
  if (!cover.isEmpty()) {
    cdda_model->setCover(QImage(cover));
    update();
  }
}

void CDDAHeaderWidget::save() {
  QString cover = KFileDialog::getSaveFileName(KUrl(QDir::homePath()), "image/png image/jpeg image/jpg image/gif image/bmp", this, i18n("Save cover"));
  if (!cover.isEmpty()) {
    cdda_model->cover().save(cover);
  }
}

void CDDAHeaderWidget::view_cover() {
  KDialog *dialog = new KDialog(this);
  dialog->setCaption(i18n("View cover"));
  dialog->setButtons(KDialog::Ok);
  QLabel *label = new QLabel();
  label->setScaledContents(TRUE);
  label->setPixmap(QPixmap::fromImage(cdda_model->cover()));
  dialog->setMainWidget(label);
  dialog->exec();
  delete label;
  delete dialog;
}

void CDDAHeaderWidget::remove() {
  cdda_model->clearCover();
  update();
}

void CDDAHeaderWidget::edit_data() {

  QApplication::restoreOverrideCursor();
  cursor_on_link1 = FALSE;

  CDDAHeaderDataDialog *dialog = new CDDAHeaderDataDialog(cdda_model, this);

  if (dialog->exec() != QDialog::Accepted) { delete dialog; return; }
  delete dialog;
  update();
  emit headerDataChanged();

}

void CDDAHeaderWidget::wikipedia() {
  //QDesktopServices::openUrl(QUrl(QString("http://%1.wikipedia.org/wiki/").arg("de") + KUrl::toPercentEncoding(cdda_model->artist())));

  int locale = Preferences::wikipediaLocale();
  QString l;
  if (locale==0) l = "en";
  if (locale==1) l = "de";
  if (locale==2) l = "fr";
  if (locale==3) l = "pl";
  if (locale==4) l = "ja";
  if (locale==5) l = "it";
  if (locale==6) l = "nl";
  if (locale==7) l = "es";
  if (locale==8) l = "pt";
  if (locale==9) l = "sv";

  if (l.isEmpty()) l = "en";

  QDesktopServices::openUrl(QUrl(QString("http://%1.wikipedia.org/wiki/").arg(l) + KUrl::toPercentEncoding(cdda_model->artist())));

}

void CDDAHeaderWidget::context_menu(const QPoint& point) {
  kDebug() << "context menu requested at point" << point;
  if (cursor_on_cover) {
    QApplication::restoreOverrideCursor();
    cursor_on_cover = FALSE;
    KMenu contextMenu(this);
    QMouseEvent *mevent = new QMouseEvent(QEvent::MouseButtonPress, point, Qt::RightButton, Qt::RightButton, Qt::NoModifier);
    contextMenu.clear();
    contextMenu.addAction(action_collection->action("fetch"));
    contextMenu.addAction(action_collection->action("load"));
    contextMenu.addAction(action_collection->action("save"));
    contextMenu.addSeparator();
    contextMenu.addAction(action_collection->action("view"));
    contextMenu.addSeparator();
    contextMenu.addAction(action_collection->action("remove"));
    contextMenu.exec(mevent->globalPos());
  }
}

void CDDAHeaderWidget::setup_actions() {

  action_collection = new KActionCollection(this);

  KAction* fetchCoverAction = new KAction(this);
  fetchCoverAction->setText(i18n("Fetch cover from amazon..."));
  action_collection->addAction("fetch", fetchCoverAction);
  connect(fetchCoverAction, SIGNAL(triggered(bool)), this, SLOT(amazon()));

  KAction* loadCoverAction = new KAction(this);
  loadCoverAction->setText(i18n("Load own cover..."));
  action_collection->addAction("load", loadCoverAction);
  connect(loadCoverAction, SIGNAL(triggered(bool)), this, SLOT(load()));

  KAction* saveCoverAction = new KAction(this);
  saveCoverAction->setText(i18n("Save cover to file..."));
  action_collection->addAction("save", saveCoverAction);
  connect(saveCoverAction, SIGNAL(triggered(bool)), this, SLOT(save()));

  KAction* viewCoverAction = new KAction(this);
  viewCoverAction->setText(i18n("Show full size cover..."));
  action_collection->addAction("view", viewCoverAction);
  connect(viewCoverAction, SIGNAL(triggered(bool)), this, SLOT(view_cover()));

  KAction* removeCoverAction = new KAction(this);
  removeCoverAction->setText(i18n("Remove cover"));
  action_collection->addAction("remove", removeCoverAction);
  connect(removeCoverAction, SIGNAL(triggered(bool)), this, SLOT(remove()));

}
