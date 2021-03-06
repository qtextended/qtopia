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

#ifndef lint
/*static char yysccsid[] = "from: @(#)yaccpar	1.9 (Berkeley) 02/21/93";*/
static char yyrcsid[] = "$Id: skeleton.c,v 1.4 1993/12/21 18:45:32 jtc Exp $";
#endif
#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define yyclearin (yychar=(-1))
#define yyerrok (yyerrflag=0)
#define YYRECOVERING (yyerrflag!=0)
#define YYPREFIX "yy"
#line 2 "vcc.y"

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
 * src: vcc.c
 * doc: Parser for vCard and vCalendar. Note that this code is
 * generated by a yacc parser generator. Generally it should not
 * be edited by hand. The real source is vcc.y. The #line directives
 * can be commented out here to make it easier to trace through
 * in a debugger. However, if a bug is found it should
 * be fixed in vcc.y and this file regenerated.
 */


/* debugging utilities */
#if __DEBUG
#define DBG_(x) printf x
#else
#define DBG_(x)
#endif

/****  External Functions  ****/

/* assign local name to parser variables and functions so that
   we can use more than one yacc based parser.
*/

#if 0
#define yyparse mime_parse
#define yylex mime_lex
#define yyerror mime_error
#define yychar mime_char
/* #define p_yyval p_mime_val */
#undef yyval
#define yyval mime_yyval
/* #define p_yylval p_mime_lval */
#undef yylval
#define yylval mime_yylval
#define yydebug mime_debug
#define yynerrs mime_nerrs
#define yyerrflag mime_errflag
#define yyss mime_ss
#define yyssp mime_ssp
#define yyvs mime_vs
#define yyvsp mime_vsp
#define yylhs mime_lhs
#define yylen mime_len
#define yydefred mime_defred
#define yydgoto mime_dgoto
#define yysindex mime_sindex
#define yyrindex mime_rindex
#define yygindex mime_gindex
#define yytable mime_table
#define yycheck mime_check
#define yyname mime_name
#define yyrule mime_rule
#ifdef YYPREFIX
#undef YYPREFIX
#endif
#define YYPREFIX "mime_"
#endif


#ifndef _NO_LINE_FOLDING
#define _SUPPORT_LINE_FOLDING 1
#endif

/* undef below if compile with MFC */
/* #define INCLUDEMFC 1 */

#if defined(WIN32) || defined(_WIN32)
#ifdef INCLUDEMFC
#include <afx.h>
#endif
#endif

#include <string.h>
#ifndef __MWERKS__
#include <stdlib.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/*#ifdef PALMTOPCENTER*/
/*#include <qpe/vobject_p.h>*/
/*#else*/
#include <vobject_p.h>
/*#endif*/

/****  Types, Constants  ****/

#define YYDEBUG		0	/* 1 to compile in some debugging code */
#define MAXTOKEN	256	/* maximum token (line) length */
#define YYSTACKSIZE 	100	/* ~unref ?*/
#define MAXLEVEL	10	/* max # of nested objects parseable */
				/* (includes outermost) */


/****  Global Variables  ****/
int QTOPIA_EXPORT q_DontDecodeBase64Photo = 0;
int mime_lineNum, mime_numErrors; /* yyerror() can use these */
static VObject* vObjList;
static VObject *curProp;
static VObject *curObj;
static VObject* ObjStack[MAXLEVEL];
static int ObjStackTop;


/* A helpful utility for the rest of the app. */
#if __CPLUSPLUS__
extern "C" {
#endif

    extern void yyerror(char *s);

#if __CPLUSPLUS__
    };
#endif

int yyparse();

enum LexMode {
	L_NORMAL,
	L_PARAMWORD,
	L_VCARD,
	L_VCAL,
	L_VEVENT,
	L_VTODO,
	L_VALUES,
	L_BASE64,
	L_QUOTED_PRINTABLE
	};

/****  Private Forward Declarations  ****/
static int pushVObject(const char *prop);
static VObject* popVObject();
static void lexPopMode(int top);
static int lexWithinMode(enum LexMode mode);
static void lexPushMode(enum LexMode mode);
static void enterProps(const char *s);
static void enterAttr(const char *s1, const char *s2);
static void enterValues(const char *value);
#define mime_error yyerror
void mime_error(char *s);
void mime_error_(char *s);

#line 191 "vcc.y"
typedef union {
    char *str;
    VObject *vobj;
    } YYSTYPE;
