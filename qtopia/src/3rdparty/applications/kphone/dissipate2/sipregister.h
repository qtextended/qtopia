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

#ifndef SIPREGISTER_H_INCLUDED
#define SIPREGISTER_H_INCLUDED

#include <qobject.h>
#include <qlist.h>
#include <qtimer.h>

#include "sipuri.h"

class QTimer;
class SipCall;
class SipUser;
class SipClient;
class SipTransaction;

/**
 * Class representing a SIP registration server and a user status with it.
 */
class SipRegister : public QObject
{
	Q_OBJECT
public:
	/**
	 * Creates a new SipRegister object representing the registration with
	 * the given proxy.
	 */
	SipRegister( SipUser *user, const SipUri &serveruri, int expires, QString qvalue );

	/**
	 * SipRegister destructor.
	 */
	~SipRegister( void );

	enum RegisterState {
		NotConnected,
		TryingServer,
		AuthenticationRequired,
		Connected,
		Disconnecting,
		TryingServerWithPassword,
		AuthenticationRequiredWithNewPassword,
		Reconnecting };

	enum AuthType {
		BasicAuthenticationRequired,
		DigestAuthenticationRequired,
		ProxyBasicAuthenticationRequired,
		ProxyDigestAuthenticationRequired };

	/**
	 * Returns the current state of the registration with the server.
	 */
	RegisterState getRegisterState( void ) const { return rstate; }

	/**
	 * Returns the current type of authentication required for this
	 * registration.
	 */
	AuthType getAuthenticationType( void ) const { return authtype; }

	/**
	 * Returns the SIP URI of the server.
	 */
	SipUri getServerUri( void ) const { return regserver; }

	/**
	 * Sets the URI of the registration server.
	 */
	void setOutboundProxyUri( const SipUri &newregserver );

	/**
	 * Requests registration from the proxy.
	 */
	void requestRegister( const QString &username = QString::null,
		const QString &password = QString::null );

	/**
	 * Performs a 3rd party registration for the given user.
	 */
	void requestRegister( const SipUri &foruri );

	/**
	 * Creates a request to clear this registration.
	 */
	void requestClearRegistration( void );

	/**
	 * Returns a pointer to the call used for performing the registrations.
	 */
	SipCall *getRegisterCall( void ) { return regcall; }

	/**
	 * Returns the realm from the authentication request.
	 */
	QString getAuthRealm( void );

	/**
	 * Returns the realm from the proxy authentication request.
	 */
	QString getProxyAuthRealm( void );

	/**
	 * Returns pointer to transaction.
	 */
	SipTransaction *getCurtrans( void ) { return curtrans; }

	void setAutoRegister( bool autoreg ) { autoregister = autoreg; }
	bool getAutoRegister( void ) { return autoregister; }
	void updateRegister( void );
	QString getOutboundProxy( void ) { return outboundProxy; }
	QString getQvalue( void ) { return qValue; }

signals:
	/**
	 * This signal is sent whenever the status of this registrations has
	 * changed.
	 */
	void statusUpdated( void );

private slots:
	void localStatusUpdated( void );
	void register_timeout( void );

private:
	bool autoregister;
	QTimer *timer;
	SipUser *regUser;
	int expiresTime;
	bool cleanRegister;
	SipCall *regcall;
	SipTransaction *curtrans;

	enum RegisterOp {
		NoRegOpPending,
		RegRequest,
		RegClear };

	RegisterOp regop;
	RegisterState rstate;
	AuthType authtype;

	SipUri regserver;

	QString authstr;
	QString proxyauthstr;

	QString authresponse;
	QString proxyauthresponse;
	QString outboundProxy;
	QString qValue;
};

typedef QListIterator<SipRegister> SipRegisterIterator;

#endif // SIPREGISTER_H_INCLUDED
