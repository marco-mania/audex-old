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

#include "commandmaskwizarddialog.h"

commandmaskwizarddialog::commandmaskwizarddialog(QWidget *parent, QSettings *settings, QProfileModel *profile_model, QString *mask) {

  Q_UNUSED(parent);

  setupUi(this);

  this->settings = settings;
  this->profile_model = profile_model;
  this->mask = mask;

  connect(buttonBox, SIGNAL(accepted()), this, SLOT(ok()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(cancel()));

  connect(pushButton, SIGNAL(clicked()), this, SLOT(insAlbumArtist()));
  pushButton->installEventFilter(this);
  connect(pushButton_3, SIGNAL(clicked()), this, SLOT(insAlbumTitle()));
  pushButton_3->installEventFilter(this);
  connect(pushButton_11, SIGNAL(clicked()), this, SLOT(insTrackArtist()));
  pushButton_11->installEventFilter(this);
  connect(pushButton_2, SIGNAL(clicked()), this, SLOT(insTrackTitle()));
  pushButton_2->installEventFilter(this);
  connect(pushButton_4, SIGNAL(clicked()), this, SLOT(insTrackNo()));
  pushButton_4->installEventFilter(this);
  connect(pushButton_5, SIGNAL(clicked()), this, SLOT(insCDNo()));
  pushButton_5->installEventFilter(this);
  connect(pushButton_7, SIGNAL(clicked()), this, SLOT(insDate()));
  pushButton_7->installEventFilter(this);
  connect(pushButton_8, SIGNAL(clicked()), this, SLOT(insGenre()));
  pushButton_8->installEventFilter(this);
  connect(pushButton_9, SIGNAL(clicked()), this, SLOT(insOutFile()));
  pushButton_9->installEventFilter(this);
  connect(pushButton_6, SIGNAL(clicked()), this, SLOT(insInFile()));
  pushButton_6->installEventFilter(this);

  connect(lineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(update_example()));

  if (!mask->isEmpty()) {
    lineEdit->setText(*mask);
    update_example();
  }

  label_3->clear();

  checkBox->setChecked(settings->value("commandmaskwizarddialog/show_help", TRUE).toBool());
  connect(checkBox, SIGNAL(toggled(bool)), this, SLOT(enable_help(bool)));
  enable_help(settings->value("commandmaskwizarddialog/show_help", TRUE).toBool());

  resize(settings->value("commandmaskwizarddialog/size", QSize(440, 550)).toSize());
  move(settings->value("commandmaskwizarddialog/pos", QPoint(100, 100)).toPoint());

}

commandmaskwizarddialog::~commandmaskwizarddialog() {
}

void commandmaskwizarddialog::insAlbumArtist() {
  QString text = lineEdit->text();
  text.insert(lineEdit->cursorPosition(), VAR_ALBUM_ARTIST);
  lineEdit->setText(text);
  update_example();
}

void commandmaskwizarddialog::insAlbumTitle() {
  QString text = lineEdit->text();
  text.insert(lineEdit->cursorPosition(), VAR_ALBUM_TITLE);
  lineEdit->setText(text);
  update_example();
}

void commandmaskwizarddialog::insTrackArtist() {
  QString text = lineEdit->text();
  text.insert(lineEdit->cursorPosition(), VAR_TRACK_ARTIST);
  lineEdit->setText(text);
  update_example();
}

void commandmaskwizarddialog::insTrackTitle() {
  QString text = lineEdit->text();
  text.insert(lineEdit->cursorPosition(), VAR_TRACK_TITLE);
  lineEdit->setText(text);
  update_example();
}

void commandmaskwizarddialog::insTrackNo() {
  QString text = lineEdit->text();
  text.insert(lineEdit->cursorPosition(), VAR_TRACK_NO);
  lineEdit->setText(text);
  update_example();
}

void commandmaskwizarddialog::insCDNo() {
  QString text = lineEdit->text();
  text.insert(lineEdit->cursorPosition(), VAR_CDNO);
  lineEdit->setText(text);
  update_example();
}

void commandmaskwizarddialog::insDate() {
  QString text = lineEdit->text();
  text.insert(lineEdit->cursorPosition(), VAR_DATE);
  lineEdit->setText(text);
  update_example();
}

void commandmaskwizarddialog::insGenre() {
  QString text = lineEdit->text();
  text.insert(lineEdit->cursorPosition(), VAR_GENRE);
  lineEdit->setText(text);
  update_example();
}

void commandmaskwizarddialog::insInFile() {
  QString text = lineEdit->text();
  text.insert(lineEdit->cursorPosition(), VAR_INPUT_FILE);
  lineEdit->setText(text);
  update_example();
}

void commandmaskwizarddialog::insOutFile() {
  QString text = lineEdit->text();
  text.insert(lineEdit->cursorPosition(), VAR_OUTPUT_FILE);
  lineEdit->setText(text);
  update_example();
}

void commandmaskwizarddialog::ok() {
  *mask = lineEdit->text();
  setResult(QDialog::Accepted);
  close();
}

void commandmaskwizarddialog::cancel() {
  setResult(QDialog::Rejected);
  close();
}

void commandmaskwizarddialog::update_example() {
  QMaskParser maskparser;
  QString filename = maskparser.parseCommandMask(lineEdit->text(),
	"/tmp/tmp.wav", QString("%1/music/Meat Loaf/02 - Meat Loaf - Blind As A Bat.ogg").arg(QDir::homePath()),
	2, 1, 1,
	"Meat Loaf", "Bat Out Of Hell III", "Meat Loaf", "Blind As A Bat",
	"2006", "Rock", "ogg", "~", FALSE);
  lineEdit_2->setText(filename);
  filename = maskparser.parseCommandMask(lineEdit->text(),
	"/tmp/tmp.wav", QString("%1/music/Bravo Hits/Volume 41/04 - Wolfsheim - Kein Zurueck.ogg").arg(QDir::homePath()),
	4, 2, 1,
	"Bravo Hits", "Volume 41", "Wolfsheim", "Kein Zurueck",
	"2003", "Darkwave", "ogg", "~", FALSE);
  lineEdit_3->setText(filename);
}

void commandmaskwizarddialog::enable_help(bool enabled) {
  if (enabled) {
    frame_3->setVisible(TRUE);
    this->setMinimumSize(QSize(440, 550));
  } else {
    frame_3->setVisible(FALSE);
    this->setMinimumSize(QSize(440, 410));
    this->resize(QSize(this->width(), this->height()-140));
  }
}

bool commandmaskwizarddialog::eventFilter(QObject *target, QEvent *event) {
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
  } else if (target == pushButton_11) {
    if (event->type() == QEvent::Enter) {
      label_3->setText(trUtf8("This variable represents the artist of a single track. It makes sense especially on compilations with different artists each track."));
    }
    if (event->type() == QEvent::Leave) {
      label_3->clear();
    }
  } else if (target == pushButton_2) {
    if (event->type() == QEvent::Enter) {
      label_3->setText(trUtf8("This variable represents the title of a single track. Normally each track on a cd holds an own title."));
    }
    if (event->type() == QEvent::Leave) {
      label_3->clear();
    }
  } else if (target == pushButton_4) {
    if (event->type() == QEvent::Enter) {
      label_3->setText(trUtf8("This variable represents the track number of a single track.\nNote: Append \".0\" to the variable means that a track number smaller than 10 will be filled with zeros. E.g. 01, 02, 03, 04... etc. Accordingly you can append \".00\", \".000\"."));
    }
    if (event->type() == QEvent::Leave) {
      label_3->clear();
    }
  } else if (target == pushButton_5) {
    if (event->type() == QEvent::Enter) {
      label_3->setText(trUtf8("This variable represents the cd number of an album or compilation (Multi-CD).\nNote 1: Only works for multi-cds.\nNote 2: Append \".0\" to the variable means that a track number smaller than 10 will be filled with zeros. E.g. 01, 02, 03, 04... etc. Accordingly you can append \".00\", \".000\"."));
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
  } else if (target == pushButton_9) {
    if (event->type() == QEvent::Enter) {
      label_3->setText(trUtf8("This variable represents the full output file incl. full path and extension. Just use it as your output for your command line encoder."));
    }
    if (event->type() == QEvent::Leave) {
      label_3->clear();
    }
  } else if (target == pushButton_6) {
    if (event->type() == QEvent::Enter) {
      label_3->setText(trUtf8("This variable represents a temporary wave file created by audex from cd audio track. You can use it as a normal input file for your command line encoder."));
    }
    if (event->type() == QEvent::Leave) {
      label_3->clear();
    }
  }
  return QWidget::eventFilter(target, event);
}

void commandmaskwizarddialog::closeEvent(QCloseEvent *event) {
  settings->setValue("commandmaskwizarddialog/pos", pos());
  settings->setValue("commandmaskwizarddialog/size", size());
  settings->setValue("commandmaskwizarddialog/show_help", checkBox->isChecked());
  event->accept();
}