#line 202 "y.tab.c"
#define EQ 257
#define COLON 258
#define DOT 259
#define SEMICOLON 260
#define SPACE 261
#define HTAB 262
#define LINESEP 263
#define NEWLINE 264
#define BEGIN_VCARD 265
#define END_VCARD 266
#define BEGIN_VCAL 267
#define END_VCAL 268
#define BEGIN_VEVENT 269
#define END_VEVENT 270
#define BEGIN_VTODO 271
#define END_VTODO 272
#define ID 273
#define STRING 274
#define YYERRCODE 256
short yylhs[] = {                                        -1,
    0,    6,    6,    5,    5,    8,    3,    9,    3,    7,
    7,   13,   10,   10,   15,   11,   11,   14,   14,   16,
   17,   18,   17,    1,   19,   12,   12,    2,    2,   21,
    4,   22,    4,   20,   20,   23,   23,   23,   26,   24,
   27,   24,   28,   25,   29,   25,
};
short yylen[] = {                                         2,
    1,    2,    1,    1,    1,    0,    4,    0,    3,    2,
    1,    0,    5,    1,    0,    3,    1,    2,    1,    2,
    1,    0,    4,    1,    0,    4,    1,    1,    0,    0,
    4,    0,    3,    2,    1,    1,    1,    1,    0,    4,
    0,    3,    0,    4,    0,    3,
};
short yydefred[] = {                                      0,
    0,    0,    0,    4,    5,    3,    0,    0,    0,    0,
    0,    2,   14,   24,    0,    0,   11,    0,    9,    0,
    0,    0,    0,   35,   36,   37,   33,    0,    7,   10,
   12,    0,    0,    0,    0,   31,   34,    0,    0,   19,
    0,    0,   42,    0,   46,    0,   20,   18,   28,    0,
    0,   40,   44,   22,   25,   13,    0,    0,   23,   26,
};
short yydgoto[] = {                                       3,
   15,   50,    4,    5,    6,    7,   22,    8,    9,   17,
   18,   51,   41,   39,   28,   40,   47,   57,   58,   23,
   10,   11,   24,   25,   26,   32,   33,   34,   35,
};
short yysindex[] = {                                   -255,
    0,    0,    0,    0,    0,    0, -255, -215, -257, -234,
 -245,    0,    0,    0,    0, -242,    0, -210,    0,    0,
    0, -215, -253,    0,    0,    0,    0, -247,    0,    0,
    0, -215, -227, -215, -226,    0,    0, -221, -247,    0,
 -211, -237,    0, -218,    0, -204,    0,    0,    0, -198,
 -199,    0,    0,    0,    0,    0, -221, -211,    0,    0,
};
short yyrindex[] = {                                      0,
 -216, -239,    0,    0,    0,    0,   65,    0,    0,    0,
    0,    0,    0,    0, -213,    0,    0,    0,    0, -214,
 -212, -264,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0, -192,    0,
 -252,    0,    0,    0,    0, -209,    0,    0,    0, -196,
    0,    0,    0,    0,    0,    0,    0, -252,    0,    0,
};
short yygindex[] = {                                      0,
  -36,    0,    0,    0,   61,    0,   -7,    0,    0,  -16,
    0,   11,    0,    0,    0,   31,    0,    0,    0,    0,
    0,    0,   48,    0,    0,    0,    0,    0,    0,
};
#define YYTABLESIZE 71
short yytable[] = {                                      30,
   16,   46,   13,   38,   38,   30,   38,   29,   19,    1,
   29,    2,   38,   13,   36,   20,   30,   21,   13,   14,
   59,   13,   27,   29,   42,   30,   44,   30,   32,   30,
   14,   30,   52,   30,   20,   14,   21,   13,   14,    6,
   13,   39,   43,   43,   17,   45,   15,   31,   21,    8,
   21,   14,   54,   53,   14,   41,    6,   14,   39,   45,
   43,   55,   49,   56,    1,   16,   27,   12,   60,   48,
   37,
};
short yycheck[] = {                                      16,
    8,   38,  256,  268,  269,   22,  271,  260,  266,  265,
  263,  267,  260,  256,  268,  269,  256,  271,  256,  273,
   57,  256,  268,  266,   32,   42,   34,   44,  268,  269,
  273,  271,  270,  273,  269,  273,  271,  256,  273,  256,
  256,  256,  270,  256,  258,  272,  260,  258,  258,  266,
  260,  273,  257,  272,  273,  270,  273,  273,  273,  272,
  273,  260,  274,  263,    0,  258,  263,    7,   58,   39,
   23,
};
#define YYFINAL 3
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 274
#if YYDEBUG
char *yyname[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"EQ","COLON","DOT","SEMICOLON",
"SPACE","HTAB","LINESEP","NEWLINE","BEGIN_VCARD","END_VCARD","BEGIN_VCAL",
"END_VCAL","BEGIN_VEVENT","END_VEVENT","BEGIN_VTODO","END_VTODO","ID","STRING",
};
char *yyrule[] = {
"$accept : mime",
"mime : vobjects",
"vobjects : vobjects vobject",
"vobjects : vobject",
"vobject : vcard",
"vobject : vcal",
"$$1 :",
"vcard : BEGIN_VCARD $$1 items END_VCARD",
"$$2 :",
"vcard : BEGIN_VCARD $$2 END_VCARD",
"items : items item",
"items : item",
"$$3 :",
"item : prop COLON $$3 values LINESEP",
"item : error",
"$$4 :",
"prop : name $$4 attr_params",
"prop : name",
"attr_params : attr_params attr_param",
"attr_params : attr_param",
"attr_param : SEMICOLON attr",
"attr : name",
"$$5 :",
"attr : name EQ $$5 name",
"name : ID",
"$$6 :",
"values : value SEMICOLON $$6 values",
"values : value",
"value : STRING",
"value :",
"$$7 :",
"vcal : BEGIN_VCAL $$7 calitems END_VCAL",
"$$8 :",
"vcal : BEGIN_VCAL $$8 END_VCAL",
"calitems : calitems calitem",
"calitems : calitem",
"calitem : eventitem",
"calitem : todoitem",
"calitem : items",
"$$9 :",
"eventitem : BEGIN_VEVENT $$9 items END_VEVENT",
"$$10 :",
"eventitem : BEGIN_VEVENT $$10 END_VEVENT",
"$$11 :",
"todoitem : BEGIN_VTODO $$11 items END_VTODO",
"$$12 :",
"todoitem : BEGIN_VTODO $$12 END_VTODO",
};
#endif
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 500
#define YYMAXDEPTH 500
#endif
#endif
int yydebug;
int yynerrs;
int yyerrflag;
int yychar;
short *yyssp;
YYSTYPE *yyvsp;
YYSTYPE yyval;
YYSTYPE yylval;
short yyss[YYSTACKSIZE];
YYSTYPE yyvs[YYSTACKSIZE];
#define yystacksize YYSTACKSIZE
#line 389 "vcc.y"
/*------------------------------------*/
static int pushVObject(const char *prop)
    {
    VObject *newObj;
    if (ObjStackTop == MAXLEVEL)
	return FALSE;

    ObjStack[++ObjStackTop] = curObj;

    if (curObj) {
        newObj = addProp(curObj,prop);
        curObj = newObj;
	}
    else
	curObj = newVObject(prop);

    return TRUE;
    }


