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

/***************************************************************************
(C) Copyright 1996 Apple Computer, Inc., AT&T Corp., International
Business Machines Corporation and Siemens Rolm Communications Inc.

For purposes of this license notice, the term Licensors shall mean,
collectively, Apple Computer, Inc., AT&T Corp., International
Business Machines Corporation and Siemens Rolm Communications Inc.
The term Licensor shall mean any of the Licensors.

Subject to acceptance of the following conditions, permission is hereby
granted by Licensors without the need for written agreement and without
license or royalty fees, to use, copy, modify and distribute this
software for any purpose.

The above copyright notice and the following four paragraphs must be
reproduced in all copies of this software and any software including
this software.

THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS AND NO LICENSOR SHALL HAVE
ANY OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS OR
MODIFICATIONS.

IN NO EVENT SHALL ANY LICENSOR BE LIABLE TO ANY PARTY FOR DIRECT,
INDIRECT, SPECIAL OR CONSEQUENTIAL DAMAGES OR LOST PROFITS ARISING OUT
OF THE USE OF THIS SOFTWARE EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
DAMAGE.

EACH LICENSOR SPECIFICALLY DISCLAIMS ANY WARRANTIES, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO ANY WARRANTY OF NONINFRINGEMENT OR THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.

The software is provided with RESTRICTED RIGHTS.  Use, duplication, or
disclosure by the government are subject to restrictions set forth in
DFARS 252.227-7013 or 48 CFR 52.227-19, as applicable.

***************************************************************************/

/*

The vCard/vCalendar C interface is implemented in the set
of files as follows:

vcc.y, yacc source, and vcc.c, the yacc output you will use
implements the core parser

vobject.c implements an API that insulates the caller from
the parser and changes in the vCard/vCalendar BNF

port.h defines compilation environment dependent stuff

vcc.h and vobject.h are header files for their .c counterparts

vcaltmp.h and vcaltmp.c implement vCalendar "macro" functions
which you may find useful.

test.c is a standalone test driver that exercises some of
the features of the APIs provided. Invoke test.exe on a
VCARD/VCALENDAR input text file and you will see the pretty
print output of the internal representation (this pretty print
output should give you a good idea of how the internal
representation looks like -- there is one such output in the
following too). Also, a file with the .out suffix is generated
to show that the internal representation can be written back
in the original text format.

For more information on this API see the readme.txt file
which accompanied this distribution.

  Also visit:

		http://www.versit.com
		http://www.ralden.com

*/

// No tr() anywhere in this file


#ifndef __VOBJECT_H__
#define __VOBJECT_H__ 1

#include <qstring.h>

#define	vCardClipboardFormat		"+//ISBN 1-887687-00-9::versit::PDI//vCard"
#define	vCalendarClipboardFormat	"+//ISBN 1-887687-00-9::versit::PDI//vCalendar"

/* The above strings vCardClipboardFormat and vCalendarClipboardFormat
are globally unique IDs which can be used to generate clipboard format
ID's as per the requirements of a specific platform. For example, in
Windows they are used as the parameter in a call to RegisterClipboardFormat.
For example:

  CLIPFORMAT foo = RegisterClipboardFormat(vCardClipboardFormat);

*/

#define vCardMimeType		"text/x-vCard"
#define vCalendarMimeType	"text/x-vCalendar"

#undef DLLEXPORT
#include <qtopia/qpeglobal.h>
#if defined(QTOPIA_MAKEDLL)
#define DLLEXPORT(t)   __declspec(dllexport) t
#elif defined(QTOPIA_DLL)
#define DLLEXPORT(t)   __declspec(dllimport) t
#else
#define DLLEXPORT(t) t
#endif

#ifndef FALSE
#define FALSE	0
#endif
#ifndef TRUE
#define TRUE	1
#endif

#include <stdlib.h>
#include <stdio.h>


