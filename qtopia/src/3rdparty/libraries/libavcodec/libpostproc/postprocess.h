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
    Copyright (C) 2001-2003 Michael Niedermayer (michaelni@gmx.at)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef NEWPOSTPROCESS_H
#define NEWPOSTPROCESS_H

/**
 * @file postprocess.h
 * @brief 
 *     external api for the pp stuff
 */

#ifdef __cplusplus
extern "C" {
#endif

#define PP_QUALITY_MAX 6

#define QP_STORE_T int8_t

typedef void pp_context_t;
typedef void pp_mode_t;

extern char *pp_help; ///< a simple help text

void  pp_postprocess(uint8_t * src[3], int srcStride[3],
                 uint8_t * dst[3], int dstStride[3],
                 int horizontalSize, int verticalSize,
                 QP_STORE_T *QP_store,  int QP_stride,
		 pp_mode_t *mode, pp_context_t *ppContext, int pict_type);


/**
 * returns a pp_mode_t or NULL if an error occured
 * name is the string after "-pp" on the command line
 * quality is a number from 0 to PP_QUALITY_MAX
 */
pp_mode_t *pp_get_mode_by_name_and_quality(char *name, int quality);
void pp_free_mode(pp_mode_t *mode);

pp_context_t *pp_get_context(int width, int height, int flags);
void pp_free_context(pp_context_t *ppContext);

#define PP_CPU_CAPS_MMX   0x80000000
#define PP_CPU_CAPS_MMX2  0x20000000
#define PP_CPU_CAPS_3DNOW 0x40000000
#define PP_CPU_CAPS_ALTIVEC 0x10000000

#define PP_FORMAT         0x00000008
#define PP_FORMAT_420    (0x00000011|PP_FORMAT)
#define PP_FORMAT_422    (0x00000001|PP_FORMAT)
#define PP_FORMAT_411    (0x00000002|PP_FORMAT)
#define PP_FORMAT_444    (0x00000000|PP_FORMAT)

#define PP_PICT_TYPE_QP2  0x00000010 ///< MPEG2 style QScale

#ifdef __cplusplus
}
#endif

#endif
