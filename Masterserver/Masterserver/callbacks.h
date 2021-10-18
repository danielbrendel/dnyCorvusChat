#ifndef _CALLBACKS_H
#define _CALLBACKS_H

#include "includes.h"
#include "clients.h"
#include "clientdata.h"
#include "concommand.h"

/*
	CorvusChat Server (dnyCorvusChat) - Masterserver component

	Developed by Daniel Brendel

	Version: 0.2
	Visit: https://github.com/danielbrendel
	Mail dbrendel1988@gmail.com

	File: callbacks.h: Callback functions interface 
*/

//======================================================================
void OnRecieveClientData(DWORD dwClientID, const BYTE* pucData, DWORD dwBufSize);

void CDH_ServerInfo(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize);
void CDH_AddToServerList(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize);

void Cmd_About(CConParser* pParser);
void Cmd_TryLocalhost(CConParser* pParser);
void Cmd_Localalias(CConParser* pParser);
void Cmd_ListClients(CConParser* pParser);
void Cmd_Quit(CConParser* pParser);
//======================================================================

#endif