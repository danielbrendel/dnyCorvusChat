#include "clientsock.h"
#include "vars.h"
#include "utils.h"

/*
	CorvusChat Server (dnyCorvusChat) - Chatserver component

	Developed by Daniel Brendel

	Version: 0.5
	Visit: https://github.com/danielbrendel
	Mail dbrendel1988@gmail.com

	File: clientsock.h: Client manager implementation 
*/

//======================================================================
bool CClientSocket::AddClient(SOCKET hSock, sockaddr_in *pAddr)
{
	//Add a client to the client list

	if (!bReady)
		return false;

	if ((!hSock) || (!pAddr))
		return false;

	//Allocate memory and set data

    clientinfo_s* pNew = new clientinfo_s;
	if (!pNew)
		return false;

	memset(pNew, 0x00, sizeof(clientinfo_s));

    pNew->hSocket = hSock;
	pNew->bHasLoggedIn = false;
	pNew->bIsBot = false;
	pNew->bIsAdmin = false;
	pNew->bGhost = false;

    memcpy(&pNew->saddr, pAddr, sizeof(sockaddr_in));

	//Initialize login time counter
	pNew->logincount.dwCur = pNew->logincount.dwLast = GetTickCount();

	//Add to list
    vClients.push_back(pNew);

	return true;
}
//======================================================================

//======================================================================
clientinfo_s* CClientSocket::CreateBot(char *pszName, char *pszClanTag, char *pszClient, char *pszFullName, char *pszEmail, char *pszCountry, bool bAdmin, CLIENTID* pciClientId)
{
	//Create a bot client

	if ((!pszName) || (!pszClanTag) || (!pszClient) || (!pszFullName) || (!pszEmail) || (!pszCountry))
		return NULL;

	//Check for name
	if (GetClientByName(pszName))
		return NULL;

	//Allocate memory and set data

    clientinfo_s* pNew = new clientinfo_s;
	if (!pNew)
		return NULL;

	memset(pNew, 0x00, sizeof(clientinfo_s));

	pNew->bHasLoggedIn = true;
	pNew->bIsBot = true;
	pNew->bIsAdmin = bAdmin;
	pNew->bGhost = false;

	strcpy_s(pNew->userinfo.szName, pszName);
	strcpy_s(pNew->userinfo.szClanTag, pszClanTag);
	strcpy_s(pNew->userinfo.szClient, pszClient);
	strcpy_s(pNew->userinfo.szFullName, pszFullName);
	strcpy_s(pNew->userinfo.szEmail, pszEmail);
	strcpy_s(pNew->userinfo.szCountry, pszCountry);
	
	//Add to list
	vClients.push_back(pNew);

	//Set ID if desired
	if (pciClientId)
		*pciClientId = (CLIENTID)vClients.size()-1;

	return pNew;
}
//======================================================================

//======================================================================
void CClientSocket::DeleteClient(CLIENTID id)
{
	//Remove a client from the list

	if (!bReady)
		return;

	if (id == INVALID_CLIENT_ID)
		return;

	//Free memory and remove from list
    if (id < (CLIENTAMOUNT)vClients.size()) {
        delete vClients[id];
        vClients.erase(vClients.begin() + id);
    }
}
//======================================================================

//======================================================================
bool CClientSocket::Initialize(WORD wPort)
{
	//Initialize socket component

	if (bReady)
		return true;

	if (!wPort)
		return false;

	//Create socket descriptor
	hSock = socket(AF_INET, SOCK_STREAM, 0);
    if (hSock == SOCKET_ERROR) {
		DWORD dwLastSockError = WSAGetLastError();

		if (events.OnErrorOccured != NULL)
			events.OnErrorOccured(dwLastSockError);

		return false;
	}

	//Fill socket address information
	saddr.sin_family = AF_INET;
    saddr.sin_port = htons(wPort);
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    
	//Bind socket to information to have a server socket
	if (bind(hSock, (sockaddr*)&saddr, sizeof(sockaddr_in)) == SOCKET_ERROR) {
		DWORD dwLastSockError = WSAGetLastError();

		if (events.OnErrorOccured != NULL)
			events.OnErrorOccured(dwLastSockError);

		closesocket(hSock);

		return false;
	}

	//Set server socket into listen mode
	if (listen(hSock, SOMAXCONN) == SOCKET_ERROR) {
		DWORD dwLastSockError = WSAGetLastError();

		if (events.OnErrorOccured != NULL)
			events.OnErrorOccured(dwLastSockError);

		closesocket(hSock);

		return false;
	}

	//Set server socket to be non-blocking
	unsigned long nonBlocking = 1;
	if (ioctlsocket(hSock, FIONBIO, &nonBlocking) == SOCKET_ERROR) {
		DWORD dwLastSockError = WSAGetLastError();

		if (events.OnErrorOccured != NULL)
			events.OnErrorOccured(dwLastSockError);

		closesocket(hSock);

		return false;
	}

	bReady = true;

	return bReady;
}
//======================================================================

