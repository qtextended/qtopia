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
#include "sipuri.h"
#include "sipvia.h"

SipVia::SipVia( void )
{
	clear();
}

SipVia::SipVia( const QString& parseinput )
{
	clear();
	parseVia( parseinput );
}

SipVia::~SipVia( void )
{
}

void SipVia::clear( void )
{
	setTransport( UDP );
	port = SipUri::getSipPort();
	hasport = false;
	hasreceived = false;
	hasrport = false;
	ishidden = false;
	hasttl = false;
	hasmaddr = false;
	hasbranch = false;
	isvalid = true;
	protocolname = "SIP";
	protocolver = "2.0";
}

const QString SipVia::getTransportString( Transport t )
{
	switch ( t ) {
		case UDP: return "UDP";
		case TCP: return "TCP";
		case TLS: return "TLS";
		case BadTransport: return "BAD";
	}
	return QString::null;
}

SipVia::Transport SipVia::matchTransport( const QString t )
{
	if ( t.compare( getTransportString( UDP ) ) == 0 ) { return UDP; }
	if ( t.compare( getTransportString( TCP ) ) == 0 ) { return TCP; };
	if ( t.compare( getTransportString( TLS ) ) == 0 ) { return TLS; };

	return BadTransport;
}

const QString SipVia::getViaParamString( ViaParam v )
{
	switch ( v ) {
		case Hidden: return "hidden";
		case Ttl: return "ttl";
		case Maddr: return "maddr";
		case Received: return "received";
		case Rport: return "rport";
		case Branch: return "branch";
		case BadViaParam: return "BAD";
	}
	return QString::null;
}

SipVia::ViaParam SipVia::matchViaParam( const QString v )
{
	if ( v.compare( getViaParamString( Hidden ) ) == 0 ) { return Hidden; }
	if ( v.compare( getViaParamString( Ttl ) ) == 0 ) { return Ttl; }
	if ( v.compare( getViaParamString( Maddr ) ) == 0 ) { return Maddr; }
	if ( v.compare( getViaParamString( Received ) ) == 0 ) { return Received; }
	if ( v.compare( getViaParamString( Branch ) ) == 0 ) { return Branch; }
	if ( v.compare( getViaParamString( Rport ) ) == 0 ) { return Rport; }

	return BadViaParam;
}

void SipVia::parseVia( const QString& parseinput )
{
	QString inputline;
	QString hostport;
	QString curparm;
	QString parmname;
	QString parmvalue;
	QString transport;
	SipVia::ViaParam parm;

	isvalid = false;

	inputline = parseinput.simplifyWhiteSpace();

	protocolname = inputline.left( inputline.find( "/" ) ).upper();
	inputline.remove( 0, inputline.find( "/" ) + 1 );

	protocolver = inputline.left( inputline.find( "/" ) );
	inputline.remove( 0, inputline.find( "/" ) + 1 );

	// fixed for notel
	transport = inputline.left( inputline.find( " " ) ).upper();
	inputline.remove( 0, inputline.find( " " ) + 1 );

	setTransport( matchTransport( transport ) );
	if( getTransport() == BadTransport ) { return; }

	if ( inputline.contains( ";" ) ) {
		hostport = inputline.left( inputline.find( ";" ) );
		inputline.remove( 0, inputline.find( ";" ) );
	} else if ( inputline.contains( " " ) ) {
		hostport = inputline.left( inputline.find( " " ) );
		inputline.remove( 0, inputline.find( " " ) + 1 );
	} else {
		hostport = inputline;
		inputline = QString::null;
	}

	if ( hostport.contains( ":" ) ) {
		setHostname( hostport.left( hostport.find( ":" ) ) );
		setPortNumber( hostport.mid( hostport.find( ":" ) + 1 ).toUInt() );
		hasport = true;
	} else {
		setHostname( hostport );
		setPortNumber( SipUri::getSipPort() );
	}
	if( !hostname.contains( "." ) ) {
		return;
	}

	while ( inputline[ 0 ] == ';' ) {
		curparm = inputline.mid( 1, inputline.find( ";", 1 ) - 1 ).stripWhiteSpace();
		inputline.remove( 0, inputline.find( ";", 1 ) );

		if( curparm.contains( '=' ) ) {
			parmname = curparm.left( curparm.find( '=' ) ).stripWhiteSpace().lower();
			parmvalue = curparm.mid( curparm.find( '=' ) + 1 ).stripWhiteSpace();
		} else {
			parmname = curparm;
			parmvalue = QString::null;
		}

		parm = matchViaParam( parmname );

		if( parm == Received ) {
			hasreceived = true;
			received = parmvalue;
		} else if( parm == Rport ) {
			hasrport = true;
			rport = parmvalue;
		} else if( parm == Hidden ) {
			ishidden = true;
		} else if( parm == Ttl ) {
			hasttl = true;
			ttl = parmvalue;
		} else if( parm == Maddr ) {
			hasmaddr = true;
			maddr = parmvalue;
		} else if( parm == Branch ) {
			hasbranch = true;
			branch = parmvalue;

//if different of the above , save not ignore it
		} else {
			restOfParams.append( SimpleParameter(parmname,parmvalue));
			//cout<<"GAMMA:via_parse:"<<parmname<<"="<<parmvalue<<endl;

		}
	}

	isvalid = true;
}

