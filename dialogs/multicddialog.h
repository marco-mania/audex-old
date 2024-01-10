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

#ifndef MULTICDDIALOG_HEADER
#define MULTICDDIALOG_HEADER

#include <QtCore>
#include <QtGui>
#include "ui_multicddialog.h"

#include "../base/wait.h"
#include "../sql/qaudexsql.h"
#include "../dialogs/errordialog.h"

//need this for signals out of the thread...
class CD {
public:
  CD() { }
  CD(const CD& cd) { title = cd.title; titles = cd.titles; cdnum_id = cd.cdnum_id; }
  ~CD() { }
  QString title;
  QStringList titles;
  int cdnum_id;
};

class Compilation {
public:
  Compilation() { }
  Compilation(const Compilation& compilation) { cds = compilation.cds; title = compilation.title; cdnum_id = compilation.cdnum_id; }
  ~Compilation() { }
  QList<CD> cds;
  QString title;
  int cdnum_id;
};

class FillTreeViewThread : public QThread {
  Q_OBJECT
public:
  QAudexSQL *audex_sql;
  int days;
  bool error;
  void run();
  void stop();
signals:
  void new_item(const Compilation& item);
private:
  QMutex to_stop_mutex;
  bool to_stop;
};

class multicddialog : public QDialog, private Ui::MultiCDDialog {
  Q_OBJECT
public:
  multicddialog(QWidget *parent, QSettings *settings, QAudexSQL *audex_sql, QCDDAMetaDataModel *cdda_metadata_model);
  ~multicddialog();

private slots:
  void extract();
  void cancel();

  void toggle_first(bool checked);
  void update_cdnum(int cdnum);
  void toggle_track_offset(bool checked);
  void update_track_offset(int offset);

  void change_view_era(int index);
  void change_view_index();

  void enableExtractButton();

  void set_new_item(const Compilation& compilation);

protected:
  void closeEvent(QCloseEvent *event);

private:
  QSettings *settings;
  QAudexSQL *audex_sql;
  QCDDAMetaDataModel *cdda_metadata_model;

  FillTreeViewThread fill_tree_view_thread;
  void stop_thread();

  void fill_tree_view(int days);

  QList<int> exclude_list;

};

#endif
