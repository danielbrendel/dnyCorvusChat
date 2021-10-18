#include "masterserver.h"

/*
	CorvusChat Server (dnyCorvusChat) - Chatserver component

	Developed by Daniel Brendel

	Version: 0.5
	Visit: https://github.com/danielbrendel
	Mail dbrendel1988@gmail.com

	File: masterserver.cpp: Masterserver manager implementation 
*/

//======================================================================
BOOL CMasterServer::EstablishConnection(const char *szIpAddress, const WORD wPort)
{
	//Establish a connection to the master server

	if (hSocket != SOCKET_ERROR)
		return TRUE;

	if ((!szIpAddress) || (!wPort))
		return FALSE;

	hostent* pHostEnt = NULL;
	unsigned long ulIPAddr = 0;

	if (isalpha(szIpAddress[0])) { //Check for DNS name by only the first char
		pHostEnt = gethostbyname(szIpAddress);
		if (!pHostEnt)
			return FALSE;

		ulIPAddr = *(unsigned long*)pHostEnt->h_addr_list[0]; //Take first entry
	} else { //Probably no DNS, string is the IP address
		ulIPAddr = inet_addr(szIpAddress);
	}

	//Set socket address structure data
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(wPort);
	saddr.sin_addr.S_un.S_addr = ulIPAddr;

	//Create a socket descriptor (TCP)
	hSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (hSocket == SOCKET_ERROR)
		return FALSE;

	//Establish a connection to the masterserver
	if (connect(hSocket, (sockaddr*)&saddr, sizeof(sockaddr_in)) == SOCKET_ERROR)
		return FALSE;

	return TRUE;
}
//======================================================================

//======================================================================
BOOL CMasterServer::CloseConnection(void)
{
	//Shutdown the socket and close it

	if (hSocket == SOCKET_ERROR)
		return TRUE;

	BOOL bResult = (shutdown(hSocket, 2) != SOCKET_ERROR) && (closesocket(hSocket) != SOCKET_ERROR);

	hSocket = SOCKET_ERROR;

	return bResult;
}
//======================================================================

//======================================================================
VOID CMasterServer::UpdateData(serverinfo_s *pData)
{ 
	//Copy chat server data for the masterserver to member structure

	if (!pData)
		return;

	memcpy(&ServerInfo, pData, sizeof(serverinfo_s));
} 
//======================================================================

//======================================================================
BOOL CMasterServer::RecieveData(void)
{
    //Check for incoming data from the masterserver

	if (hSocket == SOCKET_ERROR)
		return FALSE;

	//Recieve incoming data
	if (recv(hSocket, (char*)ucaBuf, sizeof(ucaBuf), 0) == SOCKET_ERROR)
		return FALSE;

	//Check if masterserver wants our data
	if ((ucaBuf[0] == IDENT_CHECKALIVE1) && (ucaBuf[1] == IDENT_CHECKALIVE2)) {
		ServerInfo.ucIdent = IDENT_ADDASSERVER;
		return send(hSocket, (char*)&ServerInfo, sizeof(serverinfo_s), 0) != SOCKET_ERROR; //If so, send the data to the masterserver
	}

	return FALSE;
}
//======================================================================

//======================================================================
BOOL CMasterServer::IsConnected(void)
{
    //Check if a connection to masterserver is still alive

    BYTE ucCheckBuf[] = {0xFF, 0x00};

	//Send check buffer data to the masterserver
    if (send(hSocket, (char*)ucCheckBuf, sizeof(ucCheckBuf), 0) != SOCKET_ERROR)
		return true;

	//Shutdown/Close socket if there was a connection before but now lost
	if (hSocket != SOCKET_ERROR)
		CloseConnection();

	return false;
}
//======================================================================
