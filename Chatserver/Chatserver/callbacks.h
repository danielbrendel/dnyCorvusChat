#ifndef _CALLBACKS_H
#define _CALLBACKS_H

#include "includes.h"
#include "conparser.h"
#include "plugins.h"

/*
	CorvusChat Server (dnyCorvusChat) - Chatserver component

	Developed by Daniel Brendel

	Version: 0.5
	Visit: https://github.com/danielbrendel
	Mail dbrendel1988@gmail.com

	File: callbacks.h: Callback functions interface 
*/

//======================================================================
#define CONSOLE_ATTRIBUTE_DEFAULT 0
#define FOREGROUND_YELLOW FOREGROUND_RED | FOREGROUND_GREEN
#define FOREGROUND_PINK 13
#define FOREGROUND_LIGHTBLUE 11

#define CONFIRM_CLIENT_MESSAGE(clid) if (g_GlobalVars.bSendType) { g_Objects.UserMsg.MessageBegin(g_Objects.UserMsg.GetMessageByName("MessageRecieved"), clid, ST_USER);  g_Objects.UserMsg.MessageEnd(&g_Objects.ClientSock); } //Macro for client message confirmation
#define SEND_CLIENT_OS(clid, status_gen, status_det) g_Objects.UserMsg.MessageBegin(g_Objects.UserMsg.GetMessageByName("OperationStatus"), clid, ST_USER);  g_Objects.UserMsg.WriteShort(status_gen); g_Objects.UserMsg.WriteShort(status_det); g_Objects.UserMsg.MessageEnd(&g_Objects.ClientSock); //Macro to send operation status to client
//======================================================================

//======================================================================
 //These informations will be sent to clients:

struct clinfo_s { 
    unsigned int clId;
    char szName[MAX_NETWORK_STRING_LENGTH];
	char szClanTag[MAX_CLANTAG_STR_LEN];
    char szClient[MAX_NETWORK_STRING_LENGTH];
    unsigned int uiPingValue;
    unsigned char ucaAddress[sizeof(DWORD)];
    bool bPutInServ;
    bool bIsAdmin;
    bool bGhost;
	bool bIsBot;
    char szEmail[MAX_NETWORK_STRING_LENGTH];
    char szFullname[MAX_NETWORK_STRING_LENGTH];
    char szCountry[MAX_NETWORK_STRING_LENGTH];
};

struct chaninfo_s {
	unsigned int chId;
	CHUSERID iMaxUsers;
	CHUSERID iCurrentUsers;
	bool bPassword;
	char szName[MAX_NETWORK_STRING_LENGTH];
	char szTopic[MAX_NETWORK_STRING_LENGTH];
};

struct login_s {
	char szName[MAX_NETWORK_STRING_LENGTH];
	char szClanTag[MAX_CLANTAG_STR_LEN];
	char szClient[MAX_NETWORK_STRING_LENGTH];
	char szPassword[MAX_NETWORK_STRING_LENGTH];
};

struct chanmsg_s {
	char szSender[MAX_NETWORK_STRING_LENGTH];
	char szChannel[MAX_NETWORK_STRING_LENGTH];
	char szMessage[MAX_NETWORK_STRING_LENGTH];
};

struct privmsg_s {
	char szSender[MAX_NETWORK_STRING_LENGTH];
	char szMessage[MAX_NETWORK_STRING_LENGTH];
};

struct leavemsg_s {
	CLIENTID ciClientId;
	char szClient[MAX_NETWORK_STRING_LENGTH];
	char szReason[MAX_NETWORK_STRING_LENGTH];
};

struct chanaction_s {
	char szClient[MAX_NETWORK_STRING_LENGTH];
	char szChannel[MAX_NETWORK_STRING_LENGTH];
};

struct topic_s { 
	char szChannel[MAX_NETWORK_STRING_LENGTH];
	char szTopic[MAX_NETWORK_STRING_LENGTH];
};
//======================================================================

//======================================================================
VOID ConsolePrint(WORD wConAttributes, LPCSTR lpszFmt, ...);

bool Event_OnClientConnect(const CLIENTID ciClientID);
void Event_OnClientDisconnect(const CLIENTID ciClientID, const char* pszReason);
void Event_OnClientWrite(const CLIENTID ciClientID, const BYTE* pBuffer, DWORD dwSize);
void Event_OnClientUpdatePing(const CLIENTID ciClientID, BYTE ucPingValue);
void Event_OnErrorOccured(DWORD dwErrorValue);