/*---------------------------------------*/
/* This pops the recently built vCard off the stack and returns it. */
static VObject* popVObject()
    {
    VObject *oldObj;
    if (ObjStackTop < 0) {
	yyerror("pop on empty Object Stack\n");
	return 0;
	}
    oldObj = curObj;
    curObj = ObjStack[ObjStackTop--];

    return oldObj;
    }


static void enterValues(const char *value)
    {
    if (fieldedProp && *fieldedProp) {
	if (value) {
	    addPropValue(curProp,*fieldedProp,value);
	    }
	/* else this field is empty, advance to next field */
	fieldedProp++;
	}
    else {
	if (value) {
	    setVObjectStringZValue_(curProp,strdup( value ));
	    }
	}
    deleteStr(value);
    }

static void enterProps(const char *s)
    {
    curProp = addGroup(curObj,s);
    deleteStr(s);
    }

static void enterAttr(const char *s1, const char *s2)
    {
    const char *p1, *p2=0;
    p1 = lookupProp_(s1);
    if (s2) {
	VObject *a;
	p2 = lookupProp_(s2);
	a = addProp(curProp,p1);
	setVObjectStringZValue(a,p2);
	}
    else
	addProp(curProp,p1);
    /* Lookup strings so we can quickly use pointer comparison */
    static const char* base64 = lookupProp_(VCBase64Prop);
    static const char* qp = lookupProp_(VCQuotedPrintableProp);
    static const char* photo = lookupProp_(VCPhotoProp);
    static const char* encoding = lookupProp_(VCEncodingProp);
    if ((!q_DontDecodeBase64Photo || vObjectName(curProp) != photo)
	&& (p1 == base64 || p1 == encoding && p2 == base64))
	lexPushMode(L_BASE64);
    else if (p1 == qp || p1 == encoding && p2 == qp)
	lexPushMode(L_QUOTED_PRINTABLE);
    deleteStr(s1); deleteStr(s2);
    }


#define MAX_LEX_LOOKAHEAD_0 32
#define MAX_LEX_LOOKAHEAD 64
#define MAX_LEX_MODE_STACK_SIZE 10
#define LEXMODE() (lexBuf.lexModeStack[lexBuf.lexModeStackTop])

struct LexBuf {
	/* input */
#ifdef INCLUDEMFC
    CFile *inputFile;
#else
    FILE *inputFile;
#endif
    char *inputString;
    unsigned long curPos;
    unsigned long inputLen;
	/* lookahead buffer */
	/*   -- lookahead buffer is short instead of char so that EOF
	 /      can be represented correctly.
	*/
    unsigned long len;
    short buf[MAX_LEX_LOOKAHEAD];
    unsigned long getPtr;
	/* context stack */
    unsigned long lexModeStackTop;
    enum LexMode lexModeStack[MAX_LEX_MODE_STACK_SIZE];
	/* token buffer */
    unsigned long maxToken;
    char *strs;
    unsigned long strsLen;
    } lexBuf;

static void lexPushMode(enum LexMode mode)
    {
    if (lexBuf.lexModeStackTop == (MAX_LEX_MODE_STACK_SIZE-1))
	yyerror("lexical context stack overflow");
    else {
	lexBuf.lexModeStack[++lexBuf.lexModeStackTop] = mode;
	}
    }

static void lexPopMode(int top)
    {
    /* special case of pop for ease of error recovery -- this
	version will never underflow */
    if (top)
	lexBuf.lexModeStackTop = 0;
    else
	if (lexBuf.lexModeStackTop > 0) lexBuf.lexModeStackTop--;
    }

static int lexWithinMode(enum LexMode mode) {
    unsigned long i;
    for (i=0;i<lexBuf.lexModeStackTop;i++)
	if (mode == lexBuf.lexModeStack[i]) return 1;
    return 0;
    }

static int lexGetc_()
    {
    /* get next char from input, no buffering. */
    if (lexBuf.curPos == lexBuf.inputLen)
	return EOF;
    else if (lexBuf.inputString)
	return *(lexBuf.inputString + lexBuf.curPos++);
    else {
#ifdef INCLUDEMFC
	char result;
	return lexBuf.inputFile->Read(&result, 1) == 1 ? result : EOF;
#else
	return fgetc(lexBuf.inputFile);
#endif
	}
    }

