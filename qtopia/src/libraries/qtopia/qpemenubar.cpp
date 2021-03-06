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

#define INCLUDE_MENUITEM_DEF

#include "qpemenubar.h"
#include <qapplication.h>
#include <qguardedptr.h>
#include <qtimer.h>


class QMenuBarHack : public QMenuBar
{
public:
    int activeItem() const { return actItem; }

    void goodbye()
    {
	activateItemAt(-1);
	for ( unsigned int i = 0; i < count(); i++ ) {
	    QMenuItem *mi = findItem( idAt(i) );
	    if ( mi->popup() ) {
		mi->popup()->hide();
	    }
	}
    }
};


QPEMenuToolFocusManager *QPEMenuToolFocusManager::me = 0;

QPEMenuToolFocusManager::QPEMenuToolFocusManager() : QObject()
{
    qApp->installEventFilter( this );
}

void QPEMenuToolFocusManager::addWidget( QWidget *w )
{
    list.append( GuardedWidget(w) );
}

void QPEMenuToolFocusManager::removeWidget( QWidget *w )
{
    list.remove( GuardedWidget(w) );
}

void QPEMenuToolFocusManager::setActive( bool a )
{
    if ( a ) {
	oldFocus = qApp->focusWidget();
	QValueList<GuardedWidget>::Iterator it;
	it = list.begin();
	while ( it != list.end() ) {
	    QWidget *w = (*it);
	    if ( w && w->isEnabled() && w->isVisible() &&
		 w->topLevelWidget() == qApp->activeWindow() ) {
		setFocus( w );
		return;
	    }
	    ++it;
	}
    } else {
	if ( inFocus ) {
	    if ( inFocus->inherits( "QMenuBar" ) )
		((QMenuBarHack *)(QWidget *)inFocus)->goodbye();
	    if ( inFocus->hasFocus() ) {
		if ( oldFocus && oldFocus->isVisible() && oldFocus->isEnabled() ) {
		    oldFocus->setFocus();
		} else {
		    inFocus->clearFocus();
		}
	    }
	}
	inFocus = 0;
	oldFocus = 0;
    }
}

bool QPEMenuToolFocusManager::isActive() const
{
    return !inFocus.isNull();
}

void QPEMenuToolFocusManager::moveFocus( bool next )
{
    if ( !isActive() )
	return;

    int n = list.count();
    QValueList<GuardedWidget>::Iterator it;
    it = list.find( inFocus );
    if ( it == list.end() )
	it = list.begin();
    while ( --n ) {
	if ( next ) {
	    ++it;
	    if ( it == list.end() )
		it = list.begin();
	} else {
	    if ( it == list.begin() )
		it = list.end();
	    --it;
	}
	QWidget *w = (*it);
	if ( w && w->isEnabled() && w->isVisible() &&
	     w->topLevelWidget() == qApp->activeWindow() &&
	     !w->isA("QToolBarSeparator") ) {
	    setFocus( w, next );
	    return;
	}
    }
}

void QPEMenuToolFocusManager::initialize()
{
    if ( !me )
	me = new QPEMenuToolFocusManager;
}

QPEMenuToolFocusManager *QPEMenuToolFocusManager::manager()
{
    if ( !me )
	me = new QPEMenuToolFocusManager;

    return me;
}

void QPEMenuToolFocusManager::setFocus( QWidget *w, bool next )
{
    inFocus = w;
//    qDebug( "Set focus on %s", w->className() );
    if ( inFocus->inherits( "QMenuBar" ) ) {
	QMenuBar *mb = (QMenuBar *)(QWidget *)inFocus;
	if ( next )
	    mb->activateItemAt( 0 );
	else
	    mb->activateItemAt( mb->count()-1 );
    }
    inFocus->setFocus();
}

