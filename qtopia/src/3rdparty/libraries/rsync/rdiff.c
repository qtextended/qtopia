/*= -*- c-basic-offset: 4; indent-tabs-mode: nil; -*-
 *
 * librsync -- the library for network deltas
 * $Id: rdiff.c,v 1.2 2001/09/16 17:30:42 lknoll Exp $
 * 
 * Copyright (C) 1999, 2000, 2001 by Martin Pool <mbp@samba.org>
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
                               | .. after a year and a day, mourning is
			       | dangerous to the survivor and troublesome
			       | to the dead.
			       |	      -- Harold Bloom
                               */

/*
 * rdiff.c -- Command-line network-delta tool.
 *
 * TODO: Add a -z option to gzip/gunzip patches.  This would be
 * somewhat useful, but more importantly a good test of the streaming
 * API.  Also add -I for bzip2.
 *
 * If built with debug support and we have mcheck, then turn it on.
 * (Optionally?)
 */

#include <config_rsync.h>

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <popt.h>

#ifdef HAVE_ZLIB_H
#include <zlib.h>
#endif

#ifdef HAVE_BZLIB_H
#include <bzlib.h>
#endif

#include "rsync.h"
#include "fileutil.h"
#include "util.h"
#include "trace.h"
#include "isprefix.h"


#define PROGRAM "rdiff"

static size_t block_len = RS_DEFAULT_BLOCK_LEN;
static size_t strong_len = RS_DEFAULT_STRONG_LEN;

static int show_stats = 0;

static int bzip2_level = 0;
static int gzip_level  = 0;


enum {
    OPT_GZIP = 1069, OPT_BZIP2
};

extern int rs_roll_paranoia;

const struct poptOption opts[] = {
    { "verbose",     'v', POPT_ARG_NONE, 0,             'v' },
    { "version",     'V', POPT_ARG_NONE, 0,             'V' },
    { "input-size",  'I', POPT_ARG_INT,  &rs_inbuflen },
    { "output-size", 'O', POPT_ARG_INT,  &rs_outbuflen },
    { "help",        '?', POPT_ARG_NONE, 0,             'h' },
    {  0,            'h', POPT_ARG_NONE, 0,             'h' },
    { "block-size",  'b', POPT_ARG_INT,  &block_len },
    { "sum-size",    'S', POPT_ARG_INT,  &strong_len },
    { "statistics",  's', POPT_ARG_NONE, &show_stats },
    { "stats",        0,  POPT_ARG_NONE, &show_stats },
    { "gzip",         0,  POPT_ARG_NONE, 0,             OPT_GZIP },
    { "bzip2",        0,  POPT_ARG_NONE, 0,             OPT_BZIP2 },
    { "paranoia",     0,  POPT_ARG_NONE, &rs_roll_paranoia },
    { 0 }
};


static void rdiff_usage(const char *error)
{
    fprintf(stderr, "%s\n"
            "Try `%s --help' for more information.\n",
            error, PROGRAM);
}


static void rdiff_no_more_args(poptContext opcon)
{
    if (poptGetArg(opcon)) {
        rdiff_usage("rdiff: too many arguments");
        exit(RS_SYNTAX_ERROR);
    }
}


static void bad_option(poptContext opcon, int error)
{
    char       msgbuf[1000];
    
    snprintf(msgbuf, sizeof msgbuf-1, "%s: %s: %s",
             PROGRAM, poptStrerror(error), poptBadOption(opcon, 0));
    rdiff_usage(msgbuf);
    
    exit(RS_SYNTAX_ERROR);
}


static void help(void) {
    printf("Usage: rdiff [OPTIONS] signature [BASIS [SIGNATURE]]\n"
           "             [OPTIONS] delta SIGNATURE [NEWFILE [DELTA]]\n"
           "             [OPTIONS] patch BASIS [DELTA [NEWFILE]]\n"
           "\n"
           "Options:\n"
           "  -v, --verbose             Trace internal processing\n"
           "  -V, --version             Show program version\n"
           "  -?, --help                Show this help message\n"
           "  -s, --statistics          Show performance statistics\n"
           "Delta-encoding options:\n"
           "  -b, --block-size=BYTES    Signature block size\n"
           "  -S, --sum-size=BYTES      Set signature strength\n"
           "      --paranoia            Verify all rolling checksums\n"
           "IO options:\n"
           "  -I, --input-size=BYTES    Input buffer size\n"
           "  -O, --output-size=BYTES   Output buffer size\n"
           "  -z, --gzip[=LEVEL]        gzip-compress deltas\n"
           "  -i, --bzip2[=LEVEL]       bzip2-compress deltas\n"
           );
}


static void rdiff_show_version(void)
{
    /*
     * This little declaration is dedicated to Stephen Kapp and Reaper
     * Technologies, who by all appearances redistributed a modified but
     * unacknowledged version of GNU Keyring in violation of the licence
     * and all laws of politeness and good taste.
     */
    char const *bzlib = "", *zlib = "", *trace = "";
    
#ifdef HAVE_LIBZ
    zlib = ", gzip";
#endif

#ifdef HAVE_LIBBZ2
    bzlib = ", bzip2";
#endif

#ifndef DO_RS_TRACE
    trace = ", trace disabled";
#endif
   
    printf("rdiff (%s) [%s]\n"
           "Copyright (C) 1997-2001 by Martin Pool, Andrew Tridgell and others.\n"
           "http://rproxy.samba.org/\n"
           "Capabilities: %d bit files%s%s%s\n"
           "\n"
           "librsync comes with NO WARRANTY, to the extent permitted by law.\n"
           "You may redistribute copies of librsync under the terms of the GNU\n"
           "Lesser General Public License.  For more information about these\n"
           "matters, see the files named COPYING.\n",
           rs_librsync_version, RS_CANONICAL_HOST,
           8 * sizeof(rs_long_t), zlib, bzlib, trace);
}



