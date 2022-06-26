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

#include <QCloseEvent>
#include <QMessageBox>
#include <QShortcut>

#include "glitch-misc.h"
#include "glitch-scene.h"
#include "glitch-separated-diagram-window.h"
#include "glitch-ui.h"
#include "glitch-view.h"

glitch_separated_diagram_window::
glitch_separated_diagram_window(QWidget *parent):QMainWindow(parent)
{
  m_ui.setupUi(this);
  connect(m_ui.action_Close,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(close(void)));
  connect(m_ui.action_Context_Menu,
	  &QAction::triggered,
	  this,
	  &glitch_separated_diagram_window::slotShowContextMenu);
  connect(m_ui.action_Copy,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotCopy(void)));
  connect(m_ui.action_Generate_Source,
	  &QAction::triggered,
	  this,
	  &glitch_separated_diagram_window::slotGenerateSource);
  connect(m_ui.action_Delete,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotDelete(void)));
  connect(m_ui.action_Paste,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotPaste(void)));
  connect(m_ui.action_Redo,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotRedo(void)));
  connect(m_ui.action_Save_Diagram,
	  &QAction::triggered,
	  this,
	  &glitch_separated_diagram_window::slotSaveDiagram);
  connect(m_ui.action_Select_All,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotSelectAll(void)));
  connect(m_ui.action_Undo,
	  SIGNAL(triggered(void)),
	  this,
	  SLOT(slotUndo(void)));
  prepareIcons();
  statusBar(); // Create a status bar.
}

glitch_separated_diagram_window::~glitch_separated_diagram_window()
{
}

void glitch_separated_diagram_window::closeEvent(QCloseEvent *event)
{
  if(event && m_view && m_view->hasChanged())
    {
      QMessageBox mb(this);

      mb.setIcon(QMessageBox::Question);
      mb.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
      mb.setText
	(tr("The display has not been saved. Are you sure that "
	    "you wish to close it?"));
      mb.setWindowIcon(windowIcon());
      mb.setWindowModality(Qt::ApplicationModal);
      mb.setWindowTitle(tr("Glitch: Confirmation"));

      if(mb.exec() != QMessageBox::Yes)
	{
	  QApplication::processEvents();
	  event->ignore();
	  return;
	}

      QApplication::processEvents();
    }

  deleteLater();
}

void glitch_separated_diagram_window::prepareActionWidgets(void)
{
  if(m_view)
    {
      m_ui.action_Copy->setEnabled(!m_view->scene()->selectedItems().empty());
      m_ui.action_Delete->setEnabled(!m_view->scene()->selectedItems().empty());
      m_ui.action_Paste->setEnabled(!glitch_ui::copiedObjects().isEmpty());
      m_ui.action_Save_Diagram->setEnabled(m_view->hasChanged());
      m_ui.action_Select_All->setEnabled(m_view->scene()->items().size() > 2);
    }
  else
    {
      m_ui.action_Copy->setEnabled(false);
      m_ui.action_Delete->setEnabled(false);
      m_ui.action_Paste->setEnabled(false);
      m_ui.action_Save_Diagram->setEnabled(false);
      m_ui.action_Select_All->setEnabled(false);
    }

  prepareRedoUndoActions();
}

void glitch_separated_diagram_window::prepareIcons(void)
{
  m_ui.action_Close->setIcon(QIcon::fromTheme("window-close"));
  m_ui.action_Copy->setIcon(QIcon::fromTheme("edit-copy"));
  m_ui.action_Delete->setIcon(QIcon::fromTheme("edit-delete"));
  m_ui.action_Paste->setIcon(QIcon::fromTheme("edit-paste"));
  m_ui.action_Redo->setIcon(QIcon::fromTheme("edit-redo"));
  m_ui.action_Save_Diagram->setIcon(QIcon::fromTheme("document-save"));
  m_ui.action_Select_All->setIcon(QIcon::fromTheme("edit-select-all"));
  m_ui.action_Undo->setIcon(QIcon::fromTheme("edit-undo"));
}

void glitch_separated_diagram_window::prepareRedoUndoActions(void)
{
  if(m_view)
    {
      m_ui.action_Redo->setEnabled(m_view->canRedo());

      if(m_ui.action_Redo->isEnabled())
	m_ui.action_Redo->setText(tr("Redo (%1)").arg(m_view->redoText()));
      else
	m_ui.action_Redo->setText(tr("Redo"));

      m_ui.action_Undo->setEnabled(m_view->canUndo());

      if(m_ui.action_Undo->isEnabled())
	m_ui.action_Undo->setText(tr("Undo (%1)").arg(m_view->undoText()));
      else
	m_ui.action_Undo->setText(tr("Undo"));
    }
  else
    {
      m_ui.action_Redo->setEnabled(false);
      m_ui.action_Redo->setText(tr("Redo"));
      m_ui.action_Undo->setEnabled(false);
      m_ui.action_Undo->setText(tr("Undo"));
    }
}

