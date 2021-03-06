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

#ifndef QRECT_H
#define QRECT_H

#ifndef QT_H
#include "qsize.h"
#endif // QT_H

#if defined(topLeft)
#error "Macro definition of topLeft conflicts with QRect"
// don't just silently undo people's defines: #undef topLeft
#endif

class Q_EXPORT QRect					// rectangle class
{
public:
    QRect()	{ x1 = y1 = 0; x2 = y2 = -1; }
    QRect( const QPoint &topleft, const QPoint &bottomright );
    QRect( const QPoint &topleft, const QSize &size );
    QRect( int left, int top, int width, int height );

    bool   isNull()	const;
    bool   isEmpty()	const;
    bool   isValid()	const;
    QRect  normalize()	const;

    int	   left()	const;
    int	   top()	const;
    int	   right()	const;
    int	   bottom()	const;

    QCOORD &rLeft();
    QCOORD &rTop();
    QCOORD &rRight();
    QCOORD &rBottom();

    int	   x()		const;
    int	   y()		const;
    void   setLeft( int pos );
    void   setTop( int pos );
    void   setRight( int pos );
    void   setBottom( int pos );
    void   setX( int x );
    void   setY( int y );

    void   setTopLeft( const QPoint &p );
    void   setBottomRight( const QPoint &p );
    void   setTopRight( const QPoint &p );
    void   setBottomLeft( const QPoint &p );

    QPoint topLeft()	 const;
    QPoint bottomRight() const;
    QPoint topRight()	 const;
    QPoint bottomLeft()	 const;
    QPoint center()	 const;

    void   rect( int *x, int *y, int *w, int *h ) const;
    void   coords( int *x1, int *y1, int *x2, int *y2 ) const;

    void   moveLeft( int pos );
    void   moveTop( int pos );
    void   moveRight( int pos );
    void   moveBottom( int pos );
    void   moveTopLeft( const QPoint &p );
    void   moveBottomRight( const QPoint &p );
    void   moveTopRight( const QPoint &p );
    void   moveBottomLeft( const QPoint &p );
    void   moveCenter( const QPoint &p );
    void   moveBy( int dx, int dy );

    void   setRect( int x, int y, int w, int h );
    void   setCoords( int x1, int y1, int x2, int y2 );
    void   addCoords( int x1, int y1, int x2, int y2 );

    QSize  size()	const;
    int	   width()	const;
    int	   height()	const;
    void   setWidth( int w );
    void   setHeight( int h );
    void   setSize( const QSize &s );

    QRect  operator|(const QRect &r) const;
    QRect  operator&(const QRect &r) const;
    QRect&  operator|=(const QRect &r);
    QRect&  operator&=(const QRect &r);

    bool   contains( const QPoint &p, bool proper=FALSE ) const;
    bool   contains( int x, int y ) const; // inline methods, _don't_ merge these
    bool   contains( int x, int y, bool proper ) const;
    bool   contains( const QRect &r, bool proper=FALSE ) const;
    QRect  unite( const QRect &r ) const;
    QRect  intersect( const QRect &r ) const;
    bool   intersects( const QRect &r ) const;

    friend Q_EXPORT bool operator==( const QRect &, const QRect & );
    friend Q_EXPORT bool operator!=( const QRect &, const QRect & );

private:
#if defined(Q_WS_X11) || defined(Q_OS_TEMP)
    friend void qt_setCoords( QRect *r, int xp1, int yp1, int xp2, int yp2 );
#endif
#if defined(Q_OS_MAC)
    QCOORD y1;
    QCOORD x1;
    QCOORD y2;
    QCOORD x2;
#else
    QCOORD x1;
    QCOORD y1;
    QCOORD x2;
    QCOORD y2;
#endif
};

Q_EXPORT bool operator==( const QRect &, const QRect & );
Q_EXPORT bool operator!=( const QRect &, const QRect & );


/*****************************************************************************
  QRect stream functions
 *****************************************************************************/
#ifndef QT_NO_DATASTREAM
Q_EXPORT QDataStream &operator<<( QDataStream &, const QRect & );
Q_EXPORT QDataStream &operator>>( QDataStream &, QRect & );
#endif

/*****************************************************************************
  QRect inline member functions
 *****************************************************************************/

inline QRect::QRect( int left, int top, int width, int height )
{
    x1 = (QCOORD)left;
    y1 = (QCOORD)top;
    x2 = (QCOORD)(left+width-1);
    y2 = (QCOORD)(top+height-1);
}

inline bool QRect::isNull() const
{ return x2 == x1-1 && y2 == y1-1; }

inline bool QRect::isEmpty() const
{ return x1 > x2 || y1 > y2; }

inline bool QRect::isValid() const
{ return x1 <= x2 && y1 <= y2; }

inline int QRect::left() const
{ return x1; }

inline int QRect::top() const
{ return y1; }

inline int QRect::right() const
{ return x2; }

inline int QRect::bottom() const
{ return y2; }

inline QCOORD &QRect::rLeft()
{ return x1; }

inline QCOORD & QRect::rTop()
{ return y1; }

inline QCOORD & QRect::rRight()
{ return x2; }

inline QCOORD & QRect::rBottom()
{ return y2; }

inline int QRect::x() const
{ return x1; }

inline int QRect::y() const
{ return y1; }

inline void QRect::setLeft( int pos )
{ x1 = (QCOORD)pos; }

inline void QRect::setTop( int pos )
{ y1 = (QCOORD)pos; }

inline void QRect::setRight( int pos )
{ x2 = (QCOORD)pos; }

inline void QRect::setBottom( int pos )
{ y2 = (QCOORD)pos; }

inline void QRect::setX( int x )
{ x1 = (QCOORD)x; }

inline void QRect::setY( int y )
{ y1 = (QCOORD)y; }

inline QPoint QRect::topLeft() const
{ return QPoint(x1, y1); }

inline QPoint QRect::bottomRight() const
{ return QPoint(x2, y2); }

inline QPoint QRect::topRight() const
{ return QPoint(x2, y1); }

inline QPoint QRect::bottomLeft() const
{ return QPoint(x1, y2); }

inline QPoint QRect::center() const
{ return QPoint((x1+x2)/2, (y1+y2)/2); }

inline int QRect::width() const
{ return  x2 - x1 + 1; }

inline int QRect::height() const
{ return  y2 - y1 + 1; }

inline QSize QRect::size() const
{ return QSize(x2-x1+1, y2-y1+1); }

inline bool QRect::contains( int x, int y, bool proper ) const
{
    if ( proper )
        return x > x1 && x < x2 &&
               y > y1 && y < y2;
    else
        return x >= x1 && x <= x2 &&
               y >= y1 && y <= y2;
}

inline bool QRect::contains( int x, int y ) const
{
    return x >= x1 && x <= x2 &&
	   y >= y1 && y <= y2;
}
#define Q_DEFINED_QRECT
#include "qwinexport.h"
#endif // QRECT_H