#define VC7bitProp				"7BIT"
#define VC8bitProp				"8BIT"
#define VCAAlarmProp			"AALARM"
#define VCAdditionalNamesProp	"ADDN"
#define VCAdrProp				"ADR"
#define VCAgentProp				"AGENT"
#define VCAIFFProp				"AIFF"
#define VCAOLProp				"AOL"
#define VCAppleLinkProp			"APPLELINK"
#define VCAttachProp			"ATTACH"
#define VCAttendeeProp			"ATTENDEE"
#define VCATTMailProp			"ATTMAIL"
#define VCAudioContentProp		"AUDIOCONTENT"
#define VCAVIProp				"AVI"
#define VCBase64Prop			"BASE64"
#define VCBBSProp				"BBS"
#define VCBirthDateProp			"BDAY"
#define VCBMPProp				"BMP"
#define VCBodyProp				"BODY"
#define VCBusinessRoleProp		"ROLE"
#define VCCalProp				"VCALENDAR"
#define VCCaptionProp			"CAP"
#define VCCardProp				"VCARD"
#define VCCarProp				"CAR"
#define VCCategoriesProp		"CATEGORIES"
#define VCCellularProp			"CELL"
#define VCCGMProp				"CGM"
#define VCCharSetProp			"CHARSET"
#define VCCIDProp				"CID"
#define VCCISProp				"CIS"
#define VCCityProp				"L"
#define VCClassProp				"CLASS"
#define VCCommentProp			"NOTE"
#define VCCompletedProp			"COMPLETED"
#define VCContentIDProp			"CONTENT-ID"
#define VCCountryNameProp		"C"
#define VCDAlarmProp			"DALARM"
#define VCDataSizeProp			"DATASIZE"
#define VCDayLightProp			"DAYLIGHT"
#define VCDCreatedProp			"DCREATED"
#define VCDeliveryLabelProp     "LABEL"
#define VCDescriptionProp		"DESCRIPTION"
#define VCDIBProp				"DIB"
#define VCDisplayStringProp		"DISPLAYSTRING"
#define VCDomesticProp			"DOM"
#define VCDTendProp				"DTEND"
#define VCDTstartProp			"DTSTART"
#define VCDueProp				"DUE"
#define VCEmailAddressProp		"EMAIL"
#define VCEncodingProp			"ENCODING"
#define VCEndProp				"END"
#define VCEventProp				"VEVENT"
#define VCEWorldProp			"EWORLD"
#define VCExNumProp				"EXNUM"
#define VCExpDateProp			"EXDATE"
#define VCExpectProp			"EXPECT"
#define VCExtAddressProp		"EXT ADD"
#define VCFamilyNameProp		"F"
#define VCFaxProp				"FAX"
#define VCFullNameProp			"FN"
#define VCGeoProp				"GEO"
#define VCGeoLocationProp		"GEO"
#define VCGIFProp				"GIF"
#define VCGivenNameProp			"G"
#define VCGroupingProp			"Grouping"
#define VCHomeProp				"HOME"
#define VCIBMMailProp			"IBMMail"
#define VCInlineProp			"INLINE"
#define VCInternationalProp		"INTL"
#define VCInternetProp			"INTERNET"
#define VCISDNProp				"ISDN"
#define VCJPEGProp				"JPEG"
#define VCLanguageProp			"LANG"
#define VCLastModifiedProp		"LAST-MODIFIED"
#define VCLastRevisedProp		"REV"
#define VCLocationProp			"LOCATION"
#define VCLogoProp				"LOGO"
#define VCMailerProp			"MAILER"
#define VCMAlarmProp			"MALARM"
#define VCMCIMailProp			"MCIMAIL"
#define VCMessageProp			"MSG"
#define VCMETProp				"MET"
#define VCModemProp				"MODEM"
#define VCMPEG2Prop				"MPEG2"
#define VCMPEGProp				"MPEG"
#define VCMSNProp				"MSN"
#define VCNamePrefixesProp		"NPRE"
#define VCNameProp				"N"
#define VCNameSuffixesProp		"NSUF"
#define VCNoteProp				"NOTE"
#define VCOrgNameProp			"ORGNAME"
#define VCOrgProp				"ORG"
#define VCOrgUnit2Prop			"OUN2"
#define VCOrgUnit3Prop			"OUN3"
#define VCOrgUnit4Prop			"OUN4"
#define VCOrgUnitProp			"OUN"
#define VCPagerProp				"PAGER"
#define VCPAlarmProp			"PALARM"
#define VCParcelProp			"PARCEL"
#define VCPartProp				"PART"
#define VCPCMProp				"PCM"
#define VCPDFProp				"PDF"
#define VCPGPProp				"PGP"
#define VCPhotoProp				"PHOTO"
#define VCPICTProp				"PICT"
#define VCPMBProp				"PMB"
#define VCPostalBoxProp			"BOX"
#define VCPostalCodeProp		"PC"
#define VCPostalProp			"POSTAL"
#define VCPowerShareProp		"POWERSHARE"
#define VCPreferredProp			"PREF"
#define VCPriorityProp			"PRIORITY"
#define VCProcedureNameProp		"PROCEDURENAME"
#define VCProdIdProp			"PRODID"
#define VCProdigyProp			"PRODIGY"
#define VCPronunciationProp		"SOUND"
#define VCPSProp				"PS"
#define VCPublicKeyProp			"KEY"
#define VCQPProp				"QP"
#define VCQuickTimeProp			"QTIME"
#define VCQuotedPrintableProp	"QUOTED-PRINTABLE"
#define VCRDateProp				"RDATE"
#define VCRegionProp			"R"
#define VCRelatedToProp			"RELATED-TO"
#define VCRepeatCountProp		"REPEATCOUNT"
#define VCResourcesProp			"RESOURCES"
#define VCRNumProp				"RNUM"
#define VCRoleProp				"ROLE"
#define VCRRuleProp				"RRULE"
#define VCRSVPProp				"RSVP"
#define VCRunTimeProp			"RUNTIME"
#define VCSequenceProp			"SEQUENCE"
#define VCSnoozeTimeProp		"SNOOZETIME"
#define VCStartProp				"START"
#define VCStatusProp			"STATUS"
#define VCStreetAddressProp		"STREET"
#define VCSubTypeProp			"SUBTYPE"
#define VCSummaryProp			"SUMMARY"
#define VCTelephoneProp			"TEL"
#define VCTIFFProp				"TIFF"
#define VCTimeZoneProp			"TZ"
#define VCTitleProp				"TITLE"
#define VCTLXProp				"TLX"
#define VCTodoProp				"VTODO"
#define VCTranspProp			"TRANSP"
#define VCUniqueStringProp		"UID"
#define VCURLProp				"URL"
#define VCURLValueProp			"URLVAL"
#define VCValueProp				"VALUE"
#define VCVersionProp			"VERSION"
#define VCVideoProp				"VIDEO"
#define VCVoiceProp				"VOICE"
#define VCWAVEProp				"WAVE"
#define VCWMFProp				"WMF"
#define VCWorkProp				"WORK"
#define VCX400Prop				"X400"
#define VCX509Prop				"X509"
#define VCXRuleProp				"XRULE"


