#ifndef _CLIENTS_H
#define _CLIENTS_H

#include "includes.h"

/*
	CorvusChat Server (dnyCorvusChat) - Masterserver component

	Developed by Daniel Brendel

	Version: 0.2
	Visit: https://github.com/danielbrendel
	Mail dbrendel1988@gmail.com

	File: clients.h: Client handler interface 
*/

//======================================================================
#define MAX_UPDATE_COUNT 10
#define MAX_WAIT_TIME 4000

#define MAX_NETWORK_STRING_LENGTH 250

#define INVALID_CLIENT_ID 0xFFFFFFFF
//======================================================================

//======================================================================
typedef void (*TpfnOnRecieveData)(DWORD dwClientID, const BYTE* pucData, DWORD dwBufSize);
//======================================================================

//======================================================================
struct serverinfo_s {
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

struct timer_s {
	DWORD dwLastCount; //Last updated timer value
	DWORD dwCurCount; //Current updated timer value
	BYTE dwUpdateCount; //Client update count
};

struct clientinfo_s {
	SOCKET socket;
	sockaddr_in saddr;
	bool bServer; //Is this client a server?
	serverinfo_s sinfo; //Only filled if this client is a server
	timer_s timer;
};
//======================================================================

//======================================================================
class CClient {
private:
	SOCKET hSocket;
	sockaddr_in saddr;

	bool bReady;
	bool bOnce;

	unsigned int dwLastCount;
	unsigned int dwCurCount;

	TpfnOnRecieveData pfnCallbackFunc;

	std::vector<clientinfo_s*> vClients;
public:
	CClient() { hSocket = SOCKET_ERROR; bReady = bOnce = false; }
	~CClient() { }

	bool Initialize(WORD wPort, TpfnOnRecieveData pfnCallbackFunc);
	void Close(void);

	void RemoveAll(void);

	bool AddClient(SOCKET hSocket, sockaddr_in *pSAddr);
	void RemoveClient(unsigned int id);

	void WaitForClients(void);
	void RecieveData(void);
	void CheckClientsAlive(void);
	void CheckServerUpdate(void);

	clientinfo_s *GetClientById(unsigned int id);
	unsigned int GetClientId(clientinfo_s *pClient);
	unsigned int GetClientCount(void);
	unsigned int GetServerCount(void);

	bool SendBuf(clientinfo_s *pClient, const unsigned char *pBuf, int iSize);
	bool SendBuf(DWORD dwClientID, const unsigned char *pBuf, int iSize);
};
//======================================================================

#endif
