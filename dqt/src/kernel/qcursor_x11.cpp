/****************************************************************************
** $Id: qt/qcursor_x11.cpp   3.3.5   edited Aug 31 12:13 $
**
** Implementation of QCursor class for X11
**
** Created : 940219
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
** licenses for Unix/X11 may use this file in accordance with the Qt Commercial
** License Agreement provided with the Software.
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

#include "qcursor.h"
#include "qbitmap.h"
#include "qimage.h"
#include "qapplication.h"
#include "qdatastream.h"
#include "qnamespace.h"
#include "qt_x11_p.h"
#include <X11/cursorfont.h>

#ifndef QT_NO_XCURSOR
#  include <X11/Xcursor/Xcursor.h>
#endif // QT_NO_XCURSOR

// Define QT_USE_APPROXIMATE_CURSORS when compiling if you REALLY want to
// use the ugly X11 cursors.

/*****************************************************************************
  Internal QCursorData class
 *****************************************************************************/

struct QCursorData : public QShared
{
    QCursorData( int s = 0 );
   ~QCursorData();
    int	      cshape;
    QBitmap  *bm, *bmm;
    short     hx, hy;
    XColor    fg,bg;
    Cursor    hcurs;
    Pixmap    pm, pmm;
};

QCursorData::QCursorData( int s )
{
    cshape = s;
    hcurs = 0;
    bm = bmm = 0;
    hx = hy  = 0;
    pm = pmm = 0;
}

QCursorData::~QCursorData()
{
    Display *dpy = QPaintDevice::x11AppDisplay();

    // Add in checking for the display too as on HP-UX
    // we seem to get a core dump as the cursor data is
    // deleted again from main() on exit...
    if ( hcurs && dpy )
	XFreeCursor( dpy, hcurs );
    if ( pm && dpy )
	XFreePixmap( dpy, pm );
    if ( pmm && dpy )
	XFreePixmap( dpy, pmm );
    delete bm;
    delete bmm;
}


/*****************************************************************************
  Global cursors
 *****************************************************************************/

static QCursor cursorTable[Qt::LastCursor+1];

static const int arrowCursorIdx = 0;

QT_STATIC_CONST_IMPL QCursor & Qt::arrowCursor = cursorTable[0];
QT_STATIC_CONST_IMPL QCursor & Qt::upArrowCursor = cursorTable[1];
QT_STATIC_CONST_IMPL QCursor & Qt::crossCursor = cursorTable[2];
QT_STATIC_CONST_IMPL QCursor & Qt::waitCursor = cursorTable[3];
QT_STATIC_CONST_IMPL QCursor & Qt::ibeamCursor = cursorTable[4];
QT_STATIC_CONST_IMPL QCursor & Qt::sizeVerCursor = cursorTable[5];
QT_STATIC_CONST_IMPL QCursor & Qt::sizeHorCursor = cursorTable[6];
QT_STATIC_CONST_IMPL QCursor & Qt::sizeBDiagCursor = cursorTable[7];
QT_STATIC_CONST_IMPL QCursor & Qt::sizeFDiagCursor = cursorTable[8];
QT_STATIC_CONST_IMPL QCursor & Qt::sizeAllCursor = cursorTable[9];
QT_STATIC_CONST_IMPL QCursor & Qt::blankCursor = cursorTable[10];
QT_STATIC_CONST_IMPL QCursor & Qt::splitVCursor = cursorTable[11];
QT_STATIC_CONST_IMPL QCursor & Qt::splitHCursor = cursorTable[12];
QT_STATIC_CONST_IMPL QCursor & Qt::pointingHandCursor = cursorTable[13];
QT_STATIC_CONST_IMPL QCursor & Qt::forbiddenCursor = cursorTable[14];
QT_STATIC_CONST_IMPL QCursor & Qt::whatsThisCursor = cursorTable[15];
QT_STATIC_CONST_IMPL QCursor & Qt::busyCursor = cursorTable[16];


