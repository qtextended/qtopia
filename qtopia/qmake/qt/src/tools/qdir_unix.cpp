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

#include "qplatformdefs.h"
#include "qdir.h"

#ifndef QT_NO_DIR

#include "qdir_p.h"
#include "qfileinfo.h"
#include "qregexp.h"
#include "qstringlist.h"

#ifdef QT_THREAD_SUPPORT
#  include <private/qmutexpool_p.h>
#endif // QT_THREAD_SUPPORT

#include <stdlib.h>
#include <limits.h>
#include <errno.h>


void QDir::slashify( QString& )
{
}

QString QDir::homeDirPath()
{
    QString d;
    d = QFile::decodeName(getenv("HOME"));
    slashify( d );
    if ( d.isNull() )
	d = rootDirPath();
    return d;
}

QString QDir::canonicalPath() const
{
    QString r;
    char cur[PATH_MAX+1];
    if ( ::getcwd( cur, PATH_MAX ) ) {
	char tmp[PATH_MAX+1];
	// need the cast for old solaris versions of realpath that doesn't take
	// a const char*.
	if( ::realpath( (char*)QFile::encodeName( dPath ).data(), tmp ) )
	    r = QFile::decodeName( tmp );
	slashify( r );

    	// always make sure we go back to the current dir
	::chdir( cur );
    }
    return r;
}

bool QDir::mkdir( const QString &dirName, bool acceptAbsPath ) const
{
#if defined(Q_OS_MACX)  // Mac X doesn't support trailing /'s
    QString name = dirName;
    if (dirName[dirName.length() - 1] == "/")
	name = dirName.left( dirName.length() - 1 );
    int status =
	::mkdir( QFile::encodeName(filePath(name,acceptAbsPath)), 0777 );
#else
    int status =
	::mkdir( QFile::encodeName(filePath(dirName,acceptAbsPath)), 0777 );
#endif
    return status == 0;
}

bool QDir::rmdir( const QString &dirName, bool acceptAbsPath ) const
{
    return ::rmdir( QFile::encodeName(filePath(dirName,acceptAbsPath)) ) == 0;
}

bool QDir::isReadable() const
{
    return ::access( QFile::encodeName(dPath), R_OK | X_OK ) == 0;
}

bool QDir::isRoot() const
{
    return dPath == QString::fromLatin1("/");
}

bool QDir::rename( const QString &name, const QString &newName,
		   bool acceptAbsPaths	)
{
    if ( name.isEmpty() || newName.isEmpty() ) {
#if defined(QT_CHECK_NULL)
	qWarning( "QDir::rename: Empty or null file name(s)" );
#endif
	return FALSE;
    }
    QString fn1 = filePath( name, acceptAbsPaths );
    QString fn2 = filePath( newName, acceptAbsPaths );
    return ::rename( QFile::encodeName(fn1),
		     QFile::encodeName(fn2) ) == 0;
}

bool QDir::setCurrent( const QString &path )
{
    int r;
    r = ::chdir( QFile::encodeName(path) );
    return r >= 0;
}

QString QDir::currentDirPath()
{
    QString result;

    struct stat st;
    if ( ::stat( ".", &st ) == 0 ) {
	char currentName[PATH_MAX+1];
	if ( ::getcwd( currentName, PATH_MAX ) )
	    result = QFile::decodeName(currentName);
#if defined(QT_DEBUG)
	if ( result.isNull() )
	    qWarning( "QDir::currentDirPath: getcwd() failed" );
#endif
    } else {
#if defined(QT_DEBUG)
	qWarning( "QDir::currentDirPath: stat(\".\") failed" );
#endif
    }
    slashify( result );
    return result;
}

QString QDir::rootDirPath()
{
    QString d = QString::fromLatin1( "/" );
    return d;
}

bool QDir::isRelativePath( const QString &path )
{
    int len = path.length();
    if ( len == 0 )
	return TRUE;
    return path[0] != '/';
}