DWORD CCSAPI ENG_GetServerVersion(void);
DWORD CCSAPI ENG_GetInterfaceVersion(void);
void CCSAPI ENG_ConsolePrint(LPCSTR lpszFmt, ...);
void CCSAPI ENG_FatalError(const char* szErrMsg, ...);
void CCSAPI ENG_LogMessage(const char *lpszMessage, ...);
void CCSAPI ENG_LogError(const char *lpszError, ...);
bool CCSAPI ENG_ClientConnect(const CLIENTID ciClientId);
bool CCSAPI ENG_ClientJoined(const CLIENTID ciClientId, const login_s* pLoginInfo);
void CCSAPI ENG_ClientDisconnect(const CLIENTID ciClientId, const char* szReason);
bool CCSAPI ENG_ClientPrivMsg(clientinfo_s *pSender, clientinfo_s *pRecv, char* szChatMsg);
bool CCSAPI ENG_ClientChanMsg(clientinfo_s *pSender, channel_s *pRecv, char* szChanMsg);
const CLIENTAMOUNT CCSAPI ENG_GetClientCount(void);
clientinfo_s* CCSAPI ENG_GetClientById(CLIENTID id);
clientinfo_s* CCSAPI ENG_GetClientByName(char* szName);
CLIENTID CCSAPI ENG_GetClientId(clientinfo_s* pClient);
clientinfo_s* CCSAPI ENG_CreateBot(char *pszName, char *pszClanTag, char *pszClient, char *pszFullName, char *pszEmail, char *pszCountry, bool bAdmin);
bool CCSAPI ENG_KickClient(clientinfo_s* pClientToKick, char *szReason, bool bAddToBanList);
bool CCSAPI ENG_CreateChannel(char *szName, char *szPassword, char *szAuthPassword, char *szTopic, DWORD dwMaxUsers, bool bRegistered); 
bool CCSAPI ENG_DeleteChannel(CHANNELID chChannelId); 
CHANNELID CCSAPI ENG_GetIdOfChannel(channel_s* pChannel); 
channel_s *CCSAPI ENG_GetChannelById(const CHANNELID chChannelId); 
channel_s *CCSAPI ENG_GetChannelByName(const char *szName); 
const CHANNELID CCSAPI ENG_GetChannelCount(void);
bool CCSAPI ENG_IsUserInChannel(const clientinfo_s *pClient, const channel_s *pChannel, chan_user_s** ppChanUser); 
chan_user_s *CCSAPI ENG_GetUserByIdChan(const channel_s* pChannel, const CHUSERID dwUserId); 
chan_user_s *CCSAPI ENG_GetUserByNameChan(const channel_s* pChannel, const char *szName); 
CHUSERID CCSAPI ENG_GetUserIdOfChannel(const channel_s* pChannel, const chan_user_s* pUser); 
bool CCSAPI ENG_PutUserInChannel(clientinfo_s *pClient, channel_s *pChannel); 
bool CCSAPI ENG_RemoveUserFromChannel(chan_user_s* pUser, channel_s *pChannel);
bool CCSAPI ENG_EstablishMSConnection(const char *szIpAddress, const WORD wPort);
bool CCSAPI ENG_CloseMSConnection(void);
void CCSAPI ENG_UpdateMSData(struct serverinfo_s *pData);
bool CCSAPI ENG_IsMSConnected(void);
int CCSAPI ENG_RegUserMessage(char *szName, int iSize);
int CCSAPI ENG_GetMessageByName(char* szMsg);
bool CCSAPI ENG_MessageBegin(int iMsgID, CLIENTID dwRecvID, BYTE SendType);
void CCSAPI ENG_WriteByte(BYTE bValue);
void CCSAPI ENG_WriteChar(char cValue);
void CCSAPI ENG_WriteShort(short sValue);
void CCSAPI ENG_WriteInt(int iValue);
void CCSAPI ENG_WriteFloat(float fValue);
void CCSAPI ENG_WriteString(char *szString, DWORD dwStringSize);
void CCSAPI ENG_WriteBuf(void* pBuf, DWORD dwBufSize);
bool CCSAPI ENG_MessageEnd(CClientSocket* pSocket);
bool CCSAPI ENG_AddConCommand(char *szCmdName, char *szCmdDescription, const void* pCmdProc);
bool CCSAPI ENG_DeleteConCommand(char* szCmdName);
bool CCSAPI ENG_ExecScript(const char *szScriptFile);
void CCSAPI ENG_ExecCode(const char *szScriptCode);
cvar_s* ENG_RegisterCVar(const char* szName, const CVarType_e eType, const char* pszDefaultValue);
bool ENG_RemoveCVar(const char* szName);
cvar_s* ENG_GetCVar(const char* pszName);
bool ENG_SetCVarValueString(const char* pszName, const char* szValue);
bool ENG_SetCVarValueInteger(const char* pszName, const int iValue);
bool ENG_SetCVarValueFloat(const char* pszName, const double dblValue);
bool CCSAPI ENG_AddClientHandler(struct ctpm_handler_s* pHandler);
bool CCSAPI ENG_RemoveClientHandler(char* cHandlerSig);
CClientSocket* CCSAPI ENG_GetClientInterface(void);
DWORD CCSAPI ENG_GetCommandCount(CConParser *pParser);
const char* CCSAPI ENG_GetCommandArg(CConParser *pParser, DWORD dwIndex);
bool CCSAPI ENG_IsDestinationReachable(LPCSTR lpszServer);
const char* CCSAPI ENG_AddressToString(const unsigned long ulIPAddress);
bool CCSAPI ENG_FileExists(const char* szFileName);
bool CCSAPI ENG_DirectoryExists(char* szDir);
void CCSAPI ENG_ExtractFilePath(char *szFileName);
char *CCSAPI ENG_ExtractFileName(char *filePath);
const char* CCSAPI ENG_GetCurrentDateTime(void);
bool CCSAPI ENG_UpdateServerComponent(void);

