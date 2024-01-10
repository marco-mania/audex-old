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

#include "coverbrowserdialog.h"

CoverBrowserDialog::CoverBrowserDialog(QImage *cover, const QString& locale, QWidget *parent) : KDialog(parent) {

  Q_UNUSED(parent);

  setup(cover, locale);

}

CoverBrowserDialog::CoverBrowserDialog(QImage *cover, const int locale, QWidget *parent) {

  Q_UNUSED(parent);

  switch (locale) {
    case 1 : setup(cover, "de"); break;
    case 2 : setup(cover, "fr"); break;
    case 3 : setup(cover, "ca"); break;
    case 4 : setup(cover, "jp"); break;
    default : ;
    case 0 : setup(cover, "us");
  }

}

CoverBrowserDialog::~CoverBrowserDialog() {

}

void CoverBrowserDialog::fetch(const QString& searchstring) {
  cover_fetcher.startFetch(searchstring);
  ui.label->setText(i18n("Searching for covers..."));
}

void CoverBrowserDialog::slotButtonClicked(int button) {
  if (button == KDialog::Ok)
    select_this(ui.listWidget->selectedItems().at(0));
  else
    KDialog::slotButtonClicked(button);
}

void CoverBrowserDialog::select_this(QListWidgetItem* item) {
  int match = item->data(Qt::UserRole).toInt();
  *cover = cover_fetcher.cover(match);
  accept();
}

void CoverBrowserDialog::enable_select_button() {
  enableButtonOk(ui.listWidget->selectedItems().count() > 0);
}

void CoverBrowserDialog::add_item(const QImage& cover, const QString& caption, int no) {
  QListWidgetItem *item = new QListWidgetItem;
  item->setText(caption);
  item->setToolTip(i18n("%1\nCover size: %2x%3", caption, cover.width(), cover.height()));
  item->setIcon(QIcon(QPixmap::fromImage(cover.scaled(128, 128, Qt::IgnoreAspectRatio, Qt::SmoothTransformation))));
  item->setData(Qt::UserRole, no-1);
  ui.listWidget->addItem(item);
  ui.label->setText(i18n("Fetching cover %1 / %2...", no, cover_fetcher.count()));
}

void CoverBrowserDialog::all_fetched() {
  ui.label->setText(i18n("Found %1 covers", cover_fetcher.count()));
}

void CoverBrowserDialog::nothing_fetched() {
  ui.label->setText(i18n("Found no covers"));
}

void CoverBrowserDialog::setup(QImage *cover, const QString& locale) {

  this->cover = cover;

  QWidget *widget = new QWidget(this);
  ui.setupUi(widget);

  setMainWidget(widget);

  setCaption(i18n("Fetch cover from amazon"));
  setButtons(KDialog::Ok | KDialog::Cancel);

  connect(&cover_fetcher, SIGNAL(fetched(const QImage&, const QString&, int)), this, SLOT(add_item(const QImage&, const QString&, int)));
  connect(&cover_fetcher, SIGNAL(allFetched()), this, SLOT(all_fetched()));
  connect(&cover_fetcher, SIGNAL(nothingFetched()), this, SLOT(nothing_fetched()));

  ui.listWidget->setIconSize(QSize(128, 128));
  ui.listWidget->setWordWrap(TRUE);
  ui.listWidget->setViewMode(QListView::IconMode);
  connect(ui.listWidget, SIGNAL(itemSelectionChanged()), this, SLOT(enable_select_button()));
  connect(ui.listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(select_this(QListWidgetItem*)));
  enable_select_button();

  cover_fetcher.setLocale(locale);

}