bool QPEMenuToolFocusManager::eventFilter( QObject *object, QEvent *event )
{
    if ( event->type() == QEvent::KeyPress ) {
	QKeyEvent *ke = (QKeyEvent *)event;
	if ( isActive() ) {
	    if ( object->inherits( "QButton" ) ) {
		switch ( ke->key() ) {
		    case Key_Left:
			moveFocus( FALSE );
			return TRUE;

		    case Key_Right:
			moveFocus( TRUE );
			return TRUE;

		    case Key_Up:
		    case Key_Down:
			return TRUE;
		}
	    } else if ( object->inherits( "QPopupMenu" ) ) {
		// Deactivate when a menu item is selected
		if ( ke->key() == Key_Enter || ke->key() == Key_Return ||
		     ke->key() == Key_Escape ) {
		    QTimer::singleShot( 0, this, SLOT(deactivate()) );
		}
	    } else if ( object->inherits( "QMenuBar" ) ) {
		int dx = 0;
		switch ( ke->key() ) {
		    case Key_Left:
			dx = -1;
			break;

		    case Key_Right:
			dx = 1;
			break;
		}

		QMenuBarHack *mb = (QMenuBarHack *)object;
		if ( dx && mb->activeItem() >= 0 ) {
		    int i = mb->activeItem();
		    int c = mb->count();
		    int n = c;
		    while ( n-- ) {
			i = i + dx;
			if ( i == c ) {
			    mb->goodbye();
			    moveFocus( TRUE );
			    return TRUE;
			} else if ( i < 0 ) {
			    mb->goodbye();
			    moveFocus( FALSE );
			    return TRUE;
			}
			QMenuItem *mi = mb->findItem( mb->idAt(i) );
			if ( mi->isEnabled() && !mi->isSeparator() ) {
			    break;
			}
		    }
		}
	    }
	}
	if ( ke->key() == Key_F11 ) {
	    setActive( !isActive() );
	    return TRUE;
	}
    } else if ( event->type() == QEvent::ChildInserted ) {
	QChildEvent *ce = (QChildEvent *)event;
	if ( ce->child()->isWidgetType() ) {
	    if ( ce->child()->inherits( "QMenuBar" ) ) {
		addWidget( (QWidget *)ce->child() );
		ce->child()->installEventFilter( this );
	    } else if ( object->inherits( "QToolBar" ) ) {
		addWidget( (QWidget *)ce->child() );
	    }
	}
    } else if ( event->type() == QEvent::ChildRemoved ) {
	QChildEvent *ce = (QChildEvent *)event;
	if ( ce->child()->isWidgetType() ) {
	    if ( ce->child()->inherits( "QMenuBar" ) ) {
		removeWidget( (QWidget *)ce->child() );
		ce->child()->removeEventFilter( this );
	    } else if ( object->inherits( "QToolBar" ) ) {
		removeWidget( (QWidget *)ce->child() );
	    }
	}
    } else if ( isActive() ) {
	if ( event->type() == QEvent::KeyRelease ) {
	    QKeyEvent *ke = (QKeyEvent *)event;
	    if ( object->inherits( "QButton" ) ) {
		// Deactivate when a button is selected
		if ( ke->key() == Key_Space )
		    QTimer::singleShot( 0, this, SLOT(deactivate()) );
	    }
	} else if ( event->type() == QEvent::FocusIn ) {
	    // A non-menu/tool widget has been selected - we're deactivated
	    QWidget *w = (QWidget *)object;
	    if ( !w->isPopup() && !list.contains( GuardedWidget( w ) ) ) {
		inFocus = 0;
	    }
	} else if ( event->type() == QEvent::Hide ) {
	    // Deaticvate if a menu/tool has been hidden
	    QWidget *w = (QWidget *)object;
	    if ( !w->isPopup() && !list.contains( GuardedWidget( w ) ) ) {
		setActive( FALSE );
	    }
	}
    }

    return FALSE;
}

void QPEMenuToolFocusManager::deactivate()
{
    setActive( FALSE );
}

/*!
  \class QPEMenuBar qpemenubar.h
  \brief The QPEMenuBar class is obsolete.  Use QMenuBar instead.

  \obsolete

  This class is obsolete.  Use QMenuBar instead.

*/

/*!
  Constructs a QPEMenuBar just as you would construct
  a QMenuBar, passing \a parent and \a name.
*/
QPEMenuBar::QPEMenuBar( QWidget *parent, const char *name )
    : QMenuBar( parent, name )
{
}

/*!
  \reimp
*/
QPEMenuBar::~QPEMenuBar()
{
}

/*!
  \internal
*/
void QPEMenuBar::keyPressEvent( QKeyEvent *e )
{
    QMenuBar::keyPressEvent( e );
}

