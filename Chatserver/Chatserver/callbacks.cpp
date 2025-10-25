#include "callbacks.h"
#include "vars.h"
#include "utils.h"
#include "log.h"
#include "versionfuncs.h"

/*
	CorvusChat Server (dnyCorvusChat) - Chatserver component

	Developed by Daniel Brendel

	Version: 0.5
	Visit: https://github.com/danielbrendel
	Mail dbrendel1988@gmail.com

	File: callbacks.cpp: Callback function implementations 
*/

//======================================================================
void FatalError(const char* szErrMsg, ...);
VOID ConsolePrint(WORD wConAttributes, LPCSTR lpszFmt, ...)
{
	//Print text either to stdout or send to remote controlling client

	if (!lpszFmt)
		return;

	//Get handle to standard output and standard console text attributes at first call
	//Regarding the text attributes we use the current one. Please don't change it
	//outside of this function to ensure having the standard values.
	static bool bOnce = false;
	static HANDLE hStdOut = 0;
	static CONSOLE_SCREEN_BUFFER_INFO csbi = {0};
	if (!bOnce) {
		bOnce = true;

		//Get handle to standard output
		hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
		if (!hStdOut)
			FatalError("Error: GetStdHandle failed: %d", GetLastError());

		//Get standard console screen buffer info
		if (!GetConsoleScreenBufferInfo(hStdOut, &csbi))
			FatalError("Error: GetConsoleScreenBufferInfo failed: %d", GetLastError());
	}

	char szFmtBuffer[512];
	char szOutputBuffer[512];

	//Format buffer with arguments
	va_list vaList;
	va_start(vaList, lpszFmt);
	vsprintf_s(szFmtBuffer, lpszFmt, vaList);
	va_end(vaList);

	long long ulValue = (g_GlobalVars.pOutputPrefix) ? g_GlobalVars.pOutputPrefix->iValue : 0;

	//Format output depending on the prefix value
	switch (ulValue) {
	case 1: {
		const char *szDate = GetCurrentDate();
		if (szDate) {
				sprintf_s(szOutputBuffer, "[%s] %s", szDate, szFmtBuffer);
		}
		break;
		}
	case 2: {
		const char *szTime = GetCurrentTime();
		if (szTime) {
			sprintf_s(szOutputBuffer, "[%s] %s", szTime, szFmtBuffer);
		}
		break;
		}
	case 3: {
		const char *szDateTime = GetCurrentDateTime();
		if (szDateTime) {
			sprintf_s(szOutputBuffer, "[%s] %s", szDateTime, szFmtBuffer);
		}
		break;
		}
	default:
		strcpy_s(szOutputBuffer, szFmtBuffer);
		break;
	}

	if (g_PrintOutput.type == PO_STDOUT) { //Send text to stdout and log to hard disc (if desired)
		//Correct attribute argument if equal to zero
		if (wConAttributes == CONSOLE_ATTRIBUTE_DEFAULT)
			wConAttributes = csbi.wAttributes;

		//Set console text attributes
		if (!SetConsoleTextAttribute(hStdOut, wConAttributes))
			FatalError("Error: SetConsoleTextAttribute failed: %d", GetLastError());

		std::cout << szOutputBuffer;

		//Restore old console text attributes
		if (!SetConsoleTextAttribute(hStdOut, csbi.wAttributes))
			FatalError("Error: SetConsoleTextAttribute failed: %d", GetLastError());

		int iLDValue = (g_GlobalVars.pLogToDisc) ? g_GlobalVars.pLogToDisc->iValue : 0;
		if (iLDValue) {
			LogMessage("%s", szOutputBuffer);
		}
	} else if (g_PrintOutput.type == PO_RCON) { //Send text to the rcon using client
		g_Objects.RCon.SendText(szOutputBuffer);
	}
}
//======================================================================

//======================================================================
BOOL ServerNotice(GLOBALID gidReciever, SEND_TYPE sendType, LPCSTR lpszMessage, DWORD uiBufSize = MAX_NETWORK_STRING_LENGTH)
{
	//Broadcast a server notice message

	if ((!lpszMessage) || (!uiBufSize))
		return FALSE;

	if (g_Objects.UserMsg.MessageBegin(g_Objects.UserMsg.GetMessageByName("ServerNotice"), gidReciever, sendType)) {
		g_Objects.UserMsg.WriteString((char*)lpszMessage, uiBufSize);

		return g_Objects.UserMsg.MessageEnd(&g_Objects.ClientSock);
	}

	return FALSE;
}
//======================================================================

//======================================================================
bool Event_OnClientConnect(const CLIENTID ciClientID)
{
	//Called for every client connect

	return ENG_ClientConnect(ciClientID);
}
//======================================================================

//======================================================================
void Event_OnClientDisconnect(const CLIENTID ciClientID, const char* pszReason)
{
	//Called for every client disconnect
	
	ENG_ClientDisconnect(ciClientID, pszReason);
}
//======================================================================

//======================================================================
void Event_OnClientWrite(const CLIENTID ciClientID, const BYTE* pBuffer, DWORD dwSize)
{
	//Called when recieving incoming client data

	g_Objects.ClientData.HandleData(ciClientID, pBuffer, dwSize);
}
//======================================================================

//======================================================================
void Event_OnClientUpdatePing(const CLIENTID ciClientID, BYTE ucPingValue)
{
	//Called if new ping value must be sent to a client

	int iMsg = g_Objects.UserMsg.GetMessageByName("Ping");
	if (iMsg != INVALID_USERMSG) {
		if (g_Objects.UserMsg.MessageBegin(iMsg, ciClientID, ST_USER)) {
			g_Objects.UserMsg.WriteByte(ucPingValue);
			g_Objects.UserMsg.MessageEnd(&g_Objects.ClientSock);
		} 
	}
}
//======================================================================

//======================================================================
void Event_OnErrorOccured(DWORD dwErrorValue)
{
	//Called when an error occured

	ConsolePrint(FOREGROUND_RED, "Socket Error occured: %d\n", dwErrorValue);
}
//======================================================================

//======================================================================
DWORD CCSAPI ENG_GetServerVersion(void)
{
	//Detour to: SERVERVERSION

	return SERVERVERSION();
}
//======================================================================

//======================================================================
DWORD CCSAPI ENG_GetInterfaceVersion(void)
{
	//Detour to: IFACEVERSION

	return IFACEVERSION();
}
//======================================================================

//======================================================================
void CCSAPI ENG_ConsolePrint(LPCSTR lpszFmt, ...)
{
	//Detour to: ConsolePrint

	if (!lpszFmt)
		return;

	char szBuffer[250];

	va_list vaList;
	va_start(vaList, lpszFmt);
	vsprintf_s(szBuffer, lpszFmt, vaList);
	va_end(vaList);

	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, "%s", szBuffer);
}
//======================================================================

//======================================================================
void FatalError(const char* szErrMsg, ...);
void CCSAPI ENG_FatalError(const char* szErrMsg, ...)
{
	//Detour to: FatalError

	if (!szErrMsg)
		return;

	char szBuffer[250];

	va_list vaList;
	va_start(vaList, szErrMsg);
	vsprintf_s(szBuffer, szErrMsg, vaList);
	va_end(vaList);

	FatalError("%s", szBuffer);
}
//======================================================================

//======================================================================
void CCSAPI ENG_LogMessage(const char *lpszMessage, ...)
{
	//Detour to: LogMessage

	if (!lpszMessage)
		return;

	char szBuffer[250];

	va_list vaList;
	va_start(vaList, lpszMessage);
	vsprintf_s(szBuffer, lpszMessage, vaList);
	va_end(vaList);

	LogMessage("%s", szBuffer);
}
//======================================================================

//======================================================================
void CCSAPI ENG_LogError(const char *lpszError, ...)
{
	//Detour to: LogError

	if (!lpszError)
		return;

	char szBuffer[250];

	va_list vaList;
	va_start(vaList, lpszError);
	vsprintf_s(szBuffer, lpszError, vaList);
	va_end(vaList);

	LogError("%s", szBuffer);
}
//======================================================================
 
//======================================================================
bool CCSAPI ENG_ClientConnect(const CLIENTID ciClientId)
{
	//A client connected

	//Check for plugin event functions
	for (PLUGINID pid = 0; pid < g_Objects.Plugins.GetPluginCount(); pid++) {
        plugin_s* pPlugin = g_Objects.Plugins.GetPluginById(pid);
        if ((pPlugin) && (!pPlugin->bPaused) && (pPlugin->evttable.events_pre.OnClientConnect)) { //Check for valid plugin pointer, if plugin is paused and if event function pointer is assigned
			plugin_result res = PLUGIN_CONTINUE;

			bool bRetValue;

			//Call event function
			res = pPlugin->evttable.events_pre.OnClientConnect(ciClientId, &bRetValue);

            if (res == PLUGIN_BREAK) //Prevent all event function calls of the following plugins if desired
				break;
            else if (res == PLUGIN_RETURN) //Return from this function if desired
				return bRetValue;
        }
    }

	//Get client info data pointer
	clientinfo_s* pClient = ENG_GetClientById(ciClientId);
	if (!pClient)
		return false;

	//Check if client IP is banned
	if (g_Objects.BanList.EntryExists(pClient->saddr.sin_addr.S_un.S_addr))
		return false;

	//Check for user amount
	if (ENG_GetClientCount() >= (CLIENTAMOUNT)g_GlobalVars.pMaxUsers->iValue)
		return false;

	//Send request for login
	if (g_Objects.UserMsg.MessageBegin(g_Objects.UserMsg.GetMessageByName("RequestLogin"), ciClientId, ST_USER)) {
		g_Objects.UserMsg.MessageEnd(&g_Objects.ClientSock);
	}

	//Send server version
	if (g_Objects.UserMsg.MessageBegin(g_Objects.UserMsg.GetMessageByName("Serverversion"), ciClientId, ST_USER)) {
		g_Objects.UserMsg.WriteInt(SERVERVERSION());

		g_Objects.UserMsg.MessageEnd(&g_Objects.ClientSock);
	}
	
	if (g_GlobalVars.pLogClientActions->iValue)
		ConsolePrint(FOREGROUND_YELLOW, "Client with ID %d has connected to the server\n", ciClientId + 1);

	//Check for plugin event functions
	for (PLUGINID pid = 0; pid < g_Objects.Plugins.GetPluginCount(); pid++) {
        plugin_s* pPlugin = g_Objects.Plugins.GetPluginById(pid);
        if ((pPlugin) && (!pPlugin->bPaused) && (pPlugin->evttable.events_post.OnClientConnect)) { //Check for valid plugin pointer, if plugin is paused and if event function pointer is assigned
			plugin_result res = PLUGIN_CONTINUE;

			bool bRetValue;

			//Call event function
			res = pPlugin->evttable.events_post.OnClientConnect(ciClientId, &bRetValue);

            if (res == PLUGIN_BREAK) //Prevent all event function calls of the following plugins if desired
				break;
            else if (res == PLUGIN_RETURN) //Return from this function if desired
				return bRetValue;
        }
    }

	return true;
}
//======================================================================

//======================================================================
bool CCSAPI ENG_ClientJoined(const CLIENTID ciClientId, const login_s* pLoginInfo)
{
	//A client has joined the server. Note: This returns false if the join is denied. The caller must disconnect the client then

	//Check for plugin event functions
	for (PLUGINID pid = 0; pid < g_Objects.Plugins.GetPluginCount(); pid++) {
        plugin_s* pPlugin = g_Objects.Plugins.GetPluginById(pid);
        if ((pPlugin) && (!pPlugin->bPaused) && (pPlugin->evttable.events_pre.OnClientEntered)) { //Check for valid plugin pointer, if plugin is paused and if event function pointer is assigned
			plugin_result res = PLUGIN_CONTINUE;

			bool bRetValue;

			//Call event function
			res = pPlugin->evttable.events_pre.OnClientEntered(ciClientId, pLoginInfo, &bRetValue);

            if (res == PLUGIN_BREAK) //Prevent all event function calls of the following plugins if desired
				break;
            else if (res == PLUGIN_RETURN) //Return from this function if desired
				return bRetValue;
        }
    }

	if ((ciClientId <= INVALID_CLIENT_ID) || (ciClientId >= ENG_GetClientCount()) || (!pLoginInfo))
		return false;

	//Get client info data pointer
	clientinfo_s* pClient = ENG_GetClientById(ciClientId);
	if (!pClient)
		return false;

	//Check if already logged in
	if (pClient->bHasLoggedIn)
		return true;

	//Check for server password
	if ((strcmp(g_GlobalVars.pServerPW->szValue, S_NO_PASSWORD)) && (strcmp(g_GlobalVars.pServerPW->szValue, pLoginInfo->szPassword)))
		return false;

	//Check for forced client
	if ((strcmp(g_GlobalVars.pForcedClient->szValue, S_NO_FORCED_CLIENT)) && (strcmp(pLoginInfo->szClient, g_GlobalVars.pForcedClient->szValue)))
		return false;

	//Check for valid name string length
	if (!IsValidString(pLoginInfo->szName))
		return false;

	//Check if name is already in use
	if (ClientNameAlreadyUsed(pLoginInfo->szName))
		return false;

	//Set new data

	pClient->bHasLoggedIn = true;

	strcpy_s(pClient->userinfo.szName, pLoginInfo->szName);
	strcpy_s(pClient->userinfo.szClanTag, pLoginInfo->szClanTag);
	strcpy_s(pClient->userinfo.szClient, pLoginInfo->szClient);

	//Send MOTD to client
	if (g_Objects.UserMsg.MessageBegin(g_Objects.UserMsg.GetMessageByName("MOTD"), ciClientId, ST_USER)) {
		for (DWORD i = 0; i < g_Objects.MOTD.GetLineCount(); i++) {
			char* szLine = g_Objects.MOTD.GetLine(i);
			if (szLine) {
				//Handle dynvars
				std::string strDynVar = g_Objects.DynVars.HandleExpression(szLine) + "\x04";

				g_Objects.UserMsg.WriteString((char*)strDynVar.c_str(), (DWORD)strDynVar.length());
			}
		}

		g_Objects.UserMsg.MessageEnd(&g_Objects.ClientSock);
	} 

	//Send AOTD to client
	if (g_Objects.UserMsg.MessageBegin(g_Objects.UserMsg.GetMessageByName("AOTD"), ciClientId, ST_USER)) {
		g_Objects.UserMsg.WriteBuf(&g_GlobalVars.aotd, sizeof(aotd_s));

		g_Objects.UserMsg.MessageEnd(&g_Objects.ClientSock);
	}

	//Inform all users of the join
	if (g_Objects.UserMsg.MessageBegin(g_Objects.UserMsg.GetMessageByName("ClientJoin"), INVALID_CLIENT_ID, ST_BROADCAST)) {
		g_Objects.UserMsg.WriteInt(ciClientId);
		g_Objects.UserMsg.WriteBuf(pClient->userinfo.szName, sizeof(pClient->userinfo.szName));

		g_Objects.UserMsg.MessageEnd(&g_Objects.ClientSock);
	}

	if (g_GlobalVars.pLogClientActions->iValue)
		ConsolePrint(FOREGROUND_YELLOW, "Client %d (%s) has successfully joined the server\n", ciClientId + 1, pClient->userinfo.szName);

	//Check for plugin event functions
	for (PLUGINID pid = 0; pid < g_Objects.Plugins.GetPluginCount(); pid++) {
        plugin_s* pPlugin = g_Objects.Plugins.GetPluginById(pid);
        if ((pPlugin) && (!pPlugin->bPaused) && (pPlugin->evttable.events_post.OnClientEntered)) { //Check for valid plugin pointer, if plugin is paused and if event function pointer is assigned
			plugin_result res = PLUGIN_CONTINUE;

			bool bRetValue;

			//Call event function
			res = pPlugin->evttable.events_post.OnClientEntered(ciClientId, pLoginInfo, &bRetValue);

            if (res == PLUGIN_BREAK) //Prevent all event function calls of the following plugins if desired
				break;
            else if (res == PLUGIN_RETURN) //Return from this function if desired
				return bRetValue;
        }
    }

	return true;
}
//======================================================================

