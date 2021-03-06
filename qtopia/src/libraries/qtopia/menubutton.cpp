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

#include "menubutton.h"
#include <qpopupmenu.h>
#include <qapplication.h>

/*!
  \class MenuButton menubutton.h
  \brief The MenuButton class is a pushbutton with a menu.

  When the user presses the menubutton's pushbutton, the menu pops up.
  A menu is composed of menu items each of which has a string label,
  and optionally an icon.

  The index of the item that the user's input device (e.g. stylus) is
  pointing at is the currentItem(), whose text is available using
  currentText().

  Menu items are inserted with the \link MenuButton::MenuButton()
  constructor\endlink, insertItem() or insertItems(). Separators are
  inserted with insertSeparator(). All the items in the menu can be
  removed by calling clear().

  Items can be selected programmatically using select(). When a menu
  item is selected (programmatically or by the user), the selected()
  signal is emitted.

  \ingroup qtopiaemb
*/

/*!
  \overload void MenuButton::selected(int index)

  This signal is emitted when the item at position \a index is selected.
*/

/*!
  \fn void MenuButton::selected(const QString& text)

  This signal is emitted when the item with the label \a text is selected.
*/


/*!
  Constructs a MenuButton. A menu item is created (see insertItem()
  and insertItems()) for each string in the \a items string list. The
  standard \a parent an \a name arguments are passed to the base
  class.
*/
MenuButton::MenuButton( const QStringList& items, QWidget* parent, const char* name) :
    QPushButton(parent,name)
{
    init();
    insertItems(items);
}

/*!
  Constructs an empty MenuButton.
  The standard \a parent an \a name arguments are passed to the base class.

  \sa insertItem() insertItems()
*/
MenuButton::MenuButton( QWidget* parent, const char* name) :
    QPushButton(parent,name)
{
    init();
}

void MenuButton::init()
{
    setAutoDefault(FALSE);
    pop = new QPopupMenu(this);
    nitems=0;
    connect(pop, SIGNAL(activated(int)), this, SLOT(select(int)));
    setPopup(pop);
    //setPopupDelay(0);
}

/*!
  Removes all the menu items from the button and menu.
*/
void MenuButton::clear()
{
    delete pop;
    init();
}

/*!
  A menu item is created (see insertItem()) for each string in the \a
  items string list. If any string is "--" a separator (see
  insertSeparator()) is inserted in its place.
*/
void MenuButton::insertItems( const QStringList& items )
{
    QStringList::ConstIterator it=items.begin();
    for (; it!=items.end(); ++it) {
	if ( (*it) == "--" )
	    insertSeparator();
	else
	    insertItem(*it);
    }
}

/*!
  Inserts a menu item with the icon \a icon and label \a text into
  the menu.

  \sa insertItems()
*/
void MenuButton::insertItem( const QIconSet& icon, const QString& text)
{
    pop->insertItem(icon, text, nitems++);
    if ( nitems==1 ) select(0);
}

/*!
  \overload
  Inserts a menu item with the label \a text into the menu.

  \sa insertItems()
*/
void MenuButton::insertItem( const QString& text )
{
    pop->insertItem(text, nitems++);
    if ( nitems==1 ) select(0);
}

/*!
  Inserts a separator into the menu.

  \sa insertItems()
*/
void MenuButton::insertSeparator()
{
    pop->insertSeparator();
}

/*!
  Selects the items with label text \a s.
*/
void MenuButton::select(const QString& s)
{
    for (int i=0; i<nitems; i++) {
	if ( pop->text(i) == s ) {
	    select(i);
	    break;
	}
    }
}

/*!
  \overload
  Selects the item at index position \a s.
*/
void MenuButton::select(int s)
{
    cur = s;
    updateLabel();
    if ( pop->iconSet(cur) )
	setIconSet(*pop->iconSet(cur));
    emit selected(cur);
    emit selected(currentText());
}

/*!
  Returns the index position of the current item.
*/
int MenuButton::currentItem() const
{
    return cur;
}

/*!
  Returns the label text of the current item.
*/
QString MenuButton::currentText() const
{
    return pop->text(cur);
}

/*!
  Sets the menubutton's label. If \a label is empty, the
  current item text is displayed, otherwise \a label should contain
  "%1", which will be replaced by the current item text.
*/
void MenuButton::setLabel(const QString& label)
{
    lab = label;
    updateLabel();
}

void MenuButton::updateLabel()
{
    QString t = pop->text(cur);
    if ( !lab.isEmpty() )
	t = lab.arg(t);
    setText(t);
}

/*! \internal */

void MenuButton::keyPressEvent( QKeyEvent *e )
{
    if (e->key() == Key_Left) {
	return;
    }
    if (e->key() == Key_Escape) {
	e=new QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, 0, 0);
	QApplication::sendEvent(this,e);
	QApplication::sendEvent(this,e);
	return;
    }
    if (e->key() == Key_Up || e->key() == Key_Down) {
	e=new QKeyEvent(QEvent::KeyPress, Qt::Key_Space, 0, 0);
	QApplication::sendEvent(this,e);
	return;
    }
    QPushButton::keyPressEvent( e );
}

