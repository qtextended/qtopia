/****************************************************************************
** $Id: qt/src/kernel/qwsmouse_qws.cpp   2.3.12   edited 2005-10-27 $
**
** Implementation of Qt/Embedded mouse drivers
**
** Created : 991025
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of the kernel module of the Qt GUI Toolkit.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition or Qt Professional Edition
** licenses for Qt/Embedded may use this file in accordance with the
** Qt Embedded Commercial License Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "qwindowsystem_qws.h"
#include "qsocketnotifier.h"
#include "qwsevent_qws.h"
#include "qwscommand_qws.h"
#include "qwsutils_qws.h"
#include "qwsmouse_qws.h"

#include <qapplication.h>
#include <qpointarray.h>
#include <qtimer.h>
#include <qfile.h>
#include <qtextstream.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>

#include <qgfx_qws.h>
#if !defined(_OS_QNX6_)

#ifdef QT_QWS_CASSIOPEIA
#include <linux/tpanel.h>
#endif
#ifdef QT_QWS_TSLIB
#include <tslib.h>
#endif


//#define QT_QWS_K2

#if defined(QT_QWS_IPAQ) || defined(QT_QWS_K2)
#define QT_QWS_IPAQ_RAW
typedef struct {
        unsigned short pressure;
        unsigned short x;
        unsigned short y;
        unsigned short pad;
} TS_EVENT;
#elif defined(QT_QWS_SL5XXX)
#define QT_QWS_SL5XXX_RAW
typedef struct {
       long y;
       long x;
       long pressure;
       long long millisecs;
} TS_EVENT;
#define QT_QWS_TP_SAMPLE_SIZE 10
#define QT_QWS_TP_MINIMUM_SAMPLES 4
#define QT_QWS_TP_PRESSURE_THRESHOLD 500
#define QT_QWS_TP_MOVE_LIMIT 50
#define QT_QWS_TP_JITTER_LIMIT 2
#endif

#ifndef QT_QWS_TP_SAMPLE_SIZE
#define QT_QWS_TP_SAMPLE_SIZE 5
#endif

#ifndef QT_QWS_TP_MINIMUM_SAMPLES
#define QT_QWS_TP_MINIMUM_SAMPLES 5
#endif

#ifndef QT_QWS_TP_PRESSURE_THRESHOLD
#define QT_QWS_TP_PRESSURE_THRESHOLD 1
#endif

#ifndef QT_QWS_TP_MOVE_LIMIT
#define QT_QWS_TP_MOVE_LIMIT 100
#endif

#ifndef QT_QWS_TP_JITTER_LIMIT
#define QT_QWS_TP_JITTER_LIMIT 2
#endif

//#define QWS_CUSTOMTOUCHPANEL

/*!
  \class QWSMouseHandler qwsmouse_qws.h
  \brief Mouse driver/handler for Qt/Embedded

  The mouse driver/handler handles events from system devices and
  generates mouse events.

  A QWSMouseHandler will usually open some system device in its
  constructor, create a QSocketNotifier on that opened device and when
  it receives data, it will call mouseChanged() to send the event
  to Qt/Embedded for relaying to clients.
*/

/*!
  Constructs a mouse handler. This becomes the primary mouse handler.

  Note that once created, mouse handlers are controlled by the system
  and should not be deleted.
*/
QWSMouseHandler::QWSMouseHandler()
{
    QWSServer::setMouseHandler(this);
}

/*!
  Destructs the mouse handler. You should not invoked this directly.
*/
QWSMouseHandler::~QWSMouseHandler()
{
}

/*!
  \fn void mouseChanged(const QPoint& pos, int bstate);

  This signal is emited by the mouse handler to signal that the
  mouse is now at position \a pos and the mouse buttons are now
  in the state \a bstate.
*/

enum MouseProtocol { Unknown = -1, Auto = 0,
		     MouseMan, IntelliMouse, Microsoft,
		     QVFBMouse, TPanel, BusMouse,
		     FirstAuto = MouseMan,
		     LastAuto = Microsoft };

static void limitToScreen( QPoint &pt )
{
    pt.setX( QMIN( qt_screen->deviceWidth()-1, QMAX( 0, pt.x() )));
    pt.setY( QMIN( qt_screen->deviceHeight()-1, QMAX( 0, pt.y() )));
}

static QPoint &mousePos = QWSServer::mousePosition;


typedef struct {
    char *name;
    MouseProtocol id;
} MouseConfig;

static const MouseConfig mouseConfig[] = {
#ifndef QT_NO_QWS_MOUSE_AUTO
    { "Auto",		Auto },
#endif
#ifndef QT_NO_QWS_MOUSE_PC
    { "MouseMan",	MouseMan },
    { "IntelliMouse",	IntelliMouse },
    { "USB",		IntelliMouse },
    { "Microsoft",      Microsoft },
#endif
#ifndef QT_NO_QWS_VFB
    { "QVFbMouse",      QVFBMouse },
#endif
    { "TPanel",         TPanel },
    { "BusMouse",       BusMouse },
    { 0,		Unknown }
};

#ifndef QT_NO_QWS_MOUSE_AUTO
/*
 * Automatic-detection mouse driver
 */

class QAutoMouseSubHandler {
protected:
    enum { max_buf=32 };

    int fd;

    uchar buffer[max_buf];
    int nbuf;

    QPoint motion;
    int bstate;

    int goodness;
    int badness;

    virtual int tryData()=0;

public:
    QAutoMouseSubHandler(int f) : fd(f)
    {
	nbuf = bstate = goodness = badness = 0;
    }

    int file() const { return fd; }

    void closeIfNot(int& f)
    {
	if ( fd != f ) {
	    f = fd;
	    close(fd);
	}
    }

    void worse(int by=1) { badness+=by; }
    bool reliable() const { return goodness >= 5 && badness < 50; }
    int buttonState() const { return bstate; }
    bool motionPending() const { return motion!=QPoint(0,0); }
    QPoint takeMotion() { QPoint r=motion; motion=QPoint(0,0); return r; }

    void appendData(uchar* data, int length)
    {
	memcpy(buffer+nbuf, data, length);
	nbuf += length;
    }

    enum UsageResult { Insufficient, Motion, Button };

    UsageResult useData()
    {
	int pbstate = bstate;
	int n = tryData();
	if ( n > 0 ) {
	    if ( n<nbuf )
		memmove( buffer, buffer+n, nbuf-n );
	    nbuf -= n;
	    return pbstate == bstate ? Motion : Button;
	}
	return Insufficient;
    }
};

class QAutoMouseSubHandler_intellimouse : public QAutoMouseSubHandler {
    int packetsize;
public:
    QAutoMouseSubHandler_intellimouse(int f) : QAutoMouseSubHandler(f)
    {
	init();
    }

    void init()
    {
	int n;
	uchar reply[20];

	tcflush(fd,TCIOFLUSH);
	static const uchar initseq[] = { 243, 200, 243, 100, 243, 80 };
	static const uchar query[] = { 0xf2 };
	if (write(fd, initseq, sizeof(initseq))!=sizeof(initseq)) {
	    badness = 100;
	    return;
	}
	usleep(10000);
	tcflush(fd,TCIOFLUSH);
	if (write(fd, query, sizeof(query))!=sizeof(query)) {
	    badness = 100;
	    return;
	}
	usleep(10000);
	n = read(fd, reply, 20);
	if ( n > 0 ) {
	    goodness = 10;
	    switch ( reply[n-1] ) {
	      case 3:
	      case 4:
		packetsize = 4;
		break;
	     default:
		packetsize = 3;
	    }
	} else {
	    badness = 100;
	}
    }