static void rdiff_options(poptContext opcon)
{
    int             c;
    char const      *a;
    
    while ((c = poptGetNextOpt(opcon)) != -1) {
        switch (c) {
        case 'h':
            help();
            exit(RS_DONE);
        case 'V':
            rdiff_show_version();
            exit(RS_DONE);
        case 'v':
            if (!rs_supports_trace()) {
                rs_error("library does not support trace");
            }
            rs_trace_set_level(RS_LOG_DEBUG);
            break;
            
        case OPT_GZIP:
        case OPT_BZIP2:
            if ((a = poptGetOptArg(opcon))) {
                int l = atoi(a);
                if (c == OPT_GZIP)
                    gzip_level = l;
                else
                    bzip2_level = l;
            } else {
                if (c == OPT_GZIP)
                    gzip_level = -1;      /* library default */
                else
                    bzip2_level = 9;      /* demand the best */
            }
            rs_error("sorry, compression is not really implemented yet");
            exit(RS_UNIMPLEMENTED);
            
        default:
            bad_option(opcon, c);
        }
    }
}


/**
 * Generate signature from remaining command line arguments.
 */
static rs_result rdiff_sig(poptContext opcon)
{
    FILE            *basis_file, *sig_file;
    rs_stats_t      stats;
    rs_result       result;
    
    basis_file = rs_file_open(poptGetArg(opcon), "rb");
    sig_file = rs_file_open(poptGetArg(opcon), "wb");

    rdiff_no_more_args(opcon);
    
    result = rs_sig_file(basis_file, sig_file, block_len, strong_len, &stats);
    if (result != RS_DONE)
        return result;

    if (show_stats) 
        rs_log_stats(&stats);

    return result;
}


static rs_result rdiff_delta(poptContext opcon)
{
    FILE            *sig_file, *new_file, *delta_file;
    char const      *sig_name;
    rs_result       result;
    rs_signature_t  *sumset;
    rs_stats_t      stats;

    if (!(sig_name = poptGetArg(opcon))) {
        rdiff_usage("Usage for delta: "
                    "rdiff [OPTIONS] delta SIGNATURE [NEWFILE [DELTA]]");
        return RS_SYNTAX_ERROR;
    }

    sig_file = rs_file_open(sig_name, "rb");
    new_file = rs_file_open(poptGetArg(opcon), "rb");
    delta_file = rs_file_open(poptGetArg(opcon), "wb");

    rdiff_no_more_args(opcon);

    result = rs_loadsig_file(sig_file, &sumset, &stats);
    if (result != RS_DONE)
        return result;

    if (show_stats) 
        rs_log_stats(&stats);

    if ((result = rs_build_hash_table(sumset)) != RS_DONE)
        return result;

    result = rs_delta_file(sumset, new_file, delta_file, &stats);

    if (show_stats) 
        rs_log_stats(&stats);

    return result;
}



static rs_result rdiff_patch(poptContext opcon)
{
    /*  patch BASIS [DELTA [NEWFILE]] */
    FILE               *basis_file, *delta_file, *new_file;
    char const         *basis_name;
    rs_stats_t          stats;
    rs_result           result;

    if (!(basis_name = poptGetArg(opcon))) {
        rdiff_usage("Usage for patch: "
                    "rdiff [OPTIONS] patch BASIS [DELTA [NEW]]");
        return RS_SYNTAX_ERROR;
    }

    basis_file = rs_file_open(basis_name, "rb");
    delta_file = rs_file_open(poptGetArg(opcon), "rb");
    new_file =   rs_file_open(poptGetArg(opcon), "wb");

    rdiff_no_more_args(opcon);

    result = rs_patch_file(basis_file, delta_file, new_file, &stats);

    if (show_stats) 
        rs_log_stats(&stats);

    return result;
}



static rs_result rdiff_action(poptContext opcon)
{
    const char      *action;

    action = poptGetArg(opcon);
    if (!action) 
        ;
    else if (isprefix(action, "signature")) 
        return rdiff_sig(opcon);
    else if (isprefix(action, "delta")) 
        return rdiff_delta(opcon);
    else if (isprefix(action, "patch"))
        return rdiff_patch(opcon);
    
    rdiff_usage("rdiff: You must specify an action: `signature', `delta', or `patch'.");
    return RS_SYNTAX_ERROR;
}


int main(const int argc, const char *argv[])
{
    poptContext     opcon;
    rs_result       result;

    opcon = poptGetContext(PROGRAM, argc, argv, opts, 0);
    rdiff_options(opcon);
    result = rdiff_action(opcon);

    if (result != RS_DONE)
        rs_log(RS_LOG_ERR|RS_LOG_NONAME, "%s", rs_strerror(result));

    return result;
}
