/****************************************************************************
** $Id: qt/examples/action/main.cpp   2.3.12   edited 2005-10-27 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <qapplication.h>
#include "application.h"

int main( int argc, char ** argv ) {
    QApplication a( argc, argv );
    ApplicationWindow * mw = new ApplicationWindow();
    mw->setCaption( "Document 1" );
    mw->show();
    a.connect( &a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()) );
    return a.exec();
}