    int tryData()
    {
	if ( nbuf >= packetsize ) {
	    //int overflow = (buffer[0]>>6 )& 0x03;

	    if ( /*overflow ||*/ !(buffer[0] & 8) ) {
		badness++;
		return 1;
	    } else {
		motion +=
		    QPoint((buffer[0] & 0x10) ? buffer[1]-256 : buffer[1],
			   (buffer[0] & 0x20) ? 256-buffer[2] : -buffer[2]);
		int nbstate = buffer[0] & 0x7;
		if ( motion.x() || motion.y() || bstate != nbstate ) {
		    bstate = nbstate;
		    goodness++;
		} else {
		    badness++;
		    return 1;
		}
	    }
	    return packetsize;
	}
	return 0;
    }
};

class QAutoMouseSubHandler_serial : public QAutoMouseSubHandler {
public:
    QAutoMouseSubHandler_serial(int f) : QAutoMouseSubHandler(f)
    {
	initSerial();
    }

protected:
    void setflags(int f)
    {
	termios tty;
	tcgetattr(fd, &tty);
	tty.c_iflag     = IGNBRK | IGNPAR;
	tty.c_oflag     = 0;
	tty.c_lflag     = 0;
	tty.c_cflag     = f | CREAD | CLOCAL | HUPCL;
#if !defined(_OS_FREEBSD_) && !defined(_OS_SOLARIS_)
	tty.c_line      = 0;
#endif
	tty.c_cc[VTIME] = 0;
	tty.c_cc[VMIN]  = 1;
	tcsetattr(fd, TCSANOW, &tty);
    }

private:
    void initSerial()
    {
	int speed[4] = { B9600, B4800, B2400, B1200 };

        for (int n = 0; n < 4; n++) {
	    setflags(CSTOPB | speed[n]);
	    write(fd, "*q", 2);
	    usleep(10000);
        }
    }
};

class QAutoMouseSubHandler_mousesystems : public QAutoMouseSubHandler_serial {
public:
    // ##### This driver has not been tested

    QAutoMouseSubHandler_mousesystems(int f) : QAutoMouseSubHandler_serial(f)
    {
	init();
    }

    void init()
    {
	setflags(B1200|CS8|CSTOPB);
	// 60Hz
	if (write(fd, "R", 1)!=1) {
	    badness = 100;
	    return;
	}
	tcflush(fd,TCIOFLUSH);
    }

    int tryData()
    {
	if ( nbuf >= 5 ) {
	    if ( (buffer[0] & 0xf8) != 0x80 ) {
		badness++;
		return 1;
	    }
	    motion +=
		QPoint((signed char)buffer[1] + (signed char)buffer[3],
		       -(signed char)buffer[2] + (signed char)buffer[4]);
	    int t = ~buffer[0];
	    int nbstate = ((t&3) << 1) | ((t&4) >> 2);
	    if ( motion.x() || motion.y() || bstate != nbstate ) {
		bstate = nbstate;
		goodness++;
	    } else {
		badness++;
		return 1;
	    }
	    return 5;
	}
	return 0;
    }
};

class QAutoMouseSubHandler_ms : public QAutoMouseSubHandler_serial {
    int mman;
public:
    QAutoMouseSubHandler_ms(int f) : QAutoMouseSubHandler_serial(f)
    {
	mman=0;
	init();
    }

    void init()
    {
	setflags(B1200|CS7);
	// 60Hz
	if (write(fd, "R", 1)!=1) {
	    badness = 100;
	    return;
	}
	tcflush(fd,TCIOFLUSH);
    }

    int tryData()
    {
	if ( !(buffer[0] & 0x40) ) {
	    if ( buffer[0] == 0x20 && (bstate & Qt::MidButton) ) {
		mman=1; // mouseman extension
	    }
	    return 1;
	}
	int extra = mman&&(bstate & Qt::MidButton);
	if ( nbuf >= 3+extra ) {
	    int nbstate = 0;
	    if ( buffer[0] == 0x40 && !bstate && !buffer[1] && !buffer[2] ) {
		nbstate = Qt::MidButton;
	    } else {
		nbstate = ((buffer[0] & 0x20) >> 5)
			| ((buffer[0] & 0x10) >> 3);
		if ( extra && buffer[3] == 0x20 )
		    nbstate = Qt::MidButton;
	    }

	    if ( buffer[1] & 0x40 ) {
		badness++;
		return 1;
	    } else {
		motion +=
		    QPoint((signed char)((buffer[0]&0x3)<<6)
			    |(signed char)(buffer[1]&0x3f),
			   (signed char)((buffer[0]&0xc)<<4)
			    |(signed char)(buffer[2]&0x3f));
		if ( motion.x() || motion.y() || bstate != nbstate ) {
		    bstate = nbstate;
		    goodness++;
		} else {
		    badness++;
		    return 1;
		}
		return 3+extra;
	    }
	}
	return 0;
    }
};

/*
QAutoMouseHandler::UsageResult QAutoMouseHandler::useDev(Dev& d)
{
    if ( d.nbuf >= mouseData[d.protocol].bytesPerPacket ) {
	uchar *mb = d.buf;
	int bstate = 0;
	int dx = 0;
	int dy = 0;

	switch (mouseProtocol) {
	    case MouseMan:
	    case IntelliMouse:
	    {
		bstate = mb[0] & 0x7; // assuming Qt::*Button order

		int overflow = (mb[0]>>6 )& 0x03;
		if (mouseProtocol == MouseMan && overflow) {
		    //### wheel events signalled with overflow bit, ignore for now
		}
		else {
		    bool xs = mb[0] & 0x10;
		    bool ys = mb[0] & 0x20;
		    dx = xs ? mb[1]-256 : mb[1];
		    dy = ys ? mb[2]-256 : mb[2];
		}
		break;
	    }
	    case Microsoft:
		if ( ((mb[0] & 0x20) >> 3) ) {
		    bstate |= Qt::LeftButton;
		}
		if ( ((mb[0] & 0x10) >> 4) ) {
		    bstate |= Qt::RightButton;
		}

		dx=(signed char)(((mb[0] & 0x03) << 6) | (mb[1] & 0x3f));
		dy=-(signed char)(((mb[0] & 0x0c) << 4) | (mb[2] & 0x3f));

		break;
	}
    }
    }
*/

#endif

class QAutoMouseHandler : public QWSMouseHandler {
    Q_OBJECT

public:
    QAutoMouseHandler();
    ~QAutoMouseHandler();

#ifndef QT_NO_QWS_MOUSE_AUTO
private:
    enum { max_dev=32 };
    QAutoMouseSubHandler *sub[max_dev];
    QList<QSocketNotifier> notifiers;
    int nsub;
    int retries;
#endif

private slots:
    void readMouseData(int);

private:
#ifndef QT_NO_QWS_MOUSE_AUTO
    void openDevices();
    void closeDevices();
    void notify(int fd);
    bool sendEvent(QAutoMouseSubHandler& h)
    {
	if ( h.reliable() ) {
	    mousePos += h.takeMotion();
	    limitToScreen( mousePos );
/*
qDebug("%d,%d %c%c%c",
mousePos.x(),mousePos.y(),
(h.buttonState()&Qt::LeftButton)?'L':'.',
(h.buttonState()&Qt::MidButton)?'M':'.',
(h.buttonState()&Qt::RightButton)?'R':'.');
*/
	    emit mouseChanged(mousePos,h.buttonState());
	    return TRUE;
	} else {
	    h.takeMotion();
	    if ( h.buttonState() & (Qt::RightButton|Qt::MidButton) ) {
		// Strange for the user to press right or middle without
		// a moving mouse!
		h.worse();
	    }
	    return FALSE;
	}
    }
#endif
};