//======================================================================
bool CClientSocket::Clear(void)
{
	//Free client data and close server socket
	
	if (!bReady)
		return false;

	//Free all client data
	FreeClients();
	
	//Remove any recv/send activity of this socket
	if (shutdown(hSock, 2) == SOCKET_ERROR)
		return false;

	//Free internal socket data
	if (closesocket(hSock) == SOCKET_ERROR)
		return false;

	bReady = false;

    return true;
}
//======================================================================

//======================================================================
void CClientSocket::SetEvents(sockevents_s *pEvents)
{ 
	//Get event function table

	memcpy(&events, pEvents, sizeof(sockevents_s));
}
//======================================================================

//======================================================================
bool CClientSocket::WaitForClients(void)
{
	//Wait for incoming clients

	if (!bReady)
		return false;

	SOCKET hSockTmp = 0;
    sockaddr_in saddrtmp;

    memset(&saddrtmp, 0x00, sizeof(sockaddr_in));
    int size = sizeof(sockaddr_in);

	//If there are incoming connections then accept the client
	hSockTmp = accept(hSock, (sockaddr*)&saddrtmp, &size);
	if (hSockTmp == SOCKET_ERROR) {
		DWORD dwLastSockError = WSAGetLastError();

		return false;
	}

	//Check if there are too many connections from this IP-Address
	if (GetAmountOfAddress(saddrtmp.sin_addr.s_addr) >= g_GlobalVars.pSingleaddrAmount->iValue) {
		if (g_GlobalVars.pLogClientActions->iValue)
			ConsolePrint(FOREGROUND_YELLOW, "[CClientSocket::WaitForClients] No further connections of address allowed: \"%s\"(%d), denying connection.\n", AddressToString(saddrtmp.sin_addr.s_addr), g_GlobalVars.pSingleaddrAmount->iValue);

		shutdown(hSockTmp, 2);
		closesocket(hSockTmp);

		return false;
	}

	//Check for maximum amount of possible connections
	if (GetClientCount() >= (CLIENTAMOUNT)g_GlobalVars.pMaxUsers->iValue) {
		if (g_GlobalVars.pLogClientActions->iValue)
			ConsolePrint(FOREGROUND_YELLOW, "[CClientSocket::WaitForClients] Too many connected clients (%d), denying connection.\n", GetClientCount());

		shutdown(hSockTmp, 2);
		closesocket(hSockTmp);

		return false;
	}

	//Add the client to list
	if (!AddClient(hSockTmp, &saddrtmp)) {

		shutdown(hSockTmp, 2);
		closesocket(hSockTmp);

		return false;
	}

	//Call event function
	if (!events.OnClientConnect((CLIENTID)vClients.size()-1)) {
		SockClose(GetClientById((CLIENTID)(vClients.size()-1)), "OnClientConnect() failed");
		
		return false;
	}

	//Initialize antiflood stuff
	vClients[vClients.size()-1]->antiflood.dwCur = GetTickCount();
	vClients[vClients.size()-1]->antiflood.dwLast = GetTickCount();
	vClients[vClients.size()-1]->antiflood.wDataCounter = 0;

	return true;
}
//======================================================================

//======================================================================
void CClientSocket::RecieveData(void)
{
	//Recieve data from clients

	if (!bReady)
		return;

	//Enumerate trough the client list
    for (CLIENTAMOUNT i = 0; i < (CLIENTAMOUNT)vClients.size(); i++) {
        if (vClients[i] != NULL) {
            int iDataLen = recv(vClients[i]->hSocket, (char*)ucRecvBuf, sizeof(ucRecvBuf), 0); //Check if incoming data is available for this client
            if (iDataLen != SOCKET_ERROR) {
				//Perform antiflood protection
				if (!vClients[i]->antiflood.wDataCounter) { //This is the first message, so initialize the flood timers
					vClients[i]->antiflood.dwCur = GetTickCount();
					vClients[i]->antiflood.dwLast = GetTickCount();
				}

				vClients[i]->antiflood.dwCur = GetTickCount();
				if (vClients[i]->antiflood.dwCur < vClients[i]->antiflood.dwLast + FLOOD_CHECK_DELAY) { //Check if recieved buffer was inside the flood delay time
					vClients[i]->antiflood.wDataCounter++; //Increment incoming data counter
					if (vClients[i]->antiflood.wDataCounter > FLOOD_MAX_COUNT) { //Check if maximum amount of allowed data inside the flood range has been reached
						if (g_GlobalVars.pLogClientActions->iValue)
							ConsolePrint(FOREGROUND_YELLOW, "Client %d is flooding, server will kick client out\n", i + 1);

						SockClose(i, "Flooding");
						
						continue;
					}
				}
				
                events.OnClientWrite(i, ucRecvBuf, iDataLen); //Call event function
            }
        }
    }

    return;
}
//======================================================================

