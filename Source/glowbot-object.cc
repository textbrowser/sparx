/*
** Copyright (c) 2004 - 10^10^10, Alexis Megas.
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from GlowBot without specific prior written permission.
**
** GLOWBOT IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** GLOWBOT, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "glowbot-object.h"
#include "glowbot-object-analog-read-arduino.h"
#include "glowbot-style-sheet.h"

quint64 glowbot_object::s_id = 0;

glowbot_object::glowbot_object(QWidget *parent):QWidget(0)
{
  m_id = s_id;
  m_parent = parent;
  s_id += 1;
}

glowbot_object::~glowbot_object()
{
}

glowbot_object *glowbot_object::createFromValues
(const QMap<QString, QVariant> &values, QString &error, QWidget *parent)
{
  QString type(values.value("type").toString().toLower().trimmed());
  glowbot_object *object = 0;

  if(type == "glowbot-arduino-analogread()")
    object = glowbot_object_analog_read_arduino::createFromValues
      (values, error, parent);
  else
    {
      if(type.isEmpty())
	error = tr("Empty object type.");
      else
	error = tr("The type %1 is not supported.").arg(type);
    }

  return object;
}

quint64 glowbot_object::id(void) const
{
  return m_id;
}

void glowbot_object::addDefaultActions(QMenu &menu) const
{
  if(!menu.actions().isEmpty())
    menu.addSeparator();

  menu.addAction(tr("&Delete"),
		 this,
		 SLOT(deleteLater(void)))->setEnabled(!isMandatory());
  menu.addAction(tr("&Set Style Sheet..."),
		 this,
		 SLOT(slotSetStyleSheet(void)));
}

void glowbot_object::slotSetStyleSheet(void)
{
  glowbot_style_sheet *styleSheet = new glowbot_style_sheet(m_parent);

  styleSheet->setWidget(this);

  if(styleSheet->exec() == QDialog::Accepted)
    setStyleSheet(styleSheet->styleSheet());
}
