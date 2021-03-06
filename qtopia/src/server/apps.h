/**********************************************************************
** Copyright (C) 2000-2005 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
** 
** This program is free software; you can redistribute it and/or modify it
** under the terms of the GNU General Public License as published by the
** Free Software Foundation; either version 2 of the License, or (at your
** option) any later version.
** 
** A copy of the GNU GPL license version 2 is included in this package as 
** LICENSE.GPL.
**
** This program is distributed in the hope that it will be useful, but
** WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
** See the GNU General Public License for more details.
**
** In addition, as a special exception Trolltech gives permission to link
** the code of this program with Qtopia applications copyrighted, developed
** and distributed by Trolltech under the terms of the Qtopia Personal Use
** License Agreement. You must comply with the GNU General Public License
** in all respects for all of the code used other than the applications
** licensed under the Qtopia Personal Use License Agreement. If you modify
** this file, you may extend this exception to your version of the file,
** but you are not obligated to do so. If you do not wish to do so, delete
** this exception statement from your version.
** 
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

// To add a new app, include appropriate files and add another APP().
// No tr() anywhere in this file

#ifndef APP_INCLUDES
#define APP_INCLUDES
// pim apps
#include "../applications/addressbook/addressbook.h"
#include "../applications/datebook/datebook.h"
#include "../applications/todo/mainwindow.h"
#include "../applications/qtmail/qtmailwindow.h"

// general apps
#include "../libraries/qtopiacalc/calculator.h"
//#include "../applications/citytime/citytime.h" // where did this go?
#include "../applications/clock/clock.h"
#include "../applications/helpbrowser/helpbrowser.h"
#include "../applications/imageviewer/showimg.h"
#include "../applications/mediaplayer/maindocumentwidgetstack.h" // does not work
//#include "../applications/sysinfo/sysinfo.h" // does not work
#include "../applications/textedit/textedit.h"

// games
#include "../games/fifteen/fifteen.h"
#include "../games/mindbreaker/mindbreaker.h"
#include "../games/minesweep/minesweep.h"
#include "../games/qasteroids/toplevel.h"
#include "../games/snake/interface.h"
#include "../games/solitaire/canvascardwindow.h"
//#include "../games/parashoot/interface.h" // grabs keyboard, also -ldl??
//#include "../games/wordgame/wordgame.h" // does not work

// settings
#include "../settings/systemtime/settime.h"
#include "../settings/appearance/appearance.h"

/* #include "../settings/rotation/settings.h"
#include "../settings/language/settings.h"
#include "../settings/light-and-power/settings.h" */

/* uncommonly used apps
#include "../applications/filebrowser/filebrowser.h"
#include "../applications/embeddedkonsole/konsole.h" */

#endif

// APP(app-id   class   maximize?   documentary?)
// pim apps
APP( "addressbook",AddressbookWindow,1,0 )
APP( "datebook",DateBook,1,0 )
APP( "todolist",TodoWindow,1,0 )
APP( "qtmail",QTMailWindow,1,0 )

// general apps
APP( "calculator",Calculator,1,0 )
//APP( "citytime",CityTime,1,0 )
APP( "clock",Clock,1,0 )
APP( "helpbrowser",HelpBrowser,1,1 )
APP( "showimg",ImageViewer,1,0 )
APP( "mpegplayer",MainDocumentWidgetStack,1,0 )
//APP( "sysinfo",SystemInfo,1,0 )
APP( "textedit",TextEdit,1,1 )

// games
APP( "fifteen",FifteenMainWindow,1,0 )
APP( "mindbreaker",MindBreaker,1,0 )
APP( "minesweep",MineSweep,1,0 )
APP( "qasteroids",KAstTopLevel,1,0 )
APP( "snake",SnakeGame,1,0 )
APP( "patience",CanvasCardWindow,1,0 )
//APP( "parashoot",ParaShoot,1,0 )
//APP( "wordgame",WordGame,1,0 )

// settings
APP( "systemtime",SetDateTime,1,0 )
APP( "appearance",AppearanceSettings,1,0 )
//APP( "light-and-power",LightSettings,1,0 )

/* APP( "sound",SoundSettings,1,0 ) */

/* uncommonly used apps
APP( "filebrowser",FileBrowser,1,0 )
APP( "embeddedkonsole",	Konsole,1,0 )
*/

