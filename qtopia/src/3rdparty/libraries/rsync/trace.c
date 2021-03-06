/*= -*- c-basic-offset: 4; indent-tabs-mode: nil; -*-
 *
 * librsync -- library for network deltas
 * $Id: trace.c,v 1.2 2001/09/16 17:30:42 lknoll Exp $
 *
 * Copyright (C) 2000, 2001 by Martin Pool <mbp@samba.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

                                     /*
                                      | Finality is death.
                                      | Perfection is finality.
                                      | Nothing is perfect.
                                      | There are lumps in it.
                                      */



/*
 * TODO: Have a bit set in the log level that says not to include the
 * function name.
 */

#include <config_rsync.h>

#include <stdio.h>
#if !defined (_OS_WIN32_)
#include <unistd.h>
#include <sys/file.h>
#else
#define vsnprintf _vsnprintf
#define snprintf _snprintf
#define write _write
#include <io.h>
#endif
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>

#include "rsync.h"
#include "util.h"
#include "trace.h"


rs_trace_fn_t  *rs_trace_impl = rs_trace_stderr;

int rs_trace_level = RS_LOG_INFO;

#ifdef HAVE_PROGRAM_INVOCATION_NAME
#  define MY_NAME program_invocation_short_name
#else
#  define MY_NAME "librsync"
#endif

static void rs_log_va(int level, char const *fn, char const *fmt, va_list va);

#if SIZEOF_SIZE_T > SIZEOF_LONG
#  warning size_t is larger than a long integer, values in trace messages may be wrong
#endif


/**
 * Log severity strings, if any.  Must match ordering in
 * ::rs_loglevel.
 */
static const char *rs_severities[] = {
    "EMERGENCY! ", "ALERT! ", "CRITICAL! ", "ERROR: ", "Warning: ",
    "", "", ""
};



/**
 * \brief Set the destination of trace information.
 *
 * The callback scheme allows for use within applications that may
 * have their own particular ways of reporting errors: log files for a
 * web server, perhaps, and an error dialog for a browser.
 *
 * \todo Do we really need such fine-grained control, or just yes/no
 * tracing?
 */
void
rs_trace_to(rs_trace_fn_t * new_impl)
{
    rs_trace_impl = new_impl;
}


/** 
 * Set the least important message severity that will be output.
 */
void
rs_trace_set_level(rs_loglevel level)
{
    rs_trace_level = level;
}


static void
rs_log_va(int flags, char const *fn, char const *fmt, va_list va)
{
    int level = flags & RS_LOG_PRIMASK;
    
    if (rs_trace_impl && level <= rs_trace_level) {
        char            buf[1000];
        char            full_buf[1000];

        vsnprintf(buf, sizeof buf - 1, fmt, va);
        //### revise 
        printf("rs_log_va inconplete %s:%d", __FILE__, __LINE__);
        if (flags & RS_LOG_NONAME) { 
            snprintf(full_buf, sizeof full_buf - 1,
                     "%s: %s%s\n",
                     MY_NAME, rs_severities[level], buf);
        } else { 
            snprintf(full_buf, sizeof full_buf - 1,
                     "%s: %s(%s) %s\n",
                     MY_NAME, rs_severities[level], fn, buf);
        } 

	rs_trace_impl(level, full_buf);
    }
}



/**
 * Called by a macro, used on platforms where we can't determine the
 * calling function name.
 */
void
rs_log0_nofn(int level, char const *fmt, ...)
{
    va_list         va;

    va_start(va, fmt);
    rs_log_va(level, PACKAGE, fmt, va);
    va_end(va);
}


/* Called by a macro that prepends the calling function name,
 * etc.  */
void
rs_log0(int level, char const *fn, char const *fmt, ...)
{
    va_list         va;

    va_start(va, fmt);
    rs_log_va(level, fn, fmt, va);
    va_end(va);
}


void
rs_trace_stderr(int UNUSED(level), char const *msg)
{
#if !defined (_OS_WIN32_)    
    /* NOTE NO TRAILING NUL */
    write(STDERR_FILENO, msg, strlen(msg));
#else
    write(fileno(stderr), msg, strlen(msg));    
#endif
}


/* This is called directly if the machine doesn't allow varargs
 * macros. */
void
rs_fatal0(char const *s, ...) 
{
    va_list	va;

    va_start(va, s);
    rs_log_va(RS_LOG_CRIT, PACKAGE, s, va);
    va_end(va);
}


/* This is called directly if the machine doesn't allow varargs
 * macros. */
void
rs_error0(char const *s, ...) 
{
    va_list	va;

    va_start(va, s);
    rs_log_va(RS_LOG_ERR, PACKAGE, s, va);
    va_end(va);
}


/* This is called directly if the machine doesn't allow varargs
 * macros. */
void
rs_trace0(char const *s, ...) 
{
    va_list	va;

    va_start(va, s);
    rs_log_va(RS_LOG_DEBUG, PACKAGE, s, va);
    va_end(va);
}


/**
 * Return true if the library contains trace code; otherwise false.
 * If this returns false, then trying to turn trace on will achieve
 * nothing.
 */
int
rs_supports_trace(void)
{
#ifdef DO_RS_TRACE
    return 1;
#else
    return 0;
#endif				/* !DO_RS_TRACE */
}


