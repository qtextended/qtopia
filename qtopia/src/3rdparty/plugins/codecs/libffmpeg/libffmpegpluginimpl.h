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
#ifndef LIBFFMPEG_PLUGIN_IMPL_H 
#define LIBFFMPEG_PLUGIN_IMPL_H

#include <qtopia/mediaplayerplugininterface.h>


class LibFFMpegPlugin;


class LibFFMpegPluginImpl : public MediaPlayerPluginInterface
{
public:
    LibFFMpegPluginImpl();
    virtual ~LibFFMpegPluginImpl();

#ifndef QT_NO_COMPONENT

    QRESULT queryInterface( const QUuid&, QUnknownInterface** );
    Q_REFCOUNT

#endif

    virtual MediaPlayerDecoder *decoder();
    virtual MediaPlayerEncoder *encoder();

private:
    LibFFMpegPlugin *libffmpegplugin;
    ulong ref;
};


#endif

