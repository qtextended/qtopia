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

/* This writes to:
   1) ~/Library/Preferences/...
   2) /Library/Preferences/...
*/

#include "qsettings.h"
#include <private/qsettings_p.h>
#ifndef QT_NO_SETTINGS
#include "qstring.h"
#include "qptrlist.h"
#include "qcleanuphandler.h"
#include <CoreFoundation/CoreFoundation.h>
#include <stdlib.h>

/*****************************************************************************
  QSettings debug facilities
 *****************************************************************************/
//#define DEBUG_SETTINGS_KEYS

static QString *qt_mac_settings_base = NULL;
QString cfstring2qstring(CFStringRef); //qglobal.cpp
bool qt_verify_key(const QString &); //qsettings.cpp

/*****************************************************************************
  QSettings utility functions
 *****************************************************************************/
#if 0
#include "qurl.h"
#define MACKEY_SEP '.'
static void qt_mac_fix_key(QString &k) {
    if(k.isEmpty())
	return;
#ifdef DEBUG_SETTINGS_KEYS
    QString old_k = k;
#endif
    while(k.length() && k[0] == '/')
	k = k.mid(1);
    k.replace("//", "/");
    QUrl::encode(k);
    k.replace(".", "%2E"); //when a . is in a key, we need to url encode it..
    for(int i=0; i<(int)k.length(); i++) {
	if(k[i] == '/')
	    k[i] = MACKEY_SEP;
    }
#ifdef DEBUG_SETTINGS_KEYS
    qDebug("QSettings::fixed : %s -> %s", old_k.latin1(), k.latin1());
#endif
}
static void qt_mac_unfix_key(QString &k) {
    if(k.isEmpty())
	return;

#ifdef DEBUG_SETTINGS_KEYS
    QString old_k = k;
#endif
    k.replace(".", "/");
    k.replace("%2E", "."); //just to be sure
    QUrl::decode(k);
#ifdef DEBUG_SETTINGS_KEYS
    qDebug("QSettings::unFixed : %s -> %s", old_k.latin1(), k.latin1());
#endif
}
#else
#define qt_mac_fix_key(k) 
#define qt_mac_unfix_key(k)
#define MACKEY_SEP '.'
#endif

QString 
qt_mac_get_global_setting(QString key, QString ret=QString::null, QString file=QString::null)
{
    if(file.isNull())
	file = ".GlobalPreferences";
    CFStringRef k = CFStringCreateWithCharacters(NULL, (UniChar *)key.unicode(), key.length()),
	       id = CFStringCreateWithCharacters(NULL, (UniChar *)file.unicode(), file.length());
    if(CFPropertyListRef r = CFPreferencesCopyValue(k, id, kCFPreferencesCurrentUser, kCFPreferencesAnyHost)) {
	if(CFGetTypeID(r) == CFStringGetTypeID()) {
	    ret = cfstring2qstring((CFStringRef)r);
	} else if(CFGetTypeID(r) == CFBooleanGetTypeID()) {
	    ret = CFEqual((CFBooleanRef)r, kCFBooleanTrue) ? "TRUE" : "FALSE";
	} else if(CFGetTypeID(r) == CFNumberGetTypeID()) {
	    int num;
	    if(CFNumberGetValue((CFNumberRef)r, kCFNumberIntType, &num))
                ret = QString::number(num);
        } else {
	    qDebug("qt-internal::QSettings, %s: no hablo %d", key.latin1(), (int)CFGetTypeID(r));
	}
	CFRelease(r);
    }
    CFRelease(id);
    CFRelease(k);
    return ret;
}

/*****************************************************************************
  Developers are allowed to access this to influence the base (defaults to 'com.')
 *****************************************************************************/
static QSingleCleanupHandler<QString> cleanup_base;
void qt_setSettingsBasePath(const QString &s)
{
    delete qt_mac_settings_base;
    qt_mac_settings_base = new QString(s);
    cleanup_base.set(&qt_mac_settings_base);
}    

/*****************************************************************************
  This class given the path and key will generate a proper ApplicationID as well
  as the matching key.
 *****************************************************************************/
class search_keys {
    CFStringRef i, k;
    QString qi, qk;
public:
    search_keys(QString id, QString key, const char * =NULL);
    ~search_keys();

