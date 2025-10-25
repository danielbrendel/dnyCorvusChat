#ifndef __LOG_H
#define __LOG_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

/*
	CorvusChat Server (dnyCorvusChat) - Chatserver component

	Developed by Daniel Brendel

	Version: 0.5
	Visit: https://github.com/danielbrendel
	Mail dbrendel1988@gmail.com

	File: log.h: Logging interface 
*/

//======================================================================
bool CreateLog(const char *lpszFileName, bool CreateNew);
void LogMessage(const char *lpszMessage, ...);
void LogSuccess(const char* lpszMessage, ...);
void LogWarning(const char* lpszWarning, ...);
void LogError(const char *lpszError, ...);
void CloseLog(void);
//======================================================================

#endif