//======================================================================
bool CClientSocket::SockClose(CLIENTID id, const char* pszReason)
{
	//Remove a client fully from the server (by ID)

	if (!bReady)
		return false;

	clientinfo_s *pInfo = GetClientById(id); 
	if (!pInfo)
		return false;

	events.OnClientDisconnect(id, pszReason); //Call event function

	if (!pInfo->bIsBot) {
		shutdown(pInfo->hSocket, SD_BOTH); //Remove any recv/send activity of this socket
		closesocket(pInfo->hSocket);  //Free internal socket data
	}

	DeleteClient(id); //Remove client from our list
	
	return true;
}
//======================================================================

//======================================================================
bool CClientSocket::SockClose(clientinfo_s* pClient, const char* pszReason)
{
	//Remove a client fully from the server (by data pointer)

	if (!bReady)
		return false;

	if (!pClient)
		return false;

	CLIENTID clId = GetClientId(pClient);
	if (clId == INVALID_CLIENT_ID)
		return false;

	events.OnClientDisconnect(clId, pszReason); //Call event function

	if (!pClient->bIsBot) {
		shutdown(pClient->hSocket, SD_BOTH); //Remove any recv/send activity of this socket
		closesocket(pClient->hSocket);  //Free internal socket data
	}

	DeleteClient(clId); //Remove client from our list

	return true;
}
//======================================================================

//======================================================================
void CClientSocket::FreeClients(void)
{
	//For all clients free data and remove from list

	if (!bReady)
		return;

	if (vClients.size() == 0)
		return;

	//Remove all clients
	CLIENTAMOUNT i = (CLIENTAMOUNT)(vClients.size()-1);
	while (i) {
		SockClose(i, "");

		i = (CLIENTAMOUNT)(vClients.size()-1);
	}

	//Just to be sure...
	vClients.clear();
}
//======================================================================

//======================================================================
void CClientSocket::CalculateStuff(void)
{
	//Calculate ping, check how long a client has not logged in and calculate antiflood stuff

	if (!bReady)
		return;

	for (CLIENTAMOUNT i = 0; i < (CLIENTAMOUNT)vClients.size(); i++) {
		if ((vClients[i]) && (vClients[i]->antiflood.wDataCounter)) { //Check if the initial message for this flood delay range has been counted
			vClients[i]->antiflood.dwCur = GetTickCount(); //Update timer

			if (vClients[i]->antiflood.dwCur > vClients[i]->antiflood.dwLast + FLOOD_CHECK_DELAY) { //If we are not inside the flood delay range anymore
				vClients[i]->antiflood.wDataCounter = 0; //Clear flood message counter
			}
		}

		if ((vClients[i]) && (vClients[i]->bHasLoggedIn) && (!vClients[i]->bIsBot)) {
			//Increment ping counter
            vClients[i]->ping.pingCount++;
            
			//Update timer
			vClients[i]->ping.curPing = GetTickCount();
            if (vClients[i]->ping.curPing > vClients[i]->ping.lastPing + g_GlobalVars.pPingDelay->iValue * 1000) { //If wait time has elapsed
                vClients[i]->ping.pingAmount++; //Increment counter which counts every elapsed time

                if (vClients[i]->ping.pingAmount> g_GlobalVars.pPingTimeout->iValue * 60) { //Kick client if he did not answer the required ping request within the specified time
					SockClose(i, "Ping timeout");

                    return;
                }

                vClients[i]->ping.curPingVal = rand() % MAX_UNSIGNED_BYTE; //Calculate new ping value
				events.OnClientUpdatePing(i, vClients[i]->ping.curPingVal); //Pass new ping value to event function

                vClients[i]->ping.pingCount = 0; //Reset ping counter to zero since the new ping value is now used

                vClients[i]->ping.lastPing = GetTickCount();
            }
        } else if ((vClients[i]) && (!vClients[i]->bHasLoggedIn)) { //If a client is not successfully put into the server
			//Update login timer
			vClients[i]->logincount.dwCur = GetTickCount();

			if (vClients[i]->logincount.dwCur > vClients[i]->logincount.dwLast + MAX_LOGIN_WAIT_TIME) { //Check if maximum wait time has elapsed and remove client because he did not login within the given time
				SockClose(i, "Client did not login");
			}
		}
	}
}
//======================================================================

