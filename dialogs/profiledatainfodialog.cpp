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

#include "profiledatainfodialog.h"

ProfileDataInfoDialog::ProfileDataInfoDialog(const QStringList& text, const QString& mask, const QString& suffix, QWidget *parent) : KDialog(parent) {

  Q_UNUSED(parent);

  this->text = text;
  this->mask = mask;
  this->suffix = suffix;

  QWidget *widget = new QWidget(this);
  ui.setupUi(widget);

  setMainWidget(widget);

  setCaption(i18n("Info Settings"));

  setButtons(KDialog::Ok | KDialog::Cancel | KDialog::Apply);

  connect(ui.kpushbutton_mask, SIGNAL(clicked()), this, SLOT(mask_wizard()));
  ui.kpushbutton_mask->setIcon(KIcon("tools-wizard"));

  ui.ktextedit_text->setPlainText(text.join("\n"));
  connect(ui.ktextedit_text, SIGNAL(textChanged()), this, SLOT(trigger_changed()));

  ui.klineedit_mask->setText(mask);
  connect(ui.klineedit_mask, SIGNAL(textEdited(const QString&)), this, SLOT(trigger_changed()));

  ui.klineedit_suffix->setText(suffix);
  connect(ui.klineedit_suffix, SIGNAL(textEdited(const QString&)), this, SLOT(trigger_changed()));

  ui.kpushbutton_help->setIcon(KIcon("help-hint"));
  ui.kpushbutton_load->setIcon(KIcon("document-open"));
  ui.kpushbutton_save->setIcon(KIcon("document-save"));

  connect(ui.kpushbutton_help, SIGNAL(clicked()), this, SLOT(help()));
  connect(ui.kpushbutton_load, SIGNAL(clicked()), this, SLOT(load_text()));
  connect(ui.kpushbutton_save, SIGNAL(clicked()), this, SLOT(save_text()));

  enableButtonApply(FALSE);

}

ProfileDataInfoDialog::~ProfileDataInfoDialog() {
}

void ProfileDataInfoDialog::slotButtonClicked(int button) {
  if (button == KDialog::Ok) {
    save();
    accept();
  } else if (button == KDialog::Apply) {
    save();
  } else {
    KDialog::slotButtonClicked(button);
  }
}

void ProfileDataInfoDialog::mask_wizard() {

  SimpleMaskWizardDialog *dialog = new SimpleMaskWizardDialog(ui.klineedit_mask->text(), suffix, this);

  if (dialog->exec() != QDialog::Accepted) { delete dialog; return; }

  ui.klineedit_mask->setText(dialog->mask);

  delete dialog;

  trigger_changed();

}

void ProfileDataInfoDialog::trigger_changed() {
  if (ui.ktextedit_text->toPlainText().split("\n") != text) { enableButtonApply(TRUE); return; }
  if (ui.klineedit_suffix->text() != suffix) { enableButtonApply(TRUE); return; }
  if (ui.klineedit_mask->text() != mask) { enableButtonApply(TRUE); return; }
  enableButtonApply(FALSE);
}