void SipVia::setHostname( const QString &hname )
{
	hostname = hname;
}

void SipVia::setPortNumber( unsigned int p )
{
	port = p;
}

QString SipVia::via( void )
{
	QString viatext;

	viatext += protocolname;
	viatext += "/";
	viatext += protocolver;
	viatext += "/";
	viatext += getTransportString( trans );
	viatext += " ";
	viatext += hostname;

	if( hasport || port != SipUri::getSipPort() ) {
		viatext += ":";
		viatext += QString::number( port );
	}

	if( hasreceived ) {
		viatext += ";received=";
		viatext += received;
	}

	if( hasrport ) {
		viatext += ";rport";
		if (rport != QString::null) {
			viatext += "=";
			viatext += rport;
		}
	}

	if( ishidden ) {
		viatext += ";hidden";
	}

	if( hasttl ) {
		viatext += ";ttl=";
		viatext += ttl;
	}

	if( hasmaddr ) {
		viatext += ";maddr=";
		viatext += maddr;
	}

	if( hasbranch ) {
		viatext += ";branch=";
		viatext += branch;
	}

	//add the rest of the params
	ParameterListIterator it;
	for(it = restOfParams.begin();it != restOfParams.end();++it) {
		viatext +=";";
		viatext +=(*it).getName();
		if( !(*it).getValue().isEmpty() ) {
			viatext +="=";
			viatext +=(*it).getValue();
		}
	}
	if( trans == TCP && !viatext.contains(";alias") ) {
		viatext += ";alias";
	}

	return viatext;
}

void SipVia::setTransport( Transport t )
{
	trans = t;
}

void SipVia::setReceivedParam( const QString &newreceived )
{
	if( newreceived == QString::null ) {
		hasreceived = false;
	} else {
		hasreceived = true;
		received = newreceived.lower();
	}
}

void SipVia::setRportParam( const QString &newport )
{
	rport = newport.lower();
	if (rport != QString::null) {
		hasrport = true;
	} else {
		hasrport = false;
	}
}

void SipVia::setHidden( bool hidden )
{
	ishidden = hidden;
}

void SipVia::setTtlParam( const QString &newttl )
{
	if( newttl == QString::null ) {
		hasttl = false;
	} else {
		hasttl = true;
		ttl = newttl;
	}
}

void SipVia::setMaddrParam( const QString &newmaddr )
{
	if( newmaddr == QString::null ) {
		hasmaddr = false;
	} else {
		hasmaddr = true;
		maddr = newmaddr;
	}
}

void SipVia::setBranchParam( const QString &newbranch )
{
	if( newbranch == QString::null ) {
		hasbranch = false;
	} else {
		hasbranch = true;
		branch = newbranch;
	}
}

void SipVia::generateBranchParam( void )
{
	int uniqid = rand();
	branch.sprintf( "z9hG4bK%X", uniqid );
	hasbranch = true;
}

SipVia &SipVia::operator=( const SipVia &v )
{
	setHostname( v.getHostname() );
	setTransport( v.getTransport() );
	setPortNumber( v.getPortNumber() );
	setReceivedParam( v.getReceivedParam() );
	setRportParam( v.getRportParam() );
	setHidden( v.isHidden() );
	setTtlParam( v.getTtlParam() );
	setMaddrParam( v.getMaddrParam() );
	setBranchParam( v.getBranchParam() );

	//copy the rest of params
	restOfParams = v.restOfParams;

	return *this;
}

bool SipVia::operator==( const SipVia &v ) const
{
	if( v.getHostname() == getHostname() && v.getTransport() == getTransport() &&
	    v.getPortNumber() == getPortNumber() && v.getReceivedParam() == getReceivedParam() &&
	    v.isHidden() == isHidden() && v.getTtlParam() == getTtlParam() &&
	    v.getMaddrParam() == getMaddrParam() && v.getBranchParam() == getBranchParam() &&
	    v.getRportParam() == getRportParam() ) {
		return true;
	}

	return false;
}

bool SipVia::operator!=( const SipVia &v ) const
{
	return !( *this == v );
}

bool SipVia::operator==( const QString &v ) const
{
	SipVia via( v );
	return ( *this == v );
}

bool SipVia::operator!=( const QString &v ) const
{
	return !( *this == v );
}