static int lexGeta()
    {
    ++lexBuf.len;
    return (lexBuf.buf[lexBuf.getPtr] = lexGetc_());
    }

static int lexGeta_(int i)
    {
    ++lexBuf.len;
    return (lexBuf.buf[(lexBuf.getPtr+i)%MAX_LEX_LOOKAHEAD] = lexGetc_());
    }

static void lexSkipLookahead() {
    if (lexBuf.len > 0 && lexBuf.buf[lexBuf.getPtr]!=EOF) {
	/* don't skip EOF. */
        lexBuf.getPtr = (lexBuf.getPtr + 1) % MAX_LEX_LOOKAHEAD;
	lexBuf.len--;
        }
    }

static int lexLookahead() {
    int c = (lexBuf.len)?
	lexBuf.buf[lexBuf.getPtr]:
	lexGeta();
    /* do the \r\n -> \n or \r -> \n translation here */
    if (c == '\r') {
	int a = (lexBuf.len>1)?
	    lexBuf.buf[(lexBuf.getPtr+1)%MAX_LEX_LOOKAHEAD]:
	    lexGeta_(1);
	if (a == '\n') {
	    lexSkipLookahead();
	    }
	lexBuf.buf[lexBuf.getPtr] = c = '\n';
	}
    else if (c == '\n') {
	int a = (lexBuf.len>1)?
	    lexBuf.buf[lexBuf.getPtr+1]:
	    lexGeta_(1);
	if (a == '\r') {
	    lexSkipLookahead();
	    }
	lexBuf.buf[lexBuf.getPtr] = '\n';
	}
    return c;
    }

static int lexGetc() {
    int c = lexLookahead();
    if (lexBuf.len > 0 && lexBuf.buf[lexBuf.getPtr]!=EOF) {
	/* EOF will remain in lookahead buffer */
        lexBuf.getPtr = (lexBuf.getPtr + 1) % MAX_LEX_LOOKAHEAD;
	lexBuf.len--;
        }
    return c;
    }

static void lexSkipLookaheadWord() {
    if (lexBuf.strsLen <= lexBuf.len) {
	lexBuf.len -= lexBuf.strsLen;
	lexBuf.getPtr = (lexBuf.getPtr + lexBuf.strsLen) % MAX_LEX_LOOKAHEAD;
	}
    }

static void lexClearToken()
    {
    lexBuf.strsLen = 0;
    }

static void lexAppendc(int c)
    {
    lexBuf.strs[lexBuf.strsLen] = c;
    /* append up to zero termination */
    if (c == 0) return;
    lexBuf.strsLen++;
    if (lexBuf.strsLen > lexBuf.maxToken) {
	/* double the token string size */
	lexBuf.maxToken <<= 1;
	lexBuf.strs = (char*) realloc(lexBuf.strs,(size_t)lexBuf.maxToken);
	}
    }

static char* lexStr() {
    return dupStr(lexBuf.strs,(size_t)lexBuf.strsLen+1);
    }

static void lexSkipWhite() {
    int c = lexLookahead();
    while (c == ' ' || c == '\t') {
	lexSkipLookahead();
	c = lexLookahead();
	}
    }

static char* lexGetWord() {
    int c;
    lexSkipWhite();
    lexClearToken();
    c = lexLookahead();
    while (c != EOF && !strchr("\t\n ;:=",c)) {
	lexAppendc(c);
	lexSkipLookahead();
	c = lexLookahead();
	}
    lexAppendc(0);
    return lexStr();
    }

static char* lexGetParamWord()
{
    int c;
    lexClearToken();
    c = lexLookahead();
    while (c >= ' ' && c < 127 && !strchr("[]:=,;",c)) {
	lexAppendc(c);
	lexSkipLookahead();
	c = lexLookahead();
    }
    lexAppendc(0);
    return lexStr();
}

static void lexPushLookaheadc(int c) {
    int putptr;
    /* can't putback EOF, because it never leaves lookahead buffer */
    if (c == EOF) return;
    putptr = (int)lexBuf.getPtr - 1;
    if (putptr < 0) putptr += MAX_LEX_LOOKAHEAD;
    lexBuf.getPtr = putptr;
    lexBuf.buf[putptr] = c;
    lexBuf.len += 1;
    }

static char* lexLookaheadWord() {
    /* this function can lookahead word with max size of MAX_LEX_LOOKAHEAD_0
     /  and thing bigger than that will stop the lookahead and return 0;
     / leading white spaces are not recoverable.
     */
    int c;
    int len = 0;
    int curgetptr = 0;
    lexSkipWhite();
    lexClearToken();
    curgetptr = (int)lexBuf.getPtr;	// remember!
    while (len < (MAX_LEX_LOOKAHEAD_0)) {
	c = lexGetc();
	len++;
	if (c == EOF || strchr("\t\n ;:=", c)) {
	    lexAppendc(0);
	    /* restore lookahead buf. */
	    lexBuf.len += len;
	    lexBuf.getPtr = curgetptr;
	    return lexStr();
	    }
        else
	    lexAppendc(c);
	}
    lexBuf.len += len;	/* char that has been moved to lookahead buffer */
    lexBuf.getPtr = curgetptr;
    return 0;
    }

