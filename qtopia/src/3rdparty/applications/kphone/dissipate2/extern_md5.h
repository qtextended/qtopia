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
#ifndef EXTERN_MD5_H
#define EXTERN_MD5_H

#define HASHLEN 16
typedef u_char HASH[HASHLEN];
#define HASHHEXLEN 32
typedef char HASHHEX[HASHHEXLEN+1];
#define IN
#define OUT

#ifdef __cplusplus
extern "C" {

/* calculate H(A1) as per HTTP Digest spec */
extern void DigestCalcHA1(
	IN const char * pszAlg,
	IN const char * pszUserName,
	IN const char * pszRealm,
	IN const char * pszPassword,
	IN const char * pszNonce,
	IN const char * pszCNonce,
	OUT HASHHEX SessionKey
	);

/* calculate request-digest/response-digest as per HTTP Digest spec */
extern void DigestCalcResponse(
	IN HASHHEX HA1,                 /* H(A1) */
	IN const char * pszNonce,       /* nonce from server */
	IN const char * pszNonceCount,  /* 8 hex digits */
	IN const char * pszCNonce,      /* client nonce */
	IN const char * pszQop,         /* qop-value: "", "auth", "auth-int" */
	IN const char * pszMethod,      /* method from the request */
	IN const char * pszDigestUri,   /* requested URL */
	IN HASHHEX HEntity,             /* H(entity body) if qop="auth-int" */
	OUT HASHHEX Response            /* request-digest or response-digest */
	);
}
#endif
#endif