typedef struct VObject VObject;

typedef struct VObjectIterator {
    VObject* start;
    VObject* next;
    } VObjectIterator;

extern DLLEXPORT(VObject*) newVObject(const char *id);
extern DLLEXPORT(void) deleteVObject(VObject *p);
extern DLLEXPORT(char*) dupStr(const char *s, unsigned int size);
extern DLLEXPORT(void) deleteStr(const char *p);
extern DLLEXPORT(void) unUseStr(const char *s);

extern DLLEXPORT(void) setVObjectName(VObject *o, const char* id);
extern DLLEXPORT(void) setVObjectStringZValue(VObject *o, const char *s);
extern DLLEXPORT(void) setVObjectStringZValue_(VObject *o, const char *s);
extern DLLEXPORT(void) setVObjectIntegerValue(VObject *o, unsigned int i);
extern DLLEXPORT(void) setVObjectLongValue(VObject *o, unsigned long l);
extern DLLEXPORT(void) setVObjectAnyValue(VObject *o, void *t);
extern DLLEXPORT(VObject*) setValueWithSize(VObject *prop, void *val, unsigned int size);
extern DLLEXPORT(VObject*) setValueWithSize_(VObject *prop, void *val, unsigned int size);

extern DLLEXPORT(const char*) vObjectName(VObject *o);
extern DLLEXPORT(const char*) vObjectStringZValue(VObject *o);
extern DLLEXPORT(unsigned int) vObjectIntegerValue(VObject *o);
extern DLLEXPORT(unsigned long) vObjectLongValue(VObject *o);
extern DLLEXPORT(void*) vObjectAnyValue(VObject *o);
extern DLLEXPORT(VObject*) vObjectVObjectValue(VObject *o);
extern DLLEXPORT(void) setVObjectVObjectValue(VObject *o, VObject *p);