//======================================================================
void CCSAPI ENG_ClientDisconnect(const CLIENTID ciClientId, const char* pszReason)
{
	//A client has disconnected from the server

	//Check for plugin event functions
	for (PLUGINID pid = 0; pid < g_Objects.Plugins.GetPluginCount(); pid++) {
        plugin_s* pPlugin = g_Objects.Plugins.GetPluginById(pid);
        if ((pPlugin) && (!pPlugin->bPaused) && (pPlugin->evttable.events_pre.OnClientDisconnect)) { //Check for valid plugin pointer, if plugin is paused and if event function pointer is assigned
			plugin_result res = PLUGIN_CONTINUE;

			//Call event function
			res = pPlugin->evttable.events_pre.OnClientDisconnect(ciClientId, pszReason);

            if (res == PLUGIN_BREAK) //Prevent all event function calls of the following plugins if desired
				break;
            else if (res == PLUGIN_RETURN) //Return from this function if desired
				return;
        }
    }
	
	//Inform users of the client disconnection if the client has once logged in
	clientinfo_s* pClient = g_Objects.ClientSock.GetClientById(ciClientId);
	if ((pClient) && (pClient->bHasLoggedIn)) {
		if (g_Objects.UserMsg.MessageBegin(g_Objects.UserMsg.GetMessageByName("ClientLeave"), INVALID_CLIENT_ID, ST_BROADCAST)) {
			leavemsg_s lvmsg;
			lvmsg.ciClientId = ciClientId;
			strcpy_s(lvmsg.szClient, (pClient->userinfo.szName[0]) ? pClient->userinfo.szName : S_DEFAULT_CLIENTNAME);
			strcpy_s(lvmsg.szReason, (pszReason) ? pszReason : S_DEFAULT_CLIENTLEAVEMSG);

			g_Objects.UserMsg.WriteBuf(&lvmsg, sizeof(lvmsg));

			g_Objects.UserMsg.MessageEnd(&g_Objects.ClientSock);
		}

		//Kick user out of all channels
		for (CHANNELID i = 0; i < ENG_GetChannelCount(); i++) {
			channel_s* pChannel = ENG_GetChannelById(i);
			if (pChannel) {
				chan_user_s* pUser = NULL;
				if (ENG_IsUserInChannel(pClient, pChannel, &pUser)) {
					ENG_RemoveUserFromChannel(pUser, pChannel);
				}
			}
		}
	}

	if (g_GlobalVars.pLogClientActions->iValue)
		ConsolePrint(FOREGROUND_YELLOW, "Client with ID %d (%s) has left the server (%s)\n", ciClientId + 1, (pClient->bHasLoggedIn) ? pClient->userinfo.szName: "(Unnamed)", (pszReason) ? pszReason : S_DEFAULT_CLIENTLEAVEMSG);

	//Check for plugin event functions
	for (PLUGINID pid = 0; pid < g_Objects.Plugins.GetPluginCount(); pid++) {
        plugin_s* pPlugin = g_Objects.Plugins.GetPluginById(pid);
        if ((pPlugin) && (!pPlugin->bPaused) && (pPlugin->evttable.events_post.OnClientDisconnect)) { //Check for valid plugin pointer, if plugin is paused and if event function pointer is assigned
			plugin_result res = PLUGIN_CONTINUE;

			//Call event function
			res = pPlugin->evttable.events_post.OnClientDisconnect(ciClientId, pszReason);

            if (res == PLUGIN_BREAK) //Prevent all event function calls of the following plugins if desired
				break;
            else if (res == PLUGIN_RETURN) //Return from this function if desired
				return;
        }
    }
}
//======================================================================

//======================================================================
bool CCSAPI ENG_ClientPrivMsg(clientinfo_s *pSender, clientinfo_s *pRecv, char* szChatMsg)
{
	//A client wants to send a chat message to another client

	//Check for plugin event functions
	for (PLUGINID pid = 0; pid < g_Objects.Plugins.GetPluginCount(); pid++) {
        plugin_s* pPlugin = g_Objects.Plugins.GetPluginById(pid);
        if ((pPlugin) && (!pPlugin->bPaused) && (pPlugin->evttable.events_pre.OnClientPrivMsg)) { //Check for valid plugin pointer, if plugin is paused and if event function pointer is assigned
			plugin_result res = PLUGIN_CONTINUE;

			bool bRetValue;

			//Call event function
			res = pPlugin->evttable.events_pre.OnClientPrivMsg(pSender, pRecv, szChatMsg, &bRetValue);

            if (res == PLUGIN_BREAK) //Prevent all event function calls of the following plugins if desired
				break;
            else if (res == PLUGIN_RETURN) //Return from this function if desired
				return bRetValue;
        }
    }

	if ((!pSender) || (!pRecv) || (!szChatMsg) || (!pSender->bHasLoggedIn) || (!pRecv->bHasLoggedIn))
		return false;

	//Get recievers client ID
	CLIENTID ciClientId = ENG_GetClientId(pRecv);
	if (ciClientId == INVALID_CLIENT_ID)
		return false;

	//Send private message to client
	if (g_Objects.UserMsg.MessageBegin(g_Objects.UserMsg.GetMessageByName("PrivMsg"), ciClientId, ST_USER)) {
		privmsg_s privmsg;
		strcpy_s(privmsg.szSender, pSender->userinfo.szName);
		strcpy_s(privmsg.szMessage, szChatMsg);

		g_Objects.UserMsg.WriteBuf(&privmsg, sizeof(privmsg_s));

		g_Objects.UserMsg.MessageEnd(&g_Objects.ClientSock);

		if (g_GlobalVars.pLogClientActions->iValue)
			ConsolePrint(FOREGROUND_GREEN, "[Private message] %s -> %s: \"%s\"\n", pSender->userinfo.szName, pRecv->userinfo.szName, szChatMsg);
	}

	//Check for plugin event functions
	for (PLUGINID pid = 0; pid < g_Objects.Plugins.GetPluginCount(); pid++) {
        plugin_s* pPlugin = g_Objects.Plugins.GetPluginById(pid);
        if ((pPlugin) && (!pPlugin->bPaused) && (pPlugin->evttable.events_post.OnClientPrivMsg)) { //Check for valid plugin pointer, if plugin is paused and if event function pointer is assigned
			plugin_result res = PLUGIN_CONTINUE;

			bool bRetValue;

			//Call event function
			res = pPlugin->evttable.events_post.OnClientPrivMsg(pSender, pRecv, szChatMsg, &bRetValue);

            if (res == PLUGIN_BREAK) //Prevent all event function calls of the following plugins if desired
				break;
            else if (res == PLUGIN_RETURN) //Return from this function if desired
				return bRetValue;
        }
    }

	return true;
}
//======================================================================

//======================================================================
bool CCSAPI ENG_ClientChanMsg(clientinfo_s *pSender, channel_s *pRecv, char* szChanMsg)
{
	//A client wants to send a chat message to a channel. Note: In order to work the user must have joined the channel

	//Check for plugin event functions
	for (PLUGINID pid = 0; pid < g_Objects.Plugins.GetPluginCount(); pid++) {
        plugin_s* pPlugin = g_Objects.Plugins.GetPluginById(pid);
        if ((pPlugin) && (!pPlugin->bPaused) && (pPlugin->evttable.events_pre.OnClientChanMsg)) { //Check for valid plugin pointer, if plugin is paused and if event function pointer is assigned
			plugin_result res = PLUGIN_CONTINUE;

			bool bRetValue;

			//Call event function
			res = pPlugin->evttable.events_pre.OnClientChanMsg(pSender, pRecv, szChanMsg, &bRetValue);

            if (res == PLUGIN_BREAK) //Prevent all event function calls of the following plugins if desired
				break;
            else if (res == PLUGIN_RETURN) //Return from this function if desired
				return bRetValue;
        }
    } 
	 
	if ((!pSender) || (!pSender->bHasLoggedIn) || (!pRecv) || (!szChanMsg))
		return false;

	//Get ID of the channel
	CHANNELID chChannelId = ENG_GetIdOfChannel(pRecv);
	if (chChannelId == INVALID_CHANNEL_ID)
		return false;

	//Check if user is inside the channel
	if (!ENG_IsUserInChannel(pSender, pRecv, NULL))
		return false;

	//Send message to channel
	if (g_Objects.UserMsg.MessageBegin(g_Objects.UserMsg.GetMessageByName("ChanMsg"), chChannelId, ST_CHANNEL)) {
		chanmsg_s chanmsg;
		strcpy_s(chanmsg.szSender, pSender->userinfo.szName);
		strcpy_s(chanmsg.szChannel, pRecv->szName);
		strcpy_s(chanmsg.szMessage, szChanMsg);

		g_Objects.UserMsg.WriteBuf(&chanmsg, sizeof(chanmsg_s));

		g_Objects.UserMsg.MessageEnd(&g_Objects.ClientSock);

		if (g_GlobalVars.pLogClientActions->iValue)
			ConsolePrint(FOREGROUND_GREEN, "[Channel message] %s -> %s: \"%s\"\n", pSender->userinfo.szName, pRecv->szName, szChanMsg);
	} 

	//Check for plugin event functions
	for (PLUGINID pid = 0; pid < g_Objects.Plugins.GetPluginCount(); pid++) {
        plugin_s* pPlugin = g_Objects.Plugins.GetPluginById(pid);
        if ((pPlugin) && (!pPlugin->bPaused) && (pPlugin->evttable.events_post.OnClientChanMsg)) { //Check for valid plugin pointer, if plugin is paused and if event function pointer is assigned
			plugin_result res = PLUGIN_CONTINUE;

			bool bRetValue;

			//Call event function
			res = pPlugin->evttable.events_post.OnClientChanMsg(pSender, pRecv, szChanMsg, &bRetValue);

            if (res == PLUGIN_BREAK) //Prevent all event function calls of the following plugins if desired
				break;
            else if (res == PLUGIN_RETURN) //Return from this function if desired
				return bRetValue;
        }
    }

	return true;
}
//======================================================================

//======================================================================
const CLIENTAMOUNT CCSAPI ENG_GetClientCount(void)
{
	//Detour to: GetClientCount

	return g_Objects.ClientSock.GetClientCount();
}
//======================================================================

//======================================================================
clientinfo_s* CCSAPI ENG_GetClientById(CLIENTID id)
{
	//Detour to: GetClientById

	return g_Objects.ClientSock.GetClientById(id);
}
//======================================================================

//======================================================================
clientinfo_s* CCSAPI ENG_GetClientByName(char* szName)
{
	//Detour to: GetClientByName

	return g_Objects.ClientSock.GetClientByName(szName);
}
//======================================================================

//======================================================================
CLIENTID CCSAPI ENG_GetClientId(clientinfo_s* pClient)
{
	//Detour to: GetClientId

	return g_Objects.ClientSock.GetClientId(pClient);
}
//======================================================================

//======================================================================
clientinfo_s* CCSAPI ENG_CreateBot(char *pszName, char *pszClanTag, char *pszClient, char *pszFullName, char *pszEmail, char *pszCountry, bool bAdmin)
{
	//Create a bot

	CLIENTID ciClientId;

	if (g_Objects.ClientSock.CreateBot(pszName, pszClanTag, pszClient, pszFullName, pszEmail, pszCountry, bAdmin, &ciClientId)) { //Create bot
		login_s lg;
		strcpy_s(lg.szName, pszName);
		strcpy_s(lg.szClanTag, pszClanTag);
		strcpy_s(lg.szClient, pszClient);
		strcpy_s(lg.szPassword, g_GlobalVars.pServerPW->szValue);

		if (ENG_ClientJoined(ciClientId, &lg)) { //Set bot to be fully joined
			return ENG_GetClientByName(pszName);
		}

		g_Objects.ClientSock.SockClose(ciClientId, "Bot join failed"); //Client join failed, so delete bot
	}

	return NULL;
}
//======================================================================

//======================================================================
bool CCSAPI ENG_KickClient(clientinfo_s* pClientToKick, char *szReason, bool bAddToBanList)
{
	//Kick client out of the server.

	if (!pClientToKick)
		return false;

	char szLeaveMsg[MAX_NETWORK_STRING_LENGTH];
	((szReason) && (IsValidStrLen(szReason))) ? strcpy_s(szLeaveMsg, szReason) : strcpy_s(szLeaveMsg, S_DEFAULT_KICKMSG);

	bool bKicked;
	bool bBanned;

	ULONG ulIpAddr = pClientToKick->saddr.sin_addr.S_un.S_addr;

	//Remove the client
	bKicked = g_Objects.ClientSock.SockClose(pClientToKick, szLeaveMsg);

	//Ban client if desired
	if (bAddToBanList) {
		bBanned = g_Objects.BanList.AddToList(ulIpAddr) == TRUE;
	} else {
		bBanned = true;
	}

	return (bKicked) && (bBanned);
}
//======================================================================

//======================================================================
bool CCSAPI ENG_CreateChannel(char *szName, char *szPassword, char *szAuthPassword, char *szTopic, DWORD dwMaxUsers, bool bRegistered)
{
	//Detour to: CreateChannel

	//Check for plugin event functions
	for (PLUGINID pid = 0; pid < g_Objects.Plugins.GetPluginCount(); pid++) {
        plugin_s* pPlugin = g_Objects.Plugins.GetPluginById(pid);
        if ((pPlugin) && (!pPlugin->bPaused) && (pPlugin->evttable.events_pre.OnCreateChannel)) { //Check for valid plugin pointer, if plugin is paused and if event function pointer is assigned
			plugin_result res = PLUGIN_CONTINUE;

			bool bRetValue;

			//Call event function
			res = pPlugin->evttable.events_pre.OnCreateChannel(szName, szPassword, szAuthPassword, szTopic, dwMaxUsers, bRegistered, &bRetValue);

            if (res == PLUGIN_BREAK) //Prevent all event function calls of the following plugins if desired
				break;
            else if (res == PLUGIN_RETURN) //Return from this function if desired
				return bRetValue;
        }
    }
	
	//Check for channel amount
	if (ENG_GetChannelCount() >= (CHANNELID)g_GlobalVars.pMaxChannels->iValue)
		return false;

	//Check for valid channel name string
	if (!IsValidString(szName))
		return false;

	//Create the channel
	if (!g_Objects.Channels.CreateChannel(szName, szPassword, szAuthPassword, szTopic, dwMaxUsers, bRegistered))
		return false;

	//Check for plugin event functions
	for (PLUGINID pid = 0; pid < g_Objects.Plugins.GetPluginCount(); pid++) {
        plugin_s* pPlugin = g_Objects.Plugins.GetPluginById(pid);
        if ((pPlugin) && (!pPlugin->bPaused) && (pPlugin->evttable.events_post.OnCreateChannel)) { //Check for valid plugin pointer, if plugin is paused and if event function pointer is assigned
			plugin_result res = PLUGIN_CONTINUE;

			bool bRetValue;

			//Call event function
			res = pPlugin->evttable.events_post.OnCreateChannel(szName, szPassword, szAuthPassword, szTopic, dwMaxUsers, bRegistered, &bRetValue);

            if (res == PLUGIN_BREAK) //Prevent all event function calls of the following plugins if desired
				break;
            else if (res == PLUGIN_RETURN) //Return from this function if desired
				return bRetValue;
        }
    }

	return true;
}
//======================================================================

//======================================================================
bool CCSAPI ENG_DeleteChannel(CHANNELID chChannelId)
{
	//Delete a channel

	//Check for plugin event functions
	for (PLUGINID pid = 0; pid < g_Objects.Plugins.GetPluginCount(); pid++) {
        plugin_s* pPlugin = g_Objects.Plugins.GetPluginById(pid);
        if ((pPlugin) && (!pPlugin->bPaused) && (pPlugin->evttable.events_pre.OnDeleteChannel)) { //Check for valid plugin pointer, if plugin is paused and if event function pointer is assigned
			plugin_result res = PLUGIN_CONTINUE;

			bool bRetValue;

			//Call event function
			res = pPlugin->evttable.events_pre.OnDeleteChannel(chChannelId, &bRetValue);

            if (res == PLUGIN_BREAK) //Prevent all event function calls of the following plugins if desired
				break;
            else if (res == PLUGIN_RETURN) //Return from this function if desired
				return bRetValue;
        }
    }

	if ((chChannelId == INVALID_CHANNEL_ID) || (chChannelId >= ENG_GetChannelCount()))
		return false;

	//Inform users of being removed
	channel_s* pChannel = ENG_GetChannelById(chChannelId);
	if (!pChannel)
		return false;

	for (CHUSERID i = 0; i < (CHUSERID)pChannel->vUsers.size(); i++) {
		if ((!pChannel->vUsers[i]) || (!pChannel->vUsers[i]->pClient))
			continue;

		if (g_Objects.UserMsg.MessageBegin(g_Objects.UserMsg.GetMessageByName("ChanLeave"), chChannelId, ST_CHANNEL)) {
			g_Objects.UserMsg.WriteInt(i);
			g_Objects.UserMsg.WriteBuf(pChannel->vUsers[i]->pClient->userinfo.szName, MAX_NETWORK_STRING_LENGTH);

			g_Objects.UserMsg.MessageEnd(&g_Objects.ClientSock);
		}
	}

	//Delete the channel
	if (!g_Objects.Channels.DeleteChannel(chChannelId))
		return false;

	//Check for plugin event functions
	for (PLUGINID pid = 0; pid < g_Objects.Plugins.GetPluginCount(); pid++) {
        plugin_s* pPlugin = g_Objects.Plugins.GetPluginById(pid);
        if ((pPlugin) && (!pPlugin->bPaused) && (pPlugin->evttable.events_post.OnDeleteChannel)) { //Check for valid plugin pointer, if plugin is paused and if event function pointer is assigned
			plugin_result res = PLUGIN_CONTINUE;

			bool bRetValue;

			//Call event function
			res = pPlugin->evttable.events_post.OnDeleteChannel(chChannelId, &bRetValue);

            if (res == PLUGIN_BREAK) //Prevent all event function calls of the following plugins if desired
				break;
            else if (res == PLUGIN_RETURN) //Return from this function if desired
				return bRetValue;
        }
    }

	return true;
}
//======================================================================

