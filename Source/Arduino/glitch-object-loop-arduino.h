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

#ifndef _glitch_object_loop_arduino_h_
#define _glitch_object_loop_arduino_h_

#include <QPointer>

#include "glitch-object.h"
#include "ui_glitch-object-loop-arduino.h"

class glitch_object_edit_window;

class glitch_object_loop_arduino: public glitch_object
{
  Q_OBJECT

 public:
  glitch_object_loop_arduino(QWidget *parent);
  glitch_object_loop_arduino(const quint64 id, QWidget *parent);
  ~glitch_object_loop_arduino();
  bool hasView(void) const;
  bool isMandatory(void) const;
  glitch_object_loop_arduino *clone(QWidget *parent) const;
  void addActions(QMenu &menu);
  void save(const QSqlDatabase &db, QString &error);
  void setUndoStack(QUndoStack *undoStack);

 private:
  QPointer<glitch_object_edit_window> m_editWindow;
  Ui_glitch_object_loop_arduino m_ui;
  void mouseDoubleClickEvent(QMouseEvent *event);

 private slots:
  void slotEdit(void);
};

#endif