    CFStringRef id() const { return i; }
    const QString &qtId() const { return qi; }
    CFStringRef key() const { return k; }
    const QString &qtKey() const { return qk; }
};
search_keys::search_keys(QString path, QString key, const char *where) 
{
#ifndef DEBUG_SETTINGS_KEYS
    Q_UNUSED(where);
#else
    QString oldkey = key, oldpath = path;
#endif
    qi = path;
    qk = key;
    while(qk.startsWith("/"))
	qk = qk.mid(1);
    while(qi.startsWith("/"))
	qi = qi.mid(1);
    if(qi.isEmpty()) {
	int slsh = qk.find('/');
	if(slsh != -1) {
	    qi += qk.left(slsh);
	    qk = qk.mid(slsh+1);
	}
    }
    while(qi.startsWith("/"))
	qi = qi.mid(1);
    qt_mac_unfix_key(qi);
    qi.replace('/', ".");
    if(qt_mac_settings_base)
	qi.prepend(*qt_mac_settings_base);

    qt_mac_fix_key(qk);
#ifdef DEBUG_SETTINGS_KEYS
    qDebug("search_key [%s] %s::%s -> %s::%s", where ? where : "*Unknown*", 
	   oldpath.latin1(), oldkey.latin1(), qi.latin1(), qk.latin1());
#endif
    i = CFStringCreateWithCharacters(NULL, (UniChar *)qi.unicode(), qi.length());
    k = CFStringCreateWithCharacters(NULL, (UniChar *)qk.unicode(), qk.length());
}
search_keys::~search_keys()
{
    CFRelease(i);
    CFRelease(k);
}

/*****************************************************************************
  Internal private class
 *****************************************************************************/
class QSettingsSysPrivate
{
public:
    QSettingsSysPrivate();
    bool writeEntry(QString, CFPropertyListRef, bool);
    CFPropertyListRef readEntry(QString, bool);
    QStringList entryList(QString, bool, bool);
    QStringList searchPaths;
    QStringList syncKeys;
};

QSettingsSysPrivate::QSettingsSysPrivate() 
{
    if(!qt_mac_settings_base) {
	qt_mac_settings_base = new QString("com.");
	cleanup_base.set(&qt_mac_settings_base);
    }
}

bool QSettingsSysPrivate::writeEntry(QString key, CFPropertyListRef plr, bool global)
{
#if 1
    global = FALSE; //this doesn't work very well!
#endif
    QString realKey = key;
    while(realKey.right(1) == "/")
	realKey.truncate(realKey.length() -1);

    bool ret = FALSE;
    for(QStringList::Iterator it = searchPaths.fromLast(); it != searchPaths.end(); --it) {
	search_keys k((*it), realKey, "writeEntry");
	CFStringRef scopes[] = { kCFPreferencesAnyUser, kCFPreferencesCurrentUser, NULL };
	for(int scope = (global ? 0 : 1); scopes[scope]; scope++) {
	    CFPreferencesSetValue(k.key(), plr, k.id(), scopes[scope], kCFPreferencesAnyHost);
	    if(TRUE) { //hmmm..
		if(!syncKeys.findIndex(k.qtId()) != -1)
		    syncKeys.append(k.qtId());
		ret = TRUE;
		break;
	    }
	}
	if(ret)
	    break;
    }
    return ret;
}
CFPropertyListRef QSettingsSysPrivate::readEntry(QString key, bool global)
{
#if 1
    global = FALSE; //this doesn't work very well!
#endif

    for(QStringList::Iterator it = searchPaths.fromLast(); it != searchPaths.end(); --it) {
	search_keys k((*it), key, "readEntry");
	CFStringRef scopes[] = { kCFPreferencesAnyUser, kCFPreferencesCurrentUser, NULL };
	for(int scope = (global ? 0 : 1); scopes[scope]; scope++) {
	    if(CFPropertyListRef ret = CFPreferencesCopyValue(k.key(), k.id(), 
							      scopes[scope], kCFPreferencesAnyHost)) 
		return ret;
	}
    }
    return NULL;
}

QStringList QSettingsSysPrivate::entryList(QString key, bool subkey, bool global)
{
#if 1
    global = FALSE; //this doesn't work very well!
#endif

    QStringList ret;
    for(QStringList::Iterator it = searchPaths.fromLast();  it != searchPaths.end(); --it) {
	search_keys k((*it), key, subkey ? "subkeyList" : "entryList");
	CFStringRef scopes[] = { kCFPreferencesAnyUser, kCFPreferencesCurrentUser, NULL };
	for(int scope = (global ? 0 : 1); scopes[scope]; scope++) {
	    if(CFArrayRef cfa = CFPreferencesCopyKeyList(k.id(), scopes[scope],
							 kCFPreferencesAnyHost)) {
		QString qk = cfstring2qstring(k.key());
		for(CFIndex i = 0, cnt = CFArrayGetCount(cfa); i < cnt; i++) {
		    QString s = cfstring2qstring((CFStringRef)CFArrayGetValueAtIndex(cfa, i));
		    if(s.left(qk.length()) == qk) {
			s = s.mid(qk.length());
			while(s[0] == MACKEY_SEP)
			    s = s.mid(1);
			int sep = s.find(MACKEY_SEP);
			if(sep != -1) {
			    if(subkey) {
				QString fix_s = s.left(sep);
				qt_mac_unfix_key(fix_s);
				if(!fix_s.isEmpty() && ret.findIndex(fix_s) == -1)
				    ret << fix_s;
			    }
			} else if(!subkey) {
			    QString fix_s = s;
			    qt_mac_unfix_key(fix_s);
			    ret << fix_s;
			}
		    }
		}
		CFRelease(cfa);
		return ret;
	    }
	}
    }
    return ret;
}

