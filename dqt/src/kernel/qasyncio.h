/****************************************************************************
** $Id: qt/qasyncio.h   3.3.5   edited Aug 31 12:17 $
**
** Definition of asynchronous I/O classes
**
** Created : 970617
**
** Copyright (C) 1992-2005 Trolltech AS.  All rights reserved.
**
** This file is part of the kernel module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses may use this file in accordance with the Qt Commercial License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef QASYNCIO_H
#define QASYNCIO_H

#ifndef QT_H
#include "qobject.h"
#include "qsignal.h"
#include "qtimer.h"
#endif // QT_H

#ifndef QT_NO_ASYNC_IO

class QIODevice;

class Q_EXPORT QAsyncIO {
public:
    virtual ~QAsyncIO();
    void connect(QObject*, const char *member);

protected:
    void ready();

private:
    QSignal signal;
};

class Q_EXPORT QDataSink : public QAsyncIO {
public:
    // Call this to know how much I can take.
    virtual int readyToReceive()=0;
    virtual void receive(const uchar*, int count)=0;
    virtual void eof()=0;
    void maybeReady();
};

class Q_EXPORT QDataSource : public QAsyncIO {
public:
    virtual int readyToSend()=0; // returns -1 when never any more ready
    virtual void sendTo(QDataSink*, int count)=0;
    void maybeReady();

    virtual bool rewindable() const;
    virtual void enableRewind(bool);
    virtual void rewind();
};

class Q_EXPORT QIODeviceSource : public QDataSource {
    const int buf_size;
    uchar *buffer;
    QIODevice* iod;
    bool rew;

public:
    QIODeviceSource(QIODevice*, int bufsize=4096);
   ~QIODeviceSource();

    int readyToSend();
    void sendTo(QDataSink* sink, int n);
    bool rewindable() const;
    void enableRewind(bool on);
    void rewind();
};

class Q_EXPORT QDataPump : public QObject {
    Q_OBJECT
    int interval;
    QTimer timer;
    QDataSource* source;
    QDataSink* sink;

public:
    QDataPump(QDataSource*, QDataSink*);

private slots:
    void kickStart();
    void tryToPump();
};

#endif	// QT_NO_ASYNC_IO

#endif