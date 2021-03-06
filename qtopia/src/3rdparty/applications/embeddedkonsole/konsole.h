/**********************************************************************
** Copyright (C) 2000-2005 Trolltech AS and its licensors.
** All rights reserved.
**
** This file is part of the Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
** See below for additional copyright and license information
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/
/* ----------------------------------------------------------------------- */
/*                                                                         */
/* [konsole.h]                      Konsole                                   */
/*                                                                            */
/* -------------------------------------------------------------------------- */
/*                                                                            */
/* Copyright (c) 1997,1998 by Lars Doelle <lars.doelle@on-line.de>            */
/*                                                                            */
/* This file is part of Konsole, an X terminal.                               */
/*                                                                            */
/* The material contained in here more or less directly orginates from        */
/* kvt, which is copyright (c) 1996 by Matthias Ettrich <ettrich@kde.org>     */
/*                                                                            */
/* -------------------------------------------------------------------------- */
/*                        */
/* Konsole ported to Qt/Embedded by Trolltech                                 */
/*                        */
/* -------------------------------------------------------------------------- */

#ifndef KONSOLE_H
#define KONSOLE_H


#include <qmainwindow.h>
#include <qaction.h>
#include <qpopupmenu.h>
#include <qstrlist.h>
#include <qintdict.h>
#include <qptrdict.h>
#include <qtabwidget.h>
#include <qtopia/qpetoolbar.h>
#include <qcombobox.h>

#include "MyPty.h"
#include "TEWidget.h"
#include "TEmuVt102.h"
#include "session.h"

class EKNumTabWidget;

class Konsole : public QMainWindow
{
Q_OBJECT

public:

  Konsole(QWidget* parent = 0, const char* name = 0, WFlags fl = 0);
//  Konsole(const char * name, const char* pgm, QStrList & _args, int histon);
  ~Konsole();
  void setColLin(int columns, int lines);
  QPEToolBar *secondToolBar; 
  void show();
  void setColor();
  int lastSelectedMenu;
private slots:
  void doneSession(TESession*,int);
  void changeColumns(int);
  void fontChanged(int);
  void configMenuSelected(int );
  void colorMenuSelected(int);
  void enterCommand(int);
  void hitEnter();
  void hitSpace();
  void hitTab();
  void hitPaste();
  void hitUp();
  void hitDown();
  void switchSession(QWidget *);
  void newSession();
  void changeCommand(const QString &, int);

private:
  void init(const char* _pgm, QStrList & _args);
  void initSession(const char* _pgm, QStrList & _args);
  void runSession(TESession* s);
  void setColorPixmaps();
  void setHistory(bool);
  QSize calcSize(int columns, int lines);
  TEWidget* getTe();

private:
  class VTFont 
  {
  public:
    VTFont(QString name, QFont& font) 
    {
      this->name = name;
      this->font = font;
    }

    QFont& getFont() 
    {
      return font;
    }

    QString getName() 
    {
      return name;
    }

  private:
    QString name;
    QFont font;
  };

  EKNumTabWidget* tab;
  int nsessions;
  QList<VTFont> fonts;
  int cfont;
  QCString se_pgm;
  QStrList se_args;

  QPopupMenu* fontList,*configMenu,*colorMenu;
  QComboBox *commonCombo;
    // history scrolling I think
  bool        b_scroll;

  int         n_keytab;
  int         n_scroll;
  int         n_render;
  QString     pmPath; // pixmap path
  QString     dropText;
  QFont       defaultFont;
  QSize       defaultSize;
  QAction *   newAct;    
};

#endif

