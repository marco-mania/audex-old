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

#ifndef ASSISTANTDIALOG_H
#define ASSISTANTDIALOG_H

#include <QWidget>

#include <KDebug>
#include <KAssistantDialog>

#include "preferences.h"

#include "../models/profilemodel.h"

#include "../utils/encoderassistant.h"

#include "../widgets/assistant/greetingswidget.h"
#include "../widgets/assistant/basepathwidget.h"
#include "../widgets/assistant/codecselectionwidget.h"
#include "../widgets/assistant/filenamewidget.h"
#include "../widgets/assistant/filenameschemewidget.h"
#include "../widgets/assistant/additionswidget.h"
#include "../widgets/assistant/goodbyewidget.h"

class AssistantDialog : public KAssistantDialog {

  Q_OBJECT

public:
  AssistantDialog(ProfileModel *profileModel, QWidget *parent = 0);
  ~AssistantDialog();

private slots:
  void create_profiles();

  void add_profile(const QString& name, const QString& mask, const QString& cmd, const QString& suffix,
		const bool fat32, const bool cover, const bool playlist);

  void enable_filename_scheme_page(bool enable);

private:
  ProfileModel *profile_model;
  EncoderAssistant encoder_assistant;

  greetingsWidget *greetingswidget;
  KPageWidgetItem *greetingsPage;
  basepathWidget *basepathwidget;
  KPageWidgetItem *basepathPage;
  codecselectionWidget *codecselectionwidget;
  KPageWidgetItem *codecselectionPage;
  filenameWidget *filenamewidget;
  KPageWidgetItem *filenamePage;
  filenameschemeWidget *filenameschemewidget;
  KPageWidgetItem *filenameschemePage;
  additionsWidget *additionswidget;
  KPageWidgetItem *additionsPage;
  goodbyeWidget *goodbyewidget;
  KPageWidgetItem *goodbyePage;

};

#endif