//======================================================================
void CClientSocket::CheckClientPing(CLIENTID id, BYTE ucPingValue)
{
	//Check if client has answered with the right ping value

	if (!bReady)
		return;

	clientinfo_s* pClient = GetClientById(id);
	if (!pClient)
		return;

	if ((pClient->bHasLoggedIn) && (pClient->ping.curPingVal == ucPingValue)) { //Client must have sent the proper ping value
		//Update client ping data
		pClient->ping.actualPing = pClient->ping.pingCount; //Average ping elapse count
		pClient->ping.pingAmount = 0; //Set counter which counts every elapsed time to zero
		pClient->ping.pingCount = 0; //Set ping wait counter to zero
		pClient->ping.curPingVal = 0; //Set current ping value to zero
	}
}
//======================================================================

//======================================================================
bool CClientSocket::AddressAlreadyInUse(DWORD dwIPAddr)
{
    //Check if an IP Address is already in use

	if (!bReady)
		return false;

    if (!dwIPAddr)
		return false;

	//Search in list for a client with the same IP-Address
    for (CLIENTAMOUNT i = 0; i < (CLIENTAMOUNT)vClients.size(); i++) {
        if (vClients[i] != NULL) {
            if (vClients[i]->saddr.sin_addr.s_addr == dwIPAddr) { //Compare IP addresses
                return true;
            }
        }
    }

    return false;
}
//======================================================================

//======================================================================
unsigned char CClientSocket::GetAmountOfAddress(DWORD dwIPAddr)
{
    //Get amount of clients connected with this single IP-Address

	if (!bReady)
		return 0;

	if (!dwIPAddr)
		return 0;

    unsigned char bResult = 0;

	//Search in list for a client with the same IP-Address
    for (unsigned int i = 0; i < vClients.size(); i++) {
        if (vClients[i] != NULL) {
            if (vClients[i]->saddr.sin_addr.s_addr == dwIPAddr) //If found increment counter
                bResult++;
		}
    }

    return bResult;
}
//======================================================================

//======================================================================
const CLIENTAMOUNT CClientSocket::GetClientCount(void)
{
	//Get amount of clients in list

	return (CLIENTAMOUNT)vClients.size();
}
//======================================================================

//======================================================================
clientinfo_s* CClientSocket::GetClientById(CLIENTID id)
{
	//Get client info pointer by ID

	if (!bReady)
		return NULL;

	if (id == INVALID_CLIENT_ID)
		return NULL;

    if (id < (CLIENTAMOUNT)vClients.size())
        return vClients[id];

    return NULL;
}
//======================================================================

//======================================================================
clientinfo_s* CClientSocket::GetClientByName(const char *pszName)
{
	//Get a client data pointer by login name

	if (!pszName)
		return NULL;

	//Search in client list for the client with the specified name
	for (CLIENTAMOUNT i = 0; i < GetClientCount(); i++) {
		clientinfo_s *pInfo = GetClientById(i);
		if (pInfo) {
			if (strcmp(pInfo->userinfo.szName, pszName)==0) {
				return pInfo;
			}
		}
	}

	return NULL;
}
//======================================================================

//======================================================================
int CClientSocket::GetClientId(clientinfo_s* pClient)
{
	//Get ID of a client

	if (!pClient)
		return INVALID_CLIENT_ID;

	//Search in list for the client data address
	for (CLIENTAMOUNT i = 0; i < (CLIENTAMOUNT)vClients.size(); i++) {
		if (vClients[i] == pClient) //If found then return the ID
			return i;
	}

	return INVALID_CLIENT_ID;
}
//======================================================================