QAutoMouseHandler::QAutoMouseHandler()
{
#ifndef QT_NO_QWS_MOUSE_AUTO
    notifiers.setAutoDelete( TRUE );
    retries = 0;
    openDevices();
#endif
}

QAutoMouseHandler::~QAutoMouseHandler()
{
#ifndef QT_NO_QWS_MOUSE_AUTO
    closeDevices();
#endif
}

#ifndef QT_NO_QWS_MOUSE_AUTO
void QAutoMouseHandler::openDevices()
{
    nsub=0;
    int fd;
    fd = open( "/dev/psaux", O_RDWR | O_NDELAY );
    if ( fd >= 0 ) {
	sub[nsub++] = new QAutoMouseSubHandler_intellimouse(fd);
	notify(fd);
    }
#if !defined(QT_QWS_IPAQ) && !defined(QT_QWS_SL5XXX) && !defined(QT_QWS_K2)
    char fn[] = "/dev/ttyS?";
    for (int ch='0'; ch<='3'; ch++) {
	fn[9] = ch;
	fd = open( fn, O_RDWR | O_NDELAY );
	if ( fd >= 0 ) {
	    //sub[nsub++] = new QAutoMouseSubHandler_intellimouse(fd);
	    sub[nsub++] = new QAutoMouseSubHandler_mousesystems(fd);
	    sub[nsub++] = new QAutoMouseSubHandler_ms(fd);
	    notify(fd);
	}
    }
#endif
    // ...
}

void QAutoMouseHandler::closeDevices()
{
    int pfd=-1;
    for (int i=0; i<nsub; i++) {
	sub[i]->closeIfNot(pfd);
	delete sub[i];
    }
    notifiers.clear();
}

void QAutoMouseHandler::notify(int fd)
{
    QSocketNotifier *mouseNotifier
	= new QSocketNotifier( fd, QSocketNotifier::Read, this );
    connect(mouseNotifier, SIGNAL(activated(int)),this, SLOT(readMouseData(int)));
    notifiers.append( mouseNotifier );
}
#endif

void QAutoMouseHandler::readMouseData(int fd)
{
#ifndef QT_NO_QWS_MOUSE_AUTO
    for (;;) {
	uchar buf[8];
	int n = read(fd, buf, 8);
	if ( n<=0 )
	    break;
	for (int i=0; i<nsub; i++) {
	    QAutoMouseSubHandler& h = *sub[i];
	    if ( h.file() == fd ) {
		h.appendData(buf,n);
		for (;;) {
		    switch ( h.useData() ) {
		      case QAutoMouseSubHandler::Button:
			sendEvent(h);
			break;
		      case QAutoMouseSubHandler::Insufficient:
			goto breakbreak;
		      case QAutoMouseSubHandler::Motion:
			break;
		    }
		}
		breakbreak:
		    ;
	    }
	}
    }
    bool any_reliable=FALSE;
    for (int i=0; i<nsub; i++) {
	QAutoMouseSubHandler& h = *sub[i];
	if ( h.motionPending() )
	    sendEvent(h);
	any_reliable = any_reliable || h.reliable();
    }
    if ( any_reliable ) {
	// ... get rid of all unreliable ones?  All bad ones?
    } else if ( retries < 2 ) {
	// Try again - maybe the mouse was being moved when we tried to init.
	closeDevices();
	openDevices();
	retries++;
    }
#else
    Q_UNUSED( fd );
#endif
}




/*
 * Standard mouse driver
 */

typedef struct {
    int bytesPerPacket;
} MouseData;

static const MouseData mouseData[] = {
    { 3 },  // dummy for auto protocal,  correction made by add by YYD
    { 3 },  // MouseMan
    { 4 },  // intelliMouse
    { 3 },  // Microsoft
    { 0 },  // QVFBMouse,
    { 0 },  // TPanel,
    { 3 },  // BusMouse,
};


class QWSMouseHandlerPrivate : public QWSMouseHandler {
    Q_OBJECT
public:
    QWSMouseHandlerPrivate( MouseProtocol protocol, QString mouseDev );
    ~QWSMouseHandlerPrivate();

#ifndef QT_NO_QWS_MOUSE_PC
private:
    static const int mouseBufSize = 128;
    int mouseFD;
    int mouseIdx;
    uchar mouseBuf[mouseBufSize];
    MouseProtocol mouseProtocol;
    void handleMouseData();
#endif

private slots:
    void readMouseData();

private:
    int obstate;
};


void QWSMouseHandlerPrivate::readMouseData()
{
#ifndef QT_NO_QWS_MOUSE_PC
    int n;
    if ( BusMouse == mouseProtocol ) {
	// a workaround of linux busmouse driver interface.
	// It'll only read 3 bytes a time and return all other buffer zeroed, thus cause protocol errors
	for (;;) {
	    if ( mouseBufSize - mouseIdx < 3 )
		break;
	    n = read( mouseFD, mouseBuf+mouseIdx, 3 );
	    if ( n != 3 )
		break;
	    mouseIdx += 3;
	}
    } else {
	do {
	    n = read(mouseFD, mouseBuf+mouseIdx, mouseBufSize-mouseIdx );
	    if ( n > 0 )
		mouseIdx += n;
	} while ( n > 0 );
    }
    handleMouseData();
#endif
}


#ifndef QT_NO_QWS_MOUSE_PC
/*
*/

void QWSMouseHandlerPrivate::handleMouseData()
{
    static const int accel_limit = 5;
    static const int accel = 2;

    int idx = 0;
    int bstate = 0;
    int dx = 0, dy = 0;
    bool sendEvent = false;
    int tdx = 0, tdy = 0;

    while ( mouseIdx-idx >= mouseData[mouseProtocol].bytesPerPacket ) {
	//qDebug( "Got mouse data" );
	uchar *mb = mouseBuf+idx;
	bstate = 0;
	dx = 0;
	dy = 0;
	sendEvent = false;
	switch (mouseProtocol) {
	    case MouseMan:
	    case IntelliMouse:
	    {
		if (mb[0] & 0x01)
		    bstate |= Qt::LeftButton;
		if (mb[0] & 0x02)
		    bstate |= Qt::RightButton;
		if (mb[0] & 0x04)
		    bstate |= Qt::MidButton;

		int overflow = (mb[0]>>6 )& 0x03;
		if (mouseProtocol == MouseMan && overflow) {
		    //### wheel events signalled with overflow bit, ignore for now
		}
		else {
		    bool xs = mb[0] & 0x10;
		    bool ys = mb[0] & 0x20;
		    dx = xs ? mb[1]-256 : mb[1];
		    dy = ys ? mb[2]-256 : mb[2];

		    sendEvent = true;
		}
#if 0 //debug
		if (mouseProtocol == MouseMan)
		    printf("(%2d) %02x %02x %02x ", idx, mb[0], mb[1], mb[2]);
		else
		    printf("(%2d) %02x %02x %02x %02x ",idx,mb[0],mb[1],mb[2],mb[3]);
		const char *b1 = (mb[0] & 0x01) ? "b1":"  ";//left
		const char *b2 = (mb[0] & 0x02) ? "b2":"  ";//right
		const char *b3 = (mb[0] & 0x04) ? "b3":"  ";//mid

		if ( overflow )
		    printf( "Overflow%d %s %s %s  (%4d,%4d)\n", overflow,
			    b1, b2, b3, mousePos.x(), mousePos.y() );
		else
		    printf( "%s %s %s (%+3d,%+3d)  (%4d,%4d)\n",
			    b1, b2, b3, dx, dy, mousePos.x(), mousePos.y() );
#endif
		break;
	    }
	    case Microsoft:
	        if ( (mb[0] & 0x20) )
		    bstate |= Qt::LeftButton;
		if ( (mb[0] & 0x10) )
		    bstate |= Qt::RightButton;

		dx=(signed char)(((mb[0] & 0x03) << 6) | (mb[1] & 0x3f));
		dy=-(signed char)(((mb[0] & 0x0c) << 4) | (mb[2] & 0x3f));
		sendEvent=true;

		break;
	    case BusMouse:
	        if ( !(mb[0] & 0x04) )
		    bstate |= Qt::LeftButton;
		if ( !(mb[0] & 0x01) )
		    bstate |= Qt::RightButton;

		dx=(signed char)mb[1];
		dy=(signed char)mb[2];
		sendEvent=true;
		break;

	    default:
		qWarning( "Unknown mouse protocol in QWSMouseHandlerPrivate" );
		break;
	}
	if (sendEvent) {
	    if ( QABS(dx) > accel_limit || QABS(dy) > accel_limit ) {
		dx *= accel;
		dy *= accel;
	    }
	    tdx += dx;
	    tdy += dy;
	    if ( bstate != obstate ) {
		mousePos += QPoint(tdx,-tdy);
		limitToScreen( mousePos );
		emit mouseChanged(mousePos,bstate);
		sendEvent = FALSE;
		tdx = 0;
		tdy = 0;
		obstate = bstate;
	    }
	}
	idx += mouseData[mouseProtocol].bytesPerPacket;
    }
    if ( sendEvent ) {
	mousePos += QPoint(tdx,-tdy);
	limitToScreen( mousePos );
	emit mouseChanged(mousePos,bstate);
    }

    int surplus = mouseIdx - idx;
    for ( int i = 0; i < surplus; i++ )
	mouseBuf[i] = mouseBuf[idx+i];
    mouseIdx = surplus;
}
#endif