QCursor *QCursor::find_cur( int shape )		// find predefined cursor
{
    return (uint)shape <= LastCursor ? &cursorTable[shape] : 0;
}


static bool initialized = FALSE;

/*!
    Internal function that deinitializes the predefined cursors.
    This function is called from the QApplication destructor.

    \sa initialize()
*/
void QCursor::cleanup()
{
    if ( !initialized )
	return;

    int shape;
    for( shape = 0; shape <= LastCursor; shape++ ) {
	if ( cursorTable[shape].data && cursorTable[shape].data->deref() )
	    delete cursorTable[shape].data;
	cursorTable[shape].data = 0;
    }
    initialized = FALSE;
}


/*!
    Internal function that initializes the predefined cursors.
    This function is called from the QApplication constructor.

    \sa cleanup()
*/

void QCursor::initialize()
{
    int shape;
    for( shape = 0; shape <= LastCursor; shape++ )
	cursorTable[shape].data = new QCursorData( shape );
    initialized = TRUE;
    qAddPostRoutine( cleanup );
}


/*!
    Constructs a cursor with the default arrow shape.
*/
QCursor::QCursor()
{
    if ( !initialized ) {
	if ( qApp->startingUp() ) {
	    data = 0;
	    return;
	}
	initialize();
    }
    QCursor* c = &cursorTable[arrowCursorIdx];
    c->data->ref();
    data = c->data;
}



/*!
    Constructs a cursor with the specified \a shape.

    See \l CursorShape for a list of shapes.

    \sa setShape()
*/

QCursor::QCursor( int shape )
{
    if ( !initialized )
	initialize();
    QCursor *c = find_cur( shape );
    if ( !c )					// not found
	c = &cursorTable[arrowCursorIdx];	//   then use arrowCursor
    c->data->ref();
    data = c->data;
}

/*!
    Constructs a cursor from the window system cursor \a cursor.

    \warning Using this function is not portable. This function is only
    available on X11 and Windows.
*/
QCursor::QCursor( HANDLE cursor )
{
    if ( !initialized )
	initialize();

    data = new QCursorData;
    Q_CHECK_PTR( data );
    data->hcurs = cursor;
}



void QCursor::setBitmap( const QBitmap &bitmap, const QBitmap &mask,
			 int hotX, int hotY )
{
    if ( !initialized )
	initialize();
    if ( bitmap.depth() != 1 || mask.depth() != 1 ||
	 bitmap.size() != mask.size() ) {
#if defined(QT_CHECK_NULL)
	qWarning( "QCursor: Cannot create bitmap cursor; invalid bitmap(s)" );
#endif
	QCursor *c = &cursorTable[arrowCursorIdx];
	c->data->ref();
	data = c->data;
	return;
    }
    data = new QCursorData;
    Q_CHECK_PTR( data );
    data->bm  = new QBitmap( bitmap );
    data->bmm = new QBitmap( mask );
    data->hcurs = 0;
    data->cshape = BitmapCursor;
    data->hx = hotX >= 0 ? hotX : bitmap.width()/2;
    data->hy = hotY >= 0 ? hotY : bitmap.height()/2;
    data->fg.red   = 0 << 8;
    data->fg.green = 0 << 8;
    data->fg.blue  = 0 << 8;
    data->bg.red   = 255 << 8;
    data->bg.green = 255 << 8;
    data->bg.blue  = 255 << 8;
}


/*!
    Constructs a copy of the cursor \a c.
*/

QCursor::QCursor( const QCursor &c )
{
    if ( !initialized )
	initialize();
    data = c.data;				// shallow copy
    data->ref();
}

/*!
    Destroys the cursor.
*/

QCursor::~QCursor()
{
    if ( data && data->deref() )
	delete data;
}


/*!
    Assigns \a c to this cursor and returns a reference to this
    cursor.
*/

QCursor &QCursor::operator=( const QCursor &c )
{
    if ( !initialized )
	initialize();
    c.data->ref();				// avoid c = c
    if ( data->deref() )
	delete data;
    data = c.data;
    return *this;
}