//======================================================================
CHANNELID CCSAPI ENG_GetIdOfChannel(channel_s* pChannel)
{
	//Detour to: GetIdOfChannel

	return g_Objects.Channels.GetIdOfChannel(pChannel);
}
//======================================================================

//======================================================================
channel_s *CCSAPI ENG_GetChannelById(const CHANNELID chChannelId)
{
	//Detour to: GetChannelById

	return g_Objects.Channels.GetChannelById(chChannelId);
}
//======================================================================

//======================================================================
channel_s *CCSAPI ENG_GetChannelByName(const char *szName)
{
	//Detour to: GetChannelByName

	return g_Objects.Channels.GetChannelByName(szName);
}
//======================================================================

//======================================================================
const CHANNELID CCSAPI ENG_GetChannelCount(void)
{
	//Detour to: GetChannelCount

	return g_Objects.Channels.GetChannelCount();
}
//======================================================================

//======================================================================
bool CCSAPI ENG_IsUserInChannel(const clientinfo_s *pClient, const channel_s *pChannel, chan_user_s** ppChanUser)
{
	//Detour to: IsUserInChannel

	return g_Objects.Channels.IsUserInChannel(pClient, pChannel, ppChanUser) == TRUE;
}
//======================================================================

//======================================================================
chan_user_s *CCSAPI ENG_GetUserByIdChan(const channel_s* pChannel, const CHUSERID dwUserId)
{
	//Detour to: GetUserById (Channel)

	return g_Objects.Channels.GetUserById(pChannel, dwUserId);
}
//======================================================================

//======================================================================
chan_user_s *CCSAPI ENG_GetUserByNameChan(const channel_s* pChannel, const char *szName)
{
	//Detour to: GetUserByName (Channel)

	return g_Objects.Channels.GetUserByName(pChannel, szName);
}
//======================================================================

//======================================================================
CHUSERID CCSAPI ENG_GetUserIdOfChannel(const channel_s* pChannel, const chan_user_s* pUser)
{
	//Detour to: GetUserIdOfChannel

	return g_Objects.Channels.GetUserIdOfChannel(pChannel, pUser);
}
//======================================================================

//======================================================================
bool CCSAPI ENG_PutUserInChannel(clientinfo_s *pClient, channel_s *pChannel)
{
	//Put a user into a channel

	//Check for plugin event functions
	for (PLUGINID pid = 0; pid < g_Objects.Plugins.GetPluginCount(); pid++) {
        plugin_s* pPlugin = g_Objects.Plugins.GetPluginById(pid);
        if ((pPlugin) && (!pPlugin->bPaused) && (pPlugin->evttable.events_pre.OnPutUserInChannel)) { //Check for valid plugin pointer, if plugin is paused and if event function pointer is assigned
			plugin_result res = PLUGIN_CONTINUE;

			bool bRetValue;

			//Call event function
			res = pPlugin->evttable.events_pre.OnPutUserInChannel(pClient, pChannel, &bRetValue);

            if (res == PLUGIN_BREAK) //Prevent all event function calls of the following plugins if desired
				break;
            else if (res == PLUGIN_RETURN) //Return from this function if desired
				return bRetValue;
        }
    }

	if ((!pClient) || (!pChannel))
		return false;

	CHANNELID chChannelId = g_Objects.Channels.GetIdOfChannel(pChannel);
	if (chChannelId == INVALID_CHANNEL_ID) 
		return false;

	//Put the user into the channel
	if (!g_Objects.Channels.PutUserInChannel(pClient, pChannel))
		return false;

	//Inform users of the join
	if (g_Objects.UserMsg.MessageBegin(g_Objects.UserMsg.GetMessageByName("ChannelJoin"), chChannelId, ST_CHANNEL)) {
		chanaction_s chaction;
		strcpy_s(chaction.szClient, pClient->userinfo.szName);
		strcpy_s(chaction.szChannel, pChannel->szName);

		g_Objects.UserMsg.WriteInt(chChannelId);
		g_Objects.UserMsg.WriteBuf(&chaction, sizeof(chaction));

		g_Objects.UserMsg.MessageEnd(&g_Objects.ClientSock);
	} 

	if (g_GlobalVars.pLogClientActions->iValue)
		ConsolePrint(FOREGROUND_YELLOW, "Client %s has joined channel %s\n", pClient->userinfo.szName, pChannel->szName);

	//Check for plugin event functions
	for (PLUGINID pid = 0; pid < g_Objects.Plugins.GetPluginCount(); pid++) {
        plugin_s* pPlugin = g_Objects.Plugins.GetPluginById(pid);
        if ((pPlugin) && (!pPlugin->bPaused) && (pPlugin->evttable.events_post.OnPutUserInChannel)) { //Check for valid plugin pointer, if plugin is paused and if event function pointer is assigned
			plugin_result res = PLUGIN_CONTINUE;

			bool bRetValue;

			//Call event function
			res = pPlugin->evttable.events_post.OnPutUserInChannel(pClient, pChannel, &bRetValue);

            if (res == PLUGIN_BREAK) //Prevent all event function calls of the following plugins if desired
				break;
            else if (res == PLUGIN_RETURN) //Return from this function if desired
				return bRetValue;
        }
    }

	return true;
}
//======================================================================

//======================================================================
bool CCSAPI ENG_RemoveUserFromChannel(chan_user_s* pUser, channel_s *pChannel)
{
	//Remove a user out of a channel

	//Check for plugin event functions
	for (PLUGINID pid = 0; pid < g_Objects.Plugins.GetPluginCount(); pid++) {
        plugin_s* pPlugin = g_Objects.Plugins.GetPluginById(pid);
        if ((pPlugin) && (!pPlugin->bPaused) && (pPlugin->evttable.events_pre.OnRemoveUserFromChannel)) { //Check for valid plugin pointer, if plugin is paused and if event function pointer is assigned
			plugin_result res = PLUGIN_CONTINUE;

			bool bRetValue;

			//Call event function
			res = pPlugin->evttable.events_pre.OnRemoveUserFromChannel(pUser, pChannel, &bRetValue);

            if (res == PLUGIN_BREAK) //Prevent all event function calls of the following plugins if desired
				break;
            else if (res == PLUGIN_RETURN) //Return from this function if desired
				return bRetValue;
        }
    }

	if ((!pUser) || (!pChannel))
		return false;

	CHANNELID chChannelId = g_Objects.Channels.GetIdOfChannel(pChannel);
	if (chChannelId == INVALID_CHANNEL_ID)
		return false;

	//Inform users of the leave
	if (g_Objects.UserMsg.MessageBegin(g_Objects.UserMsg.GetMessageByName("ChannelLeave"), chChannelId, ST_CHANNEL)) {
		chanaction_s chaction;
		strcpy_s(chaction.szClient, pUser->pClient->userinfo.szName);
		strcpy_s(chaction.szChannel, pChannel->szName);

		g_Objects.UserMsg.WriteInt(chChannelId);
		g_Objects.UserMsg.WriteBuf(&chaction, sizeof(chaction));

		g_Objects.UserMsg.MessageEnd(&g_Objects.ClientSock);
	}

	if (g_GlobalVars.pLogClientActions->iValue)
		ConsolePrint(FOREGROUND_YELLOW, "Client %s is leaving channel %s\n", pUser->pClient->userinfo.szName, pChannel->szName);

	//Remove the user from the channel
	if (!g_Objects.Channels.RemoveUserFromChannel(pUser, pChannel))
		return false;

	//Delete channel if it is not registered and this user was the last one
	if ((!pChannel->bRegistered) && (!pChannel->vUsers.size()))
		g_Objects.Channels.DeleteChannel(chChannelId);

	//Check for plugin event functions
	for (PLUGINID pid = 0; pid < g_Objects.Plugins.GetPluginCount(); pid++) {
        plugin_s* pPlugin = g_Objects.Plugins.GetPluginById(pid);
        if ((pPlugin) && (!pPlugin->bPaused) && (pPlugin->evttable.events_post.OnRemoveUserFromChannel)) { //Check for valid plugin pointer, if plugin is paused and if event function pointer is assigned
			plugin_result res = PLUGIN_CONTINUE;

			bool bRetValue;

			//Call event function
			res = pPlugin->evttable.events_post.OnRemoveUserFromChannel(pUser, pChannel, &bRetValue);

            if (res == PLUGIN_BREAK) //Prevent all event function calls of the following plugins if desired
				break;
            else if (res == PLUGIN_RETURN) //Return from this function if desired
				return bRetValue;
        }
    }

	return true;
}
//======================================================================

//======================================================================
bool CCSAPI ENG_EstablishMSConnection(const char *szIpAddress, const WORD wPort)
{
	//Detour to: EstablishConnection (Masterserver)

	return g_Objects.MasterServer.EstablishConnection(szIpAddress, wPort) == TRUE;
}
//======================================================================

//======================================================================
bool CCSAPI ENG_CloseMSConnection(void)
{
	//Detour to: CloseConnection (Masterserver)

	return g_Objects.MasterServer.CloseConnection() == TRUE;
}
//======================================================================

//======================================================================
void CCSAPI ENG_UpdateMSData(struct serverinfo_s *pData)
{
	//Detour to: UpdateData (Masterserver)

	g_Objects.MasterServer.UpdateData(pData);
}
//======================================================================

//======================================================================
bool CCSAPI ENG_IsMSConnected(void)
{
	//Detour to: IsConnected (Masterserver)

	return g_Objects.MasterServer.IsConnected() == TRUE;
}
//======================================================================

//======================================================================
int CCSAPI ENG_RegUserMessage(char *szName, int iSize)
{
	//Detour to: AddUserMsg

	return g_Objects.UserMsg.AddUserMsg(szName, iSize);
}
//======================================================================

//======================================================================
int CCSAPI ENG_GetMessageByName(char* szMsg)
{
	//Detour to: GetMessageByName

	return g_Objects.UserMsg.GetMessageByName(szMsg);
}
//======================================================================

//======================================================================
bool CCSAPI ENG_MessageBegin(int iMsgID, CLIENTID dwRecvID, BYTE SendType)
{
	//Detour to: MessageBegin

	return g_Objects.UserMsg.MessageBegin(iMsgID, dwRecvID, (SEND_TYPE)SendType);
}
//======================================================================

//======================================================================
void CCSAPI ENG_WriteByte(BYTE bValue)
{
	//Detour to: WriteByte

	g_Objects.UserMsg.WriteByte(bValue);
}
//======================================================================

//======================================================================
void CCSAPI ENG_WriteChar(char cValue)
{
	//Detour to: WriteChar

	g_Objects.UserMsg.WriteChar(cValue);
}
//======================================================================

//======================================================================
void CCSAPI ENG_WriteShort(short sValue)
{
	//Detour to: WriteShort

	g_Objects.UserMsg.WriteShort(sValue);
}
//======================================================================

//======================================================================
void CCSAPI ENG_WriteInt(int iValue)
{
	//Detour to: WriteInt

	g_Objects.UserMsg.WriteInt(iValue);
}
//======================================================================

//======================================================================
void CCSAPI ENG_WriteFloat(float fValue)
{
	//Detour to: WriteFloat

	g_Objects.UserMsg.WriteFloat(fValue);
}
//======================================================================

//======================================================================
void CCSAPI ENG_WriteString(char *szString, DWORD dwStringSize)
{
	//Detour to: WriteString

	g_Objects.UserMsg.WriteString(szString, dwStringSize);
}
//======================================================================

//======================================================================
void CCSAPI ENG_WriteBuf(void* pBuf, DWORD dwBufSize)
{
	//Detour to: WriteBuf

	g_Objects.UserMsg.WriteBuf(pBuf, dwBufSize);
}
//======================================================================

//======================================================================
bool CCSAPI ENG_MessageEnd(CClientSocket* pSocket)
{
	//Detour to: MessageEnd

	return g_Objects.UserMsg.MessageEnd(pSocket);
}
//======================================================================

//======================================================================
bool CCSAPI ENG_AddConCommand(char *szCmdName, char *szCmdDescription, const void* pCmdProc)
{
	//Detour to: AddCommand

	return g_Objects.ConCommand.AddCommand(szCmdName, szCmdDescription, (TCmdFunction)pCmdProc);
}
//======================================================================

//======================================================================
bool CCSAPI ENG_DeleteConCommand(char* szCmdName)
{
	//Detour to: DelCommand

	return g_Objects.ConCommand.DelCommand(szCmdName);
}
//======================================================================

//======================================================================
bool CCSAPI ENG_ExecScript(const char *szScriptFile)
{
	//Detour to: ConfigMgr::CConfigInt::Execute

	return g_Objects.oConfigInt.Execute(szScriptFile) == TRUE;
}
//======================================================================

//======================================================================
void CCSAPI ENG_ExecCode(const char *szScriptCode)
{
	//Detour to: ConfigMgr::CScriptParser::Parse

	g_Objects.oConfigInt.Parse((char*)szScriptCode);
}
//======================================================================

//======================================================================
ConfigMgr::CCVar::cvar_s* ENG_RegisterCVar(const char* szName, const ConfigMgr::CCVar::cvar_type_e eType, const char* pszDefaultValue)
{
	//Detour to method

	return g_Objects.oConfigInt.CCVar::Add(szName, eType, pszDefaultValue);
}
//======================================================================

//======================================================================
bool ENG_RemoveCVar(const char* szName)
{
	//Detour to method

	return g_Objects.oConfigInt.CCVar::Delete(szName);
}
//======================================================================

//======================================================================
ConfigMgr::CCVar::cvar_s* ENG_GetCVar(const char* pszName)
{
	//Detour to method

	return g_Objects.oConfigInt.CCVar::Find(pszName);
}
//======================================================================

//======================================================================
bool ENG_SetCVarValueString(const char* pszName, const char* szValue)
{
	//Detour to method

	return g_Objects.oConfigInt.CCVar::Set(pszName, szValue);
}
//======================================================================

//======================================================================
bool ENG_SetCVarValueInteger(const char* pszName, const int iValue)
{
	//Detour to method

	return g_Objects.oConfigInt.CCVar::Set(pszName, iValue);
}
//======================================================================

//======================================================================
bool ENG_SetCVarValueFloat(const char* pszName, const double dblValue)
{
	//Detour to method

	return g_Objects.oConfigInt.CCVar::Set(pszName, dblValue);
}
//======================================================================

//======================================================================
bool CCSAPI ENG_AddClientHandler(struct ctpm_handler_s* pHandler)
{
	//Detour to: AddHandler

	return g_Objects.CTPM.AddHandler(pHandler) == TRUE;
}
//======================================================================

//======================================================================
bool CCSAPI ENG_RemoveClientHandler(char* cHandlerSig)
{
	//Detour to: RemoveHandler

	return g_Objects.CTPM.RemoveHandler(cHandlerSig) == TRUE;
}
//======================================================================

//======================================================================
CClientSocket* CCSAPI ENG_GetClientInterface(void)
{
	//Return client socket interface

	return &g_Objects.ClientSock;
}
//======================================================================

//======================================================================
DWORD CCSAPI ENG_GetCommandCount(CConParser *pParser)
{
	//Return the amount of given arguments

	if (!pParser)
		return 0;

	return pParser->GetAmount();
}
//======================================================================

//======================================================================
const char* CCSAPI ENG_GetCommandArg(CConParser *pParser, DWORD dwIndex)
{
	//Return a pointer to a argument string buffer

	if (!pParser)
		return NULL;

	return pParser->GetArgument(dwIndex);
}
//======================================================================

