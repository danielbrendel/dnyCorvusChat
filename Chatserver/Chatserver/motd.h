#ifndef _MOTD_H
#define _MOTD_H

#include "includes.h"
#include "clientsock.h"

/*
	CorvusChat Server (dnyCorvusChat) - Chatserver component

	Developed by Daniel Brendel

	Version: 0.5
	Visit: https://github.com/danielbrendel
	Mail dbrendel1988@gmail.com

	File: motd.h: MOTD manager interface 
*/

//======================================================================
class CMessageOfTheDay {
private:
	std::vector<char*> vLines;

	BOOL AddLine(const char *szLine); 
	VOID Clear(void); 
public:
	DWORD GetLineCount(void);
	char* GetLine(DWORD dwId);
	BOOL Reload(const char* szMotdFile);
};
//======================================================================

#endif