extern DLLEXPORT(VObject*) addVObjectProp(VObject *o, VObject *p);
extern DLLEXPORT(VObject*) addProp(VObject *o, const char *id);
extern DLLEXPORT(VObject*) addProp_(VObject *o, const char *id);
extern DLLEXPORT(VObject*) addPropValue(VObject *o, const char *p, const char *v);
extern DLLEXPORT(VObject*) addPropSizedValue_(VObject *o, const char *p, const char *v, unsigned int size);
extern DLLEXPORT(VObject*) addPropSizedValue(VObject *o, const char *p, const char *v, unsigned int size);
extern DLLEXPORT(VObject*) addGroup(VObject *o, const char *g);
extern DLLEXPORT(void) addList(VObject **o, VObject *p);

extern DLLEXPORT(VObject*) isAPropertyOf(VObject *o, const char *id);

extern DLLEXPORT(VObject*) nextVObjectInList(VObject *o);
extern DLLEXPORT(void) initPropIterator(VObjectIterator *i, VObject *o);
extern DLLEXPORT(int) moreIteration(VObjectIterator *i);
extern DLLEXPORT(VObject*) nextVObject(VObjectIterator *i);

extern DLLEXPORT(const char*) lookupStr(const char *s);
extern DLLEXPORT(void) cleanStrTbl();

extern DLLEXPORT(void) cleanVObject(VObject *o);
extern DLLEXPORT(void) cleanVObjects(VObject *list);

extern DLLEXPORT(const char*) lookupProp(const char* str);
extern DLLEXPORT(const char*) lookupProp_(const char* str);

extern DLLEXPORT(void) writeVObjectToFile(char *fname, VObject *o);
extern DLLEXPORT(void) writeVObjectsToFile(char *fname, VObject *list);

extern DLLEXPORT(int) vObjectValueType(VObject *o);

/* return type of vObjectValueType: */
#define VCVT_NOVALUE	0
	/* if the VObject has no value associated with it. */
#define VCVT_STRINGZ	1
	/* if the VObject has value set by setVObjectStringZValue. */
#define VCVT_UINT		2
	/* if the VObject has value set by setVObjectIntegerValue. */
#define VCVT_ULONG	       	3
	/* if the VObject has value set by setVObjectLongValue. */
#define VCVT_RAW		4
	/* if the VObject has value set by setVObjectAnyValue. */
#define VCVT_VOBJECT	5
	/* if the VObject has value set by setVObjectVObjectValue. */

extern  DLLEXPORT(const char**) fieldedProp;

/***************************************************
 * The methods below are implemented in vcc.c (generated from vcc.y )
 ***************************************************/

/* NOTE regarding printVObject and writeVObject

The functions below are not exported from the DLL because they
take a FILE* as a parameter, which cannot be passed across a DLL
interface (at least that is my experience). Instead you can use
their companion functions which take file names or pointers
to memory. However, if you are linking this code into
your build directly then you may find them a more convenient API
and you can go ahead and use them. If you try to use them with
the DLL LIB you will get a link error.
*/
extern DLLEXPORT(void) writeVObject(FILE *fp, VObject *o);



typedef void (*MimeErrorHandler)(char *);

extern DLLEXPORT(void) registerMimeErrorHandler(MimeErrorHandler);

extern DLLEXPORT(VObject*) Parse_MIME(const char *input, unsigned long len);
extern DLLEXPORT(VObject*) Parse_MIME_FromFileName(char* fname);


/* NOTE regarding Parse_MIME_FromFile
The function above, Parse_MIME_FromFile, comes in two flavors,
neither of which is exported from the DLL. Each version takes
a CFile or FILE* as a parameter, neither of which can be
passed across a DLL interface (at least that is my experience).
If you are linking this code into your build directly then
you may find them a more convenient API that the other flavors
that take a file name. If you use them with the DLL LIB you
will get a link error.
*/


#if INCLUDEMFC
extern DLLEXPORT(VObject*) Parse_MIME_FromFile(CFile *file);
#else
extern DLLEXPORT(VObject*) Parse_MIME_FromFile(FILE *file);
#endif

extern DLLEXPORT(const char *) vObjectTypeInfo(VObject *o);


#endif /* __VOBJECT_H__ */


