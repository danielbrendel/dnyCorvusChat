#include "clientdata.h"

/*
	CorvusChat Server (dnyCorvusChat) - Chatserver component

	Developed by Daniel Brendel

	Version: 0.5
	Visit: https://github.com/danielbrendel
	Mail dbrendel1988@gmail.com

	File: clientdata.cpp: Incoming client data manager implementation 
*/

//======================================================================
bool CClientData::HandlerExists(CDIDENT ident, DWORD* pdwListID)
{
	//Check if a handler already exists

	if (!ident)
		return false;

	//Enumerate trough all entries
	for (unsigned int i = 0; i < vHandlers.size(); i++) {
		//Check for handler ident
		if (vHandlers[i].ident == ident) {
			//Set internal list ID if wanted
			if (pdwListID)
				*pdwListID = i;

			return true;
		}
	}

	return false;
}
//======================================================================

//======================================================================
bool CClientData::AddHandler(CDIDENT ident, THandlerFunction pfnFunction)
{
	//Add a new handler to list

	if ((!ident) || (!pfnFunction))
		return false;

	//Check if handler with ID already exists
	if (HandlerExists(ident, NULL))
		return false;

	//Setup data and add to list
	cd_handler_s cd;
	cd.ident = ident;
	cd.pfnHandler = pfnFunction;

	vHandlers.push_back(cd);

	return true;
}
//======================================================================

//======================================================================
void CClientData::Clear(void)
{
	//Clear handler data

	vHandlers.clear();
}
//======================================================================

//======================================================================
bool CClientData::HandleData(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize)
{
	//Handle incoming client data by calling event function

	if ((!pBuffer) || (dwBufSize < sizeof(CDIDENT)))
		return false;

	//Get ident value
	CDIDENT ident = *(CDIDENT*)pBuffer;

	DWORD dwListID;

	//Check if handler for this msg exists
	if (HandlerExists(ident, &dwListID)) {
		vHandlers[dwListID].pfnHandler(dwClientID, (const void*)((DWORD_PTR)pBuffer + sizeof(CDIDENT)), dwBufSize - sizeof(CDIDENT));

		return true;
	}

	return false;
}
//======================================================================