QWSMouseHandlerPrivate::QWSMouseHandlerPrivate( MouseProtocol protocol,
					  QString mouseDev )
{
#ifndef QT_NO_QWS_MOUSE_PC
    mouseProtocol = protocol;

    if ( mouseDev.isEmpty() )
	mouseDev = "/dev/mouse";
    obstate = -1;
    mouseFD = -1;
    mouseFD = open( mouseDev.local8Bit().data(), O_RDWR | O_NDELAY);
    if ( mouseFD < 0 ) {
	mouseFD = open( mouseDev.local8Bit().data(), O_RDONLY | O_NDELAY);
	if ( mouseFD < 0 )
	    qDebug( "Cannot open %s (%s)", mouseDev.ascii(),
		    strerror(errno));
    } else {
	// Clear pending input
	tcflush(mouseFD,TCIFLUSH);

	bool ps2 = false;

	switch (mouseProtocol) {
	    case MouseMan:
		ps2 = true;
		write(mouseFD,"",1);
		usleep(50000);
		write(mouseFD,"@EeI!",5);
		break;

	    case IntelliMouse: {
//		    ps2 = true;
		    const unsigned char init1[] = { 243, 200, 243, 100, 243, 80 };
		    const unsigned char init2[] = { 246, 230, 244, 243, 100, 232, 3 };
		    write(mouseFD,init1,sizeof(init1));
		    usleep(50000);
		    write(mouseFD,init2,sizeof(init2));
		}
		break;

	    case Microsoft:
		struct termios tty;

		tcgetattr(mouseFD, &tty);

		tty.c_iflag = IGNBRK | IGNPAR;
		tty.c_oflag = 0;
		tty.c_lflag = 0;
#if !defined(_OS_FREEBSD_) && !defined(_OS_SOLARIS_)
		tty.c_line = 0;
#endif // _OS_FREEBSD_
		tty.c_cc[VTIME] = 0;
		tty.c_cc[VMIN] = 1;
		tty.c_cflag = B1200 | CS7 | CREAD | CLOCAL | HUPCL;
		tcsetattr(mouseFD, TCSAFLUSH, &tty); /* set parameters */
		break;

	    case BusMouse:
		usleep(50000);
		break;

	    default:
		qDebug("Unknown mouse protocol");
		exit(1);
	}

	if (ps2) {
	    char buf[] = { 246, 244 };
	    write(mouseFD,buf,1);
	    write(mouseFD,buf+1,1);
	}

	usleep(50000);
	tcflush(mouseFD,TCIFLUSH);	    // ### doesn't seem to work.
	usleep(50000);
	tcflush(mouseFD,TCIFLUSH);	    // ### doesn't seem to work.

	char buf[100];				// busmouse driver will not read if bufsize < 3,  YYD
	while (read(mouseFD, buf, 100) > 0) { }  // eat unwanted replies

	mouseIdx = 0;

	QSocketNotifier *mouseNotifier;
	mouseNotifier = new QSocketNotifier( mouseFD, QSocketNotifier::Read, this );
	connect(mouseNotifier, SIGNAL(activated(int)),this, SLOT(readMouseData()));
    }
#else
    Q_UNUSED(protocol);
    Q_UNUSED(mouseDev);
#endif
}

QWSMouseHandlerPrivate::~QWSMouseHandlerPrivate()
{
#ifndef QT_NO_QWS_MOUSE_PC
    if (mouseFD >= 0) {
	tcflush(mouseFD,TCIFLUSH);	    // yyd.
	close(mouseFD);
    }
#endif
}

/*
 *
 */

QCalibratedMouseHandler::QCalibratedMouseHandler()
    : samples(5), currSample(0), numSamples(0)
{
    clearCalibration();
    readCalibration();
}

void QCalibratedMouseHandler::getCalibration( QWSPointerCalibrationData *cd )
{
    QPoint screen_tl = cd->screenPoints[ QWSPointerCalibrationData::TopLeft ];
    QPoint screen_br = cd->screenPoints[ QWSPointerCalibrationData::BottomRight ];

    int tlx = ( s * screen_tl.x() - c ) / a;
    int tly = ( s * screen_tl.y() - f ) / e;
    cd->devPoints[ QWSPointerCalibrationData::TopLeft ] = QPoint(tlx,tly);
    cd->devPoints[ QWSPointerCalibrationData::BottomRight ] =
	QPoint( tlx - (s * (screen_tl.x() - screen_br.x() ) / a),
		tly - (s * (screen_tl.y() - screen_br.y() ) / e) );
}

void QCalibratedMouseHandler::clearCalibration()
{
    a = 1;
    b = 0;
    c = 0;
    d = 0;
    e = 1;
    f = 0;
    s = 1;
}

void QCalibratedMouseHandler::writeCalibration()
{
    QString calFile = "/etc/pointercal";
#ifndef QT_NO_TEXTSTREAM
    QFile file( calFile );
    if ( file.open( IO_WriteOnly ) ) {
	QTextStream t( &file );
	t << a << " " << b << " " << c << " ";
	t << d << " " << e << " " << f << " " << s;
    } else
#endif
    {
	qDebug( "Could not save calibration: %s", calFile.latin1() );
    }
}

void QCalibratedMouseHandler::readCalibration()
{
    QString calFile = "/etc/pointercal";
#ifndef QT_NO_TEXTSTREAM
    QFile file( calFile );
    if ( file.open( IO_ReadOnly ) && file.size() > 0) {
	QTextStream t( &file );
	t >> a >> b >> c >> d >> e >> f >> s;
    } else
#endif
    {
	qDebug( "Could not read calibration: %s", calFile.latin1() );
    }
}