void ProfileDataInfoDialog::help() {

   KDialog *dialog = new KDialog(this);
   dialog->resize(QSize(700, 480));
   dialog->setCaption(i18n("Usable XML Tags for text template"));
   dialog->setButtons(KDialog::Ok);

  /*if (qName == TAG_ALBUM_ARTIST) _text += artist;
  if (qName == TAG_ALBUM_TITLE) _text += title;
  if (qName == TAG_DATE) _text += date;
  if (qName == TAG_GENRE) _text += genre;*/

   KTextBrowser *tb = new KTextBrowser(dialog);
   tb->setHtml(i18n(
   "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">"
   "<html>"
   "<head>"
     "<style type=\"text/css\">"
     "p, li { white-space: pre-wrap; }"
     "</style>"
   "</head>"
   "<body>"
     "Tags are like variables with some attributes. They will be replaced by a special value.<br />"
     "For example the tag"
     "<div style=\"font-family:monospace; background: #b3c1d6; color: black\"><pre>"
     "&lt;artist /&gt;"
     "</pre></div>"
     "will be replaced by the relevant artist of the cd.<br />"
     "Also tags can have attributes. See this example:"
     "<div style=\"font-family:monospace; background: #b3c1d6; color: black\"><pre>"
     "&lt;today format=\"yyyy-MM-dd\" /&gt;"
     "</pre></div>"
     "This means, that the current date is printed. To define how this is done, you can set a format. The example"
     "would result to the output 2008-04-13, as this would be the current date. See below for more details.<br /><br />"
     "You can make use of the following tags:<br />"
     "<table border=1>"
     "<thead>"
     "<tr>"
     "<th>Tag</th><th>Attributes</th><th>Description</th><th>Example</th>"
     "</tr>"
     "</thead>"
     "<tbody>"
     "<tr>"
     "<td>&lt;artist /&gt;</td><td></td><td>Prints the relevant artist of the extracted cd.</td><td></td>"
     "</tr>"
     "<tr>"
     "<td>&lt;title /&gt;</td><td></td><td>Prints the relevant title of the extracted cd.</td><td></td>"
     "</tr>"
     "<tr>"
     "<td>&lt;date /&gt;</td><td></td><td>Prints the relevant date (usually release year) of the extracted cd.</td><td></td>"
     "</tr>"
     "<tr>"
     "<td>&lt;genre /&gt;</td><td></td><td>Prints the relevant genre of the extracted cd.</td><td></td>"
     "</tr>"
     "<tr>"
     "<td>&lt;size /&gt;</td><td>iec,precision</td><td>Prints the overall size of all extracted (compressed) music files (incl. the cover). The attribute iec can be one of the following: b, k, m, g. b means byte, k KiB, m MiB and g GiB. The attribute precision gives the number of decimal places. Default attributes are iec=\"m\" and precision=\"2\"</td><td>&lt;size iec=\"k\" precision=\"2\" /&gt;</td>"
     "</tr>"
     "<tr>"
     "<td>&lt;length /&gt;</td><td></td><td>Prints the relevant overall length of all extracted tracks. The format is min:sec.</td><td></td>"
     "</tr>"
     "<tr>"
     "<td>&lt;nooftracks /&gt;</td><td></td><td>Prints the total number of extracted tracks.</td><td></td>"
     "</tr>"
     "<tr>"
     "<td>&lt;discid /&gt;</td><td>base</td><td>Prints the discid of the current cd. The attribute base is the base of the number. The default is 16 (hexadecimal).</td><td>&lt;discid base=\"16\" /&gt;</td>"
     "</tr>"
     "<tr>"
     "<td>&lt;today /&gt;</td><td>format</td><td>Prints the current date. The attribute format specifies the output (*).</td><td>&lt;today format=\"yyyy-MM-dddd\" /&gt;</td>"
     "</tr>"
     "</tbody>"
     "</table>"
     "<br /><br />"
     "(* date format expressions)"
     "<table cellpadding=\"2\" cellspacing=\"1\" border=\"1\">"
     "<thead><tr valign=\"top\"><th>Expression</th><th>Output</th></tr></thead>"
     "<tr valign=\"top\"><td>d</td><td>The day as a number without a leading zero (1 to 31).</td></tr>"
     "<tr valign=\"top\"><td>dd</td><td>The day as a number with a leading zero (01 to 31).</td></tr>"
     "<tr valign=\"top\"><td>ddd</td><td>The abbreviated localized day name (e.g&#x2e; 'Mon' to 'Sun').</td></tr>"
     "<tr valign=\"top\"><td>dddd</td><td>The long localized day name (e.g&#x2e; 'Monday' to 'Sunday').</td></tr>"
     "<tr valign=\"top\"><td>M</td><td>The month as a number without a leading zero (1 to 12).</td></tr>"
     "<tr valign=\"top\"><td>MM</td><td>The month as a number with a leading zero (01 to 12).</td></tr>"
     "<tr valign=\"top\"><td>MMM</td><td>The abbreviated localized month name (e.g&#x2e; 'Jan' to 'Dec').</td></tr>"
     "<tr valign=\"top\"><td>MMMM</td><td>The long localized month name (e.g&#x2e; 'January' to 'December').</td></tr>"
     "<tr valign=\"top\"><td>yy</td><td>The year as two digit number (00 to 99).</td></tr>"
     "<tr valign=\"top\"><td>yyyy</td><td>The year as four digit number.</td></tr>"
     "</table>"
   "</body>"
   "</html>"
   ));
   dialog->setMainWidget(tb);
   connect(dialog, SIGNAL(okClicked()), dialog, SLOT(close()));

   dialog->exec();

   delete dialog;

}

void ProfileDataInfoDialog::load_text() {
  QString filename = KFileDialog::getOpenFileName(KUrl(QDir::homePath()), "*", this, i18n("Load text template"));
  if (!filename.isEmpty()) {
    QFile file(filename);
    if (file.open(QFile::ReadOnly)) {
      QTextStream in(&file);
      ui.ktextedit_text->setPlainText(in.readAll());
      file.close();
    }
  }
}

void ProfileDataInfoDialog::save_text() {
  QString filename = KFileDialog::getSaveFileName(KUrl(QDir::homePath()), "*", this, i18n("Save text template"));
  if (!filename.isEmpty()) {
    QFile file(filename);
    if (file.open(QFile::WriteOnly | QFile::Truncate)) {
      QTextStream out(&file);
      out << ui.ktextedit_text->toPlainText();
      file.close();
    }
  }
}

bool ProfileDataInfoDialog::save() {
  text = ui.ktextedit_text->toPlainText().split("\n");
  suffix = ui.klineedit_suffix->text();
  mask = ui.klineedit_mask->text();
  enableButtonApply(FALSE);
  return TRUE;
}
