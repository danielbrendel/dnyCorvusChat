#ifndef _MASTERSERVER_H
#define _MASTERSERVER_H

#include "includes.h"
#include "clientsock.h"

/*
	CorvusChat Server (dnyCorvusChat) - Chatserver component

	Developed by Daniel Brendel

	Version: 0.5
	Visit: https://github.com/danielbrendel
	Mail dbrendel1988@gmail.com

	File: masterserver.h: Masterserver manager interface 
*/

//======================================================================
#define IDENT_ADDASSERVER 0x02
#define IDENT_CHECKALIVE1 0xFF
#define IDENT_CHECKALIVE2 0x00
//======================================================================

//======================================================================
struct serverinfo_s {
	unsigned char ucIdent; //Message ident
	unsigned char ucaAddr[4]; //IPv4 address of this server
	WORD usPort;	//Port the server is running on
	unsigned short ucVer[2]; //Server version
	char szName[MAX_NETWORK_STRING_LENGTH]; //Server name
	char szForcedClient[MAX_NETWORK_STRING_LENGTH]; //Forced client name
	unsigned int uiMaxclients; //Maximum amount of clients
	unsigned int uiCurClients; //Current amount of connected clients
	bool bPassword; //If server is password protected
	unsigned int uiUptime; //Uptime of server
};
//======================================================================

//======================================================================
class CMasterServer {
private:
	SOCKET hSocket;
	sockaddr_in saddr;

	BYTE ucaBuf[1024];

	serverinfo_s ServerInfo;
public:
	CMasterServer() { hSocket = SOCKET_ERROR; }
	~CMasterServer() { }

	BOOL EstablishConnection(const char *szIpAddress, const WORD wPort);
	BOOL CloseConnection(void);

	VOID UpdateData(serverinfo_s *pData);

	BOOL RecieveData(void);
	
	BOOL IsConnected(void);
};
//======================================================================

#endif