#ifndef _UTILS_H
#define _UTILS_H

#include "includes.h"
#include "vars.h"

/*
	CorvusChat Server (dnyCorvusChat) - Chatserver component

	Developed by Daniel Brendel

	Version: 0.5
	Visit: https://github.com/danielbrendel
	Mail dbrendel1988@gmail.com

	File: utils.h: Utility function interface 
*/

//======================================================================
#undef GetCurrentTime
//======================================================================

//======================================================================
BOOL IsDestinationReachable(LPCSTR lpszServer);
inline unsigned int GetHoursByMS(unsigned int dwMS);
const char* AddressToString(const unsigned long ulIPAddress);
bool FileExists(const char* szFileName);
void ExtractFilePath(char *szFileName);
char *ExtractFileName(char *filePath);
const char* GetCurrentDateTime(void);
const char* GetCurrentDate(void);
const char* GetCurrentTime(void);
BOOL UpdateServerComponent(VOID);
char* CreateRandomString(BYTE bAsciiStart, BYTE bAsciiEnd, unsigned short usStrSize);
bool DirectoryExists(char* szDir);
bool IsValidStrLen(const char* szString, size_t uiMaxStrLen = MAX_NETWORK_STRING_LENGTH);
bool toLower(char* szString);
void WebDirToWinDir(char* szString);
wchar_t *CharToWChar(char* szStr);
bool IsValidString(const char* szString);
bool ClientNameAlreadyUsed(const char* szClientName);
std::string StringReplace(std::string& stdstrExpression, const char* lpszToReplace, const char* lpszNewString);
BOOL RestartProgram(VOID);
BOOL DeleteFileObject(LPCSTR lpszFolder);
//======================================================================

#endif