#ifdef _SUPPORT_LINE_FOLDING
static void handleMoreRFC822LineBreak(int c) {
    /* suport RFC 822 line break in cases like
     *	ADR: foo;
     *    morefoo;
     *    more foo;
     */
    if (c == ';') {
	int a;
	lexSkipLookahead();
	/* skip white spaces */
	a = lexLookahead();
	while (a == ' ' || a == '\t') {
	    lexSkipLookahead();
	    a = lexLookahead();
	    }
	if (a == '\n') {
	    lexSkipLookahead();
	    a = lexLookahead();
	    if (a == ' ' || a == '\t') {
		/* continuation, throw away all the \n and spaces read so
		 * far
		 */
		lexSkipWhite();
		lexPushLookaheadc(';');
		}
	    else {
		lexPushLookaheadc('\n');
		lexPushLookaheadc(';');
		}
	    }
	else {
	    lexPushLookaheadc(';');
	    }
	}
    }

static char* lexGet1Value() {
    int c;
    lexSkipWhite();
    c = lexLookahead();
    lexClearToken();
    while (c != EOF && (c != ';' || !fieldedProp)) {
	if (c == '\\' ) {
	    int a;
	    lexSkipLookahead();
	    a = lexLookahead();
	    if ( a == ';' ) {
		lexAppendc( ';' );
		lexSkipLookahead();
	    } else if ( a == '\n' ) {
		lexAppendc( '\n' );
		lexSkipLookahead();
	    } else if ( a == '\\' ) {
		lexAppendc( '\\' );
		lexSkipLookahead();
	    } else {
		lexAppendc('\\');
	    }
	} else if (c == '\n') {
	    int a;
	    lexSkipLookahead();
	    a  = lexLookahead();
	    if (a == ' ' || a == '\t') {
		lexAppendc(' ');
		lexSkipLookahead();
		}
	    else {
		lexPushLookaheadc('\n');
		break;
		}
	    }
	else {
	    lexAppendc(c);
	    lexSkipLookahead();
	    }
	c = lexLookahead();
	}
    lexAppendc(0);
    handleMoreRFC822LineBreak(c);
    return c==EOF?0:lexStr();
    }
#endif

static int match_begin_name(int end) {
    char *n = lexLookaheadWord();
    int token = ID;
    if (n) {
	if (!qstricmp(n,"vcard")) token = end?END_VCARD:BEGIN_VCARD;
	else if (!qstricmp(n,"vcalendar")) token = end?END_VCAL:BEGIN_VCAL;
	else if (!qstricmp(n,"vevent")) token = end?END_VEVENT:BEGIN_VEVENT;
	else if (!qstricmp(n,"vtodo")) token = end?END_VTODO:BEGIN_VTODO;
	deleteStr(n);
	return token;
	}
    return 0;
    }


#ifdef INCLUDEMFC
void initLex(const char *inputstring, unsigned long inputlen, CFile *inputfile)
#else
void initLex(const char *inputstring, unsigned long inputlen, FILE *inputfile)
#endif
    {
    // initialize lex mode stack
    lexBuf.lexModeStack[lexBuf.lexModeStackTop=0] = L_NORMAL;

    // iniatialize lex buffer.
    lexBuf.inputString = (char*) inputstring;
    lexBuf.inputLen = inputlen;
    lexBuf.curPos = 0;
    lexBuf.inputFile = inputfile;

    lexBuf.len = 0;
    lexBuf.getPtr = 0;

    lexBuf.maxToken = MAXTOKEN;
    lexBuf.strs = (char*)malloc(MAXTOKEN);
    lexBuf.strsLen = 0;

    }

static void finiLex() {
    free(lexBuf.strs);
    }


/*-----------------------------------*/
/* This parses and converts the base64 format for binary encoding into
 * a decoded buffer (allocated with new).  See RFC 1521.
 */