void
QSettingsPrivate::sysInit()
{
    sysd = new QSettingsSysPrivate;
    Q_CHECK_PTR(sysd);
    sysInsertSearchPath(QSettings::Mac, "");
}

void
QSettingsPrivate::sysClear()
{
    delete sysd;
    sysd = NULL;
}

bool QSettingsPrivate::sysSync()
{
    bool ret = TRUE;
    for(QStringList::Iterator it = sysd->syncKeys.begin();  it != sysd->syncKeys.end(); --it) {
	CFStringRef csr = CFStringCreateWithCharacters(NULL, (UniChar *)(*it).unicode(), 
						       (*it).length());
#ifdef DEBUG_SETTINGS_KEYS
	qDebug("QSettingsPrivate::sysSync(%s)", (*it).latin1());
#endif
	if(CFPreferencesAppSynchronize(csr))
	    ret = FALSE;
	CFRelease(csr);
    }
    sysd->syncKeys.clear();
    return ret;
}

void QSettingsPrivate::sysInsertSearchPath(QSettings::System s, const QString &path)
{
    if(s != QSettings::Mac)
	return;
    if ( !path.isEmpty() && !qt_verify_key( path ) ) {
#if defined(QT_CHECK_STATE)
	qWarning( "QSettings::insertSearchPath: Invalid key: '%s'", path.isNull() ? "(null)" : path.latin1() );
#endif
	return;
    }


    QString realpath = path;
    while(realpath.right(1) == "/")
	realpath.truncate(realpath.length() -1);
    sysd->searchPaths.append(realpath);
}

void QSettingsPrivate::sysRemoveSearchPath(QSettings::System s, const QString &path)
{
    if(s != QSettings::Mac) 
	return;
    QString realpath = path;
    while(realpath.right(1) == "/")
	realpath.truncate(realpath.length() -1);
    sysd->searchPaths.remove(realpath);
}

bool QSettingsPrivate::sysReadBoolEntry(const QString &key, bool def, bool *ok) const
{
#ifdef QT_CHECK_STATE
    if(key.isNull() || key.isEmpty()) {
	qWarning("QSettingsPrivate::sysReadBoolEntry: invalid null/empty key.");
	if(ok)
	    *ok = FALSE;
	return def;
    }
#endif // QT_CHECK_STATE
    if(CFPropertyListRef r = sysd->readEntry(key, globalScope)) {
	if(CFGetTypeID(r) == CFBooleanGetTypeID()) {
	    bool ret = FALSE;
	    if(CFEqual((CFBooleanRef)r, kCFBooleanTrue))
		ret = TRUE;
	    CFRelease(r);
	    return ret;
	} else {
	    CFRelease(r);
	}
    }
    if(ok)
	*ok = FALSE;
    return def;
}

double QSettingsPrivate::sysReadDoubleEntry(const QString &key, double def, bool *ok) const
{
#ifdef QT_CHECK_STATE
    if(key.isNull() || key.isEmpty()) {
	qWarning("QSettingsPrivate::sysReadDoubleEntry: invalid null/empty key.");
	if(ok)
	    *ok = FALSE;
	return def;
    }
#endif // QT_CHECK_STATE
    if(CFPropertyListRef r = sysd->readEntry(key, globalScope)) {
	if(CFGetTypeID(r) == CFNumberGetTypeID()) {
	    double ret;
	    if(!CFNumberGetValue((CFNumberRef)r, kCFNumberDoubleType, &ret)) {
		if(ok)
		    *ok = FALSE;
		ret = def;
	    } else if(ok) {
		*ok = TRUE;
	    }
	    CFRelease(r);
	    return ret;
	} else {
	    CFRelease(r);
	}
    }
    if(ok)
	*ok = FALSE;
    return def;
} 

int QSettingsPrivate::sysReadNumEntry(const QString &key, int def, bool *ok) const
{
#ifdef QT_CHECK_STATE
    if(key.isNull() || key.isEmpty()) {
	qWarning("QSettingsPrivate::sysReadNumEntry: invalid null/empty key.");
	if(ok)
	    *ok = FALSE;
	return def;
    }
#endif // QT_CHECK_STATE
    if(CFPropertyListRef r = sysd->readEntry(key, globalScope)) {
	if(CFGetTypeID(r) == CFNumberGetTypeID()) {
	    int ret;
	    if(!CFNumberGetValue((CFNumberRef)r, kCFNumberIntType, &ret)) {
		if(ok)
		    *ok = FALSE;
		ret = def;
	    } else if(ok) {
		*ok = TRUE;
	    }
	    CFRelease(r);
	    return ret;
	} else {
	    CFRelease(r);
	}
    }
    if(ok)
	*ok = FALSE;
    return def;
}

