/****************************************************************************
** $Id: qt/examples/validator/main.cpp   2.3.12   edited 2005-10-27 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <qapplication.h>
#include "vw.h"

int main( int argc, char ** argv ) {
    QApplication a( argc, argv );
    VW mw;
    a.setMainWidget( &mw );
    mw.setCaption( "Qt Example - Validators" );
    mw.show();
    return a.exec();
}
