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

#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QInputDialog>
#include <QMessageBox>
#include <QSettings>

#include "glowbot-misc.h"
#include "glowbot-structures-arduino.h"
#include "glowbot-ui.h"
#include "glowbot-view-arduino.h"

glowbot_ui::glowbot_ui(void):QMainWindow(0)
{
  m_arduinoStructures = 0;
  m_ui.setupUi(this);
  connect(m_ui.action_New_Arduino,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotNewArduinoDiagram(void)));
  connect(m_ui.action_Open_Diagram,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotOpenDiagram(void)));
  connect(m_ui.action_Quit,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotQuit(void)));
  connect(m_ui.action_Save_Current_Diagram,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotSaveCurrentDiagram(void)));
  connect(m_ui.action_Structures,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotShowStructures(void)));
  connect(m_ui.menu_Tabs,
	  SIGNAL(aboutToShow(void)),
	  this,
	  SLOT(slotAboutToShowTabsMenu(void)));
  connect(m_ui.tab,
	  SIGNAL(tabCloseRequested(int)),
	  this,
	  SLOT(slotCloseDiagram(int)));
  connect(m_ui.tab->tabBar(),
	  SIGNAL(tabMoved(int, int)),
	  this,
	  SLOT(slotTabMoved(int, int)),
	  Qt::QueuedConnection);
  m_ui.menu_Tabs->setStyleSheet("QMenu {menu-scrollable: 1;}");
  m_ui.tab->setStyleSheet
    ("QTabBar::tear {"
     "image: none;"
     "}"
     );
  prepareActionWidgets();
  restoreSettings();
  show();
}

glowbot_ui::~glowbot_ui()
{
  if(m_arduinoStructures)
    m_arduinoStructures->deleteLater();
}

glowbot_view *glowbot_ui::page(const int index)
{
  return qobject_cast<glowbot_view *> (m_ui.tab->widget(index));
}

void glowbot_ui::closeEvent(QCloseEvent *event)
{
  saveSettings();

  for(int i = m_ui.tab->count() - 1; i >= 0; i--)
    slotCloseDiagram(i);

  QMainWindow::closeEvent(event);
  QApplication::exit();
}

void glowbot_ui::prepareActionWidgets(void)
{
  if(m_ui.tab->count() == 0)
    {
      m_ui.action_Structures->setEnabled(false);
    }
  else
    {
      m_ui.action_Structures->setEnabled(true);
    }
}

void glowbot_ui::restoreSettings(void)
{
  QSettings settings;

  restoreGeometry(settings.value("main_window/geometry").toByteArray());
}

void glowbot_ui::saveSettings(void)
{
  QSettings settings;

  settings.setValue("main_window/geometry", saveGeometry());
}

void glowbot_ui::slotAboutToShowTabsMenu(void)
{
  slotTabMoved(0, 0);

  if(m_ui.menu_Tabs->actions().isEmpty())
    m_ui.menu_Tabs->addAction(tr("Empty"))->setEnabled(false);
}

void glowbot_ui::slotCloseDiagram(int index)
{
  glowbot_view *page = this->page(index);

  if(page)
    page->deleteLater();

  m_ui.tab->removeTab(index);
  prepareActionWidgets();
}

void glowbot_ui::slotNewArduinoDiagram(void)
{
  QString name("");
  bool ok = true;

  name = QInputDialog::getText
    (this, tr("GlowBot: Arduino Project Name"), tr("Project Name"),
     QLineEdit::Normal, "Arduino-Diagram", &ok).trimmed();

  if(name.isEmpty() || !ok)
    return;

  name.replace(" ", "-");

  glowbot_view_arduino *page = new glowbot_view_arduino
    (name, glowbot_common::ArduinoProject, this);

  connect(page->menuAction(),
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotSelectPage(void)));
  m_ui.tab->addTab(page,
		   page->menuAction()->icon(),
		   QString("%1").arg(name));
  m_ui.tab->setCurrentWidget(page);
  prepareActionWidgets();
}

void glowbot_ui::slotOpenDiagram(void)
{
  QFileDialog dialog(this);

  dialog.setAcceptMode(QFileDialog::AcceptOpen);
  dialog.setDirectory(glowbot_misc::homePath());
  dialog.setFileMode(QFileDialog::ExistingFile);
  dialog.setLabelText(QFileDialog::Accept, tr("&Select"));
  dialog.setWindowTitle(tr("GlowBot: Open Diagram"));

  if(dialog.exec() == QDialog::Accepted)
    {
      prepareActionWidgets();
    }
}

void glowbot_ui::slotQuit(void)
{
  close();
}

void glowbot_ui::slotSaveCurrentDiagram(void)
{
  glowbot_view *page = this->page(m_ui.tab->currentIndex());

  if(page)
    {
      QString error("");

      if(!page->save(error))
	glowbot_misc::showErrorDialog
	  (tr("Unable to save %1 (%2).").arg(page->name()).arg(error), this);
    }
}

void glowbot_ui::slotSelectPage(void)
{
  QAction *action = qobject_cast<QAction *> (sender());

  if(!action)
    return;

  m_ui.tab->setCurrentWidget(action->parentWidget());
}

void glowbot_ui::slotShowStructures(void)
{
  if(qobject_cast<glowbot_view_arduino *> (m_ui.tab->currentWidget()))
    {
      if(!m_arduinoStructures)
	m_arduinoStructures = new glowbot_structures_arduino(0);

      m_arduinoStructures->showNormal();
      m_arduinoStructures->activateWindow();
      m_arduinoStructures->raise();
    }
}

void glowbot_ui::slotTabMoved(int from, int to)
{
  Q_UNUSED(from);
  Q_UNUSED(to);

  m_ui.menu_Tabs->clear();

  for(int i = 0; i < m_ui.tab->count(); i++)
    {
      glowbot_view *page = qobject_cast<glowbot_view *> (m_ui.tab->widget(i));

      if(page)
	m_ui.menu_Tabs->addAction(page->menuAction());
    }
}