//======================================================================
bool CCSAPI ENG_IsDestinationReachable(LPCSTR lpszServer)
{
	//Detour to: IsDestinationReachable

	return IsDestinationReachable(lpszServer) == TRUE;
}
//======================================================================

//======================================================================
const char* CCSAPI ENG_AddressToString(const unsigned long ulIPAddress)
{
	//Detour to: AddressToString

	return AddressToString(ulIPAddress);
}
//======================================================================

//======================================================================
bool CCSAPI ENG_FileExists(const char* szFileName)
{
	//Detour to: FileExists

	return FileExists(szFileName);
}
//======================================================================

//======================================================================
bool CCSAPI ENG_DirectoryExists(char* szDir)
{
	//Detour to: DirectoryExists

	return DirectoryExists(szDir);
}
//======================================================================

//======================================================================
void CCSAPI ENG_ExtractFilePath(char *szFileName)
{
	//Detour to ExtractFilePath

	return ExtractFilePath(szFileName);
}
//======================================================================

//======================================================================
char *CCSAPI ENG_ExtractFileName(char *filePath)
{
	//Detour to ExtractFileName

	return ExtractFileName(filePath);
}
//======================================================================

//======================================================================
const char* CCSAPI ENG_GetCurrentDateTime(void)
{
	//Detour to: GetCurrentDateTime

	return GetCurrentDateTime();
}
//======================================================================

//======================================================================
bool CCSAPI ENG_UpdateServerComponent(void)
{
	//Detour to: UpdateServerComponent

	return UpdateServerComponent() == TRUE;
}
//======================================================================

//======================================================================
void CDH_ConfirmMessage(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize)
{
	//A client has confirmed that he has recieved a user message
	
	if (g_GlobalVars.pSendType->iValue)
		g_Objects.ClientSock.MessageConfirmed(dwClientID);
}
//======================================================================

//======================================================================
void CDH_Ping(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize)
{
	//A client has replied to the ping request

	if ((!pBuffer) || (dwBufSize != sizeof(BYTE)))
		return;

	clientinfo_s* pClientInfo = g_Objects.ClientSock.GetClientById(dwClientID);
	if ((pClientInfo) && (*(BYTE*)pBuffer == pClientInfo->ping.curPingVal)) {
		pClientInfo->ping.actualPing = pClientInfo->ping.pingCount;
		pClientInfo->ping.pingAmount = 0; //No timeout, client replied
		pClientInfo->ping.pingCount = 0;
	}
}
//======================================================================

//======================================================================
void CDH_Login(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize)
{
	//Client wants to login to server

	//CONFIRM_CLIENT_MESSAGE(dwClientID);
	
	login_s mlogin; 

	if ((!pBuffer) || (dwBufSize != sizeof(mlogin))) {
		SEND_CLIENT_OS(dwClientID, OS_LOGIN, OS_INVALIDBUFFER);
		return;
	}

	memcpy(&mlogin, pBuffer, sizeof(mlogin));
	mlogin.szName[sizeof(mlogin.szName)-1] = 0;
	mlogin.szClanTag[sizeof(mlogin.szClanTag)-1] = 0;
	mlogin.szClient[sizeof(mlogin.szClient)-1] = 0;
	mlogin.szPassword[sizeof(mlogin.szPassword)-1] = 0;

	if (ENG_GetClientByName(mlogin.szName)) {
		SEND_CLIENT_OS(dwClientID, OS_LOGIN, OS_ALREADYEXISTS);
		return;
	}

	if (!ENG_ClientJoined(dwClientID, &mlogin)) {
		SEND_CLIENT_OS(dwClientID, OS_LOGIN, OS_ENGFAIL);

		clientinfo_s* pClient = ENG_GetClientById(dwClientID);
		if (pClient)
			ENG_KickClient(pClient, "Client join failed", false);
	} else {
		SEND_CLIENT_OS(dwClientID, OS_LOGIN, OS_SUCCESSFUL);
	}
}
//======================================================================

//======================================================================
void CDH_ChannelMsg(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize)
{
	//Client wants to send a channel message

	struct msg_channelmsg_s {
		char szChannel[MAX_NETWORK_STRING_LENGTH];
		char szMessage[MAX_NETWORK_STRING_LENGTH];
	} mmsg;

	if ((!pBuffer) || (dwBufSize != sizeof(mmsg))) {
		SEND_CLIENT_OS(dwClientID, OS_CHANNELMSG, OS_INVALIDBUFFER);
		return;
	}

	memcpy(&mmsg, pBuffer, sizeof(mmsg));

	if ((!IsValidStrLen(mmsg.szChannel)) || (!IsValidStrLen(mmsg.szMessage))) {
		SEND_CLIENT_OS(dwClientID, OS_CHANNELMSG, OS_INVALIDBUFFER);
		return;
	}

	clientinfo_s* pClient = g_Objects.ClientSock.GetClientById(dwClientID);
	if ((!pClient) || (!pClient->bHasLoggedIn)) {
		SEND_CLIENT_OS(dwClientID, OS_CHANNELMSG, OS_BADUSER);
		return;
	}

	channel_s* pChannel = ENG_GetChannelByName(mmsg.szChannel);
	if (!pChannel) {
		SEND_CLIENT_OS(dwClientID, OS_CHANNELMSG, OS_INVALIDBUFFER);
		return;
	}

	if (!ENG_ClientChanMsg(pClient, pChannel, mmsg.szMessage)) {
		SEND_CLIENT_OS(dwClientID, OS_CHANNELMSG, OS_ENGFAIL);
	} else {
		SEND_CLIENT_OS(dwClientID, OS_CHANNELMSG, OS_SUCCESSFUL);
	}
}
//======================================================================

//======================================================================
void CDH_PrivateMsg(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize)
{
	//Client wants to send a private message

	struct msg_privatemsg_s {
		char szUser[MAX_NETWORK_STRING_LENGTH];
		char szMessage[MAX_NETWORK_STRING_LENGTH];
	} mmsg;

	if ((!pBuffer) || (dwBufSize != sizeof(mmsg))) {
		SEND_CLIENT_OS(dwClientID, OS_PRIVATEMSG, OS_INVALIDBUFFER);
		return;
	}

	memcpy(&mmsg, pBuffer, sizeof(mmsg));

	if ((!IsValidStrLen(mmsg.szUser)) || (!IsValidStrLen(mmsg.szMessage))) {
		SEND_CLIENT_OS(dwClientID, OS_PRIVATEMSG, OS_INVALIDBUFFER);
		return;
	}

	clientinfo_s* pClient = ENG_GetClientById(dwClientID);
	if ((!pClient) || (!pClient->bHasLoggedIn)) {
		SEND_CLIENT_OS(dwClientID, OS_PRIVATEMSG, OS_BADUSER);
		return;
	}

	clientinfo_s* pRecv = ENG_GetClientByName(mmsg.szUser);
	if ((!pRecv) || (!pRecv->bHasLoggedIn)) {
		SEND_CLIENT_OS(dwClientID, OS_PRIVATEMSG, OS_BADUSER);
		return;
	}

	if (!ENG_ClientPrivMsg(pClient, pRecv, mmsg.szMessage)) {
		SEND_CLIENT_OS(dwClientID, OS_PRIVATEMSG, OS_ENGFAIL);
	} else {
		SEND_CLIENT_OS(dwClientID, OS_PRIVATEMSG, OS_SUCCESSFUL);
	}
}
//======================================================================

//======================================================================
void CDH_SetClientInfo(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize)
{
	//Client wants to set his information data

	struct infodata_s {
		char szEmail[MAX_NETWORK_STRING_LENGTH];
		char szCountry[MAX_NETWORK_STRING_LENGTH];
		char szFullName[MAX_NETWORK_STRING_LENGTH];
	} idata;

	if ((!pBuffer) || (dwBufSize != sizeof(idata))) {
		SEND_CLIENT_OS(dwClientID, OS_SETINFODATA, OS_INVALIDBUFFER);
		return;
	}

	memcpy(&idata, pBuffer, sizeof(idata));

	if ((!IsValidStrLen(idata.szFullName)) || (!IsValidStrLen(idata.szEmail) || (!IsValidStrLen(idata.szCountry)))) {
		SEND_CLIENT_OS(dwClientID, OS_SETINFODATA, OS_INVALIDBUFFER);
		return;
	}

	clientinfo_s* pClient = ENG_GetClientById(dwClientID);
	if ((!pClient) || (!pClient->bHasLoggedIn)) {
		SEND_CLIENT_OS(dwClientID, OS_SETINFODATA, OS_BADUSER);
		return;
	}

	strcpy_s(pClient->userinfo.szFullName, idata.szFullName);
	strcpy_s(pClient->userinfo.szEmail, idata.szEmail);
	strcpy_s(pClient->userinfo.szCountry, idata.szCountry);

	SEND_CLIENT_OS(dwClientID, OS_SETINFODATA, OS_SUCCESSFUL);
}
//======================================================================

//======================================================================
void CDH_GetClientCount(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize)
{
	//Client wants to get the amount of connected clients

	clientinfo_s* pClient = ENG_GetClientById(dwClientID);
	if ((!pClient) || (!pClient->bHasLoggedIn)) {
		SEND_CLIENT_OS(dwClientID, OS_CLIENTCOUNT, OS_BADUSER);
		return;
	}

	if (g_Objects.UserMsg.MessageBegin(g_Objects.UserMsg.GetMessageByName("ClientCount"), dwClientID, ST_USER)) {
		g_Objects.UserMsg.WriteInt(ENG_GetClientCount());

		g_Objects.UserMsg.MessageEnd(&g_Objects.ClientSock);

		SEND_CLIENT_OS(dwClientID, OS_CLIENTCOUNT, OS_SUCCESSFUL);

		return;
	}

	SEND_CLIENT_OS(dwClientID, OS_CLIENTCOUNT, OS_ENGFAIL);
}
//======================================================================

//======================================================================
void CDH_GetClientInfo(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize)
{
	//Client wants to get the information data of a different client

	char szClientName[MAX_NETWORK_STRING_LENGTH];

	if ((!pBuffer) || (dwBufSize != sizeof(szClientName))) {
		SEND_CLIENT_OS(dwClientID, OS_GETINFODATA, OS_INVALIDBUFFER);
		return;
	}

	memcpy(szClientName, pBuffer, dwBufSize);

	if (!IsValidStrLen(szClientName)) {
		SEND_CLIENT_OS(dwClientID, OS_GETINFODATA, OS_INVALIDBUFFER);
		return;
	}

	clientinfo_s* pClient = ENG_GetClientById(dwClientID);
	if ((!pClient) || (!pClient->bHasLoggedIn)) {
		SEND_CLIENT_OS(dwClientID, OS_GETINFODATA, OS_BADUSER);
		return;
	}

	clientinfo_s* pTarget = ENG_GetClientByName(szClientName);
	if ((pClient != pTarget) && ((!pTarget) || (!pTarget->bHasLoggedIn) || (pTarget->bGhost))) {
		SEND_CLIENT_OS(dwClientID, OS_GETINFODATA, OS_ENGFAIL);
		return;
	}

	clinfo_s cli;
	cli.bGhost = pTarget->bGhost;
	cli.bIsAdmin = pTarget->bIsAdmin;
	cli.bPutInServ = pTarget->bHasLoggedIn;
	cli.clId = ENG_GetClientId(pTarget);
	strcpy_s(cli.szClanTag, pTarget->userinfo.szClanTag);
	strcpy_s(cli.szClient, pTarget->userinfo.szClient);
	strcpy_s(cli.szCountry, pTarget->userinfo.szCountry);
	strcpy_s(cli.szEmail, pTarget->userinfo.szEmail);
	strcpy_s(cli.szFullname, pTarget->userinfo.szFullName);
	strcpy_s(cli.szName, pTarget->userinfo.szName);
	*(DWORD*)&cli.ucaAddress[0] = (pClient->bIsAdmin) ? pTarget->saddr.sin_addr.S_un.S_addr : INADDR_ANY; //Only admins can see IP addresses
	cli.uiPingValue = pTarget->ping.actualPing;

	if (g_Objects.UserMsg.MessageBegin(g_Objects.UserMsg.GetMessageByName("ClientInfo"), dwClientID, ST_USER)) {
		g_Objects.UserMsg.WriteBuf(&cli, sizeof(cli));

		g_Objects.UserMsg.MessageEnd(&g_Objects.ClientSock);

		SEND_CLIENT_OS(dwClientID, OS_CLIENTCOUNT, OS_SUCCESSFUL);

		return;
	}

	SEND_CLIENT_OS(dwClientID, OS_GETINFODATA, OS_ENGFAIL);
}
//======================================================================

//======================================================================
void CDH_JoinChannel(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize)
{
	//Client wants to join a channel

	struct joinchan_s {
		char szName[MAX_NETWORK_STRING_LENGTH];
		char szPassword[MAX_NETWORK_STRING_LENGTH];
	} jc;

	if ((!pBuffer) || (dwBufSize != sizeof(jc))) {
		SEND_CLIENT_OS(dwClientID, OS_JOINCHAN, OS_INVALIDBUFFER);
		return;
	}

	memcpy(&jc, pBuffer, dwBufSize);

	clientinfo_s* pClient = ENG_GetClientById(dwClientID);
	if ((!pClient) || (!pClient->bHasLoggedIn)) {
		SEND_CLIENT_OS(dwClientID, OS_JOINCHAN, OS_BADUSER);
		return;
	}

	if ((!IsValidStrLen(jc.szName) || (!IsValidStrLen(jc.szPassword)))) {
		SEND_CLIENT_OS(dwClientID, OS_JOINCHAN, OS_INVALIDBUFFER);
		return;
	}

	channel_s* pChannel = ENG_GetChannelByName(jc.szName);

	if (pChannel == NULL) {
		//Create channel if not already exists

		if (!ENG_CreateChannel(jc.szName, DEFAULT_CHANNEL_PASSWORD, DEFAULT_CHANNEL_AUTHPASSWORD, DEFAULT_CHANNEL_TOPIC, 100, false)) {
			SEND_CLIENT_OS(dwClientID, OS_JOINCHAN, OS_ENGFAIL);

			return;
		}

		pChannel = ENG_GetChannelByName(jc.szName);
	} else {
		//Check for password
		if ((strcmp(pChannel->szPassword, S_NO_PASSWORD)) && (strcmp(pChannel->szPassword, jc.szPassword))) {
			SEND_CLIENT_OS(dwClientID, OS_JOINCHAN, OS_BAD_PASSWORD);
			return;
		}
	}

	if (!ENG_PutUserInChannel(pClient, pChannel)) {
		SEND_CLIENT_OS(dwClientID, OS_JOINCHAN, OS_ENGFAIL);
		return;
	}

	SEND_CLIENT_OS(dwClientID, OS_JOINCHAN, OS_SUCCESSFUL);
}
//======================================================================

//======================================================================
void CDH_LeaveChannel(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize)
{
	//Client wants to leave a channel

	char szChannelName[MAX_NETWORK_STRING_LENGTH];

	if ((!pBuffer) || (dwBufSize != sizeof(szChannelName))) {
		SEND_CLIENT_OS(dwClientID, OS_LEAVECHAN, OS_INVALIDBUFFER);
		return;
	}

	memcpy(szChannelName, pBuffer, dwBufSize);

	if (!IsValidStrLen(szChannelName)) {
		SEND_CLIENT_OS(dwClientID, OS_LEAVECHAN, OS_INVALIDBUFFER);
		return;
	}

	clientinfo_s* pClient = ENG_GetClientById(dwClientID);
	if ((!pClient) || (!pClient->bHasLoggedIn)) {
		SEND_CLIENT_OS(dwClientID, OS_LEAVECHAN, OS_BADUSER);
		return;
	}

	channel_s* pChannel = ENG_GetChannelByName(szChannelName);
	if (!pChannel) {
		SEND_CLIENT_OS(dwClientID, OS_LEAVECHAN, OS_ENGFAIL);

		return;
	}

	chan_user_s* pChanUser = ENG_GetUserByNameChan(pChannel, pClient->userinfo.szName);
	if (!pChanUser) {
		SEND_CLIENT_OS(dwClientID, OS_LEAVECHAN, OS_ENGFAIL);

		return;
	}

	if (!ENG_RemoveUserFromChannel(pChanUser, pChannel)) {
		SEND_CLIENT_OS(dwClientID, OS_LEAVECHAN, OS_ENGFAIL);

		return;
	}

	SEND_CLIENT_OS(dwClientID, OS_LEAVECHAN, OS_SUCCESSFUL);
}
//======================================================================