bool QDir::readDirEntries( const QString &nameFilter,
			   int filterSpec, int sortSpec )
{
    int i;
    if ( !fList ) {
	fList  = new QStringList;
	Q_CHECK_PTR( fList );
	fiList = new QFileInfoList;
	Q_CHECK_PTR( fiList );
	fiList->setAutoDelete( TRUE );
    } else {
	fList->clear();
	fiList->clear();
    }

    QValueList<QRegExp> filters = qt_makeFilterList( nameFilter );

    bool doDirs	    = (filterSpec & Dirs)	!= 0;
    bool doFiles    = (filterSpec & Files)	!= 0;
    bool noSymLinks = (filterSpec & NoSymLinks) != 0;
    bool doReadable = (filterSpec & Readable)	!= 0;
    bool doWritable = (filterSpec & Writable)	!= 0;
    bool doExecable = (filterSpec & Executable) != 0;
    bool doHidden   = (filterSpec & Hidden)	!= 0;
    bool doSystem   = (filterSpec & System)     != 0;

    QFileInfo fi;
    DIR	     *dir;
    dirent   *file;

    dir = opendir( QFile::encodeName(dPath) );
    if ( !dir )
	return FALSE; // cannot read the directory

#if defined(QT_THREAD_SUPPORT) && defined(_POSIX_THREAD_SAFE_FUNCTIONS) && !defined(Q_OS_CYGWIN)
    union {
	struct dirent mt_file;
	char b[sizeof(struct dirent) + MAXNAMLEN + 1];
    } u;
    while ( readdir_r(dir, &u.mt_file, &file ) == 0 && file )
#else
    while ( (file = readdir(dir)) )
#endif // QT_THREAD_SUPPORT && _POSIX_THREAD_SAFE_FUNCTIONS
    {
	QString fn = QFile::decodeName(file->d_name);
	fi.setFile( *this, fn );
	if ( !qt_matchFilterList(filters, fn) && !(allDirs && fi.isDir()) )
	     continue;
	if  ( (doDirs && fi.isDir()) || (doFiles && fi.isFile()) ||
	      (doSystem && (!fi.isFile() && !fi.isDir())) ) {
	    if ( noSymLinks && fi.isSymLink() )
	        continue;
	    if ( (filterSpec & RWEMask) != 0 )
	        if ( (doReadable && !fi.isReadable()) ||
	             (doWritable && !fi.isWritable()) ||
	             (doExecable && !fi.isExecutable()) )
	            continue;
	    if ( !doHidden && fn[0] == '.' &&
	         fn != QString::fromLatin1(".")
	         && fn != QString::fromLatin1("..") )
	        continue;
	    fiList->append( new QFileInfo( fi ) );
	}
    }
    if ( closedir(dir) != 0 ) {
#if defined(QT_CHECK_NULL)
	qWarning( "QDir::readDirEntries: Cannot close the directory: %s",
		  dPath.local8Bit().data() );
#endif
    }

    // Sort...
    if(fiList->count()) {
	QDirSortItem* si= new QDirSortItem[fiList->count()];
	QFileInfo* itm;
	i=0;
	for (itm = fiList->first(); itm; itm = fiList->next())
	    si[i++].item = itm;
	qt_cmp_si_sortSpec = sortSpec;
	qsort( si, i, sizeof(si[0]), qt_cmp_si );
	// put them back in the list
	fiList->setAutoDelete( FALSE );
	fiList->clear();
	int j;
	for ( j=0; j<i; j++ ) {
	    fiList->append( si[j].item );
	    fList->append( si[j].item->fileName() );
	}
	delete [] si;
	fiList->setAutoDelete( TRUE );
    }

    if ( filterSpec == (FilterSpec)filtS && sortSpec == (SortSpec)sortS &&
	 nameFilter == nameFilt )
	dirty = FALSE;
    else
	dirty = TRUE;
    return TRUE;
}

const QFileInfoList * QDir::drives()
{
    // at most one instance of QFileInfoList is leaked, and this variable
    // points to that list
    static QFileInfoList * knownMemoryLeak = 0;

    if ( !knownMemoryLeak ) {

#ifdef QT_THREAD_SUPPORT
	QMutexLocker locker( qt_global_mutexpool ?
			     qt_global_mutexpool->get( &knownMemoryLeak ) : 0 );
#endif // QT_THREAD_SUPPORT

	if ( !knownMemoryLeak ) {
	    knownMemoryLeak = new QFileInfoList;
	    // non-win32 versions both use just one root directory
	    knownMemoryLeak->append( new QFileInfo( rootDirPath() ) );
	}
    }

    return knownMemoryLeak;
}
#endif //QT_NO_DIR
