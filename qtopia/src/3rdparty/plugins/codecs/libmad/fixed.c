/*
 * mad - MPEG audio decoder
 * Copyright (C) 2000-2001 Robert Leslie
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Id: fixed.c,v 1.4 2001/09/14 07:28:07 jryland Exp $
 */

# ifdef HAVE_CONFIG_H
#  include "libmad_config.h"
# endif

# include "libmad_global.h"

# include "fixed.h"

/*
 * NAME:	fixed->abs()
 * DESCRIPTION:	return absolute value of a fixed-point number
 */
mad_fixed_t mad_f_abs(mad_fixed_t x)
{
  return x < 0 ? -x : x;
}