//======================================================================
void CDH_ChannelAmount(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize)
{
	//Client wants to get the amount of existing channels

	clientinfo_s* pClient = ENG_GetClientById(dwClientID);
	if ((!pClient) || (!pClient->bHasLoggedIn)) {
		SEND_CLIENT_OS(dwClientID, OS_CHANAMOUNT, OS_BADUSER);
		return;
	}

	if (g_Objects.UserMsg.MessageBegin(g_Objects.UserMsg.GetMessageByName("ChanCount"), dwClientID, ST_USER)) {
		g_Objects.UserMsg.WriteInt(ENG_GetChannelCount());

		g_Objects.UserMsg.MessageEnd(&g_Objects.ClientSock);

		SEND_CLIENT_OS(dwClientID, OS_CHANAMOUNT, OS_SUCCESSFUL);

		return;
	}

	SEND_CLIENT_OS(dwClientID, OS_CHANAMOUNT, OS_ENGFAIL);
}
//======================================================================

//======================================================================
void CDH_ChannelInfoByName(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize)
{
	//Client wants to get channel information data

	char szChannelName[MAX_NETWORK_STRING_LENGTH];
	
	if ((!pBuffer) || (dwBufSize != sizeof(szChannelName))) {
		SEND_CLIENT_OS(dwClientID, OS_CHANINFO, OS_INVALIDBUFFER);
		return;
	}

	memcpy(szChannelName, pBuffer, dwBufSize);

	if (!IsValidStrLen(szChannelName)) {
		SEND_CLIENT_OS(dwClientID, OS_CHANINFO, OS_INVALIDBUFFER);
		return;
	}

	clientinfo_s* pClient = ENG_GetClientById(dwClientID);
	if ((!pClient) || (!pClient->bHasLoggedIn)) {
		SEND_CLIENT_OS(dwClientID, OS_CHANINFO, OS_BADUSER);
		return;
	}

	channel_s* pTarget = ENG_GetChannelByName(szChannelName);
	if (!pTarget) {
		SEND_CLIENT_OS(dwClientID, OS_CHANINFO, OS_ENGFAIL);
		return;
	}

	chaninfo_s chnfo;
	chnfo.bPassword = strcmp(pTarget->szPassword, S_NO_PASSWORD) != 0;
	chnfo.chId = ENG_GetIdOfChannel(pTarget);
	chnfo.iCurrentUsers = (CHUSERID)pTarget->vUsers.size();
	chnfo.iMaxUsers = pTarget->dwMaxUsers;
	strcpy_s(chnfo.szName, pTarget->szName);
	strcpy_s(chnfo.szTopic, pTarget->szTopic);
	
	if (g_Objects.UserMsg.MessageBegin(g_Objects.UserMsg.GetMessageByName("ChanInfo"), dwClientID, ST_USER)) {
		g_Objects.UserMsg.WriteBuf(&chnfo, sizeof(chnfo));

		g_Objects.UserMsg.MessageEnd(&g_Objects.ClientSock);

		SEND_CLIENT_OS(dwClientID, OS_CLIENTCOUNT, OS_SUCCESSFUL);

		return;
	}

	SEND_CLIENT_OS(dwClientID, OS_CHANINFO, OS_ENGFAIL);
}
//======================================================================

//======================================================================
void CDH_ChannelInfoByID(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize)
{
	//Client wants to get channel information data

	CHANNELID chChannelId;
	
	if ((!pBuffer) || (dwBufSize != sizeof(CHANNELID))) {
		SEND_CLIENT_OS(dwClientID, OS_CHANINFO, OS_INVALIDBUFFER);
		return;
	}

	chChannelId = *(CHANNELID*)pBuffer;

	clientinfo_s* pClient = ENG_GetClientById(dwClientID);
	if ((!pClient) || (!pClient->bHasLoggedIn)) {
		SEND_CLIENT_OS(dwClientID, OS_CHANINFO, OS_BADUSER);
		return;
	}

	channel_s* pTarget = ENG_GetChannelById(chChannelId);
	if (!pTarget) {
		SEND_CLIENT_OS(dwClientID, OS_CHANINFO, OS_ENGFAIL);
		return;
	}

	chaninfo_s chnfo;
	chnfo.bPassword = strcmp(pTarget->szPassword, S_NO_PASSWORD) != 0;
	chnfo.chId = ENG_GetIdOfChannel(pTarget);
	chnfo.iCurrentUsers = (CHUSERID)pTarget->vUsers.size();
	chnfo.iMaxUsers = pTarget->dwMaxUsers;
	strcpy_s(chnfo.szName, pTarget->szName);
	strcpy_s(chnfo.szTopic, pTarget->szTopic);
	
	if (g_Objects.UserMsg.MessageBegin(g_Objects.UserMsg.GetMessageByName("ChanInfo"), dwClientID, ST_USER)) {
		g_Objects.UserMsg.WriteBuf(&chnfo, sizeof(chnfo));

		g_Objects.UserMsg.MessageEnd(&g_Objects.ClientSock);

		SEND_CLIENT_OS(dwClientID, OS_CLIENTCOUNT, OS_SUCCESSFUL);

		return;
	}

	SEND_CLIENT_OS(dwClientID, OS_CHANINFO, OS_ENGFAIL);
}
//======================================================================

//======================================================================
void CDH_ChannelUsers(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize)
{
	//Client wants to get the user names of all users in a channel

	char szChannelName[MAX_NETWORK_STRING_LENGTH];

	if ((!pBuffer) || (dwBufSize != sizeof(szChannelName))) {
		SEND_CLIENT_OS(dwClientID, OS_CHANUSERS, OS_INVALIDBUFFER);
		return;
	}

	memcpy(szChannelName, pBuffer, dwBufSize);

	if (!IsValidStrLen(szChannelName)) {
		SEND_CLIENT_OS(dwClientID, OS_CHANUSERS, OS_INVALIDBUFFER);
		return;
	}

	channel_s* pTarget = ENG_GetChannelByName(szChannelName);
	if (!pTarget) {
		SEND_CLIENT_OS(dwClientID, OS_CHANUSERS, OS_ENGFAIL);
		return;
	}

	clientinfo_s* pClient = ENG_GetClientById(dwClientID);
	if ((!pClient) || (!pClient->bHasLoggedIn) || (!ENG_IsUserInChannel(pClient, pTarget, NULL))) {
		SEND_CLIENT_OS(dwClientID, OS_CHANUSERS, OS_BADUSER);
		return;
	}

	char szChanUsers[MAX_CHANUSERS_STRING_LENGTH];
	szChanUsers[0] = 0;

	for (CHUSERID i = 0; i < (CHUSERID)pTarget->vUsers.size(); i++) {
		if ((!pTarget->vUsers[i]) || (!pTarget->vUsers[i]->pClient))
			continue;

		if ((pTarget->vUsers[i]->pClient->bGhost) && (pTarget->vUsers[i]->pClient != pClient))
			continue;

		strcat_s(szChanUsers, pTarget->vUsers[i]->pClient->userinfo.szName);
		if (i < (CHUSERID)pTarget->vUsers.size() - 1)
			strcat_s(szChanUsers, CHANUSERS_DELIMITER);
	}

	szChanUsers[sizeof(szChanUsers)-1] = 0;

	if (g_Objects.UserMsg.MessageBegin(g_Objects.UserMsg.GetMessageByName("ChanUsers"), dwClientID, ST_USER)) {
		g_Objects.UserMsg.WriteBuf(pTarget->szName, sizeof(pTarget->szName));
		g_Objects.UserMsg.WriteBuf(szChanUsers, sizeof(szChanUsers));

		g_Objects.UserMsg.MessageEnd(&g_Objects.ClientSock);

		SEND_CLIENT_OS(dwClientID, OS_CHANUSERS, OS_SUCCESSFUL);

		return;
	}

	SEND_CLIENT_OS(dwClientID, OS_CHANUSERS, OS_ENGFAIL);
}
//======================================================================

//======================================================================
void CDH_AuthAsAdmin(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize)
{
	//Client wants to login as admin

	char szAdminPw[MAX_NETWORK_STRING_LENGTH];

	if ((!pBuffer) || (dwBufSize != sizeof(szAdminPw))) {
		SEND_CLIENT_OS(dwClientID, OS_ADMINAUTH, OS_INVALIDBUFFER);
		return;
	}

	memcpy(szAdminPw, pBuffer, dwBufSize);

	if (!IsValidStrLen(szAdminPw)) {
		SEND_CLIENT_OS(dwClientID, OS_ADMINAUTH, OS_INVALIDBUFFER);
		return;
	}

	clientinfo_s* pClient = ENG_GetClientById(dwClientID);
	if ((!pClient) || (!pClient->bHasLoggedIn)) {
		SEND_CLIENT_OS(dwClientID, OS_ADMINAUTH, OS_BADUSER);
		return;
	}

	if (strcmp(szAdminPw, g_GlobalVars.pAdminPW->szValue)==0) {
		pClient->bIsAdmin = true;

		SEND_CLIENT_OS(dwClientID, OS_ADMINAUTH, OS_SUCCESSFUL);

		#define ADMIN_WELCOME_MSG "You have been authenticated as admin"

		ServerNotice(ENG_GetClientId(pClient), ST_USER, ADMIN_WELCOME_MSG, (DWORD)strlen(ADMIN_WELCOME_MSG) + 1);

		if (g_GlobalVars.pLogClientActions->iValue)
			ConsolePrint(FOREGROUND_LIGHTBLUE, "Client %d (%s) has authenticated as an administrator\n", dwClientID + 1, pClient->userinfo.szName);
	} else {
		SEND_CLIENT_OS(dwClientID, OS_ADMINAUTH, OS_BAD_PASSWORD);
	}
}
//======================================================================

//======================================================================
void CDH_KickUser(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize)
{
	//Admin wants to kick a client

	char szTarget[MAX_NETWORK_STRING_LENGTH];

	if ((!pBuffer) || (dwBufSize != sizeof(szTarget))) {
		SEND_CLIENT_OS(dwClientID, OS_KICKUSER, OS_INVALIDBUFFER);
		return;
	}

	memcpy(szTarget, pBuffer, dwBufSize);

	if (!IsValidStrLen(szTarget)) {
		SEND_CLIENT_OS(dwClientID, OS_KICKUSER, OS_INVALIDBUFFER);
		return;
	}

	clientinfo_s* pClient = ENG_GetClientById(dwClientID);
	if ((!pClient) || (!pClient->bHasLoggedIn) || (!pClient->bIsAdmin)) {
		SEND_CLIENT_OS(dwClientID, OS_KICKUSER, OS_BADUSER);
		return;
	}

	clientinfo_s* pTarget = ENG_GetClientByName(szTarget);
	if (!pTarget) {
		SEND_CLIENT_OS(dwClientID, OS_KICKUSER, OS_BADUSER);
		return;
	}

	char szKickMsg[MAX_NETWORK_STRING_LENGTH];
	sprintf_s(szKickMsg, "Kicked by %s", pClient->userinfo.szName);

	if (!ENG_KickClient(pTarget, szKickMsg, false)) {
		SEND_CLIENT_OS(dwClientID, OS_KICKUSER, OS_ENGFAIL);
	} else {
		SEND_CLIENT_OS(dwClientID, OS_KICKUSER, OS_SUCCESSFUL);
	}
}
//======================================================================

//======================================================================
void CDH_BanUser(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize)
{
	//Admin wants to ban a client

	char szTarget[MAX_NETWORK_STRING_LENGTH];

	if ((!pBuffer) || (dwBufSize != sizeof(szTarget))) {
		SEND_CLIENT_OS(dwClientID, OS_BANUSER, OS_INVALIDBUFFER);
		return;
	}

	memcpy(szTarget, pBuffer, dwBufSize);

	if (!IsValidStrLen(szTarget)) {
		SEND_CLIENT_OS(dwClientID, OS_BANUSER, OS_INVALIDBUFFER);
		return;
	}

	clientinfo_s* pClient = ENG_GetClientById(dwClientID);
	if ((!pClient) || (!pClient->bHasLoggedIn) || (!pClient->bIsAdmin)) {
		SEND_CLIENT_OS(dwClientID, OS_BANUSER, OS_BADUSER);
		return;
	}

	clientinfo_s* pTarget = ENG_GetClientByName(szTarget);
	if (!pTarget) {
		SEND_CLIENT_OS(dwClientID, OS_BANUSER, OS_BADUSER);
		return;
	}

	char szKickMsg[MAX_NETWORK_STRING_LENGTH];
	sprintf_s(szKickMsg, "Banned by %s", pClient->userinfo.szName);

	if (!ENG_KickClient(pTarget, szKickMsg, true)) {
		SEND_CLIENT_OS(dwClientID, OS_BANUSER, OS_ENGFAIL);
	} else {
		SEND_CLIENT_OS(dwClientID, OS_BANUSER, OS_SUCCESSFUL);
	}
}
//======================================================================

//======================================================================
void CDH_ChangeTopic(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize)
{
	//Admin wants to change a channel topic

	struct chantopic_s {
		char szName[MAX_NETWORK_STRING_LENGTH];
		char szTopic[MAX_NETWORK_STRING_LENGTH];
	} ct;

	if ((!pBuffer) || (dwBufSize != sizeof(ct))) {
		SEND_CLIENT_OS(dwClientID, OS_CHANGETOPIC, OS_INVALIDBUFFER);
		return;
	}

	memcpy(&ct, pBuffer, dwBufSize);

	if (!IsValidStrLen(ct.szName) || (!IsValidStrLen(ct.szTopic))) {
		SEND_CLIENT_OS(dwClientID,OS_CHANGETOPIC, OS_INVALIDBUFFER);
		return;
	}

	clientinfo_s* pClient = ENG_GetClientById(dwClientID);
	if ((!pClient) || (!pClient->bHasLoggedIn) || (!pClient->bIsAdmin)) {
		SEND_CLIENT_OS(dwClientID, OS_CHANGETOPIC, OS_BADUSER);
		return;
	}

	channel_s* pTarget = ENG_GetChannelByName(ct.szName);
	if (!pTarget) {
		SEND_CLIENT_OS(dwClientID, OS_CHANGETOPIC, OS_ENGFAIL);
		return;
	}

	strcpy_s(pTarget->szTopic, ct.szTopic);

	SEND_CLIENT_OS(dwClientID, OS_CHANGETOPIC, OS_SUCCESSFUL);
}
//======================================================================

//======================================================================
void CDH_CreateChannel(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize)
{
	//Admin wants to create a channel

	struct createchan_s {
		DWORD dwMaxUsers;
		char szName[MAX_NETWORK_STRING_LENGTH];
		char szPassword[MAX_NETWORK_STRING_LENGTH];
		char szAuthPassword[MAX_NETWORK_STRING_LENGTH];
		char szTopic[MAX_NETWORK_STRING_LENGTH];
	} cc;
	
	if ((!pBuffer) || (dwBufSize != sizeof(cc))) {
		SEND_CLIENT_OS(dwClientID, OS_CREATECHAN, OS_INVALIDBUFFER);
		return;
	}
	
	memcpy(&cc, pBuffer, dwBufSize);

	if (!IsValidStrLen(cc.szName) || (!IsValidStrLen(cc.szTopic) || (!IsValidStrLen(cc.szPassword) || (!IsValidStrLen(cc.szAuthPassword))))) {
		SEND_CLIENT_OS(dwClientID, OS_CREATECHAN, OS_INVALIDBUFFER);
		return;
	}

	clientinfo_s* pClient = ENG_GetClientById(dwClientID);
	if ((!pClient) || (!pClient->bHasLoggedIn) || (!pClient->bIsAdmin)) {
		SEND_CLIENT_OS(dwClientID, OS_CREATECHAN, OS_BADUSER);
		return;
	}

	channel_s* pTarget = ENG_GetChannelByName(cc.szName);
	if (pTarget) {
		SEND_CLIENT_OS(dwClientID, OS_CREATECHAN, OS_ALREADYEXISTS);
		return;
	}
	
	if (!ENG_CreateChannel(cc.szName, cc.szPassword, cc.szAuthPassword, cc.szTopic, cc.dwMaxUsers, true)) {
		SEND_CLIENT_OS(dwClientID, OS_CREATECHAN, OS_ENGFAIL);
	} else {
		SEND_CLIENT_OS(dwClientID, OS_CREATECHAN, OS_SUCCESSFUL);
	}
}
//======================================================================