/*!
    Returns the cursor shape identifier. The return value is one of
    the \l CursorShape enum values (cast to an int).

    \sa setShape()
*/

int QCursor::shape() const
{
    if ( !initialized )
	initialize();
    return data->cshape;
}

/*!
    Sets the cursor to the shape identified by \a shape.

    See \l CursorShape for the list of cursor shapes.

    \sa shape()
*/

void QCursor::setShape( int shape )
{
    if ( !initialized )
	initialize();
    QCursor *c = find_cur( shape );		// find one of the global ones
    if ( !c )					// not found
	c = &cursorTable[arrowCursorIdx];	//   then use arrowCursor
    c->data->ref();
    if ( data->deref() )			// make shallow copy
	delete data;
    data = c->data;
}


/*!
    Returns the cursor bitmap, or 0 if it is one of the standard
    cursors.
*/
const QBitmap *QCursor::bitmap() const
{
    if ( !initialized )
	initialize();
    return data->bm;
}

/*!
    Returns the cursor bitmap mask, or 0 if it is one of the standard
    cursors.
*/

const QBitmap *QCursor::mask() const
{
    if ( !initialized )
	initialize();
    return data->bmm;
}

/*!
    Returns the cursor hot spot, or (0, 0) if it is one of the
    standard cursors.
*/

QPoint QCursor::hotSpot() const
{
    if ( !initialized )
	initialize();
    return QPoint( data->hx, data->hy );
}


/*!
    Returns the window system cursor handle.

    \warning
    Portable in principle, but if you use it you are probably about to
    do something non-portable. Be careful.
*/

Qt::HANDLE QCursor::handle() const
{
    if ( !initialized )
	initialize();
    if ( !data->hcurs )
	update();
    return data->hcurs;
}

/*!
    \fn QCursor::QCursor( HCURSOR handle )

    Creates a cursor with the specified window system handle \a
    handle.

    \warning
    Portable in principle, but if you use it you are probably about to
    do something non-portable. Be careful.
*/

/*!
    Returns the position of the cursor (hot spot) in global screen
    coordinates.

    You can call QWidget::mapFromGlobal() to translate it to widget
    coordinates.

    \sa setPos(), QWidget::mapFromGlobal(), QWidget::mapToGlobal()
*/
QPoint QCursor::pos()
{
    Window root;
    Window child;
    int root_x, root_y, win_x, win_y;
    uint buttons;
    Display* dpy = QPaintDevice::x11AppDisplay();
    for ( int i = 0; i < ScreenCount( dpy ); i++ ) {
	if ( XQueryPointer( dpy, QPaintDevice::x11AppRootWindow( i ), &root, &child,
			    &root_x, &root_y, &win_x, &win_y, &buttons ) )

	    return QPoint( root_x, root_y );
    }
    return QPoint();
}

/*! \internal
*/
int QCursor::x11Screen()
{
    Window root;
    Window child;
    int root_x, root_y, win_x, win_y;
    uint buttons;
    Display* dpy = QPaintDevice::x11AppDisplay();
    for ( int i = 0; i < ScreenCount( dpy ); i++ ) {
	if ( XQueryPointer( dpy, QPaintDevice::x11AppRootWindow( i ), &root, &child,
			    &root_x, &root_y, &win_x, &win_y, &buttons ) )
	    return i;
    }
    return -1;
}

/*!
    Moves the cursor (hot spot) to the global screen position (\a x,
    \a y).

    You can call QWidget::mapToGlobal() to translate widget
    coordinates to global screen coordinates.

    \sa pos(), QWidget::mapFromGlobal(), QWidget::mapToGlobal()
*/

