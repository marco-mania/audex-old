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

#include "filenameschemewidget.h"

filenameschemeWidget::filenameschemeWidget(QWidget *parent) : filenameschemeWidgetUI(parent) {

  Q_UNUSED(parent);

  kpushbutton_maskwizard->setIcon(KIcon("tools-wizard"));

  connect(kpushbutton_maskwizard, SIGNAL(clicked()), this, SLOT(mask_wizard()));

}

filenameschemeWidget::~filenameschemeWidget() {

}

void filenameschemeWidget::mask_wizard() {

  MaskWizardDialog *dialog = new MaskWizardDialog(klineedit_scheme->text(), this);

  if (dialog->exec() != QDialog::Accepted) { delete dialog; return; }

  klineedit_scheme->setText(dialog->mask);

  delete dialog;

}