void QCalibratedMouseHandler::calibrate( QWSPointerCalibrationData *cd )
{
    QPoint dev_tl = cd->devPoints[ QWSPointerCalibrationData::TopLeft ];
    QPoint dev_br = cd->devPoints[ QWSPointerCalibrationData::BottomRight ];
    QPoint screen_tl = cd->screenPoints[ QWSPointerCalibrationData::TopLeft ];
    QPoint screen_br = cd->screenPoints[ QWSPointerCalibrationData::BottomRight ];

    s = 1 << 16;

    a = s * (screen_tl.x() - screen_br.x() ) / (dev_tl.x() - dev_br.x());
    b = 0;
    c = s * screen_tl.x() - a * dev_tl.x();

    d = 0;
    e = s * (screen_tl.y() - screen_br.y() ) / (dev_tl.y() - dev_br.y());
    f = s * screen_tl.y() - e * dev_tl.y();

    writeCalibration();
}

QPoint QCalibratedMouseHandler::transform( const QPoint &p )
{
    QPoint tp;

    tp.setX( (a * p.x() + b * p.y() + c) / s );
    tp.setY( (d * p.x() + e * p.y() + f) / s );

    return tp;
}

void QCalibratedMouseHandler::setFilterSize( int s )
{
    samples.resize( s );
    numSamples = 0;
    currSample = 0;
}

bool QCalibratedMouseHandler::sendFiltered( const QPoint &p, int button )
{
    if ( !button ) {
	if ( numSamples >= samples.count() ) {
	    emit mouseChanged( mousePos, 0 );
	}
	currSample = 0;
	numSamples = 0;
	return TRUE;
    }

    bool sent = FALSE;
    samples[currSample] = p;
    numSamples++;
    if ( numSamples >= samples.count() ) {
	int maxd = 0;
	unsigned int ignore = 0;
	// throw away the "worst" sample
	for ( unsigned int i = 0; i < samples.count(); i++ ) {
	    int d = ( mousePos - samples[i] ).manhattanLength();
	    if ( d > maxd ) {
		maxd = d;
		ignore = i;
	    }
	}
	bool first = TRUE;
	QPoint pos;
	// average the rest
	for ( unsigned int i = 0; i < samples.count(); i++ ) {
	    if ( ignore != i ) {
		if ( first ) {
		    pos = samples[i];
		    first = FALSE;
		} else {
		    pos += samples[i];
		}
	    }
	}
	pos /= (int)(samples.count() - 1);
	pos = transform( pos );
	if ( pos != mousePos || numSamples == samples.count() ) {
	    mousePos = pos;
	    emit mouseChanged( mousePos, button );
	    sent = TRUE;
	}
    }
    currSample++;
    if ( currSample >= samples.count() )
	currSample = 0;

    return sent;
}

/*
 * Handler for /dev/tpanel Linux kernel driver
 */

class QVrTPanelHandlerPrivate : public QCalibratedMouseHandler {
    Q_OBJECT
public:
    QVrTPanelHandlerPrivate(MouseProtocol, QString dev);
    ~QVrTPanelHandlerPrivate();

private:
    int mouseFD;
    MouseProtocol mouseProtocol;
private slots:
    void sendRelease();
    void readMouseData();
private:
    static const int mouseBufSize = 1280;
    QTimer *rtimer;
    int mouseIdx;
    uchar mouseBuf[mouseBufSize];
};

#ifndef QT_QWS_CASSIOPEIA
QVrTPanelHandlerPrivate::QVrTPanelHandlerPrivate( MouseProtocol, QString ) :
    QCalibratedMouseHandler()
{
}
#else
QVrTPanelHandlerPrivate::QVrTPanelHandlerPrivate( MouseProtocol, QString dev ) :
    QCalibratedMouseHandler()
{
    if ( dev.isEmpty() )
	dev = "/dev/tpanel";

    if ((mouseFD = open( dev, O_RDONLY)) < 0) {
        qFatal( "Cannot open %s (%s)", dev.latin1(), strerror(errno));
    } else {
        sleep(1);
    }

    struct scanparam s;
    s.interval = 20000;
    s.settletime = 480;
    if ( ioctl(mouseFD, TPSETSCANPARM, &s) < 0
      || fcntl(mouseFD, F_SETFL, O_NONBLOCK) < 0 )
	qWarning("Error initializing touch panel.");

    QSocketNotifier *mouseNotifier;
    mouseNotifier = new QSocketNotifier( mouseFD, QSocketNotifier::Read,
					 this );
    connect(mouseNotifier, SIGNAL(activated(int)),this, SLOT(readMouseData()));

    rtimer = new QTimer( this );
    connect( rtimer, SIGNAL(timeout()), this, SLOT(sendRelease()));
    mouseIdx = 0;
    setFilterSize( 3 );

    printf("\033[?25l"); fflush(stdout); // VT100 cursor off
}
#endif

QVrTPanelHandlerPrivate::~QVrTPanelHandlerPrivate()
{
    if (mouseFD >= 0)
	close(mouseFD);
}

void QVrTPanelHandlerPrivate::sendRelease()
{
    sendFiltered( mousePos, 0 );
}

void QVrTPanelHandlerPrivate::readMouseData()
{
#ifdef QT_QWS_CASSIOPEIA
    if(!qt_screen)
	return;
    static bool pressed = FALSE;

    int n;
    do {
	n = read(mouseFD, mouseBuf+mouseIdx, mouseBufSize-mouseIdx );
	if ( n > 0 )
	    mouseIdx += n;
    } while ( n > 0 && mouseIdx < mouseBufSize );

    int idx = 0;
    while ( mouseIdx-idx >= (int)sizeof( short ) * 6 ) {
	uchar *mb = mouseBuf+idx;
	ushort *data = (ushort *) mb;
	if ( data[0] & 0x8000 ) {
	    if ( data[5] > 750 ) {
		QPoint t(data[3]-data[4],data[2]-data[1]);
		if ( sendFiltered( t, Qt::LeftButton ) )
		    pressed = TRUE;
		if ( pressed )
		    rtimer->start( 200, TRUE ); // release unreliable
	    }
	} else if ( pressed ) {
	    rtimer->start( 50, TRUE );
	    pressed = FALSE;
	}
	idx += sizeof( ushort ) * 6;
    }

    int surplus = mouseIdx - idx;
    for ( int i = 0; i < surplus; i++ )
	mouseBuf[i] = mouseBuf[idx+i];
    mouseIdx = surplus;

#endif
}


class QTPanelHandlerPrivate : public QCalibratedMouseHandler
{
     Q_OBJECT
public:
    QTPanelHandlerPrivate(MouseProtocol, QString dev);
    ~QTPanelHandlerPrivate();

private:
    static const int mouseBufSize = 2048;
    int mouseFD;
    QPoint oldmouse;
    QPoint oldTotalMousePos;
    bool waspressed;
    QPointArray samples;
    unsigned int currSample;
    unsigned int lastSample;
    unsigned int numSamples;
    int skipCount;
    int mouseIdx;
    uchar mouseBuf[mouseBufSize];

private slots:
    void readMouseData();
};


QTPanelHandlerPrivate::QTPanelHandlerPrivate( MouseProtocol, QString dev )
    : samples(QT_QWS_TP_SAMPLE_SIZE), currSample(0), lastSample(0),
    numSamples(0), skipCount(0)
{
    mouseFD = -1;
#if defined(QT_QWS_IPAQ) || defined(QT_QWS_SL5XXX) || defined(QT_QWS_K2)
    if ( dev.isEmpty() )
#if defined(QT_QWS_IPAQ)
#ifdef QT_QWS_IPAQ_RAW
    dev = "/dev/h3600_tsraw";
#else
    dev = "/dev/h3600_ts";
#endif

#elif defined(QT_QWS_SL5XXX) || defined(QT_QWS_K2)
//# ifdef QT_QWS_SL5XXX_TSRAW
# if 0
    dev = "/dev/tsraw";
#else
    dev = "/dev/ts";
#endif
#endif
    if ((mouseFD = open( dev.local8Bit().data(), O_RDONLY | O_NDELAY)) < 0) {
        qWarning( "Cannot open %s (%s)", dev.latin1(), strerror(errno));
	return;
    }

    QSocketNotifier *mouseNotifier;
    mouseNotifier = new QSocketNotifier( mouseFD, QSocketNotifier::Read,
					 this );
    connect(mouseNotifier, SIGNAL(activated(int)),this, SLOT(readMouseData()));
    waspressed=FALSE;
    mouseIdx = 0;
#else
    Q_UNUSED(dev);
#endif
}