void QCursor::setPos( int x, int y )
{
    QPoint current, target(x, y);

    // this is copied from pos(), since we need the screen number for the correct
    // root window in the XWarpPointer call
    Window root;
    Window child;
    int root_x, root_y, win_x, win_y;
    uint buttons;
    Display* dpy = QPaintDevice::x11AppDisplay();
    int screen;
    for ( screen = 0; screen < ScreenCount( dpy ); screen++ ) {
	if ( XQueryPointer( dpy, QPaintDevice::x11AppRootWindow( screen ), &root, &child,
			    &root_x, &root_y, &win_x, &win_y, &buttons ) ) {
	    current = QPoint( root_x, root_y );
	    break;
	}
    }

    if ( screen >= ScreenCount( dpy ) )
	return;

    // Need to check, since some X servers generate null mouse move
    // events, causing looping in applications which call setPos() on
    // every mouse move event.
    //
    if ( current == target )
	return;

    XWarpPointer( QPaintDevice::x11AppDisplay(), None,
		  QPaintDevice::x11AppRootWindow( screen ),
		  0, 0, 0, 0, x, y );
}

/*!
    \overload void QCursor::setPos ( const QPoint & )
*/


/*!
    \internal

    Creates the cursor.
*/

void QCursor::update() const
{
    if ( !initialized )
	initialize();
    register QCursorData *d = data;		// cheat const!
    if ( d->hcurs )				// already loaded
	return;

    Display *dpy = QPaintDevice::x11AppDisplay();
    Window rootwin = QPaintDevice::x11AppRootWindow();

    if ( d->cshape == BitmapCursor ) {
	d->hcurs = XCreatePixmapCursor( dpy, d->bm->handle(), d->bmm->handle(),
					&d->fg, &d->bg, d->hx, d->hy );
	return;
    }

#ifndef QT_NO_XCURSOR
    static const char *cursorNames[] = {
	"left_ptr",
	"up_arrow",
	"cross",
	"wait",
	"ibeam",
	"size_ver",
	"size_hor",
	"size_bdiag",
	"size_fdiag",
	"size_all",
	"blank",
	"split_v",
	"split_h",
	"pointing_hand",
	"forbidden",
	"whats_this",
	"left_ptr_watch"
    };

    d->hcurs = XcursorLibraryLoadCursor( dpy, cursorNames[d->cshape] );
    if ( d->hcurs )
	return;
#endif // QT_NO_XCURSOR

    static uchar cur_blank_bits[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    // Non-standard X11 cursors are created from bitmaps

#ifndef QT_USE_APPROXIMATE_CURSORS
    static const uchar cur_ver_bits[] = {
	0x00, 0x00, 0x00, 0x00, 0x80, 0x01, 0xc0, 0x03, 0xe0, 0x07, 0xf0, 0x0f,
	0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0xf0, 0x0f,
	0xe0, 0x07, 0xc0, 0x03, 0x80, 0x01, 0x00, 0x00 };
    static const uchar mcur_ver_bits[] = {
	0x00, 0x00, 0x80, 0x03, 0xc0, 0x07, 0xe0, 0x0f, 0xf0, 0x1f, 0xf8, 0x3f,
	0xfc, 0x7f, 0xc0, 0x07, 0xc0, 0x07, 0xc0, 0x07, 0xfc, 0x7f, 0xf8, 0x3f,
	0xf0, 0x1f, 0xe0, 0x0f, 0xc0, 0x07, 0x80, 0x03 };
    static const uchar cur_hor_bits[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x08, 0x30, 0x18,
	0x38, 0x38, 0xfc, 0x7f, 0xfc, 0x7f, 0x38, 0x38, 0x30, 0x18, 0x20, 0x08,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    static const uchar mcur_hor_bits[] = {
	0x00, 0x00, 0x00, 0x00, 0x40, 0x04, 0x60, 0x0c, 0x70, 0x1c, 0x78, 0x3c,
	0xfc, 0x7f, 0xfe, 0xff, 0xfe, 0xff, 0xfe, 0xff, 0xfc, 0x7f, 0x78, 0x3c,
	0x70, 0x1c, 0x60, 0x0c, 0x40, 0x04, 0x00, 0x00 };
    static const uchar cur_bdiag_bits[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x00, 0x3e, 0x00, 0x3c, 0x00, 0x3e,
	0x00, 0x37, 0x88, 0x23, 0xd8, 0x01, 0xf8, 0x00, 0x78, 0x00, 0xf8, 0x00,
	0xf8, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    static const uchar mcur_bdiag_bits[] = {
	0x00, 0x00, 0xc0, 0x7f, 0x80, 0x7f, 0x00, 0x7f, 0x00, 0x7e, 0x04, 0x7f,
	0x8c, 0x7f, 0xdc, 0x77, 0xfc, 0x63, 0xfc, 0x41, 0xfc, 0x00, 0xfc, 0x01,
	0xfc, 0x03, 0xfc, 0x07, 0x00, 0x00, 0x00, 0x00 };
    static const uchar cur_fdiag_bits[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x01, 0xf8, 0x00, 0x78, 0x00,
	0xf8, 0x00, 0xd8, 0x01, 0x88, 0x23, 0x00, 0x37, 0x00, 0x3e, 0x00, 0x3c,
	0x00, 0x3e, 0x00, 0x3f, 0x00, 0x00, 0x00, 0x00 };
    static const uchar mcur_fdiag_bits[] = {
	0x00, 0x00, 0x00, 0x00, 0xfc, 0x07, 0xfc, 0x03, 0xfc, 0x01, 0xfc, 0x00,
	0xfc, 0x41, 0xfc, 0x63, 0xdc, 0x77, 0x8c, 0x7f, 0x04, 0x7f, 0x00, 0x7e,
	0x00, 0x7f, 0x80, 0x7f, 0xc0, 0x7f, 0x00, 0x00 };
    static const uchar *cursor_bits16[] = {
	cur_ver_bits, mcur_ver_bits, cur_hor_bits, mcur_hor_bits,
	cur_bdiag_bits, mcur_bdiag_bits, cur_fdiag_bits, mcur_fdiag_bits,
	0, 0, cur_blank_bits, cur_blank_bits };

    static const uchar vsplit_bits[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x80, 0x00, 0x00, 0x00, 0xc0, 0x01, 0x00, 0x00, 0xe0, 0x03, 0x00,
	0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00,
	0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0xff, 0x7f, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x7f, 0x00,
	0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00,
	0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0xe0, 0x03, 0x00,
	0x00, 0xc0, 0x01, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    static const uchar vsplitm_bits[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00,
	0x00, 0xc0, 0x01, 0x00, 0x00, 0xe0, 0x03, 0x00, 0x00, 0xf0, 0x07, 0x00,
	0x00, 0xf8, 0x0f, 0x00, 0x00, 0xc0, 0x01, 0x00, 0x00, 0xc0, 0x01, 0x00,
	0x00, 0xc0, 0x01, 0x00, 0x80, 0xff, 0xff, 0x00, 0x80, 0xff, 0xff, 0x00,
	0x80, 0xff, 0xff, 0x00, 0x80, 0xff, 0xff, 0x00, 0x80, 0xff, 0xff, 0x00,
	0x80, 0xff, 0xff, 0x00, 0x00, 0xc0, 0x01, 0x00, 0x00, 0xc0, 0x01, 0x00,
	0x00, 0xc0, 0x01, 0x00, 0x00, 0xf8, 0x0f, 0x00, 0x00, 0xf0, 0x07, 0x00,
	0x00, 0xe0, 0x03, 0x00, 0x00, 0xc0, 0x01, 0x00, 0x00, 0x80, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    static const uchar hsplit_bits[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x40, 0x02, 0x00,
	0x00, 0x40, 0x02, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x40, 0x02, 0x00,
	0x00, 0x41, 0x82, 0x00, 0x80, 0x41, 0x82, 0x01, 0xc0, 0x7f, 0xfe, 0x03,
	0x80, 0x41, 0x82, 0x01, 0x00, 0x41, 0x82, 0x00, 0x00, 0x40, 0x02, 0x00,
	0x00, 0x40, 0x02, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x40, 0x02, 0x00,
	0x00, 0x40, 0x02, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    static const uchar hsplitm_bits[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xe0, 0x07, 0x00, 0x00, 0xe0, 0x07, 0x00, 0x00, 0xe0, 0x07, 0x00,
	0x00, 0xe0, 0x07, 0x00, 0x00, 0xe2, 0x47, 0x00, 0x00, 0xe3, 0xc7, 0x00,
	0x80, 0xe3, 0xc7, 0x01, 0xc0, 0xff, 0xff, 0x03, 0xe0, 0xff, 0xff, 0x07,
	0xc0, 0xff, 0xff, 0x03, 0x80, 0xe3, 0xc7, 0x01, 0x00, 0xe3, 0xc7, 0x00,
	0x00, 0xe2, 0x47, 0x00, 0x00, 0xe0, 0x07, 0x00, 0x00, 0xe0, 0x07, 0x00,
	0x00, 0xe0, 0x07, 0x00, 0x00, 0xe0, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    static const uchar whatsthis_bits[] = {
        0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x05, 0xf0, 0x07, 0x00,
        0x09, 0x18, 0x0e, 0x00, 0x11, 0x1c, 0x0e, 0x00, 0x21, 0x1c, 0x0e, 0x00,
        0x41, 0x1c, 0x0e, 0x00, 0x81, 0x1c, 0x0e, 0x00, 0x01, 0x01, 0x07, 0x00,
        0x01, 0x82, 0x03, 0x00, 0xc1, 0xc7, 0x01, 0x00, 0x49, 0xc0, 0x01, 0x00,
        0x95, 0xc0, 0x01, 0x00, 0x93, 0xc0, 0x01, 0x00, 0x21, 0x01, 0x00, 0x00,
        0x20, 0xc1, 0x01, 0x00, 0x40, 0xc2, 0x01, 0x00, 0x40, 0x02, 0x00, 0x00,
        0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };
    static const uchar whatsthism_bits[] = {
        0x01, 0x00, 0x00, 0x00, 0x03, 0xf0, 0x07, 0x00, 0x07, 0xf8, 0x0f, 0x00,
        0x0f, 0xfc, 0x1f, 0x00, 0x1f, 0x3e, 0x1f, 0x00, 0x3f, 0x3e, 0x1f, 0x00,
        0x7f, 0x3e, 0x1f, 0x00, 0xff, 0x3e, 0x1f, 0x00, 0xff, 0x9d, 0x0f, 0x00,
        0xff, 0xc3, 0x07, 0x00, 0xff, 0xe7, 0x03, 0x00, 0x7f, 0xe0, 0x03, 0x00,
        0xf7, 0xe0, 0x03, 0x00, 0xf3, 0xe0, 0x03, 0x00, 0xe1, 0xe1, 0x03, 0x00,
        0xe0, 0xe1, 0x03, 0x00, 0xc0, 0xe3, 0x03, 0x00, 0xc0, 0xe3, 0x03, 0x00,
        0x80, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };
    static const uchar busy_bits[] = {
	0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00,
	0x09, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x21, 0x00, 0x00, 0x00,
	0x41, 0xe0, 0xff, 0x00, 0x81, 0x20, 0x80, 0x00, 0x01, 0xe1, 0xff, 0x00,
	0x01, 0x42, 0x40, 0x00, 0xc1, 0x47, 0x40, 0x00, 0x49, 0x40, 0x55, 0x00,
	0x95, 0x80, 0x2a, 0x00, 0x93, 0x00, 0x15, 0x00, 0x21, 0x01, 0x0a, 0x00,
	0x20, 0x01, 0x11, 0x00, 0x40, 0x82, 0x20, 0x00, 0x40, 0x42, 0x44, 0x00,
	0x80, 0x41, 0x4a, 0x00, 0x00, 0x40, 0x55, 0x00, 0x00, 0xe0, 0xff, 0x00,
	0x00, 0x20, 0x80, 0x00, 0x00, 0xe0, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    static const uchar busym_bits[] = {
	0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00,
	0x0f, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x00,
	0x7f, 0xe0, 0xff, 0x00, 0xff, 0xe0, 0xff, 0x00, 0xff, 0xe1, 0xff, 0x00,
	0xff, 0xc3, 0x7f, 0x00, 0xff, 0xc7, 0x7f, 0x00, 0x7f, 0xc0, 0x7f, 0x00,
	0xf7, 0x80, 0x3f, 0x00, 0xf3, 0x00, 0x1f, 0x00, 0xe1, 0x01, 0x0e, 0x00,
	0xe0, 0x01, 0x1f, 0x00, 0xc0, 0x83, 0x3f, 0x00, 0xc0, 0xc3, 0x7f, 0x00,
	0x80, 0xc1, 0x7f, 0x00, 0x00, 0xc0, 0x7f, 0x00, 0x00, 0xe0, 0xff, 0x00,
	0x00, 0xe0, 0xff, 0x00, 0x00, 0xe0, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    static const uchar * const cursor_bits32[] = {
	vsplit_bits, vsplitm_bits, hsplit_bits, hsplitm_bits,
	0, 0, 0, 0, whatsthis_bits, whatsthism_bits, busy_bits, busym_bits
    };

    static const uchar forbidden_bits[] = {
	0x00,0x00,0x00,0x80,0x1f,0x00,0xe0,0x7f,0x00,0xf0,0xf0,0x00,0x38,0xc0,0x01,
	0x7c,0x80,0x03,0xec,0x00,0x03,0xce,0x01,0x07,0x86,0x03,0x06,0x06,0x07,0x06,
	0x06,0x0e,0x06,0x06,0x1c,0x06,0x0e,0x38,0x07,0x0c,0x70,0x03,0x1c,0xe0,0x03,
	0x38,0xc0,0x01,0xf0,0xe0,0x00,0xe0,0x7f,0x00,0x80,0x1f,0x00,0x00,0x00,0x00 };

    static const unsigned char forbiddenm_bits[] = {
	0x80,0x1f,0x00,0xe0,0x7f,0x00,0xf0,0xff,0x00,0xf8,0xff,0x01,0xfc,0xf0,0x03,
	0xfe,0xc0,0x07,0xfe,0x81,0x07,0xff,0x83,0x0f,0xcf,0x07,0x0f,0x8f,0x0f,0x0f,
	0x0f,0x1f,0x0f,0x0f,0x3e,0x0f,0x1f,0xfc,0x0f,0x1e,0xf8,0x07,0x3e,0xf0,0x07,
	0xfc,0xe0,0x03,0xf8,0xff,0x01,0xf0,0xff,0x00,0xe0,0x7f,0x00,0x80,0x1f,0x00};

    static const uchar * const cursor_bits20[] = {
	forbidden_bits, forbiddenm_bits
    };

    if ( d->cshape >= SizeVerCursor && d->cshape < SizeAllCursor ||
	 d->cshape == BlankCursor ) {
	XColor bg, fg;
	bg.red   = 255 << 8;
	bg.green = 255 << 8;
	bg.blue  = 255 << 8;
	fg.red   = 0;
	fg.green = 0;
	fg.blue  = 0;
	int i = (d->cshape - SizeVerCursor)*2;
	d->pm  = XCreateBitmapFromData( dpy, rootwin, (char *)cursor_bits16[i],
					16, 16 );
	d->pmm = XCreateBitmapFromData( dpy, rootwin, (char *)cursor_bits16[i+1],
					16,16);
	d->hcurs = XCreatePixmapCursor( dpy, d->pm, d->pmm, &fg, &bg, 8, 8 );
	return;
    }
    if ( ( d->cshape >= SplitVCursor && d->cshape <= SplitHCursor ) ||
         d->cshape == WhatsThisCursor || d->cshape == BusyCursor ) {
	XColor bg, fg;
	bg.red   = 255 << 8;
	bg.green = 255 << 8;
	bg.blue  = 255 << 8;
	fg.red   = 0;
	fg.green = 0;
	fg.blue  = 0;
	int i = (d->cshape - SplitVCursor)*2;
	d->pm  = XCreateBitmapFromData( dpy, rootwin, (char *)cursor_bits32[i],
					32, 32 );
	d->pmm = XCreateBitmapFromData( dpy, rootwin, (char *)cursor_bits32[i+1],
					32, 32);
	int hs = ( d->cshape == PointingHandCursor ||
		   d->cshape == WhatsThisCursor ||
		   d->cshape == BusyCursor ) ? 0 : 16;
	d->hcurs = XCreatePixmapCursor( dpy, d->pm, d->pmm, &fg, &bg, hs, hs );
	return;
    }
    if ( d->cshape == ForbiddenCursor ) {
	XColor bg, fg;
	bg.red   = 255 << 8;
	bg.green = 255 << 8;
	bg.blue  = 255 << 8;
	fg.red   = 0;
	fg.green = 0;
	fg.blue  = 0;
	int i = (d->cshape - ForbiddenCursor)*2;
	d->pm  = XCreateBitmapFromData( dpy, rootwin, (char *)cursor_bits20[i],
					20, 20 );
	d->pmm = XCreateBitmapFromData( dpy, rootwin, (char *)cursor_bits20[i+1],
					20, 20);
	d->hcurs = XCreatePixmapCursor( dpy, d->pm, d->pmm, &fg, &bg, 10, 10 );
	return;
    }
#endif /* ! QT_USE_APPROXIMATE_CURSORS */

    uint sh;
    switch ( d->cshape ) {			// map Q cursor to X cursor
    case ArrowCursor:
	sh = XC_left_ptr;
	break;
    case UpArrowCursor:
	sh = XC_center_ptr;
	break;
    case CrossCursor:
	sh = XC_crosshair;
	break;
    case WaitCursor:
	sh = XC_watch;
	break;
    case IbeamCursor:
	sh = XC_xterm;
	break;
    case SizeAllCursor:
	sh = XC_fleur;
	break;
    case PointingHandCursor:
	sh = XC_hand2;
	break;
#ifdef QT_USE_APPROXIMATE_CURSORS
    case SizeBDiagCursor:
	sh = XC_top_right_corner;
	break;
    case SizeFDiagCursor:
	sh = XC_bottom_right_corner;
	break;
    case BlankCursor:
	XColor bg, fg;
	bg.red   = 255 << 8;
	bg.green = 255 << 8;
	bg.blue  = 255 << 8;
	fg.red   = 0;
	fg.green = 0;
	fg.blue  = 0;
	d->pm  = XCreateBitmapFromData( dpy, rootwin,
					(char *)cur_blank_bits, 16, 16 );
	d->pmm = XCreateBitmapFromData( dpy, rootwin,
					(char *)cur_blank_bits, 16,16);
	d->hcurs = XCreatePixmapCursor( dpy, d->pm, d->pmm, &fg,
					&bg, 8, 8 );
	return;
	break;
    case SizeVerCursor:
    case SplitVCursor:
	sh = XC_sb_v_double_arrow;
	break;
    case SizeHorCursor:
    case SplitHCursor:
	sh = XC_sb_h_double_arrow;
	break;
    case WhatsThisCursor:
	sh = XC_question_arrow;
	break;
    case ForbiddenCursor:
	sh = XC_circle;
	break;
    case BusyCursor:
	sh = XC_watch;
	break;
#endif /* QT_USE_APPROXIMATE_CURSORS */
    default:
#if defined(QT_CHECK_RANGE)
	qWarning( "QCursor::update: Invalid cursor shape %d", d->cshape );
#endif
	return;
    }
    d->hcurs = XCreateFontCursor( dpy, sh );
}
