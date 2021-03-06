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

#include "codes.h"  
#include "man.h"
#include "base.h"

#include <qtopia/resource.h>

#include <qregexp.h>

int mancount;

Man::Man(QCanvas* canvas) :
    QCanvasSprite(0, canvas),
    splat("lose") // No tr
{
    manarray = new QCanvasPixmapArray();
    QString m0 = Resource::findPixmap("parashoot/man0001");
    m0.replace(QRegExp("0001"),"%1");
    manarray->readPixmaps(m0, 7);  
    setSequence(manarray);
    setAnimated(true);
    mancount++;
    dead = false;
    start();
}

Man::Man(QCanvas* canvas, int x, int y) :
    QCanvasSprite(0, canvas),
    splat("bang") // No tr
{
    manarray = new QCanvasPixmapArray();
    QString m0 = Resource::findPixmap("parashoot/man0001");
    m0.replace(QString("0001"),"%1");
    manarray->readPixmaps(m0, 7);
    setSequence(manarray);
    move(x, y);
    setFrame(5);
    setZ(300);
    show();

    static bool first_time = TRUE;
    if (first_time) {
        first_time = FALSE;
        QTime midnight(0, 0, 0);
        srand(midnight.secsTo(QTime::currentTime()) );
    } 
    int yfallspeed = 0;
    yfallspeed = (rand() % 3) + 1;
    setVelocity(0, yfallspeed);

    mancount++;
    dead = false;
}
int f = 0;

void Man::advance(int phase)
{
    QCanvasSprite::advance(phase);
    if (phase == 0) {
	checkCollision();
	if (dead) {
	    if (count < 10) {
		setFrame(6);
		setVelocity(0,0);
		count++;
	    } else {
		delete this;
		return;
	    }
	}
	if (y() > canvas()->height()-43) {
	    setFrame(f%5);
	    f++;
	    move(x(), canvas()->height()-26);
	    setVelocity(-2.0, 0);
	} else if (xVelocity() == -2.0) {
	    //
	    // There's been a resize event while this Man has
	    // been on the ground.  Move the man back to the
	    // new ground location.  This is not neat.
	    //
	    move(x(), canvas()->height()-26);
	}
    }
} 

void Man::setInitialCoords()
{
    static bool first_time = TRUE;
    if (first_time) {
        first_time = FALSE;
        QTime midnight(0, 0, 0);
        srand(midnight.secsTo(QTime::currentTime()) );
    }
    dx = rand() % (canvas()->width()-16);
    dy = -43;  //height of a man off the screen
}

//check if man has reached the base
void Man::checkCollision()
{ 
    if ( (x() < 23) && (y() == canvas()->height()-26)) {
       QCanvasItem* item;
       QCanvasItemList l=collisions(FALSE);
          for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it) {
             item = *it;
             if ( (item->rtti()== 1800) && (item->collidesWith(this)) ) {
                 Base* base = (Base*) item;
                 base->damageBase();
                 start();
             }
          }
    }

    //
    // resize events may cause Man objects to appear
    // outside the screen.  Get rid of them if this
    // is the case.
    //
    if ((x() < 0) || (x() > canvas()->width())) {
	delete this;
	return;
    }
}

void Man::start()
{
   setInitialCoords();
   move(dx, dy);
   setFrame(5);
   setZ(300);
   show();

   static bool first_time = TRUE;
   if (first_time) {
      first_time = FALSE;
      QTime midnight(0, 0, 0);
      srand(midnight.secsTo(QTime::currentTime()) );
   }
   int yfallspeed = 0;
   yfallspeed = (rand() % 3) + 1;
   setVelocity(0, yfallspeed);
}

void Man::done()
{
   splat.play();
   count = 0;
   dead = true;
   setFrame(6);
}

int Man::getManCount()
{
   return mancount;
}

void Man::setManCount(int count)
{
    mancount = count;
}


int Man::rtti() const
{
   return man_rtti;
}

Man::~Man()
{
   mancount--;
}