QTPanelHandlerPrivate::~QTPanelHandlerPrivate()
{
#if defined(QT_QWS_IPAQ) || defined(QT_QWS_SL5XXX) || defined(QT_QWS_K2)
    if (mouseFD >= 0)
	close(mouseFD);
#endif
}

void QTPanelHandlerPrivate::readMouseData()
{
#if defined(QT_QWS_IPAQ) || defined(QT_QWS_SL5XXX) || defined(QT_QWS_K2)
    if(!qt_screen)
	return;

    int n;
    do {
	n = read(mouseFD, mouseBuf+mouseIdx, mouseBufSize-mouseIdx );
	if ( n > 0 )
	    mouseIdx += n;
    } while ( n > 0 && mouseIdx < mouseBufSize );

    TS_EVENT *data;
    int idx = 0;

    // perhaps we shouldn't be reading EVERY SAMPLE.
    while ( mouseIdx-idx >= (int)sizeof( TS_EVENT ) ) {
	uchar *mb = mouseBuf+idx;
	data = (TS_EVENT *) mb;
	if(data->pressure >= QT_QWS_TP_PRESSURE_THRESHOLD) {
#ifdef QT_QWS_SL5XXX
	    samples[currSample] = QPoint( 1000 - data->x, data->y );
#else
	    samples[currSample] = QPoint( data->x, data->y );
#endif

	    numSamples++;
	    if ( numSamples >= QT_QWS_TP_MINIMUM_SAMPLES ) {
		int sampleCount = QMIN(numSamples + 1,samples.count());

		// average the rest
		mousePos = QPoint( 0, 0 );
		QPoint totalMousePos = oldTotalMousePos;
		totalMousePos += samples[currSample];
		if(numSamples >= samples.count())
		    totalMousePos -= samples[lastSample];

		mousePos = totalMousePos / (sampleCount - 1);

# if defined(QT_QWS_IPAQ_RAW) || defined(QT_QWS_SL5XXX_RAW)
		mousePos = transform( mousePos );
# endif
		if(!waspressed)
		    oldmouse = mousePos;
		QPoint dp = mousePos - oldmouse;
		int dxSqr = dp.x() * dp.x();
		int dySqr = dp.y() * dp.y();
		if ( dxSqr + dySqr < (QT_QWS_TP_MOVE_LIMIT * QT_QWS_TP_MOVE_LIMIT) ) {
		    if ( waspressed ) {
			if ( (dxSqr + dySqr > (QT_QWS_TP_JITTER_LIMIT * QT_QWS_TP_JITTER_LIMIT) ) || skipCount > 2) {
			    emit mouseChanged(mousePos,Qt::LeftButton);
			    oldmouse = mousePos;
			    skipCount = 0;
			} else {
			    skipCount++;
			}
		    } else {
			emit mouseChanged(mousePos,Qt::LeftButton);
			oldmouse=mousePos;
			waspressed=true;
		    }

		    // save recuring information
		    currSample++;
		    if (numSamples >= samples.count())
			lastSample++;
		    oldTotalMousePos = totalMousePos;
		} else {
		    numSamples--; // don't use this sample, it was bad.
		}
	    } else {
		// build up the average
		oldTotalMousePos += samples[currSample];
		currSample++;
	    }
	    if ( currSample >= samples.count() )
		currSample = 0;
	    if ( lastSample >= samples.count() )
		lastSample = 0;
	} else {
	    currSample = 0;
	    lastSample = 0;
	    numSamples = 0;
	    skipCount = 0;
	    oldTotalMousePos = QPoint(0,0);
	    if ( waspressed ) {
		emit mouseChanged(oldmouse,0);
		oldmouse = QPoint( -100, -100 );
		waspressed=false;
	    }
	}
	idx += sizeof( TS_EVENT );
    }

    int surplus = mouseIdx - idx;
    for ( int i = 0; i < surplus; i++ )
	mouseBuf[i] = mouseBuf[idx+i];
    mouseIdx = surplus;
#endif
}

// YOPY touch panel support based on changes contributed by Ron Victorelli
// (victorrj at icubed.com) to Custom TP driver.
//
class QYopyTPanelHandlerPrivate : public QWSMouseHandler {
    Q_OBJECT
public:
    QYopyTPanelHandlerPrivate(MouseProtocol, QString mouseDev);
    ~QYopyTPanelHandlerPrivate();

private:
    int mouseFD;
    int prevstate;
private slots:
    void readMouseData();

};

QYopyTPanelHandlerPrivate::QYopyTPanelHandlerPrivate( MouseProtocol, QString mouseDev )
{
#ifdef QT_QWS_YOPY
    if ( mouseDev.isEmpty() )
	mouseDev = "/dev/ts";
 
    if ((mouseFD = open( mouseDev.local8Bit().data(), O_RDONLY)) < 0) {
        qWarning( "Cannot open %s (%s)", mouseDev.local8Bit().data(), strerror(errno));
	return;
    } else {
        sleep(1);
    }
    prevstate=0;
    QSocketNotifier *mouseNotifier;
    mouseNotifier = new QSocketNotifier( mouseFD, QSocketNotifier::Read,
					 this );
    connect(mouseNotifier, SIGNAL(activated(int)),this, SLOT(readMouseData()));
#else
    Q_UNUSED(mouseDev);
#endif
}

QYopyTPanelHandlerPrivate::~QYopyTPanelHandlerPrivate()
{
    if (mouseFD >= 0)
	close(mouseFD);
}

#define YOPY_XPOS(d) (d[1]&0x3FF)
#define YOPY_YPOS(d) (d[2]&0x3FF)
#define YOPY_PRES(d) (d[0]&0xFF)
#define YOPY_STAT(d) (d[3]&0x01 )

struct YopyTPdata {

  unsigned char status;
  unsigned short xpos;
  unsigned short ypos;

};

void QYopyTPanelHandlerPrivate::readMouseData()
{
#ifdef QT_QWS_YOPY
    if(!qt_screen)
	return;
    YopyTPdata data;

    unsigned int yopDat[4];

    int ret;

    ret=read(mouseFD,&yopDat,sizeof(yopDat));

    if(ret) {
        data.status= ( YOPY_PRES(yopDat) ) ? 1 : 0;
	data.xpos=YOPY_XPOS(yopDat);
	data.ypos=YOPY_YPOS(yopDat);
	QPoint q;
	q.setX(data.xpos);
	q.setY(data.ypos);
	mousePos=q;
	if(data.status && !prevstate) {
          emit mouseChanged(mousePos,Qt::LeftButton);
        } else if( !data.status && prevstate ) {
	  emit mouseChanged(mousePos,0);
        }
        prevstate = data.status;
    }
    if(ret<0) {
	qDebug("Error %s",strerror(errno));
    }
#endif
}

