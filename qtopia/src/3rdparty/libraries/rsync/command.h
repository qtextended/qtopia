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
/*= -*- c-basic-offset: 4; indent-tabs-mode: nil; -*-
 *
 * librsync -- library for network deltas
 * $Id: command.h,v 1.1 2001/09/16 17:20:32 lknoll Exp $
 * 
 * Copyright (C) 1999, 2000, 2001 by Martin Pool <mbp@samba.org>
 * Copyright (C) 1999 by Andrew Tridgell <tridge@samba.org>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


/*
 * command.h -- Types of commands present in the encoding stream.
 *
 * The vague idea is that eventually this file will be more abstract
 * than protocol.h, but it's not clear that will ever be required.
 */


/**
 * Classes of operation that can be present.  Each may have several different
 * possible representations.
 */
enum rs_op_kind {
    RS_KIND_END = 1000,
    RS_KIND_LITERAL,
    RS_KIND_SIGNATURE,
    RS_KIND_COPY,
    RS_KIND_CHECKSUM,
    RS_KIND_RESERVED,           /* for future expansion */

    /* This one should never occur in file streams.  It's an
     * internal marker for invalid commands. */
    RS_KIND_INVALID
};


typedef struct rs_op_kind_name {
    char const           *name;
    enum rs_op_kind const    kind;
} rs_op_kind_name_t;

char const * rs_op_kind_name(enum rs_op_kind);