static int lexGetDataFromBase64()
    {
    unsigned long bytesLen = 0, bytesMax = 0;
    int quadIx = 0, pad = 0;
    unsigned long trip = 0;
    unsigned char b;
    int c;
    unsigned char *bytes = NULL;
    unsigned char *oldBytes = NULL;

    DBG_(("db: lexGetDataFromBase64\n"));
    while (1) {
	c = lexGetc();
	lexSkipWhite();
	if (c == '\n') {
	    ++mime_lineNum;
	    if (lexLookahead() == '\n') {
		/* a '\n' character by itself means end of data */
		break;
		}
	    else continue; /* ignore '\n' */
	    }
	else {
	    if ((c >= 'A') && (c <= 'Z'))
		b = (unsigned char)(c - 'A');
	    else if ((c >= 'a') && (c <= 'z'))
		b = (unsigned char)(c - 'a') + 26;
	    else if ((c >= '0') && (c <= '9'))
		b = (unsigned char)(c - '0') + 52;
	    else if (c == '+')
		b = 62;
	    else if (c == '/')
		b = 63;
	    else if (c == '=') {
		b = 0;
		pad++;
	    } else { /* error condition */
		if (bytes) free(bytes);
		else if (oldBytes) free(oldBytes);
		// error recovery: skip until 2 adjacent newlines.
		DBG_(("db: invalid character 0x%x '%c'\n", c,c));
		if (c != EOF)  {
		    c = lexGetc();
		    while (c != EOF) {
			if (c == '\n') {
			    lexSkipWhite();
			    if(lexLookahead() == '\n') {
				++mime_lineNum;
				break;
				}
			    }
			c = lexGetc();
			}
		    }
		return c != EOF;
		}
	    trip = (trip << 6) | b;
	    if (++quadIx == 4) {
		unsigned char outBytes[3];
		int numOut;
		int i;
		for (i = 0; i < 3; i++) {
		    outBytes[2-i] = (unsigned char)(trip & 0xFF);
		    trip >>= 8;
		    }
		numOut = 3 - pad;
		if (bytesLen + numOut > bytesMax) {
		    if (!bytes) {
			bytesMax = 1024;
			bytes = (unsigned char*)malloc((size_t)bytesMax);
			}
		    else {
			bytesMax <<= 2;
			oldBytes = bytes;
			bytes = (unsigned char*)realloc(bytes,(size_t)bytesMax);
			}
		    if (bytes == 0) {
			mime_error("out of memory while processing BASE64 data\n");
			}
		    }
		if (bytes) {
		    memcpy(bytes + bytesLen, outBytes, numOut);
		    bytesLen += numOut;
		    }
		trip = 0;
		quadIx = 0;
		}
	    }
	} /* while */
    DBG_(("db: bytesLen = %d\n",  bytesLen));
    /* kludge: all this won't be necessary if we have tree form
	representation */
    if (bytes) {
	setValueWithSize(curProp,bytes,(unsigned int)bytesLen);
	free(bytes);
	}
    else if (oldBytes) {
	setValueWithSize(curProp,oldBytes,(unsigned int)bytesLen);
	free(oldBytes);
	}
    return bytesLen;
    }

static int match_begin_end_name(int end) {
    int token;
    lexSkipWhite();
    if (lexLookahead() != ':') return ID;
    lexSkipLookahead();
    lexSkipWhite();
    token = match_begin_name(end);
    if (token == ID) {
	lexPushLookaheadc(':');
	DBG_(("db: ID '%s'\n", yylval.str));
	return ID;
	}
    else if (token != 0) {
	lexSkipLookaheadWord();
	deleteStr(yylval.str);
	DBG_(("db: begin/end %d\n", token));
	return token;
	}
    return 0;
    }

static char* lexGetQuotedPrintable()
{
    int c;
    lexSkipWhite();
    c = lexLookahead();
    lexClearToken();

    while (c != EOF && (c != ';' || !fieldedProp)) {
	if (c == '\n') {
	    // break, leave '\n' on remaining chars.
	    break;
	} else if (c == '=') {
	    int cur = 0;
	    int next;

	    lexSkipLookahead(); // skip '='
	    next = lexLookahead();

	    if (next == '\n') {
		// skip and only skip the \n
		lexSkipLookahead(); 
		c = lexLookahead();
		++mime_lineNum; // aid in error reporting
		continue;
	    } else if (next >= '0' && next <= '9') {
		cur = next - '0';
	    } else if (next >= 'A' && next <= 'F') {
		cur = next - 'A' + 10;
	    } else {
		// we have been sent buggy stuff. doesn't matter
		// what we do so long as we keep going.
		// should probably spit an error here
		lexSkipLookahead(); 
		c = lexLookahead();
		continue;
	    }

	    lexSkipLookahead(); // skip A-Z0-9
	    next = lexLookahead();

	    cur = cur * 16;
	    // this time really just expecting 0-9A-F
	    if (next >= '0' && next <= '9') {
		cur += next - '0';
	    } else if (next >= 'A' && next <= 'F') {
		cur += next - 'A' + 10;
	    } else {
		// we have been sent buggy stuff. doesn't matter
		// what we do so long as we keep going.
		// should probably spit an error here
		lexSkipLookahead(); 
		c = lexLookahead();
		continue;
	    }

	    // got a valid escaped =.  append it.
	    lexSkipLookahead(); // skip second 0-9A-F
	    lexAppendc(cur);
	} else {
	    lexSkipLookahead(); // skip whatever we just read.
	    lexAppendc(c); // and append it.
	}
	c = lexLookahead();
    }
    lexAppendc(0);
    return c==EOF?0:lexStr();
}