//======================================================================
void CDH_DeleteChannel(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize)
{
	//Admin wants to delete a channel

	char szChannelName[MAX_NETWORK_STRING_LENGTH];

	if ((!pBuffer) || (dwBufSize != sizeof(szChannelName))) {
		SEND_CLIENT_OS(dwClientID, OS_DELETECHAN, OS_INVALIDBUFFER);
		return;
	}

	memcpy(szChannelName, pBuffer, dwBufSize);

	if (!IsValidStrLen(szChannelName)) {
		SEND_CLIENT_OS(dwClientID, OS_DELETECHAN, OS_INVALIDBUFFER);
		return;
	}

	clientinfo_s* pClient = ENG_GetClientById(dwClientID);
	if ((!pClient) || (!pClient->bHasLoggedIn) || (!pClient->bIsAdmin)) {
		SEND_CLIENT_OS(dwClientID, OS_DELETECHAN, OS_BADUSER);
		return;
	}

	channel_s* pTarget = ENG_GetChannelByName(szChannelName);
	if (!pTarget) {
		SEND_CLIENT_OS(dwClientID, OS_DELETECHAN, OS_ENGFAIL);
		return;
	}

	CHANNELID chChannelId = ENG_GetIdOfChannel(pTarget);
	if (chChannelId == INVALID_CHANNEL_ID) {
		SEND_CLIENT_OS(dwClientID, OS_DELETECHAN, OS_ENGFAIL);
		return;
	}

	if (!ENG_DeleteChannel(chChannelId)) {
		SEND_CLIENT_OS(dwClientID, OS_DELETECHAN, OS_ENGFAIL);
	} else {
		SEND_CLIENT_OS(dwClientID, OS_DELETECHAN, OS_SUCCESSFUL);
	}
}
//======================================================================

//======================================================================
void CDH_KickUserFromChannel(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize)
{
	//Admin wants to kick a client out of a channel

	struct chanuser_s {
		char szChan[MAX_NETWORK_STRING_LENGTH];
		char szUser[MAX_NETWORK_STRING_LENGTH];
	} cu;

	if ((!pBuffer) || (dwBufSize != sizeof(cu))) {
		SEND_CLIENT_OS(dwClientID, OS_KICKCHANUSER, OS_INVALIDBUFFER);
		return;
	}

	memcpy(&cu, pBuffer, dwBufSize);

	if (!IsValidStrLen(cu.szChan) || (!IsValidStrLen(cu.szUser))) {
		SEND_CLIENT_OS(dwClientID, OS_KICKCHANUSER, OS_INVALIDBUFFER);
		return;
	}

	clientinfo_s* pClient = ENG_GetClientById(dwClientID);
	if ((!pClient) || (!pClient->bHasLoggedIn) || (!pClient->bIsAdmin)) {
		SEND_CLIENT_OS(dwClientID, OS_KICKCHANUSER, OS_BADUSER);
		return;
	}

	channel_s* pTarget = ENG_GetChannelByName(cu.szChan);
	if (!pTarget) {
		SEND_CLIENT_OS(dwClientID, OS_KICKCHANUSER, OS_ENGFAIL);
		return;
	}

	chan_user_s* pChanUser = ENG_GetUserByNameChan(pTarget, cu.szUser);
	if (!pChanUser) {
		SEND_CLIENT_OS(dwClientID, OS_KICKCHANUSER, OS_ENGFAIL);
		return;
	}

	if (!ENG_RemoveUserFromChannel(pChanUser, pTarget)) {
		SEND_CLIENT_OS(dwClientID, OS_KICKCHANUSER, OS_ENGFAIL);
	} else {
		SEND_CLIENT_OS(dwClientID, OS_KICKCHANUSER, OS_SUCCESSFUL);
	}
}
//======================================================================

//======================================================================
void CDH_SetGhostMode(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize)
{
	//Admin wants to set his ghostmode status

	if (!g_GlobalVars.pGhostModeAllowed->iValue) {
		SEND_CLIENT_OS(dwClientID, OS_GHOSTMODE, OS_NOTALLOWED);
		return;
	}

	bool bStatus;

	if ((!pBuffer) || (dwBufSize != sizeof(bStatus))) {
		SEND_CLIENT_OS(dwClientID, OS_GHOSTMODE, OS_INVALIDBUFFER);
		return;
	}

	memcpy(&bStatus, pBuffer, dwBufSize);

	clientinfo_s* pClient = ENG_GetClientById(dwClientID);
	if ((!pClient) || (!pClient->bHasLoggedIn) || (!pClient->bIsAdmin)) {
		SEND_CLIENT_OS(dwClientID, OS_GHOSTMODE, OS_BADUSER);
		return;
	}

	pClient->bGhost = bStatus;

	SEND_CLIENT_OS(dwClientID, OS_GHOSTMODE, OS_SUCCESSFUL);

	char szMsg[MAX_NETWORK_STRING_LENGTH];
	sprintf_s(szMsg, "Your ghost mode status: %s", (bStatus) ? "Enabled" : "Disabled");

	ServerNotice(ENG_GetClientId(pClient), ST_USER, szMsg, (DWORD)strlen(szMsg)+1);
}
//======================================================================

//======================================================================
void CDH_AuthAsChannelAdmin(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize)
{
	//A user wants to authenticate as channel administrator

	struct chanadmin_s {
		char szChan[MAX_NETWORK_STRING_LENGTH];
		char szPw[MAX_NETWORK_STRING_LENGTH];
	} ca;

	if ((!pBuffer) || (dwBufSize != sizeof(ca))) {
		SEND_CLIENT_OS(dwClientID, OS_AUSASCHANADMIN, OS_INVALIDBUFFER);
		return;
	}

	memcpy(&ca, pBuffer, dwBufSize);

	if (!IsValidStrLen(ca.szChan) || (!IsValidStrLen(ca.szPw))) {
		SEND_CLIENT_OS(dwClientID, OS_AUSASCHANADMIN, OS_INVALIDBUFFER);
		return;
	}

	clientinfo_s* pClient = ENG_GetClientById(dwClientID);
	if ((!pClient) || (!pClient->bHasLoggedIn)) {
		SEND_CLIENT_OS(dwClientID, OS_AUSASCHANADMIN, OS_BADUSER);
		return;
	}

	channel_s* pTarget = ENG_GetChannelByName(ca.szChan);
	if (!pTarget) {
		SEND_CLIENT_OS(dwClientID, OS_AUSASCHANADMIN, OS_ENGFAIL);
		return;
	}

	chan_user_s* pChanUser = ENG_GetUserByNameChan(pTarget, pClient->userinfo.szName);
	if (!pChanUser) {
		SEND_CLIENT_OS(dwClientID, OS_AUSASCHANADMIN, OS_ENGFAIL);
		return;
	}

	if (strcmp(ca.szPw, pTarget->szAdminPassword)==0) {
		pChanUser->bIsChannelAdmin = true;

		if (g_GlobalVars.pLogClientActions->iValue) {
			ConsolePrint(FOREGROUND_LIGHTBLUE, "Client %s(%d) has authenticated as channel admin for channel %s\n", pClient->userinfo.szName, dwClientID, pTarget->szName);
		}

		SEND_CLIENT_OS(dwClientID, OS_AUSASCHANADMIN, OS_SUCCESSFUL);
	} else {
		SEND_CLIENT_OS(dwClientID, OS_AUSASCHANADMIN, OS_BAD_PASSWORD);
	}
}
//======================================================================

//======================================================================
void CDH_SetChannelData(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize)
{
	//A channel admin wants to modify the channel data

	struct chandata_s {
		char szChannel[MAX_NETWORK_STRING_LENGTH];
		char szTopic[MAX_NETWORK_STRING_LENGTH]; 
		char szPassword[MAX_NETWORK_STRING_LENGTH];
		char szAdminPassword[MAX_NETWORK_STRING_LENGTH]; 
		CHUSERID dwMaxUsers;
	} cd;

	if ((!pBuffer) || (dwBufSize != sizeof(cd))) {
		SEND_CLIENT_OS(dwClientID, OS_SETCHANNELDATA, OS_INVALIDBUFFER);
		return;
	}

	memcpy(&cd, pBuffer, dwBufSize);

	if (!IsValidStrLen(cd.szChannel) || (!IsValidStrLen(cd.szTopic) || (!IsValidStrLen(cd.szPassword)) || (!IsValidStrLen(cd.szAdminPassword)))) {
		SEND_CLIENT_OS(dwClientID, OS_SETCHANNELDATA, OS_INVALIDBUFFER);
		return;
	}

	clientinfo_s* pClient = ENG_GetClientById(dwClientID);
	if ((!pClient) || (!pClient->bHasLoggedIn)) {
		SEND_CLIENT_OS(dwClientID, OS_SETCHANNELDATA, OS_BADUSER);
		return;
	}

	channel_s* pTarget = ENG_GetChannelByName(cd.szChannel);
	if (!pTarget) {
		SEND_CLIENT_OS(dwClientID, OS_SETCHANNELDATA, OS_ENGFAIL);
		return;
	}

	chan_user_s* pChanUser = ENG_GetUserByNameChan(pTarget, pClient->userinfo.szName);
	if (!pChanUser) {
		SEND_CLIENT_OS(dwClientID, OS_SETCHANNELDATA, OS_ENGFAIL);
		return;
	}

	if (pChanUser->bIsChannelAdmin) {
		if (strcmp(cd.szTopic, "#0")) {
			strcpy_s(pTarget->szTopic, cd.szTopic);
		}

		if (strcmp(cd.szPassword, "#0")) {
			strcpy_s(pTarget->szPassword, cd.szPassword);
		}

		if (strcmp(cd.szAdminPassword, "#0")) {
			strcpy_s(pTarget->szAdminPassword, cd.szAdminPassword);

			for (CHUSERID i = 0; i < (CHUSERID)pTarget->vUsers.size(); i++) {
				if (pTarget->vUsers[i] != pChanUser)
					pTarget->vUsers[i]->bIsChannelAdmin = false;
			}
		}

		if (cd.dwMaxUsers != INVALID_CHANNEL_ID) {
			pTarget->dwMaxUsers = cd.dwMaxUsers;
		}

		SEND_CLIENT_OS(dwClientID, OS_AUSASCHANADMIN, OS_SUCCESSFUL);
	} else {
		SEND_CLIENT_OS(dwClientID, OS_AUSASCHANADMIN, OS_BADUSER);
	}
}
//======================================================================

//======================================================================
void CDH_Plugin(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize)
{
	//Client wants to send a buffer to a handler of a plugin

	char szSignature[MAX_SIG_STRING];

	if ((!pBuffer) || (dwBufSize <= sizeof(szSignature))) {
		SEND_CLIENT_OS(dwClientID, OS_PLUGINMSG, OS_INVALIDBUFFER);
		return;
	}

	memcpy(szSignature, pBuffer, MAX_SIG_STRING);

	if (!IsValidStrLen(szSignature, MAX_SIG_STRING)) {
		SEND_CLIENT_OS(dwClientID, OS_PLUGINMSG, OS_INVALIDBUFFER);
		return;
	}

	clientinfo_s* pClient = ENG_GetClientById(dwClientID);
	if ((!pClient) || (!pClient->bHasLoggedIn)) {
		SEND_CLIENT_OS(dwClientID, OS_PLUGINMSG, OS_BADUSER);
		return;
	}

	if (!g_Objects.CTPM.CallHandler(szSignature, pClient, (LPCVOID)((DWORD_PTR)pBuffer + sizeof(szSignature)), dwBufSize - sizeof(szSignature))) {
		SEND_CLIENT_OS(dwClientID, OS_PLUGINMSG, OS_ENGFAIL);
	} else {
		SEND_CLIENT_OS(dwClientID, OS_PLUGINMSG, OS_SUCCESSFUL);
	}
}
//======================================================================

//======================================================================
void CDH_Quit(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize)
{
	//Client wants to quit the server

	char szQuitMsg[MAX_NETWORK_STRING_LENGTH];

	if ((!pBuffer) || (dwBufSize > sizeof(szQuitMsg))) {
		SEND_CLIENT_OS(dwClientID, OS_QUIT, OS_INVALIDBUFFER);
		return;
	}

	if (dwBufSize) {
		memcpy(szQuitMsg, pBuffer, dwBufSize);

		if (!IsValidStrLen(szQuitMsg)) {
			SEND_CLIENT_OS(dwClientID, OS_QUIT, OS_INVALIDBUFFER);
			return;
		}
	}

	clientinfo_s* pClient = ENG_GetClientById(dwClientID);
	if ((!pClient) || (!pClient->bHasLoggedIn)) {
		SEND_CLIENT_OS(dwClientID, OS_PLUGINMSG, OS_BADUSER);
		return;
	}

	if (!ENG_KickClient(pClient, (dwBufSize) ? szQuitMsg: S_DEFAULT_CLIENTLEAVEMSG, false)) {
		SEND_CLIENT_OS(dwClientID, OS_QUIT, OS_ENGFAIL);
	}
}
//======================================================================

//======================================================================
void Cmd_Help(CConParser* pParser)
{
	//Help command

	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "Available commands:\n");
	
	for (unsigned int i = 0; i < g_Objects.ConCommand.GetCmdAmount(); i++) {
		concommand_s cmddata;

		if (!g_Objects.ConCommand.GetConCommandData(i, &cmddata))
			continue;

		ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "#%d %s: \"%s\"\n", i + 1, cmddata.name, cmddata.description);
	}
}
//======================================================================

//======================================================================
void Cmd_Version(CConParser* pParser)
{
	//Show server version information

	DWORD dwIFVer = IFACEVERSION();

	static char szIfVer[100];
	sprintf_s(szIfVer, "%d.%d", *(WORD*)&dwIFVer, *(WORD*)((DWORD)&dwIFVer + 2));

	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX PROGRAM_NAME " coded by " PROGRAM_AUTHOR " (" PROGRAM_CONTACT ")\n");
	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "Version " PROGRAM_VERSION " :: interface version %s (built for " PLATFORM " platform)\n\n", szIfVer);
}
//======================================================================

//======================================================================
void Cmd_Status(CConParser* pParser)
{
	//Show server status

	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "Server component status:\n");
	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "========================\n");
	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "Platform: " PLATFORM "\n");
	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "Server running since: %d hours (%d days)\n", g_GlobalVars.dwElapsedHours, g_GlobalVars.dwElapsedHours / 24);
	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "Send type: %d\n", g_GlobalVars.pSendType->iValue);
	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "Server port: %d\n", g_GlobalVars.pClientPort->iValue);
	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "Remote control port: %d\n", g_GlobalVars.pRConPort->iValue);
	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "Amount of clients: %d/%d\n", g_Objects.ClientSock.GetClientCount(), g_GlobalVars.pMaxUsers->iValue);
	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "Single address amount: %d\n", g_GlobalVars.pSingleaddrAmount->iValue);
	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "Server name: %s\n", g_GlobalVars.pServerName->szValue);
	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "Forced client: %s\n", g_GlobalVars.pForcedClient->szValue);
	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "Masterserver status: %s:%d (%d)\n", g_GlobalVars.pMSAddr->szValue, g_GlobalVars.pMSPort->iValue, ENG_IsMSConnected());
	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "Program path: %s\n", g_GlobalVars.szAppPath);
	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "Scripts path: %s\n", g_GlobalVars.szScripts);
	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "Plugin path: %s\n", g_GlobalVars.szPluginDir);
	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "MOTD: %s\n", g_GlobalVars.szMOTD);
	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "Banlist: %s\n", g_GlobalVars.szIPBanList);
	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "Server password: %s\n", g_GlobalVars.pServerPW->szValue);
	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "Admin password: %s\n", g_GlobalVars.pAdminPW->szValue);
	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "Remote control password: %s\n", g_GlobalVars.pRConPW->szValue);
	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "Ghost mode allowed: %d\n", g_GlobalVars.pGhostModeAllowed->iValue);
	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "Update URL: %s\n", g_GlobalVars.pUpdateURL->szValue);
	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "AOTD: %s (%dx%d)\n", g_GlobalVars.aotd.szURL, g_GlobalVars.aotd.x, g_GlobalVars.aotd.y);
	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "Logging to disc: %d\n", g_GlobalVars.pLogToDisc->iValue);
	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "Log Client actions: %d\n", g_GlobalVars.pLogClientActions->iValue);
	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "Output log prefix: %d\n", g_GlobalVars.pOutputPrefix->iValue);
	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "Ping timeout value: %d\n", g_GlobalVars.pPingTimeout->iValue);
}
//======================================================================

