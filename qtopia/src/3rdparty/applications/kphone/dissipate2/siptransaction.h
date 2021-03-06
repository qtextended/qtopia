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

#ifndef SIPTRANSACTION_H_INCLUDED
#define SIPTRANSACTION_H_INCLUDED

#include <qobject.h>
#include <qlist.h>

#include "sipuri.h"
#include "sipstatus.h"
#include "sipprotocol.h"
#include "sipurilist.h"
#include "mimecontenttype.h"

class SipClient;
class SipCall;
class SipCallMember;
class SipMessage;
class QTimer;


/**
 * This class represents a single SIP transaction.  Any
 * From RFC2543:
 *
 * (SIP) transaction: A SIP transaction occurs between a client and a
 *      server and comprises all messages from the first request sent
 *      from the client to the server up to a final (non-1xx) response
 *      sent from the server to the client.
 */
class SipTransaction : public QObject
{
	Q_OBJECT

	friend class SipCall;
	friend class SipClient;
public:
	/**
	 * Creates a new SIP transaction with a given initial CSeq number, the
	 * farend call member, and the parent call.
	 */
	SipTransaction( unsigned int seqn, SipCallMember *farend, SipCall *call );

	/**
	 * SipTransaction destructor.
	 */
	~SipTransaction( void );

	/**
	 * Returns a pointer to the far end call member for this transaction.
	 */
	SipCallMember *getCallMember( void ) const { return remote; }

	/**
	 * Returns the CSeq value for this transaction (seqnum + method).
	 */
	QString getCSeq( void ) const;

	enum Direction {
		RemoteRequest,
		LocalRequest,
		None };

	/**
	 * Returns the Direction of this request, either
	 * SipTransaction::RemoteRequest or SipTransaction::LocalRequest.
	 */
	Direction getDirection( void ) const { return direction; }

	/**
	 * Returns the numeric part of the CSeq for this transaction.
	 */
	unsigned int getSeqNum( void ) const { return seqnum; }

	/**
	 * Returns the Method part of the CSeq for this transaction.
	 */
	QString getSeqMethod( void ) const;

	/**
	 * Returns the current status of this transaction.
	 */
	const SipStatus &getStatus( void ) const { return laststatus; }

	/**
	 * Returns true if this request was cancelled.
	 */
	bool wasCancelled( void ) const { return cancelled; }

	/**
	 * Returns the initial request which initiated this transaction.
	 * Useful for extracting meaning to the whole transaction.
	 */
	SipMessage *getRequest( void ) const { return requestmessage; }

	/**
	 * Sends a SIP response under this transaction.
	 */
	void sendResponse( const SipStatus &status,
		const QString &body = QString::null,
		const MimeContentType &bodytype = MimeContentType::null );

	/**
	 * Cancels this request transaction by sending a CANCEL request.
	 */
	void cancelRequest( const QString &body = QString::null,
		const MimeContentType &bodytype = MimeContentType::null );

	/**
	 * Returns the last applicable message body received.  Useful for
	 * getting the session description, or the reason for the redirect, or
	 * whatever.  Be sure to check the content-type as well.
	 */
	QString getFinalMessageBody( void );

	/**
	 * Returns the MIME content-type of the final message body.
	 */
	MimeContentType getFinalContentType( void );

	/**
	 * Returns the contact list of the final response.
	 */
	SipUriList getFinalContactList( void );

	QString getFinalWWWAuthString( void );
	QString getFinalProxyAuthString( void );

	/**
	 * Returns the body of the request message.
	 */
	QString getRequestMessageBody( void ) const;

	/**
	 * Returns the MIME content-type of the request message body.
	 */
	MimeContentType getRequestMessageContentType( void ) const;

signals:
	void statusUpdated( void );

private slots:
	void send_202( void );

private:
	SipCall *parent;
	SipMessage *requestmessage;
	QList<SipMessage> responses;

	// Last status for this transaction
	SipStatus laststatus;

	unsigned int seqnum;
	QString branch;
	SipCallMember *remote;
	Direction direction;
	bool cancelled;
	QTimer *timer_202;

	void setStatus( const SipStatus &stat );

	// For SipCall
	void incomingRequest( SipMessage *message );
	void incomingRequestRetransmission( SipMessage *message );
	void incomingResponse( SipMessage *message );

	void sendRegister( const SipUri &registerserver, int expiresTime,
		const QString &authentication = QString::null,
		const QString &proxyauthentication = QString::null,
		const QString &body = QString::null,
		const MimeContentType &bodytype = MimeContentType::null,
		const QString &qValue = QString::null );

	bool sendRequest( Sip::Method meth,
		const QString &body = QString::null,
		const MimeContentType &bodytype = MimeContentType::null,
		const SipUri &transferto = SipUri::null,
		const QString &proxyauthentication = QString::null,
		const int expiresTime = -1 );

	// For SipClient
	bool auditPending( void );

	QString stateText( QString state );

};

#endif // SIPTRANSACTION_H_INCLUDED