static int yylex() {

    int lexmode = LEXMODE();
    if (lexmode == L_VALUES) {
	int c = lexGetc();
	int c2;
	if (c == ';' && fieldedProp) {
	    DBG_(("db: SEMICOLON\n"));
	    lexPushLookaheadc(c);
	    handleMoreRFC822LineBreak(c);
	    lexSkipLookahead();
	    return SEMICOLON;
	    }
	else if (strchr("\n",c) && (c2 = lexLookahead()) != ' ' && c2 != '\t') {
	    ++mime_lineNum;
	    /* consume all line separator(s) adjacent to each other */
	    while (strchr("\n",c2)) {
		lexSkipLookahead();
		c2 = lexLookahead();
		++mime_lineNum;
		}
	    DBG_(("db: LINESEP\n"));
	    return LINESEP;
	    }
	else {
	    char *p = 0;
	    lexPushLookaheadc(c);
	    if (lexWithinMode(L_BASE64)) {
		/* get each char and convert to bin on the fly... */
		yylval.str = NULL;
		return lexGetDataFromBase64() ? STRING : 0;
		}
	    else if (lexWithinMode(L_QUOTED_PRINTABLE)) {
		p = lexGetQuotedPrintable();
		}
	    else {
#ifdef _SUPPORT_LINE_FOLDING
		p = lexGet1Value();
#else
		p = lexGetStrUntil(";\n");
#endif
		}
	    if (p) {
		DBG_(("db: STRING: '%s'\n", p));
		yylval.str = p;
		return STRING;
		}
	    else return 0;
	    }
    } else if (lexmode == L_PARAMWORD) {
	yylval.str = lexGetParamWord();
	return ID;
    } else {
	/* normal mode */
	while (1) {
	    int c = lexGetc();
	    switch(c) {
		case ':': {
		    /* consume all line separator(s) adjacent to each other */
		    /* ignoring linesep immediately after colon. */
		    /* I don't see this in the spec, and it breaks null values -- WA
		    c = lexLookahead();
		    while (strchr("\n",c)) {
			lexSkipLookahead();
			c = lexLookahead();
			++mime_lineNum;
			}
		    */
		    DBG_(("db: COLON\n"));
		    return COLON;
		    }
		case ';':
		    DBG_(("db: SEMICOLON\n"));
		    return SEMICOLON;
		case '=':
		    DBG_(("db: EQ\n"));
		    return EQ;
		/* ignore whitespace in this mode */
		case '\t':
		case ' ': continue;
		case '\n': {
		    ++mime_lineNum;
		    continue;
		    }
		case EOF: return 0;
		    break;
		default: {
		    lexPushLookaheadc(c);
		    if (isalnum(c)) {
			char *t = lexGetWord();
			yylval.str = t;
			if (!qstricmp(t, "begin")) {
			    return match_begin_end_name(0);
			    }
			else if (!qstricmp(t,"end")) {
			    return match_begin_end_name(1);
			    }
		        else {
			    DBG_(("db: ID '%s'\n", t));
			    return ID;
			    }
			}
		    else {
			/* unknow token */
			return 0;
			}
		    break;
		    }
		}
	    }
	}
    return 0;
    }


/***************************************************************************/
/***							Public Functions						****/
/***************************************************************************/

static VObject* Parse_MIMEHelper()
    {
    ObjStackTop = -1;
    mime_numErrors = 0;
    mime_lineNum = 1;
    vObjList = 0;
    curObj = 0;

    if (yyparse() != 0)
	return 0;

    finiLex();
    return vObjList;
    }

/*--------------------------------------------*/
DLLEXPORT(VObject*) Parse_MIME(const char *input, unsigned long len)
    {
    initLex(input, len, 0);
    return Parse_MIMEHelper();
    }


#if INCLUDEMFC

DLLEXPORT(VObject*) Parse_MIME_FromFile(CFile *file)
    {
    unsigned long startPos;
    VObject *result;	

    initLex(0,-1,file);
    startPos = file->GetPosition();
    if (!(result = Parse_MIMEHelper()))
	file->Seek(startPos, CFile::begin);
    return result;
    }

#else

VObject* Parse_MIME_FromFile(FILE *file)
    {
    VObject *result;	
    long startPos;

    initLex(0,(unsigned long)-1,file);
    startPos = ftell(file);
    if (!(result = Parse_MIMEHelper())) {
	fseek(file,startPos,SEEK_SET);
	}
    return result;
    }

DLLEXPORT(VObject*) Parse_MIME_FromFileName(char *fname)
    {
    FILE *fp = fopen(fname,"r");
    if (fp) {
	VObject* o = Parse_MIME_FromFile(fp);
	fclose(fp);
	return o;
	}
    else {
	char msg[80];
	sprintf(msg, "can't open file '%s' for reading\n", fname);
	mime_error_(msg);
	return 0;
	}
    }

#endif

/*-------------------------------------*/

static MimeErrorHandler mimeErrorHandler;

DLLEXPORT(void) registerMimeErrorHandler(MimeErrorHandler me)
    {
    mimeErrorHandler = me;
    }

void mime_error(char *s)
    {
    char msg[256];
    if (mimeErrorHandler) {
	sprintf(msg,"%s at line %d", s, mime_lineNum);
	mimeErrorHandler(msg);
	}
    }

void mime_error_(char *s)
    {
    if (mimeErrorHandler) {
	mimeErrorHandler(s);
	}
    }

#line 1239 "y.tab.c"
#define YYABORT goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR goto yyerrlab
int
#if defined(__STDC__)
yyparse(void)
#else
yyparse()
#endif
{
    register int yym, yyn, yystate;
#if YYDEBUG
    register char *yys;
    extern char *getenv();

    if (yys = getenv("YYDEBUG"))
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif

    yynerrs = 0;
    yyerrflag = 0;
    yychar = (-1);

    yyssp = yyss;
    yyvsp = yyvs;
    *yyssp = yystate = 0;

yyloop:
    if ((yyn = yydefred[yystate]) != 0) goto yyreduce;
    if (yychar < 0)
    {
        if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
    }
    if ((yyn = yysindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    YYPREFIX, yystate, yytable[yyn]);
#endif
        if (yyssp >= yyss + yystacksize - 1)
        {
            goto yyoverflow;
        }
        *++yyssp = yystate = yytable[yyn];
        *++yyvsp = yylval;
        yychar = (-1);
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if ((yyn = yyrindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag) goto yyinrecovery;
    yyerror("syntax error");
#ifdef lint
    goto yyerrlab;
#endif
yyerrlab:
    ++yynerrs;
yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if ((yyn = yysindex[*yyssp]) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *yyssp, yytable[yyn]);
#endif
                if (yyssp >= yyss + yystacksize - 1)
                {
                    goto yyoverflow;
                }
                *++yyssp = yystate = yytable[yyn];
                *++yyvsp = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: error recovery discarding state %d\n",
                            YYPREFIX, *yyssp);
#endif
                if (yyssp <= yyss) goto yyabort;
                --yyssp;
                --yyvsp;
            }
        }
    }
    else
    {
        if (yychar == 0) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
        yychar = (-1);
        goto yyloop;
    }
yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                YYPREFIX, yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    yyval = yyvsp[1-yym];
    switch (yyn)
    {
case 2:
#line 223 "vcc.y"
{ addList(&vObjList, yyvsp[0].vobj); curObj = 0; }
break;
case 3:
#line 225 "vcc.y"
{ addList(&vObjList, yyvsp[0].vobj); curObj = 0; }
break;
case 6:
#line 234 "vcc.y"
{
	lexPushMode(L_VCARD);
	if (!pushVObject(VCCardProp)) YYERROR;
	}
break;
case 7:
#line 239 "vcc.y"
{
	lexPopMode(0);
	yyval.vobj = popVObject();
	}
break;
case 8:
#line 244 "vcc.y"
{
	lexPushMode(L_VCARD);
	if (!pushVObject(VCCardProp)) YYERROR;
	}
break;
case 9:
#line 249 "vcc.y"
{
	lexPopMode(0);
	yyval.vobj = popVObject();
	}
break;
case 12:
#line 260 "vcc.y"
{
	lexPushMode(L_VALUES);
	}
break;
case 13:
#line 264 "vcc.y"
{
	if (lexWithinMode(L_BASE64) || lexWithinMode(L_QUOTED_PRINTABLE))
	   lexPopMode(0);
	lexPopMode(0);
	}
break;
case 15:
#line 273 "vcc.y"
{
	enterProps(yyvsp[0].str);
	}
break;
case 17:
#line 278 "vcc.y"
{
	enterProps(yyvsp[0].str);
	}
break;
case 21:
#line 291 "vcc.y"
{
	enterAttr(yyvsp[0].str,0);
	}
break;
case 22:
#line 295 "vcc.y"
{
	lexPushMode(L_PARAMWORD);
	}
break;
case 23:
#line 299 "vcc.y"
{
	lexPopMode(0);
	enterAttr(yyvsp[-3].str,yyvsp[0].str);
	}
break;
case 25:
#line 308 "vcc.y"
{ enterValues(yyvsp[-1].str); }
break;
case 27:
#line 310 "vcc.y"
{ enterValues(yyvsp[0].str); }
break;
case 29:
#line 315 "vcc.y"
{ yyval.str = 0; }
break;
case 30:
#line 320 "vcc.y"
{ if (!pushVObject(VCCalProp)) YYERROR; }
break;
case 31:
#line 323 "vcc.y"
{ yyval.vobj = popVObject(); }
break;
case 32:
#line 325 "vcc.y"
{ if (!pushVObject(VCCalProp)) YYERROR; }
break;
case 33:
#line 327 "vcc.y"
{ yyval.vobj = popVObject(); }
break;
case 39:
#line 342 "vcc.y"
{
	lexPushMode(L_VEVENT);
	if (!pushVObject(VCEventProp)) YYERROR;
	}
break;
case 40:
#line 348 "vcc.y"
{
	lexPopMode(0);
	popVObject();
	}
break;
case 41:
#line 353 "vcc.y"
{
	lexPushMode(L_VEVENT);
	if (!pushVObject(VCEventProp)) YYERROR;
	}
break;
case 42:
#line 358 "vcc.y"
{
	lexPopMode(0);
	popVObject();
	}
break;
case 43:
#line 366 "vcc.y"
{
	lexPushMode(L_VTODO);
	if (!pushVObject(VCTodoProp)) YYERROR;
	}
break;
case 44:
#line 372 "vcc.y"
{
	lexPopMode(0);
	popVObject();
	}
break;
case 45:
#line 377 "vcc.y"
{
	lexPushMode(L_VTODO);
	if (!pushVObject(VCTodoProp)) YYERROR;
	}
break;
case 46:
#line 382 "vcc.y"
{
	lexPopMode(0);
	popVObject();
	}
break;
#line 1545 "y.tab.c"
    }
    yyssp -= yym;
    yystate = *yyssp;
    yyvsp -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
#endif
        yystate = YYFINAL;
        *++yyssp = YYFINAL;
        *++yyvsp = yyval;
        if (yychar < 0)
        {
            if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
            if (yydebug)
            {
                yys = 0;
                if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
                if (!yys) yys = "illegal-symbol";
                printf("%sdebug: state %d, reading %d (%s)\n",
                        YYPREFIX, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == 0) goto yyaccept;
        goto yyloop;
    }
    if ((yyn = yygindex[yym]) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *yyssp, yystate);
#endif
    if (yyssp >= yyss + yystacksize - 1)
    {
        goto yyoverflow;
    }
    *++yyssp = yystate;
    *++yyvsp = yyval;
    goto yyloop;
yyoverflow:
    yyerror("yacc stack overflow");
yyabort:
    return (1);
yyaccept:
    return (0);
}
