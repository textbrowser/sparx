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
**    derived from Glitch without specific prior written permission.
**
** GLITCH IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** GLITCH, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "glitch-object-wire.h"
#include "glitch-undo-command.h"

glitch_object_wire::glitch_object_wire
(QWidget *parent):glitch_object_wire(1, parent)
{
}

glitch_object_wire::glitch_object_wire
(const quint64 id, QWidget *parent):glitch_object(id, parent)
{
  m_type = "glitch-wire";
  setName(m_type);
}

glitch_object_wire::~glitch_object_wire()
{
}

QString glitch_object_wire::code(void) const
{
  return "";
}

glitch_object_wire *glitch_object_wire::clone(QWidget *parent) const
{
  auto clone = new glitch_object_wire(parent);

  clone->m_properties = m_properties;
  clone->setStyleSheet(styleSheet());
  return clone;
}

glitch_object_wire *glitch_object_wire::
createFromValues(const QMap<QString, QVariant> &values,
		 QString &error,
		 QWidget *parent)
{
  Q_UNUSED(error);

  auto object = new glitch_object_wire
    (values.value("myoid").toULongLong(), parent);

  object->setProperties(values.value("properties").toString().split('&'));
  object->setStyleSheet(values.value("stylesheet").toString());
  return object;
}

void glitch_object_wire::addActions(QMenu &menu)
{
  Q_UNUSED(menu);
}

void glitch_object_wire::save(const QSqlDatabase &db, QString &error)
{
  glitch_object::save(db, error);

  if(!error.isEmpty())
    return;

  QMap<QString, QVariant> properties;

  glitch_object::saveProperties(properties, db, error);
}

void glitch_object_wire::setLeftObject(glitch_object *object)
{
  if(!object)
    return;
  else if(m_leftObject || m_rightObject == object)
    return;

  connect(object,
	  &glitch_object::destroyed,
	  this,
	  &glitch_object_wire::deleteLater);
  m_leftObject = object;
}

void glitch_object_wire::setRightObject(glitch_object *object)
{
  if(!object)
    return;
  else if(m_leftObject == object || m_rightObject)
    return;

  connect(object,
	  &glitch_object::destroyed,
	  this,
	  &glitch_object_wire::deleteLater);
  m_rightObject = object;
}