void CDH_ConfirmMessage(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize);
void CDH_Ping(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize);
void CDH_Login(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize);
void CDH_ChannelMsg(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize);
void CDH_PrivateMsg(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize);
void CDH_SetClientInfo(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize);
void CDH_GetClientCount(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize);
void CDH_GetClientInfo(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize);
void CDH_JoinChannel(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize);
void CDH_LeaveChannel(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize);
void CDH_ChannelAmount(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize);
void CDH_ChannelInfoByName(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize);
void CDH_ChannelInfoByID(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize);
void CDH_ChannelUsers(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize);
void CDH_AuthAsAdmin(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize);
void CDH_KickUser(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize);
void CDH_BanUser(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize);
void CDH_ChangeTopic(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize);
void CDH_CreateChannel(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize);
void CDH_DeleteChannel(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize);
void CDH_KickUserFromChannel(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize);
void CDH_SetGhostMode(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize);
void CDH_AuthAsChannelAdmin(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize);
void CDH_SetChannelData(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize);
void CDH_Plugin(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize);
void CDH_Quit(DWORD dwClientID, const void* pBuffer, DWORD dwBufSize);

void Cmd_Help(CConParser* pParser);
void Cmd_Version(CConParser* pParser);
void Cmd_Status(CConParser* pParser);
void Cmd_Readme(CConParser* pParser);
void Cmd_Exec(CConParser* pParser);
void Cmd_BroadcastServerNotice(CConParser* pParser);
void Cmd_ChannelServerNotice(CConParser* pParser);
void Cmd_ClientServerNotice(CConParser* pParser);
void Cmd_ListClients(CConParser* pParser);
void Cmd_ListChannels(CConParser* pParser);
void Cmd_ListChanUsers(CConParser* pParser);
void Cmd_CreateChannel(CConParser* pParser);
void Cmd_DeleteChannel(CConParser* pParser);
void Cmd_KickClient(CConParser* pParser);
void Cmd_BanClient(CConParser* pParser);
void Cmd_KickChanUser(CConParser* pParser);
void Cmd_ReloadMotd(CConParser* pParser);
void Cmd_ShowMotd(CConParser* pParser);
void Cmd_ListPlugins(CConParser* pParser);
void Cmd_PluginInfos(CConParser* pParser);
void Cmd_LoadPlugin(CConParser* pParser);
void Cmd_UnloadPlugin(CConParser* pParser);
void Cmd_PluginPause(CConParser* pParser);
void Cmd_Update(CConParser* pParser);
void Cmd_Restart(CConParser* pParser);
void Cmd_QuitApplication(CConParser* pParser);

void DV_Author(char* szValue);
void DV_Version(char* szValue);
void DV_Platform(char* szValue);
void DV_ServerName(char* szValue);
void DV_ServerPort(char* szValue);
void DV_Uptime(char* szValue);
void DV_DateTime(char* szValue);
void DV_ForcedClient(char* szValue);
void DV_CurrentUsers(char* szValue);
void DV_MaximumUsers(char* szValue);
void DV_CurrentChans(char* szValue);
void DV_MaximumChans(char* szValue);
void DV_UserNames(char* szValue);
void DV_ChanNames(char* szValue);
void DV_UpdateURL(char* szValue);
void DV_Masterserver(char* szValue);
void DV_AOTD(char* szValue);
void DV_GhostMode(char* szValue);
void DV_Pluginamount(char* szValue);
void DV_Plugins(char* szValue);
//======================================================================

#endif