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

#include <QApplication>
#include <QDir>
#ifdef Q_OS_ANDROID
#if (QT_VERSION >= QT_VERSION_CHECK(6, 1, 0))
#include <QJniObject>
#endif
#endif
#include <QSettings>

#include <iostream>

#ifdef Q_OS_MAC
#include "CocoaInitializer.h"
#endif

#include "glitch-misc.h"
#include "glitch-ui.h"
#include "glitch-version.h"

int main(int argc, char *argv[])
{
  for(int i = 0; i < argc; i++)
    if(!argv || !argv[i])
      continue;
    else if(strcmp(argv[i], "--help") == 0)
      {
	std::cout << "Glitch [options]" << std::endl;
	std::cout << "--help" << std::endl;
	std::cout << "--new-arduino-diagram diagram-name" << std::endl;
	std::cout << "--open-arduino-diagram absolute-file-name" << std::endl;
	std::cout << "--show-arduino-structures" << std::endl;
	std::cout << "--show-tools" << std::endl;
	std::cout << "--version" << std::endl;
	return EXIT_SUCCESS;
      }
    else if(strcmp(argv[i], "--version") == 0)
      {
	std::cout << "Glitch version "
		  << GLITCH_VERSION_STRING
		  << "."
		  << std::endl;
	return EXIT_SUCCESS;
      }

#if defined(Q_OS_MACOS) || defined(Q_OS_WIN)
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);
  QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps, true);
#endif

  QApplication qapplication(argc, argv);
  auto font(qapplication.font());

  font.setStyleStrategy
    (QFont::StyleStrategy(QFont::PreferAntialias | QFont::PreferQuality));
  qapplication.setFont(font);
  qapplication.setWindowIcon(QIcon(":Logo/glitch-logo.png"));

  QDir dir;

  dir.mkdir(glitch_misc::homePath());

#ifdef Q_OS_MAC
  /*
  ** Eliminate pool errors on OS X.
  */

  CocoaInitializer ci;
#endif
#if (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))
  QApplication::setAttribute(Qt::AA_DontUseNativeDialogs, true);
#endif
  QApplication::setAttribute(Qt::AA_DontUseNativeMenuBar, true);
  QCoreApplication::setApplicationName("Glitch");
  QCoreApplication::setApplicationVersion(GLITCH_VERSION_STRING);
  QCoreApplication::setOrganizationName("Glitch");
  QSettings::setDefaultFormat(QSettings::IniFormat);
  QSettings::setPath(QSettings::IniFormat,
		     QSettings::UserScope,
                     glitch_misc::homePath());
  glitch_ui::s_defaultApplicationFont = font;

  glitch_ui ui;

  ui.show();

  auto rc = qapplication.exec();

#ifdef Q_OS_ANDROID
#if (QT_VERSION >= QT_VERSION_CHECK(6, 1, 0))
  auto activity = QJniObject(QNativeInterface::QAndroidApplication::context());

  activity.callMethod<void> ("finishAndRemoveTask");
#endif
#endif
  return rc;
}