//======================================================================
void Cmd_Readme(CConParser* pParser)
{
	//Show the readme.txt file with the standard editor

	char szReadme[MAX_PATH];
	sprintf_s(szReadme, "%sreadme.txt", g_GlobalVars.szAppPath);

	if (FileExists(szReadme))
		ShellExecuteA(0, "open", szReadme, "", g_GlobalVars.szAppPath, SW_SHOWNORMAL);
}
//======================================================================

//======================================================================
void Cmd_Exec(CConParser* pParser)
{
	//Execute a script file

	const char* szArg = pParser->GetArgument(1);
	if (!szArg) {
		ConsolePrint(FOREGROUND_RED, CMD_STDOUT_PREFIX "Please specify a script file name\n");
		return;
	}

	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "Executing script file: %s...\n", szArg);

	(g_Objects.oConfigInt.Execute(szArg)) ? ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "Execution successful\n") : ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "Execution failed\n");
}
//======================================================================

//======================================================================
void Cmd_BroadcastServerNotice(CConParser* pParser)
{
	//Broadcast a server notice message

	const char* pszMessage = pParser->GetArgument(1);
	if (!pszMessage) {
		ConsolePrint(FOREGROUND_RED, CMD_STDOUT_PREFIX "Please specify a text message\n");
		return;
	}

	char szMessage[MAX_NETWORK_STRING_LENGTH];
	strcpy_s(szMessage, pszMessage);

	(ServerNotice(INVALID_CLIENT_ID, ST_BROADCAST, szMessage, sizeof(szMessage))) ? ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "Broadcast of message \"%s\" succeeded\n", szMessage): ConsolePrint(FOREGROUND_RED, CMD_STDOUT_PREFIX "Broadcast of message \"%s\" failed\n", szMessage);
}
//======================================================================

//======================================================================
void Cmd_ChannelServerNotice(CConParser* pParser)
{
	//Send a notice message to a channel

	const char* pszChannel = pParser->GetArgument(1);
	if (!pszChannel) {
		ConsolePrint(FOREGROUND_RED, CMD_STDOUT_PREFIX "Please specify a channel name\n");
		return;
	}

	channel_s* pChannel = ENG_GetChannelByName(pszChannel);
	if (!pChannel) {
		ConsolePrint(FOREGROUND_RED, CMD_STDOUT_PREFIX "Channel %s does not exist\n", pszChannel);
		return;
	}

	CHANNELID chChannelId = ENG_GetIdOfChannel(pChannel);
	if (chChannelId == INVALID_CHANNEL_ID) {
		ConsolePrint(FOREGROUND_RED, CMD_STDOUT_PREFIX "Could not find an ID for the channel\n");
		return;
	}

	const char* pszMessage = pParser->GetArgument(2);
	if (!pszMessage) {
		ConsolePrint(FOREGROUND_RED, CMD_STDOUT_PREFIX "Please specify a text message\n");
		return;
	}

	char szMessage[MAX_NETWORK_STRING_LENGTH];
	strcpy_s(szMessage, pszMessage);

	(ServerNotice(chChannelId, ST_CHANNEL, szMessage, sizeof(szMessage))) ? ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "Channel message \"%s\" has been sent successfully\n", szMessage): ConsolePrint(FOREGROUND_RED, CMD_STDOUT_PREFIX "Channel message \"%s\" could not be sent\n", szMessage);
}
//======================================================================

//======================================================================
void Cmd_ClientServerNotice(CConParser* pParser)
{
	//Send a notice message to a client

	const char* pszClient = pParser->GetArgument(1);
	if (!pszClient) {
		ConsolePrint(FOREGROUND_RED, CMD_STDOUT_PREFIX "Please specify a user name\n");
		return;
	}

	clientinfo_s* pClient = ENG_GetClientByName((char*)pszClient);
	if (!pClient) {
		ConsolePrint(FOREGROUND_RED, CMD_STDOUT_PREFIX "The specified client could not be found\n");
		return;
	}

	CLIENTID ciClientId = ENG_GetClientId(pClient);
	if (ciClientId == INVALID_CLIENT_ID) {
		ConsolePrint(FOREGROUND_RED, CMD_STDOUT_PREFIX "Could not find an ID for the user\n");
		return;
	}

	const char* pszMessage = pParser->GetArgument(2);
	if (!pszMessage) {
		ConsolePrint(FOREGROUND_RED, CMD_STDOUT_PREFIX "Please specify a text message\n");
		return;
	}

	char szMessage[MAX_NETWORK_STRING_LENGTH];
	strcpy_s(szMessage, pszMessage);

	(ServerNotice(ciClientId, ST_USER, szMessage, sizeof(szMessage))) ? ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "User message \"%s\" has been sent successfully\n", szMessage): ConsolePrint(FOREGROUND_RED, CMD_STDOUT_PREFIX "User message \"%s\" could not be sent\n", szMessage);
}
//======================================================================

//======================================================================
void Cmd_ListClients(CConParser* pParser)
{
	//List all connected clients

	if (!ENG_GetClientCount()) {
		ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "There are no clients connected\n");
		return;
	}

	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, "\n" CMD_STDOUT_PREFIX "ID  |  IP  |  Clantag  |  Name  |  Client  |  Logged in  |  Is admin  |  Is Ghost  |  Is Bot\n\n");

	for (CLIENTAMOUNT i = 0; i < ENG_GetClientCount(); i++) {
		clientinfo_s* pClient = ENG_GetClientById(i);
		if (pClient) {
			ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "#%d [%s] %s:%s (%s) (%d | %d | %d | %d)\n", i + 1, AddressToString(pClient->saddr.sin_addr.S_un.S_addr), pClient->userinfo.szClanTag, pClient->userinfo.szName, pClient->userinfo.szClient, pClient->bHasLoggedIn, pClient->bIsAdmin, pClient->bGhost, pClient->bIsBot);
		}
	}

	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, "\n");
}
//======================================================================

//======================================================================
void Cmd_ListChannels(CConParser* pParser)
{
	//List all existing channels

	if (!ENG_GetChannelCount()) {
		ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "There are no existing channels\n");
		return;
	}

	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, "\n" CMD_STDOUT_PREFIX "ID  |  Name  |  Topic  |  Password  |  Users  |  Is registered\n\n");

	for (CHANNELAMOUNT i = 0; i < ENG_GetChannelCount(); i++) {
		channel_s* pChannel = ENG_GetChannelById(i);
		if (pChannel) {
			ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "#%d %s \"%s\" [%s | %s] (%d/%d) %d\n", i + 1, pChannel->szName, pChannel->szTopic, pChannel->szPassword, pChannel->szAdminPassword, pChannel->vUsers.size(), pChannel->dwMaxUsers, pChannel->bRegistered);
		}
	}

	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, "\n");
}
//======================================================================

//======================================================================
void Cmd_ListChanUsers(CConParser* pParser)
{
	//List the users of a channel

	const char* pszChannel = pParser->GetArgument(1);
	if (!pszChannel) {
		ConsolePrint(FOREGROUND_RED, CMD_STDOUT_PREFIX "Please specify a channel name\n");
		return;
	}

	channel_s* pChannel = ENG_GetChannelByName(pszChannel);
	if (!pChannel) {
		ConsolePrint(FOREGROUND_RED, CMD_STDOUT_PREFIX "Channel %s does not exist\n", pszChannel);
		return;
	}

	if (!pChannel->vUsers.size()) {
		ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "There are no users in this channels\n");
		return;
	}

	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, "\n" CMD_STDOUT_PREFIX "ID  |  Clan Tag  |  Name  |  Channel admin\n\n");

	for (CHUSERID i = 0; i < (CHUSERID)pChannel->vUsers.size(); i++) {
		if ((pChannel->vUsers[i]) && (pChannel->vUsers[i]->pClient)) {
			ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "#%d [%s] %s:%s %d\n", i + 1, AddressToString(pChannel->vUsers[i]->pClient->saddr.sin_addr.S_un.S_addr), pChannel->vUsers[i]->pClient->userinfo.szClanTag, pChannel->vUsers[i]->pClient->userinfo.szName, pChannel->vUsers[i]->bIsChannelAdmin);
		}
	}

	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, "\n");
}
//======================================================================

//======================================================================
void Cmd_CreateChannel(CConParser* pParser)
{
	//Create a new channel (registered)

	const char* pszName = pParser->GetArgument(1);
	if (!pszName) {
		ConsolePrint(FOREGROUND_RED, CMD_STDOUT_PREFIX "Please specify a channel name\n");
		return;
	}

	const char* pszTopic = pParser->GetArgument(2);
	if (!pszTopic) {
		ConsolePrint(FOREGROUND_RED, CMD_STDOUT_PREFIX "Please specify the channel topic\n");
		return;
	}

	const char* pszPassword = pParser->GetArgument(3);
	if (!pszPassword) {
		ConsolePrint(FOREGROUND_RED, CMD_STDOUT_PREFIX "Please specify a channel password\n");
		return;
	}

	const char* pszAuthPassword = pParser->GetArgument(4);
	if (!pszAuthPassword) {
		ConsolePrint(FOREGROUND_RED, CMD_STDOUT_PREFIX "Please specify an authentication password\n");
		return;
	}

	const char* pszMaxUsers = pParser->GetArgument(5);
	if (!pszMaxUsers) {
		ConsolePrint(FOREGROUND_RED, CMD_STDOUT_PREFIX "Please specify the maximum amount of users\n");
		return;
	}

	int iMaxUsers = atoi(pszMaxUsers);
	if (iMaxUsers <= 0) iMaxUsers = 10;

	(ENG_CreateChannel((char*)pszName, (char*)pszPassword, (char*)pszAuthPassword, (char*)pszTopic, iMaxUsers, true)) ? ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "Channel %s has been created\n", pszName): ConsolePrint(FOREGROUND_RED, CMD_STDOUT_PREFIX "Could not create channel %s\n", pszName);
}
//======================================================================

//======================================================================
void Cmd_DeleteChannel(CConParser* pParser)
{
	//Delete a channel

	const char* pszName = pParser->GetArgument(1);
	if (!pszName) {
		ConsolePrint(FOREGROUND_RED, CMD_STDOUT_PREFIX "Please specify a channel name\n");
		return;
	}

	channel_s* pChannel = ENG_GetChannelByName(pszName);
	if (!pChannel) {
		ConsolePrint(FOREGROUND_RED, CMD_STDOUT_PREFIX "Channel %s does not exist\n", pszName);
		return;
	}

	CHANNELID chChannelId = ENG_GetIdOfChannel(pChannel);
	if (chChannelId == INVALID_CHANNEL_ID) {
		ConsolePrint(FOREGROUND_RED, CMD_STDOUT_PREFIX "Could not find an ID for the channel\n");
		return;
	}

	(ENG_DeleteChannel(chChannelId)) ? ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "Channel %s has been deleted\n", pszName) : ConsolePrint(FOREGROUND_RED, CMD_STDOUT_PREFIX "Could not delete channel %s\n", pszName);
}
//======================================================================

//======================================================================
void Cmd_KickClient(CConParser* pParser)
{
	//Kick a client

	const char* pszName = pParser->GetArgument(1);
	if (!pszName) {
		ConsolePrint(FOREGROUND_RED, CMD_STDOUT_PREFIX "Please specify a user name\n");
		return;
	}

	clientinfo_s* pClient = ENG_GetClientByName((char*)pszName);
	if (!pClient) {
		ConsolePrint(FOREGROUND_RED, CMD_STDOUT_PREFIX "The specified client could not be found\n");
		return;
	}

	const char* pszReason = pParser->GetArgument(2);

	(ENG_KickClient(pClient, (pszReason) ? pszReason : S_DEFAULT_KICKMSG, false)) ? ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "Client %s has been kicked (%s)\n", pszName, (pszReason) ? pszReason : S_DEFAULT_KICKMSG): ConsolePrint(FOREGROUND_RED, CMD_STDOUT_PREFIX "Could not kick client %s\n", pszName);
}
//======================================================================

//======================================================================
void Cmd_BanClient(CConParser* pParser)
{
	//Ban a client

	const char* pszName = pParser->GetArgument(1);
	if (!pszName) {
		ConsolePrint(FOREGROUND_RED, CMD_STDOUT_PREFIX "Please specify a user name\n");
		return;
	}

	clientinfo_s* pClient = ENG_GetClientByName((char*)pszName);
	if (!pClient) {
		ConsolePrint(FOREGROUND_RED, CMD_STDOUT_PREFIX "The specified client could not be found\n");
		return;
	}

	const char* pszReason = pParser->GetArgument(2);

	(ENG_KickClient(pClient, (pszReason) ? pszReason : S_DEFAULT_BANMSG, true)) ? ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "Client %s has been banned (%s)\n", pszName, (pszReason) ? pszReason : S_DEFAULT_BANMSG) : ConsolePrint(FOREGROUND_RED, CMD_STDOUT_PREFIX "Could not ban client %s\n", pszName);
}
//======================================================================

//======================================================================
void Cmd_KickChanUser(CConParser* pParser)
{
	//Kick a user out of a channel

	const char* pszName = pParser->GetArgument(1);
	if (!pszName) {
		ConsolePrint(FOREGROUND_RED, CMD_STDOUT_PREFIX "Please specify a user name\n");
		return;
	}

	const char* pszChannel = pParser->GetArgument(2);
	if (!pszChannel) {
		ConsolePrint(FOREGROUND_RED, CMD_STDOUT_PREFIX "Please specify a channel name\n");
		return;
	}

	channel_s* pChannel = ENG_GetChannelByName(pszChannel);
	if (!pChannel) {
		ConsolePrint(FOREGROUND_RED, CMD_STDOUT_PREFIX "The specified channel does not exist\n");
		return;
	}

	chan_user_s* pChanUser = ENG_GetUserByNameChan(pChannel, pszName);
	if (!pChanUser) {
		ConsolePrint(FOREGROUND_RED, CMD_STDOUT_PREFIX "There is no user with this name in this channel\n");
		return;
	}

	(ENG_RemoveUserFromChannel(pChanUser, pChannel)) ? ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "User %s has been kicked from channel %s\n", pszName, pszChannel): ConsolePrint(FOREGROUND_RED, CMD_STDOUT_PREFIX "Could not kick user %s from channel %s\n", pszName, pszChannel);
}
//======================================================================

//======================================================================
void Cmd_ReloadMotd(CConParser* pParser)
{
	//Reload the MOTD content

	char szMotdFile[MAX_PATH];

	const char* pszFile = pParser->GetArgument(1);
	if (pszFile) {
		sprintf_s(szMotdFile, "%s%s", g_GlobalVars.szAppPath, pszFile);
	} else {
		strcpy_s(szMotdFile, g_GlobalVars.szMOTD);
	}

	(g_Objects.MOTD.Reload(szMotdFile)) ? ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "MOTD file content reloaded from: %s\n", szMotdFile) : ConsolePrint(FOREGROUND_RED, CMD_STDOUT_PREFIX "Could not load MOTD content from %s\n", szMotdFile);
}
//======================================================================

//======================================================================
void Cmd_ShowMotd(CConParser* pParser)
{
	//Show MOTD content

	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "Current MOTD content:\n\n");

	if (!g_Objects.MOTD.GetLineCount()) {
		ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "(Empty)\n");
		return;
	}

	bool bDynVars = false;

	const char* pszWithDynVars = pParser->GetArgument(1);
	if ((pszWithDynVars) && (pszWithDynVars[0])) {
		bDynVars = atoi(pszWithDynVars) != 0;
	}

	for (DWORD i = 0; i < g_Objects.MOTD.GetLineCount(); i++) {
		char* pszLine = g_Objects.MOTD.GetLine(i);
		if (pszLine) {
			if (bDynVars) {
				std::string str = g_Objects.DynVars.HandleExpression(pszLine);

				if (str.length() > 0)
					ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "%s\n", str.c_str());
			} else {
				ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "%s\n", pszLine);
			}
		}
	}

	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, "\n");
}
//======================================================================

//======================================================================
void Cmd_ListPlugins(CConParser* pParser)
{
	//List all loaded plugins

	if (!g_Objects.Plugins.GetPluginCount()) {
		ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "There are no plugins loaded\n");
		return;
	}

	for (PLUGINID i = 0; i < g_Objects.Plugins.GetPluginCount(); i++) {
		plugin_s* pPlugin = g_Objects.Plugins.GetPluginById(i);
		if (pPlugin)
			ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "#%d %s (%s)\n", i + 1, pPlugin->infos.plName, pPlugin->name);
	}
}
//======================================================================