class QCustomTPanelHandlerPrivate : public QWSMouseHandler {
    Q_OBJECT
public:
    QCustomTPanelHandlerPrivate(MouseProtocol, QString dev);
    ~QCustomTPanelHandlerPrivate();

private:
    int mouseFD;
private slots:
    void readMouseData();

};

QCustomTPanelHandlerPrivate::QCustomTPanelHandlerPrivate( MouseProtocol, QString mouseDev)
{
#ifdef QWS_CUSTOMTOUCHPANEL
    mouseFD = -1;
    if ( mouseDev.isEmpty() )
      mouseDev = "/dev/ts";
    if ((mouseFD = open( mouseDev.local8Bit().data(), O_RDONLY)) < 0) {
        qWarning( "Cannot open %s (%s)", mouseDev.latin1(), strerror(errno));
	return;
    } else {
        sleep(1);
    }

    QSocketNotifier *mouseNotifier;
    mouseNotifier = new QSocketNotifier( mouseFD, QSocketNotifier::Read,
					 this );
    connect(mouseNotifier, SIGNAL(activated(int)),this, SLOT(readMouseData()));
#else
    Q_UNUSED(mouseDev);
#endif
}

QCustomTPanelHandlerPrivate::~QCustomTPanelHandlerPrivate()
{
    if (mouseFD >= 0)
	close(mouseFD);
}

struct CustomTPdata {

  unsigned char status;
  unsigned short xpos;
  unsigned short ypos;

};

void QCustomTPanelHandlerPrivate::readMouseData()
{
#ifdef QWS_CUSTOMTOUCHPANEL
    if(!qt_screen)
	return;
    CustomTPdata data;

    unsigned char data2[5];

    int ret;

    ret=read(mouseFD,data2,5);

    if(ret==5) {
	data.status=data2[0];
	data.xpos=(data2[1] << 8) | data2[2];
	data.ypos=(data2[3] << 8) | data2[4];
	QPoint q;
	q.setX(data.xpos);
	q.setY(data.ypos);
	mousePos=q;
	if(data.status & 0x40) {
          emit mouseChanged(mousePos,Qt::LeftButton);
	} else {
	  emit mouseChanged(mousePos,0);
	}
    }
    if(ret<0) {
	qDebug("Error %s",strerror(errno));
    }
#endif
}

/*
 * Virtual framebuffer mouse driver
 */

#ifndef QT_NO_QWS_VFB
#include "qvfbhdr.h"
extern int qws_display_id;
#endif

class QVFbMouseHandlerPrivate : public QWSMouseHandler {
    Q_OBJECT
public:
    QVFbMouseHandlerPrivate(MouseProtocol, QString dev);
    ~QVFbMouseHandlerPrivate();

#ifndef QT_NO_QWS_VFB
    bool isOpen() const { return mouseFD > 0; }

private:
    static const int mouseBufSize = 128;
    int mouseFD;
    int mouseIdx;
    uchar mouseBuf[mouseBufSize];
#endif

private slots:
    void readMouseData();
};

QVFbMouseHandlerPrivate::QVFbMouseHandlerPrivate( MouseProtocol, QString mouseDev )
{
#ifndef QT_NO_QWS_VFB
    mouseFD = -1;
    if ( mouseDev.isEmpty() )
	mouseDev = QString(QT_VFB_MOUSE_PIPE).arg(qws_display_id);

    if ((mouseFD = open( mouseDev.local8Bit().data(), O_RDONLY | O_NDELAY)) < 0) {
	qDebug( "Cannot open %s (%s)", mouseDev.ascii(),
		strerror(errno));
    } else {
	// Clear pending input
	char buf[2];
	while (read(mouseFD, buf, 1) > 0) { }

	mouseIdx = 0;

	QSocketNotifier *mouseNotifier;
	mouseNotifier = new QSocketNotifier( mouseFD, QSocketNotifier::Read, this );
	connect(mouseNotifier, SIGNAL(activated(int)),this, SLOT(readMouseData()));
    }
#endif
}

QVFbMouseHandlerPrivate::~QVFbMouseHandlerPrivate()
{
#ifndef QT_NO_QWS_VFB
    if (mouseFD >= 0)
	close(mouseFD);
#endif
}

void QVFbMouseHandlerPrivate::readMouseData()
{
#ifndef QT_NO_QWS_VFB
    int n;
    do {
	n = read(mouseFD, mouseBuf+mouseIdx, mouseBufSize-mouseIdx );
	if ( n > 0 )
	    mouseIdx += n;
    } while ( n > 0 );

    int idx = 0;
    while ( mouseIdx-idx >= int(sizeof( QPoint ) + sizeof( int )) ) {
	uchar *mb = mouseBuf+idx;
	QPoint *p = (QPoint *) mb;
	mb += sizeof( QPoint );
	int *bstate = (int *)mb;
	mousePos = *p;
	limitToScreen( mousePos );
	emit mouseChanged(mousePos, *bstate);
	idx += sizeof( QPoint ) + sizeof( int );
    }

    int surplus = mouseIdx - idx;
    for ( int i = 0; i < surplus; i++ )
	mouseBuf[i] = mouseBuf[idx+i];
    mouseIdx = surplus;
#endif
}

/*
  mouse handler for tslib
  see http://cvs.arm.linux.org.uk/
 */
/*

 Copyright (C) 2003, 2004, 2005 Texas Instruments, Inc.
 Copyright (C)       2004, 2005 Holger Hans Peter Freyther
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

   Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

   Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

   Neither the name Texas Instruments, Inc nor the names of its
   contributors may be used to endorse or promote products derived
   from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.

*/
class QTSLibHandlerPrivate : public QCalibratedMouseHandler
{
    Q_OBJECT
public:
    QTSLibHandlerPrivate( MouseProtocol, QString mouseDev );
   ~QTSLibHandlerPrivate();

    virtual void clearCalibration();
    virtual void calibrate( QWSPointerCalibrationData * );
    static int sortByX( const void*, const void* );
    static int sortByY( const void*, const void* );

private:
    bool m_raw : 1;
    QSocketNotifier *m_notify;
    QString m_dev;

#ifdef QT_QWS_TSLIB
    struct tsdev *m_ts;
#endif
    void openTs();
    void closeTs();
    void interpolateSample();

private slots:
    void readMouseData();

};

QTSLibHandlerPrivate::QTSLibHandlerPrivate(MouseProtocol, QString mouseDev)
    : m_raw(false), m_notify(0), m_dev(mouseDev)
{
   openTs();
}

QTSLibHandlerPrivate::~QTSLibHandlerPrivate()
{
    closeTs();
}

void QTSLibHandlerPrivate::openTs()
{
#ifdef QT_QWS_TSLIB
   QString tsdevice(getenv("TSLIB_TSDEVICE"));

   if(tsdevice.isEmpty() && !m_dev.isEmpty())
       tsdevice = m_dev;

   if (tsdevice.isEmpty())
       tsdevice = "/dev/ts";

   m_ts = ts_open( tsdevice.local8Bit().data(), 1 ); //1 = nonblocking, 0 = blocking mode

   if (!m_ts) {
      qWarning( "Cannot open touchscreen %s (%s)", tsdevice.local8Bit().data(), strerror( errno));
      return;
   }

   if (ts_config( m_ts)) {
      qWarning( "Cannot configure touchscreen %s (%s)", tsdevice.local8Bit().data(), strerror( errno));
      return;
   }
   m_notify = new QSocketNotifier( ts_fd(m_ts), QSocketNotifier::Read, this );
   connect( m_notify, SIGNAL(activated(int)),this, SLOT(readMouseData()));
#endif
}

void QTSLibHandlerPrivate::closeTs()
{
#ifdef QT_QWS_TSLIB
    if (m_ts)
        ts_close(m_ts);
    m_ts = 0;
#endif

    delete m_notify;
    m_notify = 0;
    m_raw = false;
}

