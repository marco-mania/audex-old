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

#include "simplemaskwizarddialog.h"

simplemaskwizarddialog::simplemaskwizarddialog(QWidget *parent, QSettings *settings, QProfileModel *profile_model, QString *mask, const QString& extension) {

  Q_UNUSED(parent);

  setupUi(this);

  this->settings = settings;
  this->profile_model = profile_model;
  this->mask = mask;
  this->extension = extension;

  connect(buttonBox, SIGNAL(accepted()), this, SLOT(ok()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(cancel()));

  connect(pushButton, SIGNAL(clicked()), this, SLOT(insAlbumArtist()));
  pushButton->installEventFilter(this);
  connect(pushButton_3, SIGNAL(clicked()), this, SLOT(insAlbumTitle()));
  pushButton_3->installEventFilter(this);
  connect(pushButton_5, SIGNAL(clicked()), this, SLOT(insCDNo()));
  pushButton_5->installEventFilter(this);
  connect(pushButton_7, SIGNAL(clicked()), this, SLOT(insDate()));
  pushButton_7->installEventFilter(this);
  connect(pushButton_8, SIGNAL(clicked()), this, SLOT(insGenre()));
  pushButton_8->installEventFilter(this);

  connect(lineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(update_example()));

  if (!mask->isEmpty()) {
    lineEdit->setText(*mask);
    update_example();
  }

  label_3->clear();

  checkBox->setChecked(settings->value("simplemaskwizarddialog/show_help", TRUE).toBool());
  connect(checkBox, SIGNAL(toggled(bool)), this, SLOT(enable_help(bool)));
  enable_help(settings->value("simplemaskwizarddialog/show_help", TRUE).toBool());

  resize(settings->value("simplemaskwizarddialog/size", QSize(440, 400)).toSize());
  move(settings->value("simplemaskwizarddialog/pos", QPoint(100, 100)).toPoint());

}

simplemaskwizarddialog::~simplemaskwizarddialog() {
}

void simplemaskwizarddialog::insAlbumArtist() {
  QString text = lineEdit->text();
  text.insert(lineEdit->cursorPosition(), VAR_ALBUM_ARTIST);
  lineEdit->setText(text);
  update_example();
}

void simplemaskwizarddialog::insAlbumTitle() {
  QString text = lineEdit->text();
  text.insert(lineEdit->cursorPosition(), VAR_ALBUM_TITLE);
  lineEdit->setText(text);
  update_example();
}

void simplemaskwizarddialog::insCDNo() {
  QString text = lineEdit->text();
  text.insert(lineEdit->cursorPosition(), VAR_CDNO);
  lineEdit->setText(text);
  update_example();
}

void simplemaskwizarddialog::insDate() {
  QString text = lineEdit->text();
  text.insert(lineEdit->cursorPosition(), VAR_DATE);
  lineEdit->setText(text);
  update_example();
}

void simplemaskwizarddialog::insGenre() {
  QString text = lineEdit->text();
  text.insert(lineEdit->cursorPosition(), VAR_GENRE);
  lineEdit->setText(text);
  update_example();
}

void simplemaskwizarddialog::ok() {
  *mask = lineEdit->text();
  setResult(QDialog::Accepted);
  close();
}

void simplemaskwizarddialog::cancel() {
  setResult(QDialog::Rejected);
  close();
}

void simplemaskwizarddialog::update_example() {
  QMaskParser maskparser;
  QString filename = maskparser.parseSimpleMask(lineEdit->text(),
	1, "Meat Loaf", "Bat Out Of Hell III", "2006", "Rock", extension, FALSE);
  lineEdit_2->setText(filename);
}

void simplemaskwizarddialog::enable_help(bool enabled) {
  if (enabled) {
    frame_3->setVisible(TRUE);
    this->setMinimumSize(QSize(440, 400));
  } else {
    frame_3->setVisible(FALSE);
    this->setMinimumSize(QSize(440, 320));
    this->resize(QSize(this->width(), this->height()-80));
  }
}

bool simplemaskwizarddialog::eventFilter(QObject *target, QEvent *event) {
  if (target == pushButton) {
    if (event->type() == QEvent::Enter) {
      label_3->setText(trUtf8("This variable represents the artist or title of the <b>whole</b> album or compilation.\nDepending on the definition in your profile it could be anything else."));
    }
    if (event->type() == QEvent::Leave) {
      label_3->clear();
    }
  } else if (target == pushButton_3) {
    if (event->type() == QEvent::Enter) {
      label_3->setText(trUtf8("This variable represents the title or subtitle of the <b>whole</b> album or compilation.\nDepending on the definition in your profile it could be anything else."));
    }
    if (event->type() == QEvent::Leave) {
      label_3->clear();
    }
  } else if (target == pushButton_5) {
    if (event->type() == QEvent::Enter) {
      label_3->setText(trUtf8("This variable represents the cd number of an album or compilation (Multi-CD).\nNote 2: Append \".0\" to the variable means that a track number smaller than 10 will be filled with zeros. E.g. 01, 02, 03, 04... etc."));
    }
    if (event->type() == QEvent::Leave) {
      label_3->clear();
    }
  } else if (target == pushButton_7) {
    if (event->type() == QEvent::Enter) {
      label_3->setText(trUtf8("This variable represents just the release date of the cd. In almost all cases this is the year."));
    }
    if (event->type() == QEvent::Leave) {
      label_3->clear();
    }
  } else if (target == pushButton_8) {
    if (event->type() == QEvent::Enter) {
      label_3->setText(trUtf8("This variable represents the genre."));
    }
    if (event->type() == QEvent::Leave) {
      label_3->clear();
    }
  }
  return QWidget::eventFilter(target, event);
}

void simplemaskwizarddialog::closeEvent(QCloseEvent *event) {
  settings->setValue("simplemaskwizarddialog/pos", pos());
  settings->setValue("simplemaskwizarddialog/size", size());
  settings->setValue("simplemaskwizarddialog/show_help", checkBox->isChecked());
  event->accept();
}
