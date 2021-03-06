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

#ifndef SIPHEADER_H_INCLUDED
#define SIPHEADER_H_INCLUDED

#include <qstring.h>

class SipHeader
{
public:
	enum SipHeaderId {
		Accept,
		Accept_Encoding,
		Accept_Language,
		Allow,
		Allow_Events,
		Also,
		Authorization,
		Call_ID,
		Contact,
		Content_Encoding,
		Content_Length,
		Content_Type,
		CSeq,
		Date,
		Encryption,
		Event,
		Expires,
		From,
		Hide,
		Max_Forwards,
		Organization,
		Proxy_Authenticate,
		Proxy_Authorization,
		Proxy_Require,
		Priority,
		Require,
		Retry_After,
		Response_Key,
		Record_Route,
		Refer_To,
		Referred_By,
		Route,
		Server,
		Subject,
		Timestamp,
		To,
		Unsupported,
		User_Agent,
		Via,
		Warning,
		WWW_Authenticate,
		BadHeader }; // SipHeaderID

	SipHeaderId id;
	QString data;

	SipHeader( SipHeaderId newid, QString newdata );
	~SipHeader( void );

	static const QString getHeaderString( SipHeaderId id, bool shortform = false );
	static SipHeaderId matchHeader( const QString& h );
};

#endif // SIPHEADER_H_INCLUDED