QString QSettingsPrivate::sysReadEntry(const QString &key, const QString &def, bool *ok) const
{
#ifdef QT_CHECK_STATE
    if(key.isNull() || key.isEmpty()) {
	qWarning("QSettingsPrivate::sysReadEntry: invalid null/empty key.");
	if(ok)
	    *ok = FALSE;
	return def;
    }
#endif // QT_CHECK_STATE
    if(CFPropertyListRef r = sysd->readEntry(key, globalScope)) {
	if(CFGetTypeID(r) == CFStringGetTypeID()) {
	    if(ok)
		*ok = TRUE;
	    QString ret = cfstring2qstring((CFStringRef)r);
	    CFRelease(r);
	    return ret;
	} else {
	    CFRelease(r);
	}
    }
    if(ok)
	*ok = FALSE;
    return def;
}

#if !defined(Q_NO_BOOL_TYPE)
bool QSettingsPrivate::sysWriteEntry(const QString &key, bool value)
{
#ifdef QT_CHECK_STATE
    if(key.isNull() || key.isEmpty()) {
	qWarning("QSettingsPrivate::sysWriteEntry (bool): invalid null/empty key.");
	return FALSE;
    }
#endif // QT_CHECK_STATE
    CFBooleanRef val = value ? kCFBooleanTrue : kCFBooleanFalse;
    bool ret = sysd->writeEntry(key, val, globalScope);
    return ret;
}
#endif

bool QSettingsPrivate::sysWriteEntry(const QString &key, double value)
{
#ifdef QT_CHECK_STATE
    if(key.isNull() || key.isEmpty()) {
	qWarning("QSettingsPrivate::sysWriteEntry (double): invalid null/empty key.");
	return FALSE;
    }
#endif // QT_CHECK_STATE
    CFNumberRef val = CFNumberCreate(NULL, kCFNumberDoubleType, &value);
    bool ret = sysd->writeEntry(key, val, globalScope);
    CFRelease(val);
    return ret;
}

bool QSettingsPrivate::sysWriteEntry(const QString &key, int value)
{
#ifdef QT_CHECK_STATE
    if(key.isNull() || key.isEmpty()) {
	qWarning("QSettingsPrivate::sysWriteEntry (int): invalid null/empty key.");
	return FALSE;
    }
#endif // QT_CHECK_STATE
    CFNumberRef val = CFNumberCreate(NULL, kCFNumberIntType, &value);
    bool ret = sysd->writeEntry(key, val, globalScope);
    CFRelease(val);
    return ret;
}

bool QSettingsPrivate::sysWriteEntry(const QString &key, const QString &value)
{
#ifdef QT_CHECK_STATE
    if(key.isNull() || key.isEmpty()) {
	qWarning("QSettingsPrivate::sysWriteEntry (QString): invalid null/empty key.");
	return FALSE;
    }
#endif // QT_CHECK_STATE
    CFStringRef val = CFStringCreateWithCharacters(NULL, (UniChar *)value.unicode(), 
						 value.length());
    bool ret = sysd->writeEntry(key, val, globalScope);
    CFRelease(val);
    return ret;
}

bool QSettingsPrivate::sysRemoveEntry(const QString &key)
{
#ifdef QT_CHECK_STATE
    if(key.isNull() || key.isEmpty()) {
	qWarning("QSettingsPrivate::sysRemoveEntry: invalid null/empty key.");
	return FALSE;
    }
#endif // QT_CHECK_STATE
    return sysd->writeEntry(key, NULL, globalScope);
}

QStringList QSettingsPrivate::sysEntryList(const QString &key) const
{
#ifdef QT_CHECK_STATE
    if(key.isNull() || key.isEmpty()) {
	qWarning("QSettingsPrivate::sysEntryList: invalid null/empty key.");
	return QStringList();
    }
#endif // QT_CHECK_STATE
    return sysd->entryList(key, FALSE, globalScope);
}

QStringList QSettingsPrivate::sysSubkeyList(const QString &key) const
{
#ifdef QT_CHECK_STATE
    if(key.isNull() || key.isEmpty()) {
	qWarning("QSettingsPrivate::sysSubkeyList: invalid null/empty key.");
	return QStringList();
    }
#endif // QT_CHECK_STATE
    return sysd->entryList(key, TRUE, globalScope);
}

#endif //QT_NO_SETTINGS
