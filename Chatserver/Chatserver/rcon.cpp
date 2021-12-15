#include "rcon.h"
#include "vars.h"
#include "utils.h"
#include "versionfuncs.h"

/*
	CorvusChat Server (dnyCorvusChat) - Chatserver component

	Developed by Daniel Brendel

	Version: 0.5
	Visit: https://github.com/danielbrendel
	Mail dbrendel1988@gmail.com

	File: rcon.cpp: Remote control manager implementation 
*/

//======================================================================
BOOL CRemoteControl::Initialize(WORD wPort, r_print_output_s* pPrintOutput)
{
	//Initialize the component

	if (bIsReady)
		return TRUE;

	if ((!wPort) || (!pPrintOutput))
		return FALSE;

	//Create UDP protocol based socket
	hSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (hSocket == SOCKET_ERROR)
		return FALSE;

	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(wPort);
	saddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	//Bind socket to information to have a server socket
	if (bind(hSocket, (sockaddr*)&saddr, sizeof(saddr)) == SOCKET_ERROR)
		return FALSE;

	bIsReady = true;
	
	rcondata.pPrintOutput = pPrintOutput;

	//Set socket to be nonblocking
	DWORD dwNonBlocking = 1;
	return ioctlsocket(hSocket, FIONBIO, &dwNonBlocking) != SOCKET_ERROR;
}
//======================================================================

//======================================================================
BOOL CRemoteControl::Clear(VOID)
{
	//Clear data

	if (!bIsReady)
		return TRUE;

	//Shutdown socket
	if (shutdown(hSocket, SD_BOTH) == SOCKET_ERROR)
		return FALSE;

	//Clear internal socket data
	if (closesocket(hSocket) == SOCKET_ERROR)
		return FALSE;

	memset(&rcondata, 0x00, sizeof(rcondata));

	bIsReady = false;

	return TRUE;
}
//======================================================================

//======================================================================
VOID CRemoteControl::Process(CConCommand* pConCommand)
{
	//Process communication

	if (!pConCommand)
		return;

	static BYTE ucCmdSInfoIdent[] = {0x01, 'C', 'C', 'I', 'N', 'F', 'O', 0xFF}; //Command identification buffer for server info
	static BYTE ucCmdRConIdent[] = {0x02, 'C', 'C', 'R', 'C', 'O', 'N', 0xFF}; //Command identification buffer for rcon
	static BYTE ucConType = 'c';
	static BYTE ucScriptType = 's';

	char szBuffer[sizeof(ucCmdRConIdent) + MAX_PASSWORD_LEN + 1 + MAX_RCON_BUFFER];

	int iToLen = sizeof(rcondata.saddr);

	//Check if there is incoming client data
	if (recvfrom(hSocket, szBuffer, sizeof(szBuffer), 0, (sockaddr*)&rcondata.saddr, &iToLen) != SOCKET_ERROR) {
		if (!memcmp(szBuffer, ucCmdSInfoIdent, sizeof(ucCmdSInfoIdent))) { //Check for server info identifier
			serverinfo_udp_s si;
			strcpy_s(si.ident, "INFO");
			si.ident[4] = 0xFF;
			strcpy_s(si.name, g_GlobalVars.pServerName->szValue);
            si.port = (WORD)g_GlobalVars.pClientPort->iValue;
            si.version = SERVERVERSION();
            si.ifver = IFACEVERSION();
            strcpy_s(si.fclient, g_GlobalVars.pForcedClient->szValue);
            si.maxclients = (DWORD)g_GlobalVars.pMaxUsers->iValue;
            si.curclients = ENG_GetClientCount();
            si.maxchans = (DWORD)g_GlobalVars.pMaxChannels->iValue;
            si.curchanamount = ENG_GetChannelCount();
            si.bPassword = strcmp(g_GlobalVars.pServerPW->szValue, "#0") != 0;
            si.uptime = g_GlobalVars.dwElapsedHours;

			sendto(hSocket, (const char*)&si, sizeof(si), 0, (sockaddr*)&rcondata.saddr, sizeof(sockaddr_in));
		} else if (!memcmp(szBuffer, ucCmdRConIdent, sizeof(ucCmdRConIdent))) { //Check for RCON identifier
			char* szPassword = &szBuffer[sizeof(ucCmdRConIdent)];
			if ((IsValidStrLen(szPassword, MAX_PASSWORD_LEN)) && (strcmp(szPassword, g_GlobalVars.pRConPW->szValue)==0)) { //Check password length and password text
				char cType = szBuffer[sizeof(ucCmdRConIdent) + MAX_PASSWORD_LEN];
				char* szCommand = &szBuffer[sizeof(ucCmdRConIdent) + MAX_PASSWORD_LEN + sizeof(char)];
				if ((szCommand[0]) && (strlen(szCommand) < (MAX_RCON_BUFFER - sizeof(ucCmdRConIdent) - MAX_PASSWORD_LEN - sizeof(char)))) { //Check command text to be in valid size
					if (cType == ucConType) {
						rcondata.pPrintOutput->type = R_PO_RCON; //Activate rcon handling of ConsolePrint()
						pConCommand->HandleCommand(szCommand); //Handle the given remote control command
						rcondata.pPrintOutput->type = R_PO_STDOUT; //Disable rcon handling of ConsolePrint()
					} else if (cType == ucScriptType) {
						//Pass buffer to config manager component
						rcondata.pPrintOutput->type = R_PO_RCON; //Activate rcon handling of EchoEvent()
						g_Objects.oConfigInt.Parse(szCommand);
						rcondata.pPrintOutput->type = R_PO_STDOUT; //Disable rcon handling of EchoEvent()
					}
				}
			}
		}
	}
}
//======================================================================

//======================================================================
BOOL CRemoteControl::SendText(LPCSTR lpszRemoteText)
{
	//Send text string to the specified client

	if (!bIsReady)
		return FALSE;

	//Send data to client
	return sendto(hSocket, lpszRemoteText, (int)strlen(lpszRemoteText) + 1, 0, (sockaddr*)&rcondata.saddr, sizeof(rcondata.saddr)) != SOCKET_ERROR;
}
//======================================================================

