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
#ifndef DOCUMENT_LIST_H
#define DOCUMENT_LIST_H


#include <qobject.h>
#include <qtopia/applnk.h>


class DocumentListPrivate;
class ServerInterface;

class DocumentList : public QObject {
    Q_OBJECT
public:
    DocumentList( ServerInterface *serverGui, bool scanDocs = TRUE,
		  QObject *parent = 0, const char *name = 0 );
    ~DocumentList();

    void linkChanged( QString link );
    void restoreDone();
    void storageChanged();
    void sendAllDocLinks();

    void reloadAppLnks();
    void reloadDocLnks();

// Orig DocList stuff
    void start();
    void pause();
    void resume();
    void rescan();

    static AppLnkSet *appLnkSet;

/*
    void resend();
signals:
    void added( const DocLnk& doc );
    void removed( const DocLnk& doc );
    void changed( const DocLnk& oldDoc, const DocLnk& newDoc );
    void allRemoved();
    void doneForNow();
private slots:
   void resendWorker();
*/
    void ensureLoaded( const QString &type );

private slots:
    void startInitialScan();
protected:
    void loadAppsForTypes( const QStringList &t, bool force = TRUE );
    bool requirementsSatisfied( AppLnk *l ) const;

    void timerEvent( QTimerEvent *te );
private:
    void add( const DocLnk& doc );
    void sendDocLnk( DocLnk *doc );
    void sendIcon( QString exe, QString icon, QString type );

    DocumentListPrivate *d;

};


#endif
