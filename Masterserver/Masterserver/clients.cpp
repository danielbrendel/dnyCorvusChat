#include "clients.h"
#include "clientdata.h"

/*
	CorvusChat Server (dnyCorvusChat) - Masterserver component

	Developed by Daniel Brendel

	Version: 0.2
	Visit: https://github.com/danielbrendel
	Mail dbrendel1988@gmail.com

	File: client.cpp: Client handler implementation 
*/

//======================================================================
bool CClient::Initialize(WORD wPort, TpfnOnRecieveData pfnCallbackFunc)
{
	//Initialize this component instance

	 if (bReady)
        return true;

    if ((!wPort) || (!pfnCallbackFunc))
        return false;

	this->pfnCallbackFunc = pfnCallbackFunc;

     WSADATA wsa;
     if (WSAStartup(MAKEWORD(2, 0), &wsa) == SOCKET_ERROR) //Initialize WinSock
         return false;

     //Setup socket address data
     saddr.sin_family = AF_INET;
     saddr.sin_port = htons(wPort);
     saddr.sin_addr.s_addr = htonl(INADDR_ANY);

     //Create TCP socket
     hSocket = socket(AF_INET, SOCK_STREAM, 0);
     if (hSocket == SOCKET_ERROR)
         return false;

	 //Bind socket descriptor to socket address data, create the server
     if (bind(hSocket, (sockaddr*)&saddr, sizeof(sockaddr_in)) == SOCKET_ERROR) {
        closesocket(hSocket);
        return false;
     }

     //Set to non blocking
     u_long NonBlocking = 1;
     ioctlsocket(hSocket, FIONBIO, &NonBlocking);

     bReady = true;

     return bReady;
}
//======================================================================

//======================================================================
void CClient::Close(void)
{
	//Clean up everything

    if (!bReady)
        return;

    RemoveAll();

    bReady = false;
}
//======================================================================

//======================================================================
void CClient::RemoveAll(void)
{
	//Shutdown/Close client sockets and free memory

	if (!bReady)
		return;

	for (unsigned int i = 0; i < vClients.size(); i++) {
		shutdown(vClients[i]->socket, 2);
		closesocket(vClients[i]->socket);

		delete vClients[i];
	}

	vClients.clear();
}
//======================================================================

//======================================================================
bool CClient::AddClient(SOCKET hSocket, sockaddr_in *pSAddr)
{
    //Add a new client

    if (!bReady)
        return false;

    if ((hSocket == SOCKET_ERROR) || (!pSAddr))
        return false;

	//Allocate memory and set data

    clientinfo_s* pInfo = new clientinfo_s;
    if (!pInfo)
        return false;

    pInfo->socket = hSocket;
    memcpy(&pInfo->saddr, pSAddr, sizeof(sockaddr_in));
    pInfo->bServer = false;
    pInfo->timer.dwCurCount = GetTickCount();
    pInfo->timer.dwLastCount = GetTickCount();
    pInfo->timer.dwUpdateCount = 0;

    vClients.push_back(pInfo);

    return true;
}
//======================================================================

//======================================================================
void CClient::RemoveClient(unsigned int id)
{
     //Shutdown/Close client socket and free memory

     if (!bReady)
        return;

     if (id < vClients.size()) {
		shutdown(vClients[id]->socket, 2);
		closesocket(vClients[id]->socket);

        delete vClients[id];

        vClients.erase(vClients.begin() + id);
     }
}
//======================================================================

//======================================================================
void CClient::WaitForClients(void)
{
    //Wait for clients

    if (!bReady)
        return;

    SOCKET hTmpSock = 0;
    sockaddr_in stmpaddr;

    int iLen = sizeof(sockaddr_in);

    if (listen(hSocket, 5) != SOCKET_ERROR) { //Five clients in queue.
        hTmpSock = accept(hSocket, (sockaddr*)&stmpaddr, &iLen);
        if (hTmpSock != SOCKET_ERROR) {
			//Add to list
            if (!AddClient(hTmpSock, &stmpaddr)) {
				//On failure shutdown/close socket
				shutdown(hTmpSock, 2);
				closesocket(hTmpSock);
			}
        }
    }
}
//======================================================================

//======================================================================
void CClient::RecieveData(void)
{
     //Handle incoming data from a client

    if (!bReady)
        return;

    BYTE ucBuf[sizeof(serverinfo_s) + sizeof(CDIDENT)];

    for (unsigned int i = 0; i < vClients.size(); i++) {
        clientinfo_s *pClient = GetClientById(i);
        if (pClient) {
		   int iRecvLen = recv(pClient->socket, (char*)ucBuf, sizeof(serverinfo_s) + sizeof(CDIDENT), 0); //Check for incoming data from this client
           if (iRecvLen != SOCKET_ERROR) {
				//Ignore chatservers alive buffer
				if (ucBuf[0] == 0xFF)
					continue;

				pfnCallbackFunc(i, ucBuf, iRecvLen); //Call event function
           }
        }
    }
}
//======================================================================

