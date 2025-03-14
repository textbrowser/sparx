/*
** Copyright (c) 2015 - 10^10^10, Alexis Megas.
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
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

#include <QPushButton>

#include "glitch-tab-tabbar.h"
#include "glitch-tab.h"
#include "glitch-view.h"

glitch_tab::glitch_tab(QWidget *parent):QTabWidget(parent)
{
  m_tabBar = new glitch_tab_tabbar(this);
  m_tabBar->setContextMenuPolicy(Qt::CustomContextMenu);
  m_tabBar->setIconSize(QSize(24, 24));
  connect(m_tabBar,
	  SIGNAL(separate(QWidget *)),
	  this,
	  SIGNAL(separate(QWidget *)));
  setStyleSheet("QTabWidget::tab-bar {"
		"alignment: left;"
		"}");
  setTabBar(m_tabBar);
}

glitch_tab::~glitch_tab()
{
}

QTabBar *glitch_tab::tabBar(void) const
{
  return m_tabBar;
}

int glitch_tab::addTab
(glitch_view *view, const QIcon &icon, const QString &label)
{
  if(!view)
    return -1;
  else if(indexOf(view) >= 0)
    return indexOf(view);

  auto const index = QTabWidget::addTab(view, icon, label.trimmed());

  if(index < 0)
    return index;

  auto pushButton = new QPushButton(this);

  connect(pushButton,
	  &QPushButton::clicked,
	  view,
	  &glitch_view::slotSave);
  connect(view,
	  SIGNAL(destroyed(void)),
	  pushButton,
	  SLOT(deleteLater(void)));
  m_tabBar->setTabButton
    (index, m_tabBar->preferredCloseButtonPositionOpposite(), pushButton);
  pushButton->setFlat(true);
  pushButton->setIcon(QIcon(":/save.png"));
  pushButton->setIconSize(QSize(24, 24));
  pushButton->setToolTip(tr("Save"));
  return index;
}

void glitch_tab::disableSeparation(void)
{
  m_tabBar->disableSeparation();
}

void glitch_tab::setPushButton(QPushButton *pushButton, const int index)
{
  if(index < 0 || pushButton == nullptr)
    return;

  auto const position = m_tabBar->preferredCloseButtonPositionOpposite();

  if(m_tabBar->tabButton(index, position) == nullptr)
    m_tabBar->setTabButton(index, position, pushButton);
}
