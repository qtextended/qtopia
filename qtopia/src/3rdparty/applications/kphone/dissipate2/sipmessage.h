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

#ifndef SIPMESSAGE_H_INCLUDED
#define SIPMESSAGE_H_INCLUDED

#include <sys/time.h>
#include <qstring.h>
#include <qlist.h>

#include "sipheader.h"
#include "sipuri.h"
#include "sipstatus.h"
#include "sipprotocol.h"
#include "sipvialist.h"
#include "sipurilist.h"

/**
 * @short	Representation of a single SIP message.
 * @author	Billy Biggs <bbiggs@div8.net>
 *
 * @ref SipMessage is used for creating and for parsing individual SIP
 * messages.  It also contains a retransmission timer for use when sending the
 * message.
 */
class SipMessage
{
public:
	/**
	 * Create a blank SIP message.
	 */
	SipMessage( void );

	/**
	 * Parse the given input as a SIP message.
	 */
	SipMessage( const QString& parseinput );

	/**
	 * SipMessage destructor.
	 */
	~SipMessage( void );

	enum MsgType {
		Request,
		Response,
		BadType };

	/**
	 * Sets the type of SIP message this represents.  See @ref
	 * SipMessage::getType() for a list of possible values.
	 */
	void setType( MsgType newtype ) { type = newtype; }

	/**
	 * Returns the type of SIP message represented.
	 */
	MsgType getType( void ) const { return type; }

	/**
	 * Sets the method on this SIP message.  Valid only for SIP requests.
	 */
	void setMethod( Sip::Method newmethod ) { meth = newmethod; }

	/**
	 * Returns the method of this SIP message. Valid only for SIP requests.
	 */
	Sip::Method getMethod( void ) const { return meth; }

	/**
	 * Returns true if the message has a body.
	 */
	bool haveBody( void ) const { return havebody; }

	/**
	 * Sets the body of the SIP message.
	 */
	void setBody( const QString& newbody );

	/**
	 * Generates and returns the version string for this message.
	 */
	QString getVersionString( void ) const;

	/**
	 * Generates and returns the start line for this message.
	 */
	QString startLine( void ) const;

	/**
	 * Generates and returns all of the heads of this message.
	 */
	QString messageHeaders( void );

	/**
	 * Returns the message body.
	 */
	QString messageBody( void ) const;

	/**
	 * Generates and returns the SIP message in full.
	 */
	QString message( void );

	/**
	 * Parses the given input as a SIP message.
	 */
	void parseMessage( const QString& parseinput );

	/**
	 * Inserts a new header into the message.
	 */
	void insertHeader( SipHeader::SipHeaderId id, QString data );

	/**
	 * Returns true if the message contains the specified header.
	 */
	bool hasHeader( SipHeader::SipHeaderId id );

	/**
	 * Returns the data contained in the specified header.
	 */
	QString getHeaderData( SipHeader::SipHeaderId id );

	/**
	 * Sets the request URI for the message.  Relevant only if it is a SIP
	 * request.  This will also set the destination host and port for the
	 * message.
	 */
	void setRequestUri( const SipUri &newrequri );

	/**
	 * Returns the request URI for this message.  Relevant only if it is a
	 * SIP request.
	 */
	SipUri &getRequestUri( void ) { return requesturi; }

	/**
	 * Sets the status of the message.  Relevant only if it is a SIP
	 * response.
	 */
	void setStatus( const SipStatus &stat );

	/**
	 * Returns the status of the message. Relevant only if it is a SIP
	 * response.
	 */
	const SipStatus &getStatus( void ) const { return status; }

	/**
	 * Returns a reference to the via list for this message.  Usually
	 * useful for getting and setting the topmost via entry.
	 */
	SipViaList &getViaList( void ) { return vialist; }

	/**
	 * Returns a reference to the Record-Route for this message.
	 */
	SipUriList &getRecordRoute( void ) { return recordroute; }

	/**
	 * Returns a reference to the contact list for this message.
	 */
	SipUriList &getContactList( void ) { return contactlist; }

	/**
	 * Sets the via list to be a copy of an existing via list.  Useful for
	 * building responses.
	 */
	void setViaList( const SipViaList &copylist );

	/**
	 * Sets the contact list for this message.
	 */
	void setContactList( const SipUriList &newclist );

	/**
	 * Sets the record route for this message.
	 */
	void setRecordRoute( const SipUriList &newrr );

	/**
	 * Returns the initial timestamp placed on the message.
	 */
	struct timeval *getInitialTimestamp( void ) { return &itimestamp; }

	/**
	 * Returns the timestamp of the last retransmission.
	 */
	struct timeval *getTimestamp( void ) { return &timestamp; }

	/**
	 * Recalculates the timestamp on the message.
	 */
	void setTimestamp( void );

	/**
	 * Returns the last time tick.
	 */
	unsigned int lastTimeTick( void ) const { return lastt; }

	/**
	 * Sets the current time tick.
	 */
	void setTimeTick( unsigned int newtt );

	/**
	 * Returns the current value of the retransmission counter for this
	 * message.
	 */
	unsigned int sendCount( void ) const { return sendcount; }

	/**
	 * Increments the retransmission coutner.
	 */
	void incrSendCount( void );

	/**
	 * Static method to create a new SIP Call ID.
	 */
	static QString createCallId( void );

	void setQvalue( const QString& value );

	bool isValid( void );

private:
	SipViaList vialist;
	QList<SipHeader> headerlist;

	SipUriList recordroute;
	SipUriList contactlist;

	MsgType type;
	Sip::Method meth;

	SipStatus status;

	SipUri requesturi;

	bool havebody;
	QString messagebody;

	void setDefaultVars( void );
	void parseStartLine( QString startline );
	void parseHeaders( const QString &inbuf );

	struct timeval timestamp;
	struct timeval itimestamp;
	unsigned int lastt;
	unsigned int sendcount;

	bool haveQ;
	QString qValue;
};

#endif // SIPMESSAGE_H_INCLUDED