void glitch_separated_diagram_window::setCentralWidget(QWidget *widget)
{
  if(centralWidget())
    centralWidget()->deleteLater();

  if(m_view)
    {
      disconnect(m_view,
		 SIGNAL(changed(void)),
		 this,
		 SLOT(slotPageChanged(void)));
      disconnect(m_view,
		 SIGNAL(saved(void)),
		 this,
		 SLOT(slotPageSaved(void)));
      disconnect(m_view,
		 SIGNAL(selectionChanged(void)),
		 this,
		 SLOT(slotSelectionChanged(void)));
      disconnect(m_view,
		 QOverload<const glitch_tools::Operations>::
		 of(&glitch_view::toolsOperationChanged),
		 this,
		 QOverload<const glitch_tools::Operations>::
		 of(&glitch_separated_diagram_window::
		    slotToolsOperationChanged));
    }

  m_view = qobject_cast<glitch_view *> (widget);

  if(m_view)
    {
      connect(m_view,
	      SIGNAL(changed(void)),
	      this,
	      SLOT(slotPageChanged(void)));
      connect(m_view,
	      SIGNAL(saved(void)),
	      this,
	      SLOT(slotPageSaved(void)));
      connect(m_view,
	      SIGNAL(selectionChanged(void)),
	      this,
	      SLOT(slotSelectionChanged(void)));
      connect(m_view,
	      QOverload<const glitch_tools::Operations>::
	      of(&glitch_view::toolsOperationChanged),
	      this,
	      QOverload<const glitch_tools::Operations>::
	      of(&glitch_separated_diagram_window::slotToolsOperationChanged));
      QMainWindow::setCentralWidget(m_view);
      prepareActionWidgets();
      slotToolsOperationChanged(m_view->toolsOperation());
    }
}

void glitch_separated_diagram_window::slotCopy(void)
{
  emit copy(m_view);
  m_ui.action_Paste->setEnabled(!glitch_ui::copiedObjects().empty());
}

void glitch_separated_diagram_window::slotDelete(void)
{
  if(m_view)
    {
      m_view->deleteItems();
      m_ui.action_Undo->setEnabled(m_view->canUndo());

      if(m_ui.action_Undo->isEnabled())
	m_ui.action_Undo->setText(tr("Undo (%1)").arg(m_view->undoText()));
      else
	m_ui.action_Undo->setText(tr("Undo"));
    }
}

void glitch_separated_diagram_window::slotGenerateSource(void)
{
  if(m_view)
    {
      if(statusBar())
	{
	  statusBar()->showMessage(tr("Generating source. Please be patient."));
	  statusBar()->repaint();
	}

      m_view->generateSource();

      if(statusBar())
	{
	  statusBar()->showMessage("");
	  statusBar()->repaint();
	}
    }
}

void glitch_separated_diagram_window::slotPageChanged(void)
{
  if(m_view)
    {
      if(m_view->hasChanged())
	QMainWindow::setWindowTitle(tr("Glitch: %1 (*)").arg(m_view->name()));
      else
	QMainWindow::setWindowTitle(tr("Glitch: %1").arg(m_view->name()));
    }
  else
    QMainWindow::setWindowTitle(tr("Glitch"));

  prepareActionWidgets();
}

void glitch_separated_diagram_window::slotPageSaved(void)
{
  slotPageChanged();
}

void glitch_separated_diagram_window::slotPaste(void)
{
  emit paste(m_view);
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  prepareRedoUndoActions();
  QApplication::restoreOverrideCursor();
}

void glitch_separated_diagram_window::slotRedo(void)
{
  if(m_view)
    {
      m_view->redo();
      prepareRedoUndoActions();
    }
}

void glitch_separated_diagram_window::slotSaveDiagram(void)
{
  if(m_view)
    {
      QString error("");

      if(!m_view->save(error))
	glitch_misc::showErrorDialog
	  (tr("Unable to save %1 (%2).").arg(m_view->name()).arg(error), this);
    }

  prepareActionWidgets();
}

void glitch_separated_diagram_window::slotSelectAll(void)
{
  if(m_view)
    m_view->selectAll();
}

void glitch_separated_diagram_window::slotSelectionChanged(void)
{
  prepareActionWidgets();
}

void glitch_separated_diagram_window::slotShowContextMenu(void)
{
  if(m_view)
    {
      auto menu = m_view->defaultContextMenu();

      if(menu)
	{
	  menu->update();
	  menu->raise();
	  menu->exec(mapToGlobal(QPoint(size().width() / 2, 0)));
	}
    }
}

void glitch_separated_diagram_window::slotToolsOperationChanged
(const glitch_tools::Operations operation)
{
  if(statusBar())
    {
      if(operation == glitch_tools::Operations::SELECT)
	statusBar()->showMessage(tr("Select Mode"));
      else if(operation == glitch_tools::Operations::WIRE_CONNECT)
	statusBar()->showMessage(tr("Wire (Connect) Mode"));
      else
	statusBar()->showMessage(tr("Wire (Disconnect) Mode"));

      statusBar()->repaint();
    }
}

void glitch_separated_diagram_window::slotUndo(void)
{
  if(m_view)
    {
      m_view->undo();
      prepareRedoUndoActions();
      slotPageChanged();
    }
}
