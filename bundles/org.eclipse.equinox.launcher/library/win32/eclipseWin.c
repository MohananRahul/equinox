/*******************************************************************************
 * Copyright (c) 2000, 2005 IBM Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at 
 * http://www.eclipse.org/legal/epl-v10.html
 * 
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *     Kevin Cornell (Rational Software Corporation)
 *******************************************************************************/

#include "eclipseOS.h"
#include "eclipseCommon.h"

#include <windows.h>
#include <commctrl.h>
#include <process.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

#ifdef __MINGW32__
#include <stdlib.h>
#endif

extern HWND topWindow;

/* Global Variables */
_TCHAR*  consoleVM     = _T("java.exe");
_TCHAR*  defaultVM     = _T("javaw.exe");
_TCHAR*  vmLibrary 	   = _T("jvm.dll");
_TCHAR*  shippedVMDir  = _T("jre\\bin\\");

/* Define the window system arguments for the Java VM. */
static _TCHAR*  argVM[] = { NULL };

/* define default locations in which to find the jvm shared library
 * these are paths relative to the java exe, the shared library is
 * for example jvmLocations[0] + dirSeparator + vmLibrary */
#define MAX_LOCATION_LENGTH 10 /* none of the jvmLocations strings should be longer than this */ 
static const _TCHAR* jvmLocations [] = { _T("j9vm"),
										 _T("client"), 
										 _T("server"), 
										 _T("classic"), 
								 		 NULL };
/* Show the Splash Window
 *
 * Open the bitmap, insert into the splash window and display it.
 *
 */
int showSplash( const _TCHAR* featureImage )
{
	static int splashing = 0;
    RECT    rect;
    HBITMAP hBitmap = 0;
    HDC     hDC;
    int     depth;
    int     x, y;
    int     width, height;

	if(splashing) {
		/*splash screen is already showing, do nothing */
		return 0;
	}
	
	/* if Java was started first and is calling back to show the splash, we might not
	 * have initialized the window system yet
	 */
	initWindowSystem(0, NULL, 1);
	
    /* Load the bitmap for the feature. */
    hDC = GetDC( NULL);
    depth = GetDeviceCaps( hDC, BITSPIXEL ) * GetDeviceCaps( hDC, PLANES);
    ReleaseDC(NULL, hDC);
    if (featureImage != NULL)
    	hBitmap = LoadImage(NULL, featureImage, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    /* If the bitmap could not be found, return an error. */
    if (hBitmap == 0)
    	return ERROR_FILE_NOT_FOUND;

	/* Load the bitmap into the splash popup window. */
    SendMessage( topWindow, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM) hBitmap );

    /* Centre the splash window and display it. */
    GetWindowRect (topWindow, &rect);
    width = GetSystemMetrics (SM_CXSCREEN);
    height = GetSystemMetrics (SM_CYSCREEN);
    x = (width - (rect.right - rect.left)) / 2;
    y = (height - (rect.bottom - rect.top)) / 2;
    SetWindowPos (topWindow, 0, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
    ShowWindow( topWindow, SW_SHOW );
    BringWindowToTop( topWindow );
	splashing = 1;
	
    /* Process messages */
	dispatchMessages();
	return 0;
}

void dispatchMessages() {
	MSG     msg;
	
	if(topWindow == 0)
		return;
	while (PeekMessage( &msg, NULL, 0, 0, PM_REMOVE))
   	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}
}

long getSplashHandle() {
	return (long)topWindow;
}

void takeDownSplash() {
	if(topWindow != NULL) {
		DestroyWindow(topWindow);
		dispatchMessages();
	}
}

/* Get the window system specific VM args */
_TCHAR** getArgVM( _TCHAR *vm )
{
	return argVM;
}

/* Local functions */

/*
 * Find the VM shared library starting from the java executable 
 */
_TCHAR* findVMLibrary( _TCHAR* command ) {
	int i;
	int pathLength;	
	struct _stat stats;
	_TCHAR * path;				/* path to resulting jvm shared library */
	_TCHAR * location;			/* points to begining of jvmLocations section of path */
	
	if (command != NULL) {
		location = _tcsrchr( command, dirSeparator ) + 1;
		
		/*check first to see if command already points to the library */
		if (_tcscmp(location, vmLibrary) == 0) {
			return command;
		}
		
		pathLength = location - command;
		path = malloc((pathLength + MAX_LOCATION_LENGTH + 1 + _tcslen(vmLibrary)) * sizeof(_TCHAR *));
		_tcsncpy(path, command, pathLength);
		location = &path[pathLength];
		 
		/* 
		 * We are trying base/jvmLocations[*]/vmLibrary
		 * where base is the directory containing the given java command, normally jre/bin
		 */
		i = -1;
		while(jvmLocations[++i] != NULL) {
			int length = _tcslen(jvmLocations[i]);			
			_tcscpy(location, jvmLocations[i]);
			location[length] = dirSeparator;
			location[length + 1] = _T('\0');
			_tcscat(location, vmLibrary);
			if (_tstat( path, &stats ) == 0 && (stats.st_mode & S_IFREG) != 0)
			{	/* found it */
				return path;
			}
		}
	}
	
	/* Not found yet, try the registry, we will use the first 1.4 or 1.5 vm we can find*/
	HKEY keys[2] = { HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE };
	_TCHAR * jreKeyName = _T("Software\\JavaSoft\\Java Runtime Environment");
	for (i = 0; i < 2; i++) {
		HKEY jreKey = NULL;
		if (RegOpenKeyEx(keys[i], jreKeyName, 0, KEY_READ, &jreKey) == ERROR_SUCCESS) {
			int j = 0;
			_TCHAR keyName[MAX_PATH];
			DWORD length = MAX_PATH;
			while (RegEnumKeyEx(jreKey, j++, keyName, &length, 0, 0, 0, 0) == ERROR_SUCCESS) {  
				/*look for a 1.4 or 1.5 vm*/ 
				if( _tcsncmp(_T("1.4"), keyName, 3) == 0 || _tcsncmp(_T("1.5"), keyName, 3) == 0) {
					HKEY subKey = NULL;
					if(RegOpenKeyEx(jreKey, keyName, 0, KEY_READ, &subKey) == ERROR_SUCCESS) {
						length = MAX_PATH;
						_TCHAR lib[MAX_PATH];
						/*The RuntimeLib value should point to the library we want*/
						if(RegQueryValueEx(subKey, _T("RuntimeLib"), NULL, NULL, (void*)&lib, &length) == ERROR_SUCCESS) {
							if (_tstat( lib, &stats ) == 0 && (stats.st_mode & S_IFREG) != 0)
							{	/*library exists*/
								path = malloc( length * sizeof(TCHAR*));
								path[0] = _T('\0');
								_tcscat(path, lib);
								
								RegCloseKey(subKey);
								RegCloseKey(jreKey);
								return path;
							}
						}
						RegCloseKey(subKey);
					}
				}
			}
			RegCloseKey(jreKey);
		}
	}
	return NULL;
}