void QTSLibHandlerPrivate::clearCalibration()
{
    m_raw = true;
}

void QTSLibHandlerPrivate::calibrate( QWSPointerCalibrationData * cd)
{
    QPoint dev_tl = cd->devPoints[ QWSPointerCalibrationData::TopLeft ];
    QPoint dev_br = cd->devPoints[ QWSPointerCalibrationData::BottomRight ];
    QPoint screen_tl = cd->screenPoints[ QWSPointerCalibrationData::TopLeft ];
    QPoint screen_br = cd->screenPoints[ QWSPointerCalibrationData::BottomRight ];
    int a, b, c, d, e, f, s;

    s = 1 << 16;

    a = s * (screen_tl.x() - screen_br.x() ) / (dev_tl.x() - dev_br.x());
    b = 0;
    c = s * screen_tl.x() - a * dev_tl.x();

    d = 0;
    e = s * (screen_tl.y() - screen_br.y() ) / (dev_tl.y() - dev_br.y());
    f = s * screen_tl.y() - e * dev_tl.y();

    QString calFile = "/etc/pointercal";
#ifndef QT_NO_TEXTSTREAM
    QFile file( calFile );
    if ( file.open( IO_WriteOnly ) ) {
        QTextStream t( &file );
        t << a << " " << b << " " << c << " ";
        t << d << " " << e << " " << f << " " << s;
       file.flush(); closeTs();
       openTs();
    } else
#endif
    {
        qDebug( "Could not save calibration: %s", calFile.latin1() );
    }
}

void QTSLibHandlerPrivate::readMouseData()
{
#ifdef QT_QWS_TSLIB
    if(!qt_screen)
        return;

    /*
     * After clear Calibration
     * we're in raw mode and do some easy median
     * search.
     */
//    if ( m_raw )
//        return interpolateSample(); //commented out until it works

    static struct ts_sample sample;
    static int ret;

    /*
     * Ok. We need to see if we can read more than one event
     * We do this not to lose an update.
     */
    while ( true ) {
        if ((ret = ts_read(m_ts, &sample, 1)) != 1 )
            return;


        QPoint pos( sample.x, sample.y );
        emit mouseChanged( pos, sample.pressure != 0 ? 1 : 0 );
    }
#endif
}

/*
 * Lets take all down events and then sort them
 * and take the event in the middle.
 *
 * inspired by testutils.c
 */
void QTSLibHandlerPrivate::interpolateSample() {
#ifdef QT_QWS_TSLIB
#define TSLIB_MAX_SAMPLES 25
    static struct ts_sample samples[TSLIB_MAX_SAMPLES];
    int index = 0;
    int read_samples = 0;
    int ret;

    do {
        /* do not access negative arrays */
        if ( index < 0 )
            index = 0;
	    
        /* we're opened non-blocking */
        if((ret= ts_read_raw(m_ts, &samples[index], 1 ) ) !=  1 )
            /* no event yet, so try again */
            if (ret==-1 )
                continue;
	
	read_samples++;
	index = (index+1)%TSLIB_MAX_SAMPLES;
    } while (samples[index == 0 ? (TSLIB_MAX_SAMPLES-1) : index-1].pressure != 0);

    /*
     * If we've wrapped around each sample is used otherwise
     * we will use the index
     */
    index = read_samples >= TSLIB_MAX_SAMPLES ?
            (TSLIB_MAX_SAMPLES-1 ) : index;
    int x, y;

    /*
     * now let us use the median value
     * even index does not have an item in the middle
     * so let us take the average of n/2 and (n/2)-1 as the middle
     */
    int m = index/2;
    ::qsort(samples, index, sizeof(ts_sample), QTSLibHandlerPrivate::sortByX);
    x = (index % 2 ) ? samples[m].x :
        ( samples[m-1].x + samples[m].x )/2;

    ::qsort(samples, index, sizeof(ts_sample), QTSLibHandlerPrivate::sortByY);
    y = (index % 2 ) ? samples[m].y :
        ( samples[m-1].y + samples[m].y )/2;

    emit mouseChanged( QPoint(x, y), 1 );
    emit mouseChanged( QPoint(0, 0), 0 );
#endif
}

int QTSLibHandlerPrivate::sortByX( const void* one, const void* two) {
#ifdef QT_QWS_TSLIB
    return reinterpret_cast<const struct ts_sample*>(one)->x -
        reinterpret_cast<const struct ts_sample*>(two)->x;
#else
    Q_UNUSED(one);
    Q_UNUSED(two);
    return 0;
#endif
}

int QTSLibHandlerPrivate::sortByY( const void* one, const void* two) {
#ifdef QT_QWS_TSLIB
    return reinterpret_cast<const struct ts_sample*>(one)->y -
        reinterpret_cast<const struct ts_sample*>(two)->y;
#else
    Q_UNUSED(one);
    Q_UNUSED(two);
    return 0;
#endif
}
//////

/*
 * return a QWSMouseHandler that supports /a spec.
 */

QWSMouseHandler* QWSServer::newMouseHandler(const QString& spec)
{
    static int init=0;
    if ( !init && qt_screen ) {
	init = 1;
//	mousePos = QPoint(qt_screen->width()/2,
//			  qt_screen->height()/2);
    }

    int c = spec.find(':');
    QString mouseProto;
    QString mouseDev;
    if ( c >= 0 ) {
	mouseProto = spec.left(c);
	mouseDev = spec.mid(c+1);
    } else {
	mouseProto = spec;
    }

    if ( mouseProto == "USB" && mouseDev.isEmpty() )
	mouseDev = "/dev/input/mice";

    MouseProtocol mouseProtocol = Unknown;

    int idx = 0;
    while (mouseProtocol == Unknown && mouseConfig[idx].name) {
	if (mouseProto == QString(mouseConfig[idx].name)) {
	    mouseProtocol = mouseConfig[idx].id;
	}
	idx++;
    }


    QWSMouseHandler *handler = 0;

    switch ( mouseProtocol ) {
#ifndef QT_NO_QWS_MOUSE_AUTO
	case Auto:
	    handler = new QAutoMouseHandler();
	    break;
#endif

#ifndef QT_NO_QWS_MOUSE_PC
	case MouseMan:
	case IntelliMouse:
	case Microsoft:
	case BusMouse:
	    handler = new QWSMouseHandlerPrivate( mouseProtocol, mouseDev );
	    break;
#endif

#ifndef QT_NO_QWS_VFB
	case QVFBMouse:
	    handler = new QVFbMouseHandlerPrivate( mouseProtocol, mouseDev );
	    break;
#endif

	case TPanel:
#if defined(QWS_CUSTOMTOUCHPANEL)
	    handler = new QCustomTPanelHandlerPrivate(mouseProtocol,mouseDev);
#elif defined(QT_QWS_TSLIB)
	    handler = new QTSLibHandlerPrivate(mouseProtocol,mouseDev);
#elif defined(QT_QWS_YOPY)
	    handler = new QYopyTPanelHandlerPrivate(mouseProtocol,mouseDev);
#elif defined(QT_QWS_IPAQ) || defined(QT_QWS_SL5XXX) || defined(QT_QWS_K2)
	    handler = new QTPanelHandlerPrivate(mouseProtocol,mouseDev);
#elif defined(QT_QWS_CASSIOPEIA)
	    handler = new QVrTPanelHandlerPrivate( mouseProtocol, mouseDev );
#endif
	    break;

	default:
	    qDebug( "Mouse type %s unsupported", spec.latin1() );
    }

    return handler;
}

#include "qwsmouse_qws.moc"

#endif // QNX6