//======================================================================
void CClient::CheckClientsAlive(void)
{
     //Check if clients are alive

     if (!bReady)
        return;

	 static const unsigned char ping[] = {0xFF, 0x00};

     for (unsigned int i = 0; i < vClients.size(); i++) {
		clientinfo_s *pInfo = GetClientById(i);
		if (pInfo) {
			send(pInfo->socket, (const char*)&ping[0], sizeof(ping), 0);
		}
	}
}
//======================================================================

//======================================================================
void CClient::CheckServerUpdate(void)
{
        //Calculate timers. A chatserver should update itself within the given time, else he will be removed from list

        if (!bReady)
            return;

         if (!bOnce) {
             //Initialize timer values
             dwCurCount = dwLastCount = GetTickCount();

             bOnce = true;
         }

         dwCurCount = GetTickCount();

         if (dwCurCount > dwLastCount + MAX_WAIT_TIME) {
             for (unsigned int i = 0; i < vClients.size(); i++) {
                 clientinfo_s *pInfo = GetClientById(i);
                 if ((pInfo) && (pInfo->bServer)) {
                     pInfo->timer.dwUpdateCount++;  //Increment the value we use to check how often we are called

                     if (pInfo->timer.dwUpdateCount > MAX_UPDATE_COUNT) { //Close client connection if he doesn't update itself within the given time
                        RemoveClient(i);
                     }
                 }
             }

             dwLastCount = GetTickCount();
         }
}
//======================================================================

//======================================================================
clientinfo_s *CClient::GetClientById(unsigned int id)
{
    //Returns a player info data pointer by id

    if (!bReady)
        return NULL;

    if (id < vClients.size())
        return vClients[id];

    return NULL;
}
//======================================================================

//======================================================================
unsigned int CClient::GetClientCount(void)
{
    //Returns the amount of connected clients

    if (!bReady)
        return 0;

    return (unsigned int)vClients.size();
}
//======================================================================

//======================================================================
unsigned int CClient::GetServerCount(void)
{
    //Returns the amount of connected servers

    if (!bReady)
        return 0;

    unsigned int result = 0;

    for (unsigned int i = 0; i < vClients.size(); i++) {
        if (vClients[i]->bServer)
            result++;
    }

    return result;
}
//======================================================================

//======================================================================
bool CClient::SendBuf(clientinfo_s *pClient, const unsigned char *pBuf, int iSize)
{
     //Send a buffer to a client (by data pointer)

    if (!bReady)
        return false;

    if ((!pClient) || (!pBuf) || (!iSize))
        return false;

    u_long nonBlocking;

    nonBlocking = 0;
    ioctlsocket(hSocket, FIONBIO, &nonBlocking);

    bool bRet = send(pClient->socket, (const char*)pBuf, iSize, 0) != SOCKET_ERROR;

    nonBlocking = 1;
    ioctlsocket(hSocket, FIONBIO, &nonBlocking);

    return bRet;
}
//======================================================================

//======================================================================
bool CClient::SendBuf(DWORD dwClientID, const unsigned char *pBuf, int iSize)
{
	//Send a buffer to a client (by ID)

	if (!bReady)
        return false;

    if ((dwClientID >= vClients.size()) || (!vClients[dwClientID]) || (!pBuf) || (!iSize))
        return false;

	u_long nonBlocking;

    nonBlocking = 0;
    ioctlsocket(vClients[dwClientID]->socket, FIONBIO, &nonBlocking);

    bool bRet = send(vClients[dwClientID]->socket, (const char*)pBuf, iSize, 0) != SOCKET_ERROR;

    nonBlocking = 1;
    ioctlsocket(vClients[dwClientID]->socket, FIONBIO, &nonBlocking);

    return bRet;
}
//======================================================================

//======================================================================
unsigned int CClient::GetClientId(clientinfo_s *pClient)
{
     //Shuts down a client connection

    if (!bReady)
        return INVALID_CLIENT_ID;

    if (!pClient)
		return INVALID_CLIENT_ID;

	//Search in list
	for (unsigned int i = 0; i < vClients.size(); i++) {
		if (vClients[i] == pClient)
			return i;
	}

	return INVALID_CLIENT_ID;
}
//======================================================================
