/**********************************************************************
** Copyright (C) 2000-2005 Trolltech AS and its licensors.
** All rights reserved.
**
** This file is part of the Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
** See below for additional copyright and license information
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/
/*
 * Copyright (c) 2000 Billy Biggs <bbiggs@div8.net>
 * Copyright (c) 2004 Wirlab <kphone@wirlab.net>
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
 * License for more details.
 * 
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 *
 */

#ifndef UDPMESSAGESOCKET_H_INCLUDED
#define UDPMESSAGESOCKET_H_INCLUDED

#include "messagesocket.h"

/**
 * This is the UDP implementation of the MessageSocket class.  It is used
 * for performing operations on a new or existing UDP socket.
 *
 * @short Generic UDP Socket class
 * @author Billy Biggs <bbiggs@bbcr.uwaterloo.ca>
 */
class UDPMessageSocket : public MessageSocket
{
public:
	UDPMessageSocket( void );
	UDPMessageSocket( int newfd );
	~UDPMessageSocket( void );
	int connect( unsigned int portnum );
	int SetTOS( void );
	int send( const char *sendbuffer, unsigned int length );
	int receive( char *recvbuffer, unsigned int maxlength );
	unsigned int listen( unsigned int portnum );
	int accept( void );
	int listenOnEvenPortOS( void );
	int listenOnEvenPort( int min = 0, int max = 0 );
	int listenOnEvenPortForVideo( void );
private:
	bool didcomplain;
};

#endif // UDPMESSAGESOCKET_H_INCLUDED
