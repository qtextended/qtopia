/*= -*- c-basic-offset: 4; indent-tabs-mode: nil; -*-
 *
 * librsync -- the library for network deltas
 * $Id: checksum.c,v 1.2 2001/09/16 17:30:42 lknoll Exp $
 * 
 * Copyright (C) 1999, 2000, 2001 by Martin Pool <mbp@samba.org>
 * Copyright (C) 1996 by Andrew Tridgell
 * Copyright (C) 1996 by Paul Mackerras
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

#include <config_rsync.h>

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "rsync.h"
#include "checksum.h"


/* This can possibly be used to restart the checksum system in the
 * case where we detected corruption.  I'm not sure yet how to make
 * this useful in librsync. */
int checksum_seed = 0;

/*
 * A simple 32 bit checksum that can be updated from either end
 * (inspired by Mark Adler's Adler-32 checksum)
 */
unsigned int rs_calc_weak_sum(void const *p, int len)
{
        int i;
        unsigned        s1, s2;
        unsigned char const    *buf = (unsigned char const *) p;

        s1 = s2 = 0;
        for (i = 0; i < (len - 4); i += 4) {
                s2 += 4 * (s1 + buf[i]) + 3 * buf[i + 1] +
                        2 * buf[i + 2] + buf[i + 3] + 10 * RS_CHAR_OFFSET;
                s1 += (buf[i + 0] + buf[i + 1] + buf[i + 2] + buf[i + 3] +
                       4 * RS_CHAR_OFFSET);
        }
        for (; i < len; i++) {
                s1 += (buf[i] + RS_CHAR_OFFSET);
                s2 += s1;
        }
        return (s1 & 0xffff) + (s2 << 16);
}


/**
 * Calculate and store into SUM a strong MD4 checksum of the file
 * blocks seen so far.
 *
 * In plain rsync, the checksum is perturbed by a seed value.  This is
 * used when retrying a failed transmission: we've discovered that the
 * hashes collided at some point, so we're going to try again with
 * different hashes to see if we can get it right.  (Check tridge's
 * thesis for details and to see if that's correct.)
 *
 * Since we can't retry a web transaction I'm not sure if it's very
 * useful in rproxy.
 */
void rs_calc_strong_sum(void const *buf, size_t len, rs_strong_sum_t *sum)
{
    rs_mdfour((unsigned char *) sum, buf, len);
}
