/* ADEX CDDA EXTRACTOR
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

#include "assistantdialog.h"

AssistantDialog::AssistantDialog(ProfileModel *profileModel, QWidget *parent) : KAssistantDialog(parent) {

  Q_UNUSED(parent);

  profile_model = profileModel;

  greetingswidget = new greetingsWidget(this);
  greetingsPage = addPage(greetingswidget, i18n("Greetings"));
  setAppropriate(greetingsPage, TRUE);

  basepathwidget = new basepathWidget(this);
  basepathPage = addPage(basepathwidget, i18n("Base Path"));
  setAppropriate(basepathPage, TRUE);

  codecselectionwidget = new codecselectionWidget(&encoder_assistant, this);
  codecselectionPage = addPage(codecselectionwidget, i18n("Codec Selection"));
  setAppropriate(codecselectionPage, TRUE);

  filenamewidget = new filenameWidget(this);
  connect(filenamewidget->radioButton_manual, SIGNAL(toggled(bool)), this, SLOT(enable_filename_scheme_page(bool)));
  filenamePage = addPage(filenamewidget, i18n("Filename"));
  setAppropriate(filenamePage, TRUE);

  filenameschemewidget = new filenameschemeWidget(this);
  filenameschemePage = addPage(filenameschemewidget, i18n("Filename Scheme"));
  setAppropriate(filenameschemePage, FALSE);

  additionswidget = new additionsWidget(this);
  additionsPage = addPage(additionswidget, i18n("Additions"));
  setAppropriate(additionsPage, TRUE);

  goodbyewidget = new goodbyeWidget(this);
  goodbyePage = addPage(goodbyewidget, i18n("Goodbye"));
  setAppropriate(goodbyePage, TRUE);

  showButton(KDialog::Help, FALSE);
  connect(this, SIGNAL(user1Clicked()), this, SLOT(create_profiles()));

}

AssistantDialog::~AssistantDialog() {

}

void AssistantDialog::create_profiles() {

  QStringList list = codecselectionwidget->selectedEncoderTexts();

  for (int i = 0; i < list.count(); ++i) {

    EncoderAssistant::Encoder encoder = encoder_assistant.encoderByListText(list.at(i));
    bool supp_enc = encoder_assistant.encoderSupportQuality(encoder);

    QString name = list.at(i);
    QString mask;
    if (filenamewidget->radioButton_manual->isChecked())
      mask = filenameschemewidget->klineedit_scheme->text();
    else
      mask = "<basepath />/<artist />/<artist /> [<date />] <title />/<cdno /><trackno length=\"2\" fillchar=\"0\" /> - <tartist /> - <ttitle />.<suffix />";
    QString suffix = encoder_assistant.suffix(encoder);
    bool fat32 = additionswidget->checkBox_fat32->isChecked();
    bool cover = additionswidget->checkBox_cover->isChecked();
    bool playlist = additionswidget->checkBox_playlist->isChecked();

    if (supp_enc) {
      add_profile(name+i18n(" (Mobile Quality)"), mask, encoder_assistant.encoderMask(encoder, EncoderAssistant::QMobile),
		suffix, fat32, cover, playlist);
      add_profile(name+i18n(" (Good Quality)"), mask, encoder_assistant.encoderMask(encoder, EncoderAssistant::QCompromise),
		suffix, fat32, cover, playlist);
      add_profile(name+i18n(" (Perfect Quality)"), mask, encoder_assistant.encoderMask(encoder, EncoderAssistant::QHQArchival),
		suffix, fat32, cover, playlist);
    } else {
      add_profile(name, mask, encoder_assistant.encoderMask(encoder),
		suffix, fat32, cover, playlist);
    }

  }

  profile_model->commit();

  Preferences::setBasePath(basepathwidget->kurlrequester_basepath->lineEdit()->text());
  Preferences::self()->writeConfig();

  accept();

}

void AssistantDialog::add_profile(const QString& name, const QString& mask, const QString& cmd, const QString& suffix,
		const bool fat32, const bool cover, const bool playlist) {

  if (!profile_model->nameExists(name)) {
    int row = profile_model->rowCount();
    profile_model->insertRows(row, 1);
    profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_NAME_INDEX), name);
    profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_MASK_INDEX), mask);
    profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_COMMAND_INDEX), cmd);
    profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_SUFFIX_INDEX), suffix);
    profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_PROFILEINDEX_INDEX), profile_model->profileIndexMax()+1);
    profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_FAT32COMPATIBLE_INDEX), fat32);
    profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_SC_INDEX), cover);
    if (cover) {
      profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_SC_FORMAT_INDEX), "JPG");
      profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_SC_NAME_INDEX), "<title />");
    }
    profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_PL_INDEX), playlist);
    if (playlist) {
      profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_PL_FORMAT_INDEX), "M3U");
      profile_model->setData(profile_model->index(row, PROFILE_MODEL_COLUMN_PL_NAME_INDEX), "<title />");
    }
  }

}

void AssistantDialog::enable_filename_scheme_page(bool enable) {

  setAppropriate(filenameschemePage, enable);

}
