#ifndef _CLIENTDATA_H
#define _CLIENTDATA_H

#include "includes.h"

/*
	CorvusChat Server (dnyCorvusChat) - Masterserver component

	Developed by Daniel Brendel

	Version: 0.2
	Visit: https://github.com/danielbrendel
	Mail dbrendel1988@gmail.com

	File: clientdata.h: Incoming client data manager interface
*/

//======================================================================
typedef BYTE CDIDENT;

typedef void (*THandlerFunction)(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize);
//======================================================================

//======================================================================
struct cd_handler_s {
	CDIDENT ident; //Handler ID
	THandlerFunction pfnHandler; //Pointer to event function
};
//======================================================================

//======================================================================
class CClientData {
private:
	std::vector<cd_handler_s> vHandlers;

	bool HandlerExists(CDIDENT ident, DWORD* pdwListID);
public:
	bool AddHandler(CDIDENT ident, THandlerFunction pfnFunction);
	void Clear(void);

	bool HandleData(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize);
};
//======================================================================

#endif