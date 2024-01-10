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

#include "wavewidget.h"

waveWidget::waveWidget(Parameters *parameters, QWidget *parent) : waveWidgetUI(parent) {

  Q_UNUSED(parent);

  this->parameters = parameters;
  if (!parameters) {
    kDebug() << "ParameterString is NULL!";
    return;
  }

  klineedit_suffix->setText(parameters->value(ENCODER_WAVE_SUFFIX_KEY, ENCODER_WAVE_SUFFIX));

  connect(klineedit_suffix, SIGNAL(textEdited(const QString&)), this, SLOT(trigger_changed()));

  changed = FALSE;

}

waveWidget::~waveWidget() {

}

bool waveWidget::save() {

  bool success = TRUE;

  parameters->setValue(ENCODER_WAVE_SUFFIX_KEY, klineedit_suffix->text());

  changed = FALSE;

  return success;

}

void waveWidget::trigger_changed() {

  changed = (
    klineedit_suffix->text() != parameters->value(ENCODER_WAVE_SUFFIX_KEY, ENCODER_WAVE_SUFFIX)
  );

  emit triggerChanged();

}
