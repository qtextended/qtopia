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
#ifndef LIBFLASH_PLUGIN_H 
#define LIBFLASH_PLUGIN_H


#include <qstring.h>
#include <qapplication.h>
#include "flash.h"
#include <qtopia/mediaplayerplugininterface.h>


class LibFlashPlugin : public MediaPlayerDecoder {

public:
    LibFlashPlugin(); 
    ~LibFlashPlugin() { close(); }
 
    const char *pluginName() { return "LibFlashPlugin: " PLUGIN_NAME " " FLASH_VERSION_STRING; }
    const char *pluginComment() { return "This is the libflash library: " PLUGIN_NAME " " FLASH_VERSION_STRING; }
    double pluginVersion() { return 1.0; }

    bool isFileSupported( const QString& fileName ) { return fileName.right(4) == ".swf"; } 
    bool open( const QString& fileName );
    bool close() { FlashClose( file ); file = NULL; return TRUE; }
    bool isOpen() { return file != NULL; }
    const QString &fileInfo() { return strInfo = qApp->translate( "MediaPlayer", "No Information Available", "media plugin text" ); }

    // If decoder doesn't support audio then return 0 here
    int audioStreams() { return 1; }
    int audioChannels( int /*stream*/ ) { return 2; }
    int audioFrequency( int /*stream*/ ) { return 44100; }
    int audioSamples( int /*stream*/ ) { return 1000000; }
    bool audioSetSample( long sample, int stream );
    long audioGetSample( int stream );
    //bool audioReadMonoSamples( short *output, long samples, long& samplesRead, int stream );
    //bool audioReadStereoSamples( short *output, long samples, long& samplesRead, int stream );
    bool audioReadSamples( short *output, int channels, long samples, long& samplesRead, int stream );
    //bool audioReadSamples( short *output, int channel, long samples, int stream );
    //bool audioReReadSamples( short *output, int channel, long samples, int stream );

    // If decoder doesn't support video then return 0 here
    int videoStreams();
    int videoWidth( int stream );
    int videoHeight( int stream );
    double videoFrameRate( int stream );
    int videoFrames( int stream );
    bool videoSetFrame( long frame, int stream );
    long videoGetFrame( int stream );
    bool videoReadFrame( unsigned char **output_rows, int in_x, int in_y, int in_w, int in_h, ColorFormat color_model, int stream );
    bool videoReadScaledFrame( unsigned char **output_rows, int in_x, int in_y, int in_w, int in_h, int out_w, int out_h, ColorFormat color_model, int stream );
    bool videoReadYUVFrame( char *y_output, char *u_output, char *v_output, int in_x, int in_y, int in_w, int in_h, int stream );

    // Profiling
    double getTime();

    // Ignore if these aren't supported
    bool setSMP( int cpus );
    bool setMMX( bool useMMX );

    // Capabilities
    bool supportsAudio() { return TRUE; }
    bool supportsVideo() { return TRUE; }
    bool supportsYUV() { return TRUE; }
    bool supportsMMX() { return TRUE; }
    bool supportsSMP() { return TRUE; }
    bool supportsStereo() { return TRUE; }
    bool supportsScaling() { return TRUE; }

private:
    FlashHandle file;
    FlashDisplay *fd;
    QString strInfo;

};


#endif

