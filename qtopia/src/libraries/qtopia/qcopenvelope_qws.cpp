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

#ifndef QT_NO_COP
#define QTOPIA_INTERNAL_FILEOPERATIONS
#include "qcopenvelope_qws.h"
#endif
#include "global.h"
#include <qbuffer.h>
#include <qdatastream.h>
#include <qfile.h>

#include <errno.h>
#ifndef Q_OS_WIN32
#include <unistd.h>
#include <sys/file.h>
#else
#include <stdlib.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#ifndef QT_NO_COP

/*!
  \class QCopEnvelope qcopenvelope_qws.h
  \brief The QCopEnvelope class encapsulates and sends QCop messages
  over QCopChannels.

  QCop messages allow applications to communicate with each other.
  These messages are sent using QCopEnvelope, and received by connecting
  to a QCopChannel.

  To send a message, use the following protocol:

  \code
     QCopEnvelope e(channelname, messagename);
     e << parameter1 << parameter2 << ...;
  \endcode

  For messages without parameters, simply use:

  \code
     QCopEnvelope e(channelname, messagename);
  \endcode

  (Do not try to simplify this further as it may confuse some
  compilers.)

  The \c{channelname} of channels within Qtopia all start with "QPE/".
  The \c{messagename} is a function identifier followed by a list of types
  in parentheses. There is no whitespace in the message name.

  To receive a message, you will generally just use your application's
  predefined QPE/Application/\e{appname} channel
  (see QPEApplication::appMessage()), but you can make another channel
  and connect it to a slot like this:

  \code
      myChannel = new QCopChannel( "QPE/FooBar", this );
      connect( myChannel, SIGNAL(received(const QCString&,const QByteArray&)),
               this, SLOT(fooBarMessage(const QCString&,const QByteArray&)) );
  \endcode

  See also, the \link qcop.html list of Qtopia messages\endlink.

  \ingroup qtopiaemb
*/

/*!
  Constructs a QCopEnvelope that will write \a message to \a channel.
  If \a message has parameters, you must then use operator<<() to
  add these parameters to the envelope.
*/
QCopEnvelope::QCopEnvelope( const QCString& channel, const QCString& message ) :
    QDataStream(new QBuffer),
    ch(channel), msg(message)
{
    device()->open(IO_WriteOnly);
}

/*!
  Writes the message and then destroys the QCopEnvelope.
*/
QCopEnvelope::~QCopEnvelope()
{
    QByteArray data = ((QBuffer*)device())->buffer();
    const int pref=16;
    if ( qstrncmp(ch.data(),"QPE/Application/",pref)==0 ) {
	QString qcopfn(Global::tempDir() + "qcop-msg-");
	qcopfn += ch.mid(pref);
	QFile qcopfile(qcopfn);

	if ( qcopfile.open(IO_WriteOnly | IO_Append) ) {
#ifndef Q_OS_WIN32
	    if(flock(qcopfile.handle(), LOCK_EX)) {
		/* some error occurred */
		qWarning(QString("Failed to obtain file lock on %1 (%2)")
			.arg(qcopfn).arg( errno ));
	    }
#endif
	    {
		QDataStream ds(&qcopfile);
		ds << ch << msg << data;
		qcopfile.flush();
#ifndef Q_OS_WIN32
		flock(qcopfile.handle(), LOCK_UN);
#endif
		qcopfile.close();
	    }

	    QByteArray b;
	    QDataStream stream(b, IO_WriteOnly);
	    stream << QString(ch.mid(pref));
	    QCopChannel::send("QPE/Server", "processQCop(QString)", b);
	    delete device();
	    return;
	} else {
	    qWarning(QString("Failed to open file %1")
			.arg(qcopfn));
	} // endif open
    }
    else if (qstrncmp(ch.data(), "QPE/SOAP/", 9) == 0) {
      // If this is a message that should go along the SOAP channel, we move the
      // endpoint URL to the data section.
      QString endpoint = ch.mid(9);
      
      ch = "QPE/SOAP";
      // Since byte arrays are explicitly shared, this is appended to the data variable..
      *this << endpoint;
    }

    QCopChannel::send(ch,msg,data);
    delete device();
}

#endif
