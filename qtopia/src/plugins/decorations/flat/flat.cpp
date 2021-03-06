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

#include "flat.h"
#include <qtopia/resource.h>
#include <qpainter.h>
#include <qapplication.h>


static QImage scaleButton( const QImage &img, int height )
{
    if ( img.height() != height ) {
	return img.smoothScale( img.width()*height/img.height(), height );
    } else {
	return img;
    }
}

static void colorize( QImage &img, const QColor &c, bool rev )
{
    for ( int i = 0; i < img.numColors(); i++ ) {
	bool sc = (img.color(i) & 0xff000000);
	if ( rev ) sc = !sc;
	img.setColor(i, sc ? c.rgb() : 0x00000000 );
    }
}

FlatDecoration::FlatDecoration() : ref(0), buttonCache(16)
{
    buttonCache.setAutoDelete(TRUE);
}

FlatDecoration::~FlatDecoration()
{
}

int FlatDecoration::metric( Metric m, const WindowData *wd ) const
{
    switch ( m ) {
	case TopBorder:
	    return 1;
	    break;
	case LeftBorder:
	case RightBorder:
	    return 2;
	case BottomBorder:
	    return 4;
	case TitleHeight:
	    if ( QApplication::desktop()->height() > 320 )
		return 20;
	    else
		return 18;
	case OKWidth:
	case CloseWidth:
	case HelpWidth:
	case MaximizeWidth:
	    return metric(TitleHeight,wd);
	    break;
	default:
	    return WindowDecorationInterface::metric( m, wd );
	    break;
    }

    return 0;
}

void FlatDecoration::drawArea( Area a, QPainter *p, const WindowData *wd ) const
{
    int th = metric( TitleHeight, wd );
    QRect r = wd->rect;

    switch ( a ) {
	case Border:
	    {
		const QColorGroup &cg = wd->palette.active();
		if ( wd->flags & WindowData::Active ) {
		    p->setBrush( cg.color(QColorGroup::Highlight) );
		} else {
		    p->setBrush( cg.color(QColorGroup::Background) );
		}
		p->setPen( cg.foreground() );
		int lb = metric(LeftBorder,wd);
		int rb = metric(RightBorder,wd);
		int tb = metric(TopBorder,wd);
		int bb = metric(BottomBorder,wd);
		p->drawRect( r.x()-lb, r.y()-tb-th, r.width()+lb+rb,
			     r.height()+th+tb+bb );
	    }
	    break;
	case Title:
	    if ( r.height() < 2 ) {
		WindowDecorationInterface::drawArea( a, p, wd );
	    } else {
		const QColorGroup &cg = wd->palette.active();
		QColor c;
		if ( wd->flags & WindowData::Active )
		    c = cg.color(QColorGroup::Highlight);
		else
		    c = cg.color(QColorGroup::Background);
		p->fillRect( QRect(r.x(),r.y()-th,r.width(),th), c );
	    }
	    break;
	case TitleText:
	    p->drawText( r.left()+3+metric(HelpWidth,wd), r.top()-th,
			 r.width()-metric(HelpWidth,wd)-metric(CloseWidth,wd), th,
			 Qt::AlignVCenter, wd->caption );
	    break;
	default:
	    FlatDecoration::drawArea( a, p, wd );
	    break;
    }
}

void FlatDecoration::drawButton( Button b, QPainter *p, const WindowData *wd,
    int x, int y, int /* w */, int /* h */, QWSButton::State state ) const
{
    QColor c;
    const QColorGroup &cg = wd->palette.active();
    if ( wd->flags & WindowDecorationInterface::WindowData::Active )
	c = cg.color(QColorGroup::HighlightedText);
    else
	c = cg.color(QColorGroup::Foreground);
    bool r = (state & QWSButton::MouseOver) && (state & QWSButton::Clicked);
    int th = metric(TitleHeight, wd);

    QString key( "%1-%2-%3-%4" );
    key = key.arg(b).arg(th).arg(c.name()).arg(r ? "1" : "0");
    QImage *img = buttonCache.find( key );
    if ( !img ) {
	QImage tmp;
	switch ( b ) {
	    case OK:
		tmp = Resource::loadImage("decorations/flat/ok");
		break;
	    case Close:
		tmp = Resource::loadImage("decorations/flat/close");
		break;
	    case Help:
		tmp = Resource::loadImage("decorations/flat/help");
		break;
	    case Maximize:
		tmp = Resource::loadImage("decorations/flat/maximize");
		break;
	}
	colorize( tmp, c, r );
	img = new QImage( scaleButton(tmp,th) );
	FlatDecoration *that = (FlatDecoration *)this;
	that->buttonCache.insert( key, img, 1 );
    }
    
    p->drawImage( x, y, *img );
}

QRegion FlatDecoration::mask( const WindowData *wd ) const
{
    return WindowDecorationInterface::mask( wd );
}

QString FlatDecoration::name() const
{
    return qApp->translate( "Decoration", "Flat" );
}

QPixmap FlatDecoration::icon() const
{
    return QPixmap();
}

QRESULT FlatDecoration::queryInterface( const QUuid &uuid, QUnknownInterface **iface )
{
    *iface = 0;
    if ( uuid == IID_QUnknown )
	*iface = this;
    else if ( uuid == IID_WindowDecoration )
	*iface = this;
    else
	return QS_FALSE;

    (*iface)->addRef();
    return QS_OK;
}

Q_EXPORT_INTERFACE()
{
    Q_CREATE_INSTANCE( FlatDecoration )
}