//======================================================================
void Cmd_PluginInfos(CConParser* pParser)
{
	//List specific plugin informations of a plugin

	const char* pszName = pParser->GetArgument(1);
	if (!pszName) {
		ConsolePrint(FOREGROUND_RED, CMD_STDOUT_PREFIX "Please specify a plugin ID\n");
		return;
	}

	PLUGINID pidPluginId = atoi(pszName);
	if (pidPluginId == 0)
		pidPluginId = 1;

	plugin_s* pPlugin = g_Objects.Plugins.GetPluginById(pidPluginId - 1);
	if (!pPlugin) {
		ConsolePrint(FOREGROUND_RED, CMD_STDOUT_PREFIX "Plugin with ID %d could not be found\n", pidPluginId);
		return;
	}

	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "Plugin informations\n");
	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "===================\n");
	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "Module: %s\n", pPlugin->name);
	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "Handle: 0x%p\n", pPlugin->hModule);
	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "Name: %s\n", pPlugin->infos.plName);
	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "Version: %s\n", pPlugin->infos.plVersion);
	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "Description: %s\n", pPlugin->infos.plDescription);
	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "Author: %s\n", pPlugin->infos.plAuthor);
	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "Contact: %s\n", pPlugin->infos.plContact);
	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "Pre event table: 0x%p\n", &pPlugin->evttable.events_pre);
	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "Post event table: 0x%p\n", &pPlugin->evttable.events_post);
	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "Is paused: %d\n", pPlugin->bPaused);
	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, "\n");
}
//======================================================================

//======================================================================
void Cmd_LoadPlugin(CConParser* pParser)
{
	//Load a plugin

	const char* pszName = pParser->GetArgument(1);
	if (!pszName) {
		ConsolePrint(FOREGROUND_RED, CMD_STDOUT_PREFIX "Please specify a plugin module file name\n");
		return;
	}

	char szPluginFile[MAX_PATH];
	sprintf_s(szPluginFile, "%s%s", g_GlobalVars.szPluginDir, pszName);

	if (!FileExists(szPluginFile)) {
		ConsolePrint(FOREGROUND_RED, CMD_STDOUT_PREFIX "Plugin %s could not be found\n", szPluginFile);
		return;
	}

	(g_Objects.Plugins.LoadPlugin(szPluginFile, true)) ? ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "Plugin %s has been loaded\n", szPluginFile) : ConsolePrint(FOREGROUND_RED, CMD_STDOUT_PREFIX "Could not load plugin %s\n", szPluginFile);
}
//======================================================================

//======================================================================
void Cmd_UnloadPlugin(CConParser* pParser)
{
	//Unload a plugin

	const char* pszName = pParser->GetArgument(1);
	if (!pszName) {
		ConsolePrint(FOREGROUND_RED, CMD_STDOUT_PREFIX "Please specify a plugin ID\n");
		return;
	}

	PLUGINID pidPluginId = atoi(pszName);
	if (pidPluginId == 0)
		pidPluginId = 1;

	plugin_s* pPlugin = g_Objects.Plugins.GetPluginById(pidPluginId - 1);
	if (!pPlugin) {
		ConsolePrint(FOREGROUND_RED, CMD_STDOUT_PREFIX "Plugin with ID %d could not be found\n", pidPluginId);
		return;
	}

	(g_Objects.Plugins.RemovePlugin(pidPluginId - 1)) ? ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "Plugin %d has been unloaded\n", pidPluginId): ConsolePrint(FOREGROUND_RED, CMD_STDOUT_PREFIX "Could not unload plugin %d\n", pidPluginId);
}
//======================================================================

//======================================================================
void Cmd_PluginPause(CConParser* pParser)
{
	//Set the pause status of a plugin

	const char* pszName = pParser->GetArgument(1);
	if (!pszName) {
		ConsolePrint(FOREGROUND_RED, CMD_STDOUT_PREFIX "Please specify a plugin ID\n");
		return;
	}

	const char* pszValue = pParser->GetArgument(2);
	if (!pszValue) {
		ConsolePrint(FOREGROUND_RED, CMD_STDOUT_PREFIX "Please specify the pause value\n");
		return;
	}

	PLUGINID pidPluginId = atoi(pszName);
	if (pidPluginId == 0)
		pidPluginId = 1;

	plugin_s* pPlugin = g_Objects.Plugins.GetPluginById(pidPluginId - 1);
	if (!pPlugin) {
		ConsolePrint(FOREGROUND_RED, CMD_STDOUT_PREFIX "Plugin with ID %d could not be found\n", pidPluginId);
		return;
	}

	pPlugin->bPaused = (BOOL)(atoi(pszValue)) == TRUE;

	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "Plugin pause value of plugin %d: %d\n", pidPluginId, pPlugin->bPaused);
}
//======================================================================

//======================================================================
void Cmd_Update(CConParser* pParser)
{
	//Update the server component

	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "Checking for program updates...\n");

	if (!g_Objects.Update.OpenInternetHandle()) {
		ConsolePrint(FOREGROUND_RED, CMD_STDOUT_PREFIX "CUpdate::OpenInternetHandle failed\n");
		return;
	}

	(g_Objects.Update.QueryUpdateInfo()) ? ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "Update done\n") : ConsolePrint(FOREGROUND_RED, CMD_STDOUT_PREFIX "Update failed or no new version available\n");

	g_Objects.Update.CloseInternetHandle();
}
//======================================================================

//======================================================================
void Cmd_Restart(CConParser* pParser)
{
	//Attempt server program restart

	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "Attempting program restart...\n");

	if (!RestartProgram())
		ConsolePrint(FOREGROUND_RED, CMD_STDOUT_PREFIX "RestartProgram() failed\n");
}
//======================================================================

//======================================================================
void Cmd_QuitApplication(CConParser* pParser)
{
	//Quit server by setting the main loop control variable to false

	g_bProgramRunning = false;
}
//======================================================================

//======================================================================
void Cmd_Echo(void)
{
	//Print text output

	if (g_PrintOutput.type == PO_STDOUT) { //Send text to stdout and log to hard disc (if desired)
		ConsolePrint(FOREGROUND_PINK, "%s\n", g_Objects.oConfigInt.ExpressionItemValue(1).c_str());

		//long long ulValue = (g_GlobalVars.pLogToDisc) ? g_GlobalVars.pLogToDisc->iValue : 0;

		//if (ulValue)
			//LogMessage("%s", lpszOutputText);
	}
	else if (g_PrintOutput.type == PO_RCON) { //Send text to the rcon using client
		g_Objects.RCon.SendText(g_Objects.oConfigInt.ExpressionItemValue(1).c_str());
	}
}
//======================================================================

//======================================================================
void Cmd_CreateChannel(void)
{
	//Create a channel

	std::string szArg1 = g_Objects.oConfigInt.ExpressionItemValue(1);
	std::string szArg2 = g_Objects.oConfigInt.ExpressionItemValue(2);
	std::string szArg3 = g_Objects.oConfigInt.ExpressionItemValue(3);
	std::string szArg4 = g_Objects.oConfigInt.ExpressionItemValue(4);
	std::string szArg5 = g_Objects.oConfigInt.ExpressionItemValue(5);

	ENG_CreateChannel((char*)szArg1.c_str(), (char*)szArg2.c_str(), (char*)szArg3.c_str(), (char*)szArg4.c_str(), atoi(szArg5.c_str()), true);
}
//======================================================================

//======================================================================
void Cmd_LoadPlugin(void)
{
	//Load a plugin

	std::string szArg1 = g_Objects.oConfigInt.ExpressionItemValue(1);
	std::string szArg2 = g_Objects.oConfigInt.ExpressionItemValue(2);

	bool bManually;

	if (szArg2.length())
		bManually = (BOOL)atoi(szArg2.c_str()) == TRUE;
	else
		bManually = false;

	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, "Loading plugin \"%s\"... ", szArg1.c_str());

	(g_Objects.Plugins.LoadPlugin(szArg1.c_str(), bManually)) ? ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, "Done\n") : ConsolePrint(FOREGROUND_RED, "Failed\n");
}
//======================================================================

//======================================================================
void Cmd_SetBanner(void)
{
	//Set banner data

	std::string szArg1 = g_Objects.oConfigInt.ExpressionItemValue(1);
	std::string szArg2 = g_Objects.oConfigInt.ExpressionItemValue(2);
	std::string szArg3 = g_Objects.oConfigInt.ExpressionItemValue(3);

	if ((szArg1.length()) && (szArg2.length()) && (szArg3.length())) {
		strcpy_s(g_GlobalVars.aotd.szURL, szArg1.c_str());
		g_GlobalVars.aotd.x = atoi(szArg2.c_str());
		g_GlobalVars.aotd.y = atoi(szArg3.c_str());
	}
}
//======================================================================

//======================================================================
void DV_Author(char* szValue)
{
	//DynVar callback function for: Product author

	if (!szValue)
		return;

	strcpy_s(szValue, DYNVAR_MAX_VALUE_LEN, PROGRAM_AUTHOR);
}
//======================================================================

//======================================================================
void DV_Version(char* szValue)
{
	//DynVar callback function for: Product version

	if (!szValue)
		return;

	strcpy_s(szValue, DYNVAR_MAX_VALUE_LEN, PROGRAM_VERSION);
}
//======================================================================

//======================================================================
void DV_Platform(char* szValue)
{
	//DynVar callback function for: platform

	if (!szValue)
		return;

	strcpy_s(szValue, DYNVAR_MAX_VALUE_LEN, PLATFORM);
}
//======================================================================

//======================================================================
void DV_ServerName(char* szValue)
{
	//DynVar callback function for: Server name

	if (!szValue)
		return;

	strcpy_s(szValue, DYNVAR_MAX_VALUE_LEN, g_GlobalVars.pServerName->szValue);
}
//======================================================================

//======================================================================
void DV_ServerPort(char* szValue)
{
	//DynVar callback function for: Server port

	if (!szValue)
		return;

	sprintf_s(szValue, DYNVAR_MAX_VALUE_LEN, "%d", g_GlobalVars.pClientPort->iValue);
}
//======================================================================

//======================================================================
void DV_Uptime(char* szValue)
{
	//DynVar callback function for: Server uptime (hours)

	if (!szValue)
		return;

	sprintf_s(szValue, DYNVAR_MAX_VALUE_LEN, "%d", g_GlobalVars.dwElapsedHours);
}
//======================================================================

//======================================================================
void DV_DateTime(char* szValue)
{
	//DynVar callback function for: Date and time

	if (!szValue)
		return;

	strcpy_s(szValue, DYNVAR_MAX_VALUE_LEN, GetCurrentDateTime());
}
//======================================================================

//======================================================================
void DV_ForcedClient(char* szValue)
{
	//DynVar callback function for: forced client

	if (!szValue)
		return;

	strcpy_s(szValue, DYNVAR_MAX_VALUE_LEN, g_GlobalVars.pForcedClient->szValue);
}
//======================================================================

//======================================================================
void DV_CurrentUsers(char* szValue)
{
	//DynVar callback function for: current user amount

	if (!szValue)
		return;

	sprintf_s(szValue, DYNVAR_MAX_VALUE_LEN, "%d", ENG_GetClientCount());
}
//======================================================================

//======================================================================
void DV_MaximumUsers(char* szValue)
{
	//DynVar callback function for: maximum user amount

	if (!szValue)
		return;

	sprintf_s(szValue, DYNVAR_MAX_VALUE_LEN, "%d", g_GlobalVars.pMaxUsers->iValue);
}
//======================================================================

//======================================================================
void DV_CurrentChans(char* szValue)
{
	//DynVar callback function for: current channel amount

	if (!szValue)
		return;

	sprintf_s(szValue, DYNVAR_MAX_VALUE_LEN, "%d", ENG_GetChannelCount());
}
//======================================================================

//======================================================================
void DV_MaximumChans(char* szValue)
{
	//DynVar callback function for: maximum channel amount

	if (!szValue)
		return;

	sprintf_s(szValue, DYNVAR_MAX_VALUE_LEN, "%d", g_GlobalVars.pMaxChannels->iValue);
}
//======================================================================

//======================================================================
void DV_UserNames(char* szValue)
{
	//DynVar callback function for: all user names

	if (!szValue)
		return;

	char szUserNames[DYNVAR_MAX_VALUE_LEN];
	szUserNames[0] = 0;

	for (CLIENTAMOUNT i = 0; i < ENG_GetClientCount(); i++) {
		clientinfo_s* pClient = ENG_GetClientById(i);
		if ((pClient) && (pClient->bHasLoggedIn)) {
			//Don't go beyond the array
			if (strlen(szUserNames) + strlen(pClient->userinfo.szName)  + 1 >= DYNVAR_MAX_VALUE_LEN)
				break;

			strcat_s(szUserNames, pClient->userinfo.szName);

			if (i < ENG_GetClientCount() - 1)
				strcat_s(szUserNames, ", ");
		}
	}

	szUserNames[DYNVAR_MAX_VALUE_LEN - 1] = 0;

	strcpy_s(szValue, DYNVAR_MAX_VALUE_LEN, szUserNames);
}
//======================================================================

//======================================================================
void DV_ChanNames(char* szValue)
{
	//DynVar callback function for: all channel names

	if (!szValue)
		return;

	char szChanNames[DYNVAR_MAX_VALUE_LEN];
	szChanNames[0] = 0;

	for (CHANNELID i = 0; i < ENG_GetChannelCount(); i++) {
		channel_s* pChannel = ENG_GetChannelById(i);
		if (pChannel) {
			//Don't go beyond the array
			if (strlen(szChanNames) + strlen(pChannel->szName)  + 1 > DYNVAR_MAX_VALUE_LEN)
				break;

			strcat_s(szChanNames, pChannel->szName);

			if (i < ENG_GetChannelCount() - 1)
				strcat_s(szChanNames, ", ");
		}
	}

	szChanNames[DYNVAR_MAX_VALUE_LEN - 1] = 0;
	
	strcpy_s(szValue, DYNVAR_MAX_VALUE_LEN, szChanNames);
}
//======================================================================

//======================================================================
void DV_UpdateURL(char* szValue)
{
	//DynVar callback function for: update URL

	if (!szValue)
		return;

	strcpy_s(szValue, DYNVAR_MAX_VALUE_LEN, g_GlobalVars.pUpdateURL->szValue);
}
//======================================================================

//======================================================================
void DV_Masterserver(char* szValue)
{
	//DynVar callback function for: master server address

	if (!szValue)
		return;

	sprintf_s(szValue, DYNVAR_MAX_VALUE_LEN, "%s:%d", g_GlobalVars.pMSAddr->szValue, g_GlobalVars.pMSPort->iValue);
}
//======================================================================

//======================================================================
void DV_AOTD(char* szValue)
{
	//DynVar callback function for: AOTD data

	if (!szValue)
		return;

	sprintf_s(szValue, DYNVAR_MAX_VALUE_LEN, "%s (%dx%d)", g_GlobalVars.aotd.szURL, g_GlobalVars.aotd.x	, g_GlobalVars.aotd.y);
}
//======================================================================

//======================================================================
void DV_GhostMode(char* szValue)
{
	//DynVar callback function for: ghost mode allowed

	if (!szValue)
		return;

	sprintf_s(szValue, DYNVAR_MAX_VALUE_LEN, "%d", (int)g_GlobalVars.pGhostModeAllowed->iValue);
}
//======================================================================

//======================================================================
void DV_Pluginamount(char* szValue)
{
	//DynVar callback function for: amount of plugins

	if (!szValue)
		return;

	sprintf_s(szValue, DYNVAR_MAX_VALUE_LEN, "%d", g_Objects.Plugins.GetPluginCount());
}
//======================================================================

//======================================================================
void DV_Plugins(char* szValue)
{
	//DynVar callback function for: all plugin names

	if (!szValue)
		return;

	char szPluginNames[DYNVAR_MAX_VALUE_LEN];
	szPluginNames[0] = 0;

	for (unsigned int i = 0; i < g_Objects.Plugins.GetPluginCount(); i++) {
		plugin_s* pPlugin = g_Objects.Plugins.GetPluginById(i);
		if (pPlugin) {
			//Don't go beyond the array
			if (strlen(szPluginNames) + strlen(pPlugin->name)  + 1 >= DYNVAR_MAX_VALUE_LEN)
				break;

			strcat_s(szPluginNames, pPlugin->name);

			if (i < g_Objects.Plugins.GetPluginCount() - 1)
				strcat_s(szPluginNames, ", ");
		}
	}

	szPluginNames[DYNVAR_MAX_VALUE_LEN - 1] = 0;

	strcpy_s(szValue, DYNVAR_MAX_VALUE_LEN, szPluginNames);
}
//======================================================================