//======================================================================
bool CClientSocket::SendBuf(CLIENTID id, const PVOID pBuf, DWORD dwBufSize, bool bSendDirect)
{
	//Put a buffer in the user message queue for this client or send it directly

	if (!bReady)
		return false;

	if ((!pBuf) || (!dwBufSize))
		return false;

    clientinfo_s *pInfo = GetClientById(id);
    if (!pInfo)
		return false;

	if (pInfo->bIsBot)
		return true;

    if (bSendDirect == 0) { //Send directly
        return send(pInfo->hSocket, (const char*)pBuf, dwBufSize, 0) != SOCKET_ERROR;
    } else { //Put message into queue
        void *pMem = malloc(dwBufSize); //Memory allocation
        if (pMem) {
            memcpy(pMem, pBuf, dwBufSize);

            um_queue_s queue = {0};
            queue.pBuffer = pMem; //Pointer to memory area
            queue.dwSize = dwBufSize; //Size of memory area
            queue.dwCurrent = GetTickCount(); //Current timer value
            queue.dwLast = GetTickCount(); //Last timer value

            pInfo->vUMsgs.push_back(queue); //Push entry to last onto last entry

            return true;
		}
    }

    return false;
}
//======================================================================

//======================================================================
bool CClientSocket::SendBuf(clientinfo_s* pClient, const PVOID pBuf, DWORD dwBufSize, bool bSendDirect)
{
	//Put a buffer in the user message queue for this client or send it directly

	if (!bReady)
		return false;

	if ((!pClient) || (!pBuf) || (!dwBufSize))
		return false;

	if (pClient->bIsBot)
		return true;

    if (bSendDirect == 0) { //Send directly
        return send(pClient->hSocket, (const char*)pBuf, dwBufSize, 0) != SOCKET_ERROR;
    } else { //Put message into queue
        void *pMem = malloc(dwBufSize); //Memory allocation
        if (pMem) {
            memcpy(pMem, pBuf, dwBufSize);

            um_queue_s queue = {0};
            queue.pBuffer = pMem; //Pointer to memory area
            queue.dwSize = dwBufSize; //Size of memory area
            queue.dwCurrent = GetTickCount(); //Current timer value
            queue.dwLast = GetTickCount(); //Last timer value

            pClient->vUMsgs.push_back(queue); //Push entry to last onto last entry

            return true;
		}
    }

    return false;
}
//======================================================================

//======================================================================
bool CClientSocket::BroadCast(const PVOID pBuf, DWORD dwBufSize, bool bSendDirect)
{
	//Broadcast data to all clients

	if (!bReady)
		return false;

	if ((!pBuf) || (!dwBufSize))
		return false;

	unsigned int dwResult = 0;;
	for (DWORD i = 0; i < vClients.size(); i++) { //Loop trough list
		if (SendBuf(i, pBuf, dwBufSize, bSendDirect)) //Send data
			dwResult++; //Increment counter for success check
	}

	return dwResult == GetClientCount(); //Return true if data has send to all clients
}
//======================================================================

//======================================================================
void CClientSocket::MessageConfirmed(CLIENTID clientid)
{
    //Confirm that a client has recieved the last message

	if (!bReady)
		return;

    clientinfo_s *pClient = GetClientById(clientid);
    if ((pClient) && (pClient->vUMsgs.size() > 0)) {
        free(pClient->vUMsgs[0].pBuffer); //Free memory from heap
        pClient->vUMsgs.erase(pClient->vUMsgs.begin() + 0); //Erase first entry
    }
}
//======================================================================

//======================================================================
void CClientSocket::ProcessMessages(void)
{
    //Process User Message sending for all clients

	if (!bReady)
		return;

    for (CLIENTAMOUNT i = 0; i < (CLIENTAMOUNT)vClients.size(); i++) {
        clientinfo_s* pClient = GetClientById(i); //Get pointer to client data
        if ((pClient) && (pClient->vUMsgs.size())) { //If an entry in queue exists
            pClient->vUMsgs[0].dwCurrent = GetTickCount(); //Refresh timer
			if (pClient->vUMsgs[0].dwCurrent > pClient->vUMsgs[0].dwLast + UM_QUEUE_WAIT_TIME) { //If time is over
                send(pClient->hSocket, (const char*)pClient->vUMsgs[0].pBuffer, pClient->vUMsgs[0].dwSize, 0); //Send first entrys data

                pClient->vUMsgs[0].dwLast = GetTickCount(); //Refresh
            }
        }
    }
}
//======================================================================

//======================================================================
void CClientSocket::FreeClientUMsgs(CLIENTID id)
{
	//Remove all usermsg queue data of this client

	for (CLIENTAMOUNT i = 0; i < (CLIENTAMOUNT)vClients[id]->vUMsgs.size(); i++)
		free(vClients[id]->vUMsgs[i].pBuffer);

	vClients[id]->vUMsgs.clear();
}
//======================================================================

