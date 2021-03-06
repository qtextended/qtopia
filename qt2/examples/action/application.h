/****************************************************************************
** $Id: qt/examples/action/application.h   2.3.12   edited 2005-10-27 $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef APPLICATION_H
#define APPLICATION_H

#include <qmainwindow.h>

class QMultiLineEdit;
class QAction;

class ApplicationWindow: public QMainWindow
{
    Q_OBJECT
public:
    ApplicationWindow();
    ~ApplicationWindow();

protected:
    void closeEvent( QCloseEvent* );

private slots:
    void newDoc();
    void load();
    void load( const char *fileName );
    void save();
    void saveAs();
    void print();

    void about();
    void aboutQt();

private:
    QPrinter *printer;
    QMultiLineEdit *e;
    QString filename;

};


#endif
