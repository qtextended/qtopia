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

#ifndef QT_H
#endif /* QT_H */

#ifndef QT_NO_CHECK
# define QT_NO_CHECK
#endif
#define QT_NO_PROCESS
#define QT_NO_PALETTE
#define QT_NO_ACTION
#ifndef QT_NO_TEXTCODEC /* moc? */
#define QT_NO_TEXTCODEC
#endif
#define QT_NO_UNICODETABLES
#define QT_NO_IMAGEIO_BMP
#define QT_NO_IMAGEIO_PPM
#define QT_NO_IMAGEIO_XBM
#define QT_NO_IMAGEIO_XPM
/* #define QT_NO_IMAGEIO_PNG //done by configure -no-png */
#define QT_NO_ASYNC_IO
#define QT_NO_ASYNC_IMAGE_IO
/* //#define QT_NO_FREETYPE //done by configure -no-freetype */
#define QT_NO_BDF
//#define QT_NO_FONTDATABASE
#define QT_NO_TRANSLATION
#define QT_NO_MIME
#define QT_NO_SOUND
/* #define QT_NO_PROPERTIES */
#define QT_NO_QWS_CURSOR
#define QT_NO_CURSOR
#define QT_NO_QWS_GFX_SPEED
#define QT_NO_NETWORK
#define QT_NO_COLORNAMES
#define QT_NO_TRANSFORMATIONS
#define QT_NO_PRINTER
#define QT_NO_PICTURE
#define QT_NO_LAYOUT
#define QT_NO_DRAWUTIL
#define QT_NO_IMAGE_TRUECOLOR
#define QT_NO_IMAGE_SMOOTHSCALE
#define QT_NO_IMAGE_TEXT
#define QT_NO_DIR
#define QT_NO_QWS_MANAGER
#define QT_NO_TEXTSTREAM
#define QT_NO_DATASTREAM
#define QT_NO_QWS_SAVEFONTS
//#define QT_NO_STRINGLIST
#define QT_NO_TEMPLATE_VARIANT
#define QT_NO_SESSIONMANAGER
#define QT_NO_QWS_KEYBOARD
#define QT_NO_SYNTAXHIGHLIGHTER

#define QT_NO_ACCEL
#define QT_NO_BUTTON
#define QT_NO_DIALOG
#define QT_NO_FRAME
#define QT_NO_SEMIMODAL

#define QT_NO_STYLE
#define QT_NO_EFFECTS
#define QT_NO_COP

#define QT_NO_SQL

#define QT_NO_REGEXP_CAPTURE
#define QT_NO_REGEXP_WILDCARD

#define QT_NO_VALIDATOR
#define QT_NO_SPRINTF

#define QT_NO_REGEXP

#define QT_NO_IMAGEIO

/* #define QT_NO_VARIANT //needed for signals/slots */

#define QT_NO_RANGECONTROL
#define QT_NO_QUUID_STRING
#define QT_NO_SIGNALMAPPER

#define QT_NO_WHEELEVENT
#define QT_NO_BEZIER

#define QT_NO_QWS_MOUSE_AUTO
/* #define QT_NO_QWS_MOUSE_MANUAL */

#define QT_NO_IMAGE_DITHER_TO_1
#define QT_NO_IMAGE_HEURISTIC_MASK
#define QT_NO_IMAGE_MIRROR

#ifndef QT_NO_STL
# define QT_NO_STL
#endif

#define QT_NO_DATESTRING
#define QT_NO_WMATRIX

#define QT_NO_DIRECTPAINTER
