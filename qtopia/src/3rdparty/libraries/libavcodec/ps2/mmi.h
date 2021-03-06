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
#ifndef __mmi_H
#define __mmi_H

#define align16 __attribute__ ((aligned (16)))

/*
#define r0 $zero
#define r1 $at	//assembler!
#define r2 $v0	//return
#define r3 $v1	//return
#define r4 $a0	//arg
#define r5 $a1	//arg
#define r6 $a2	//arg
#define r7 $a3	//arg
#define r8 $t0	//temp
#define r9 $t1	//temp
#define r10 $t2	//temp
#define r11 $t3	//temp
#define r12 $t4	//temp
#define r13 $t5	//temp
#define r14 $t6	//temp
#define r15 $t7	//temp
#define r16 $s0	//saved temp
#define r17 $s1	//saved temp
#define r18 $s2	//saved temp
#define r19 $s3	//saved temp
#define r20 $s4	//saved temp
#define r21 $s5	//saved temp
#define r22 $s6	//saved temp
#define r23 $s7	//saved temp
#define r24 $t8	//temp
#define r25 $t9	//temp
#define r26 $k0	//kernel
#define r27 $k1	//kernel
#define r28 $gp	//global ptr
#define r29 $sp //stack ptr
#define r30 $fp //frame ptr
#define r31 $ra //return addr
*/


#define	lq(base, off, reg)	\
	__asm__ __volatile__ ("lq " #reg ", %0("#base ")" : : "i" (off) )

#define	lq2(mem, reg)	\
	__asm__ __volatile__ ("lq " #reg ", %0" : : "r" (mem))

#define	sq(reg, off, base)	\
	__asm__ __volatile__ ("sq " #reg ", %0("#base ")" : : "i" (off) )

/*
#define	ld(base, off, reg)	\
	__asm__ __volatile__ ("ld " #reg ", " #off "("#base ")")
*/

#define	ld3(base, off, reg)	\
	__asm__ __volatile__ (".word %0" : : "i" ( 0xdc000000 | (base<<21) | (reg<<16) | (off)))

#define	ldr3(base, off, reg)	\
	__asm__ __volatile__ (".word %0" : : "i" ( 0x6c000000 | (base<<21) | (reg<<16) | (off)))

#define	ldl3(base, off, reg)	\
	__asm__ __volatile__ (".word %0" : : "i" ( 0x68000000 | (base<<21) | (reg<<16) | (off)))

/*
#define	sd(reg, off, base)	\
	__asm__ __volatile__ ("sd " #reg ", " #off "("#base ")")
*/
//seems assembler has bug encoding mnemonic 'sd', so DIY
#define	sd3(reg, off, base)	\
	__asm__ __volatile__ (".word %0" : : "i" ( 0xfc000000 | (base<<21) | (reg<<16) | (off)))

#define	sw(reg, off, base)	\
	__asm__ __volatile__ ("sw " #reg ", " #off "("#base ")")

#define	sq2(reg, mem)	\
	__asm__ __volatile__ ("sq " #reg ", %0" : : "m" (*(mem)))

#define	pinth(rs, rt, rd) \
	__asm__ __volatile__ ("pinth  " #rd ", " #rs ", " #rt )

#define	phmadh(rs, rt, rd) \
	__asm__ __volatile__ ("phmadh " #rd ", " #rs ", " #rt )

#define	pcpyud(rs, rt, rd) \
	__asm__ __volatile__ ("pcpyud " #rd ", " #rs ", " #rt )

#define	pcpyld(rs, rt, rd) \
	__asm__ __volatile__ ("pcpyld " #rd ", " #rs ", " #rt )

#define	pcpyh(rt, rd) \
	__asm__ __volatile__ ("pcpyh  " #rd ", " #rt )

#define	paddw(rs, rt, rd) \
	__asm__ __volatile__ ("paddw  " #rd ", " #rs ", " #rt )

#define	pextlw(rs, rt, rd) \
	__asm__ __volatile__ ("pextlw " #rd ", " #rs ", " #rt )

#define	pextuw(rs, rt, rd) \
	__asm__ __volatile__ ("pextuw " #rd ", " #rs ", " #rt )

#define	pextlh(rs, rt, rd) \
	__asm__ __volatile__ ("pextlh " #rd ", " #rs ", " #rt )

#define	pextuh(rs, rt, rd) \
	__asm__ __volatile__ ("pextuh " #rd ", " #rs ", " #rt )

#define	psubw(rs, rt, rd) \
	__asm__ __volatile__ ("psubw  " #rd ", " #rs ", " #rt )

#define	psraw(rt, sa, rd) \
	__asm__ __volatile__ ("psraw  " #rd ", " #rt ", %0" : : "i"(sa) )

#define	ppach(rs, rt, rd) \
	__asm__ __volatile__ ("ppach  " #rd ", " #rs ", " #rt )

#define	ppacb(rs, rt, rd) \
	__asm__ __volatile__ ("ppacb  " #rd ", " #rs ", " #rt )

#define	prevh(rt, rd) \
	__asm__ __volatile__ ("prevh  " #rd ", " #rt )

#define	pmulth(rs, rt, rd) \
	__asm__ __volatile__ ("pmulth " #rd ", " #rs ", " #rt )

#define	pmaxh(rs, rt, rd) \
	__asm__ __volatile__ ("pmaxh " #rd ", " #rs ", " #rt )

#define	pminh(rs, rt, rd) \
	__asm__ __volatile__ ("pminh " #rd ", " #rs ", " #rt )

#define	pinteh(rs, rt, rd) \
	__asm__ __volatile__ ("pinteh  " #rd ", " #rs ", " #rt )

#define	paddh(rs, rt, rd) \
	__asm__ __volatile__ ("paddh  " #rd ", " #rs ", " #rt )

#define	psubh(rs, rt, rd) \
	__asm__ __volatile__ ("psubh  " #rd ", " #rs ", " #rt )

#define	psrah(rt, sa, rd) \
	__asm__ __volatile__ ("psrah  " #rd ", " #rt ", %0" : : "i"(sa) )

#define	pmfhl_uw(rd) \
	__asm__ __volatile__ ("pmfhl.uw  " #rd)

#define	pextlb(rs, rt, rd) \
	__asm__ __volatile__ ("pextlb  " #rd ", " #rs ", " #rt )

#endif

