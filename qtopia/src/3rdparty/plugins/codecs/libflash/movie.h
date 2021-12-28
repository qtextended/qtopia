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
/////////////////////////////////////////////////////////////
// Flash Plugin and Player
// Copyright (C) 1998 Olivier Debon
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 
///////////////////////////////////////////////////////////////
#ifndef _MOVIE_H_
#define _MOVIE_H_

#include "swf.h"

struct FlashMovie {
	/* true if a button has been moved */
	int buttons_updated;

	/* current keyboard focus */
	DisplayListEntry 	*cur_focus;

	/* mouse state */
	long mouse_active;
	long mouse_x;
	long mouse_y;
	int button_pressed;

	Button *lost_over;

	/* a button can return to a given state after some time */
	FlashEvent           scheduledEvent;
	struct timeval       scheduledTime;

	int		 refresh;

	CInputScript 	*main;
	long		 msPerFrame;
	GraphicDevice	*gd;
	SoundMixer   	*sm;

	void		(*getUrl)(char *,char *, void *);
	void		*getUrlClientData;

	void		(*getSwf)(char *url, int level, void *clientData);
	void		*getSwfClientData;

	void		(*cursorOnOff)(int , void *);
	void		*cursorOnOffClientData;

	FlashMovie();
	~FlashMovie();
	int		 processMovie(GraphicDevice *gd, SoundMixer *sm);
	int		 handleEvent(GraphicDevice *gd, SoundMixer *sm, FlashEvent *event);
	void 		 renderMovie();
	void 		 renderFocus();
};

#endif /* _MOVIE_H_ */