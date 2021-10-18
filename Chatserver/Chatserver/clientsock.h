#ifndef _SOCKET_H
#define _SOCKET_H

#include "includes.h"

#pragma comment(lib, "WSOCK32.LIB")

/*
	CorvusChat Server (dnyCorvusChat) - Chatserver component

	Developed by Daniel Brendel

	Version: 0.5
	Visit: https://github.com/danielbrendel
	Mail dbrendel1988@gmail.com

	File: clientsock.h: Client manager interface 
*/

//======================================================================
#define MAX_DATA 4096
#define LOCALHOST_ADDR 0x0100007F
#define UM_QUEUE_WAIT_TIME 250
#define MAX_SINGLE_ADDR_CONNECTIONS 10
#define MAX_PING_WAIT_TIME 1000
#define MAX_PING_TIMEOUT 25
#define MAX_LOGIN_WAIT_TIME 30000
#define MAX_UNSIGNED_BYTE 255
#define INVALID_CLIENT_ID -1
#define MAX_NETWORK_STRING_LENGTH 250
#ifndef SD_BOTH
#define SD_BOTH 2
#endif
#define FLOOD_CHECK_DELAY 20
#define FLOOD_MAX_COUNT 20
#define MAX_CLANTAG_STR_LEN 20
//======================================================================

//======================================================================
typedef int CLIENTID;
typedef CLIENTID CLIENTAMOUNT;
//======================================================================

//======================================================================
struct ping_s {
	DWORD actualPing;
	DWORD pingCount; //Time from msg creation till answer
	DWORD lastPing;
	DWORD curPing;
	BYTE pingAmount; //Pimg timeout counter
	BYTE curPingVal; //Ping msg id
};

struct um_queue_s {
    void* pBuffer;
    unsigned int dwSize;
    unsigned int dwCurrent;
    unsigned int dwLast;
};

struct logincount_s { //Used to handle that a user needs to login within a certain time
	DWORD dwCur;
	DWORD dwLast;
};

struct antiflood_s {
	DWORD dwCur; //Updates whenever data has been recieved by a client
	DWORD dwLast; //Updates when delay time is over
	WORD wDataCounter; //Incremented whenever data of this client is recieved within the flood check delay. Decremented outside of the flood delay check range
};

struct userinfo_s {
	char szName[MAX_NETWORK_STRING_LENGTH];
	char szClanTag[MAX_CLANTAG_STR_LEN];
	char szClient[MAX_NETWORK_STRING_LENGTH];
	char szEmail[MAX_NETWORK_STRING_LENGTH];
	char szCountry[MAX_NETWORK_STRING_LENGTH];
	char szFullName[MAX_NETWORK_STRING_LENGTH];
};

struct clientinfo_s {
    SOCKET hSocket; //Socket descriptor
    sockaddr_in saddr; //sockdet address data
	ping_s ping;
	userinfo_s userinfo; 
	bool bHasLoggedIn; //Client has logged in with name and eventually password
	bool bIsAdmin; //If this user is authed as an admin
	bool bIsBot; //If this user is a bot
	bool bGhost; //If this user is a ghost
	logincount_s logincount;
	antiflood_s antiflood;
	std::vector<um_queue_s>vUMsgs;
};

struct sockevents_s {
    bool (*OnClientConnect)(const CLIENTID);
	void (*OnClientDisconnect)(const CLIENTID, const char* pszReason);
    void (*OnClientWrite)(const CLIENTID, const BYTE*, DWORD);
	void (*OnClientUpdatePing)(const CLIENTID, BYTE);
    void (*OnErrorOccured)(DWORD);
};
//======================================================================

//======================================================================
class CClientSocket {
private:
	bool bReady;

	WORD wPort;
	SOCKET hSock;
	sockaddr_in saddr;
	BYTE ucRecvBuf[MAX_DATA];

	sockevents_s events;

	std::vector<clientinfo_s*> vClients;

	bool AddClient(SOCKET hSock, sockaddr_in *pAddr);
	void DeleteClient(CLIENTID id);
public:
	CClientSocket() { bReady = false; hSock = 0; memset(&saddr, 0x00, sizeof(sockaddr_in)); memset(&events, 0x00, sizeof(sockevents_s)); vClients.clear(); wPort = 0; memset(ucRecvBuf, 0x00, sizeof(ucRecvBuf)); }
	~CClientSocket() { }

	bool Initialize(WORD wPort);
	bool Clear(void);

	void SetEvents(sockevents_s *pEvents);

	bool WaitForClients(void);
	void RecieveData(void);
	bool SockClose(CLIENTID id, const char* pszReason);
	bool SockClose(clientinfo_s* pClient, const char* pszReason);
	void FreeClients(void);

	void CalculateStuff(void);

	void CheckClientPing(CLIENTID id, BYTE ucPingValue);

	bool AddressAlreadyInUse(DWORD dwIPAddr);
	unsigned char GetAmountOfAddress(DWORD dwIPAddr);

	const CLIENTAMOUNT GetClientCount(void);
	clientinfo_s* GetClientById(CLIENTID id);
	clientinfo_s* GetClientByName(const char *pszName);
	CLIENTID GetClientId(clientinfo_s* pClient);
	clientinfo_s* CreateBot(char *pszName, char *pszClanTag, char *pszClient, char *pszFullName, char *pszEmail, char *pszCountry, bool bAdmin, CLIENTID* pciClientId);
	
	bool SendBuf(CLIENTID id, const PVOID pBuf, DWORD dwBufSize, bool bSendDirect);
	bool SendBuf(clientinfo_s* pClient, const PVOID pBuf, DWORD dwBufSize, bool bSendDirect);
	bool BroadCast(const PVOID pBuf, DWORD dwBufSize, bool bSendDirect);

	void ProcessMessages(void);
	void MessageConfirmed(CLIENTID clientid);
	void FreeClientUMsgs(CLIENTID id);
};
//======================================================================

#endif