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
#ifndef MINEFIELD_H
#define MINEFIELD_H

#include <qscrollview.h>

class Mine;
class Config;

class MineField : public QScrollView
{
    Q_OBJECT
public:
    MineField( QWidget* parent = 0, const char* name = 0 );
    ~MineField();

    enum State { Waiting, Playing, GameOver };

    State state() const { return stat; }

    void readConfig(Config&);
    void writeConfig(Config&) const;

    int level() const { return lev; }

    void setAvailableRect( const QRect & );

    QSize sizeHint() const;

public slots:
    void setup( int level );

    void showMines();

signals:
    void gameOver( bool won );
    void gameStarted();
    void mineCount( int );
    void newGameSelected();
    
protected:

    void contentsMousePressEvent( QMouseEvent* );
    void contentsMouseReleaseEvent( QMouseEvent* );
    void keyPressEvent( QKeyEvent* );
    void keyReleaseEvent( QKeyEvent* );
    void drawContents( QPainter * p, int clipx, int clipy, int clipw, int cliph );
    
    int getHint( int row, int col );
    void setHint( int r, int c );
    void updateMine( int row, int col );
    void paletteChange( const QPalette & );
    void updateCell( int r, int c );
    bool onBoard( int r, int c ) const { return r >= 0 && r < numRows && c >= 0 && c < numCols; }
    Mine *mine( int row, int col ) { return onBoard(row, col ) ? mines[row+numCols*col] : 0; }
    const Mine *mine( int row, int col ) const { return onBoard(row, col ) ? mines[row+numCols*col] : 0; }
    
protected slots:
    void cellPressed( int row, int col );
    void cellClicked( int row, int col );
    void held();

private:

    int findCellSize();
    void setCellSize( int );
    
#ifdef QTOPIA_PHONE
    bool mAlreadyHeld; // true if we've already taken action for holding down on the current key press
#endif
    State stat;
    void MineField::setState( State st );
    void MineField::placeMines();
    enum FlagAction { NoAction, FlagOn, FlagNext };
    FlagAction flagAction;
    bool ignoreClick;
    int currRow;
    int currCol;
    int numRows, numCols;
    bool pressed;

    int minecount;
    int mineguess;
    int nonminecount;
    int lev;
    QRect availableRect;
    int cellSize;
    QTimer *holdTimer;
    Mine **mines;
};

#endif // MINEFIELD_H
