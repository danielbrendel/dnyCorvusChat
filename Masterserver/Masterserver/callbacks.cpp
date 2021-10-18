#include "callbacks.h"
#include "vars.h"

/*
	CorvusChat Server (dnyCorvusChat) - Masterserver component

	Developed by Daniel Brendel

	Version: 0.2
	Visit: https://github.com/danielbrendel
	Mail dbrendel1988@gmail.com

	File: callbacks.h: Callback functions implementation
*/

//======================================================================
void OnRecieveClientData(DWORD dwClientID, const BYTE* pucData, DWORD dwBufSize)
{
	//Called when recieving client data, pass to handle method
	
	g_Objects.ClientData.HandleData(dwClientID, pucData, dwBufSize);
}
//======================================================================

//======================================================================
void CDH_ServerInfo(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize)
{
	//A client wants to get the server list

	clientinfo_s* pClient = g_Objects.Client.GetClientById(dwClientID);
	if ((!pClient) || (pClient->bServer)) //Also servers don't need to get a server list
		return;

	for (unsigned int i = 0; i < g_Objects.Client.GetClientCount(); i++) {
		clientinfo_s *pInfo = g_Objects.Client.GetClientById(i);
		if ((pInfo) && (pInfo->bServer)) {

			//Send server list entry to client
			if (g_Objects.UserMsg.MessageBegin(g_Objects.UserMsg.GetMessageByName("SendServerInfo"), dwClientID)) {
				g_Objects.UserMsg.WriteBuf(&pInfo->sinfo, sizeof(serverinfo_s));
				printf("Sending list: %s\n", pInfo->sinfo.szName);
				g_Objects.UserMsg.MessageEnd(&g_Objects.Client);
			}

			Sleep(50);
		}
	}

	g_Objects.Client.RemoveClient(dwClientID); //There is no need for a longer lasting connection with this client now
}
//======================================================================

//======================================================================
ULONG GetLocalhostAddr(DWORD dwEntryId);
void CDH_AddToServerList(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize)
{
	//A client wants to add itself as a chatserver

	if (dwBufSize != sizeof(serverinfo_s) - 1)
		return;

	clientinfo_s* pClient = g_Objects.Client.GetClientById(dwClientID);
	if (!pClient) 
		return;

	pClient->bServer = true;
	memcpy(&pClient->sinfo, (LPCVOID)((DWORD_PTR)pBuffer+1), dwBufSize);

	if (pClient->saddr.sin_addr.s_addr == GetLocalhostAddr(g_ucLocalhostId))
		memcpy(&pClient->sinfo.ucaAddr, &g_dwLocalAlias, 4);
	else
		memcpy(&pClient->sinfo.ucaAddr, &pClient->saddr.sin_addr.s_addr, 4);

	pClient->timer.dwUpdateCount = 0;
}
//======================================================================

//======================================================================
void Cmd_About(CConParser* pParser)
{
	//Show about information

	printf(CMD_STDOUT_PREFIX " " PROGRAM_NAME " v" PROGRAM_VERSION " developed by " PROGRAM_AUTHOR " (" PROGRAM_CONTACT ") \n");
}
//======================================================================

//======================================================================
const char* AddressToString(const unsigned long ulIPAddress);
void Cmd_TryLocalhost(CConParser* pParser)
{
	//Try a localhost list ID

	const char* szArg = pParser->GetArgument(1);
	if (!szArg) {
		printf(CMD_STDOUT_PREFIX "You have to specify an ID (0-n)\n");
		return;
	}

	g_ucLocalhostId = (BYTE)atoi(szArg);

	ULONG ulLocalHostAddr = GetLocalhostAddr(g_ucLocalhostId);
	if (!ulLocalHostAddr) {
		printf(CMD_STDOUT_PREFIX "[Warining] GetLocalhostAddr returned 0, trying to use ID 0 instead of %d\n", g_ucLocalhostId);

		ulLocalHostAddr = GetLocalhostAddr(0);
		g_ucLocalhostId = 0;
	}

	if (!ulLocalHostAddr)
		printf(CMD_STDOUT_PREFIX "[Warining] GetLocalhostAddr failed with ID 0\n");

	printf(CMD_STDOUT_PREFIX "Trying ID: %d (%s)\n", g_ucLocalhostId, AddressToString(ulLocalHostAddr));
}
//======================================================================

//======================================================================
void Cmd_Localalias(CConParser* pParser)
{
	//Set the localhost alias

	const char* szArg = pParser->GetArgument(1);
	if (!szArg) {
		printf(CMD_STDOUT_PREFIX "You have to specify an alias\n");
		return;
	}

	g_dwLocalAlias = inet_addr(szArg);
	
	(g_dwLocalAlias) ? printf(CMD_STDOUT_PREFIX "Using localhost alias: %s\n", szArg) : printf(CMD_STDOUT_PREFIX "Invalid localhost alias\n");
}
//======================================================================

//======================================================================
void Cmd_ListClients(CConParser* pParser)
{
	//List all clients

	if (!g_Objects.Client.GetServerCount()) {
		printf(CMD_STDOUT_PREFIX "There are no servers in list currently\n");
		return;
	}

	printf(CMD_STDOUT_PREFIX "Total amount: %d servers\n", g_Objects.Client.GetServerCount());

	for (unsigned int i = 0; i < g_Objects.Client.GetClientCount(); i++) {
		clientinfo_s *pInfo = g_Objects.Client.GetClientById(i);
		if ((pInfo) && (pInfo->bServer))
			printf(CMD_STDOUT_PREFIX "#%d) %d.%d.%d.%d:%d  Version: %d.%d | Name: %s | FClient: %s | Clients: %d / %d | Protected: %d | Uptime: %d\n", i + 1, *(unsigned char*)&pInfo->saddr.sin_addr.s_addr, *(unsigned char*)((unsigned int)&pInfo->saddr.sin_addr.s_addr + 1), *(unsigned char*)((unsigned int)&pInfo->saddr.sin_addr.s_addr + 2), *(unsigned char*)((unsigned int)&pInfo->saddr.sin_addr.s_addr + 3), pInfo->sinfo.usPort, pInfo->sinfo.ucVer[0], pInfo->sinfo.ucVer[1], pInfo->sinfo.szName, pInfo->sinfo.szForcedClient, pInfo->sinfo.uiCurClients, pInfo->sinfo.uiMaxclients, pInfo->sinfo.bPassword, pInfo->sinfo.uiUptime);
	}
}
//======================================================================

//======================================================================
void Cmd_Quit(CConParser* pParser)
{
	//Force program shutdown

	g_bServerRunning = false;
}
//======================================================================
