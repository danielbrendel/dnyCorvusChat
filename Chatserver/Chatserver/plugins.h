#ifndef _PLUGINS_H
#define _PLUGINS_H

#include "includes.h"
#include "clientsock.h"
#include "channels.h"
#include "config.h"

/*
	CorvusChat Server (dnyCorvusChat) - Chatserver component

	Developed by Daniel Brendel

	Version: 0.5
	Visit: https://github.com/danielbrendel
	Mail dbrendel1988@gmail.com

	File: plugins.h: Plugin manager interface 
*/

//======================================================================
#define CCSAPI __cdecl
#define MAX_SIG_STRING 100
//======================================================================

//======================================================================
typedef DWORD PLUGINID;
//======================================================================

//======================================================================
enum plugin_result {
    PLUGIN_CONTINUE, PLUGIN_BREAK, PLUGIN_RETURN
};
//======================================================================

//======================================================================
struct enginefunctions_s { //Table of engine functions exported to each plugin
	//Version functions
	DWORD (CCSAPI *VER_GetServerVersion)(void);
	DWORD (CCSAPI *VER_GetInterfaceVersion)(void);

	//Output functions
	void (CCSAPI *OUT_ConsolePrint)(LPCSTR lpszFmt, ...);
	void (CCSAPI *OUT_FatalError)(const char* szErrMsg, ...);
	void (CCSAPI *OUT_LogMessage)(const char *lpszMessage, ...);
	void (CCSAPI *OUT_LogError)(const char *lpszError, ...);

	//Connection functions
	bool (CCSAPI *CON_ClientConnect)(const CLIENTID ciClientId);
	bool (CCSAPI *CON_ClientJoined)(const CLIENTID ciClientId, const struct login_s* pLoginInfo, bool* pbRetValue);
	void (CCSAPI *CON_ClientDisconnect)(const CLIENTID ciClientId);

	//Message functions
    bool (CCSAPI *CHAT_ClientPrivMsg)(clientinfo_s *pSender, clientinfo_s *pRecv, char* szChatMsg);
	bool (CCSAPI *CHAT_ClientChanMsg)(clientinfo_s *pSender, channel_s *pRecv, char* szChanMsg);

	//Client functions
	const CLIENTAMOUNT (CCSAPI *CL_GetClientCount)(void);
	clientinfo_s* (CCSAPI *CL_GetClientById)(CLIENTID id);
	clientinfo_s* (CCSAPI *CL_ENG_GetClientByName)(char* szName);
	CLIENTID (CCSAPI *CL_GetClientId)(clientinfo_s* pClient);
	clientinfo_s* (CCSAPI *CL_CreateBot)(char *pszName, char *pszClanTag, char *pszClient, char *pszFullName, char *pszEmail, char *pszCountry, bool bAdmin);
	bool (CCSAPI *CL_KickClient)(clientinfo_s* pClientToKick, char *szReason, bool bAddToBanList);

	//Channel functions
	bool (CCSAPI *CHAN_CreateChannel)(char *szName, char *szPassword, char *szAuthPassword, char *szTopic, DWORD dwMaxUsers, bool bRegistered); 
	bool (CCSAPI *CHAN_DeleteChannel)(CHANNELID chChannelId); 
	CHANNELID (CCSAPI *CHAN_GetIdOfChannel)(channel_s* pChannel); 
	channel_s *(CCSAPI *CHAN_GetChannelById)(const CHANNELID chChannelId); 
	channel_s *(CCSAPI *CHAN_GetChannelByName)(const char *szName); 
	const CHANNELID (CCSAPI *CHAN_GetChannelCount)(void);
	bool (CCSAPI *CHAN_IsUserInChannel)(const clientinfo_s *pClient, const channel_s *pChannel, chan_user_s** ppChanUser); 
	chan_user_s *(CCSAPI *CHAN_GetUserByIdChan)(const channel_s* pChannel, const CHUSERID dwUserId); 
	chan_user_s *(CCSAPI *CHAN_GetUserByNameChan)(const channel_s* pChannel, const char *szName); 
	CHUSERID (CCSAPI *CHAN_GetUserIdOfChannel)(const channel_s* pChannel, const chan_user_s* pUser); 
	bool (CCSAPI *CHAN_PutUserInChannel)(clientinfo_s *pClient, channel_s *pChannel); 
	bool (CCSAPI *CHAN_RemoveUserFromChannel)(chan_user_s* pUser, channel_s *pChannel);

	//Masterserver functions
	bool (CCSAPI *MS_EstablishMSConnection)(const char *szIpAddress, const WORD wPort);
	bool (CCSAPI *MS_CloseMSConnection)(void);
	void (CCSAPI *MS_UpdateMSData)(struct serverinfo_s *pData);
	bool (CCSAPI *MS_IsMSConnected)(void);

	//User message functions
	int (CCSAPI *UMSG_RegUserMessage)(char *szName, int iSize);
	int (CCSAPI *UMSG_GetMessageByName)(char* szMsg);
	bool (CCSAPI *UMSG_MessageBegin)(int iMsgID, CLIENTID dwRecvID, BYTE SendType);
	void (CCSAPI *UMSG_WriteByte)(BYTE bValue);
	void (CCSAPI *UMSG_WriteChar)(char cValue);
	void (CCSAPI *UMSG_WriteShort)(short sValue);
	void (CCSAPI *UMSG_WriteInt)(int iValue);
	void (CCSAPI *UMSG_WriteFloat)(float fValue);
	void (CCSAPI *UMSG_WriteString)(char *szString, DWORD dwStringSize);
	void (CCSAPI *UMSG_WriteBuf)(void* pBuf, DWORD dwBufSize);
	bool (CCSAPI *UMSG_MessageEnd)(CClientSocket* pSocket);

	//User interface and config functions
	bool (CCSAPI *CMD_AddConCommand)(char *szCmdName, char *szCmdDescription, const void* pCmdProc);
	bool (CCSAPI *CMD_DeleteConCommand)(char* szCmdName);
	bool (CCSAPI *CCE_ExecScript)(const char *szScriptfile);
	void (CCSAPI *CCE_ExecCode)(const char* szScriptCode);
	cvar_s* (CCSAPI *CVAR_RegisterCVar)(const char* szName, const CVarType_e eType, const char* pszDefaultValue);
	bool (CCSAPI *CVAR_RemoveCVar)(const char* szName);
	cvar_s* (CCSAPI *CVAR_GetCVar)(const char* pszName);
	bool (CCSAPI *CVAR_SetCVarValueString)(const char* pszName, const char* szValue);
	bool (CCSAPI *CVAR_SetCVarValueInteger)(const char* pszName, const int iValue);
	bool (CCSAPI *CVAR_SetCVarValueFloat)(const char* pszName, const double dblValue);

	//Client message handler functions
	bool (CCSAPI *CMSG_AddClientHandler)(struct ctpm_handler_s* pHandler);
	bool (CCSAPI *CMSG_RemoveClientHandler)(char* cHandlerSig);

	//Retriever functions
	CClientSocket* (CCSAPI *RETRV_GetClientInterface)(void);
	DWORD (CCSAPI *RETRV_GetCommandCount)(class CConParser *pParser);
	const char* (CCSAPI* RETRV_GetCommandArg)(CConParser *pParser, DWORD dwIndex);

	//Untility functions
	bool (CCSAPI *UTIL_IsDestinationReachable)(LPCSTR lpszServer);
	const char* (CCSAPI *UTIL_AddressToString)(const unsigned long ulIPAddress);
	bool (CCSAPI *UTIL_FileExists)(const char* szFileName);
	bool (CCSAPI *UTIL_DirectoryExists)(char* szDir);
	void (CCSAPI *UTIL_ExtractFilePath)(char *szFileName);
	char *(CCSAPI *UTIL_ExtractFileName)(char *filePath);
	const char* (CCSAPI *UTIL_GetCurrentDateTime)(void);
	bool (CCSAPI *UTIL_UpdateServerComponent)(void);
};

struct eventfuncs_s {
    plugin_result (CCSAPI *OnServerInitialize)(bool* pbRetValue);
    plugin_result (CCSAPI *OnServerShutdown)(void);
    plugin_result (CCSAPI *OnStartFrame)(void);
    plugin_result (CCSAPI *OnClientConnect)(const CLIENTID ciClientId, bool* pbRetValue);
    plugin_result (CCSAPI *OnClientEntered)(const CLIENTID ciClientId, const struct login_s* pLoginInfo, bool* pbRetValue);
    plugin_result (CCSAPI *OnClientDisconnect)(const CLIENTID ciClientId, const char* pszReason);
    plugin_result (CCSAPI *OnClientPrivMsg)(clientinfo_s *pSender, clientinfo_s *pRecv, char* szChatMsg, bool* pbRetValue);
    plugin_result (CCSAPI *OnClientChanMsg)(clientinfo_s *pSender, channel_s *pRecv, char* szChanMsg, bool* pbRetValue);
    plugin_result (CCSAPI *OnCreateChannel)(char *szName, char *szPassword, char *szAuthPassword, char *szTopic, DWORD dwMaxUsers, bool bRegistered, bool* pbRetValue);
    plugin_result (CCSAPI *OnDeleteChannel)(CHANNELID ciChannelId, bool* pbRetValue);
    plugin_result (CCSAPI *OnPutUserInChannel)(clientinfo_s *pClient, channel_s *pChannel, bool* pbRetValue);
    plugin_result (CCSAPI *OnRemoveUserFromChannel)(chan_user_s* pChanUser, channel_s *pChannel, bool* pbRetValue);
};

struct plugin_eventtable_s {
    eventfuncs_s events_pre; //Called before func code
    eventfuncs_s events_post; //called after func code
};

struct plugininfos_s {
    char plName[250];
    char plVersion[250];
    char plAuthor[250];
    char plDescription[250];
    char plContact[250];
};

struct plugin_s {
    HMODULE hModule;
    char name[250];
    bool bPaused;
    plugin_eventtable_s evttable;
    plugininfos_s infos;
};

struct ctpm_handler_s {
	char cHandlerSig[MAX_SIG_STRING];
	void (*TClientMsgHandler)(clientinfo_s* pClient, const void* lpvBuffer, unsigned int dwBufSize);
};
//======================================================================

//======================================================================
typedef BOOL (CCSAPI *TPluginInit)(PLUGINID pidPluginId, DWORD dwInterfaceVersion, struct enginefunctions_s *pEngineFunctions, plugin_eventtable_s *pEventTable, struct globalvars_s* pGlobalVariables);
typedef VOID (CCSAPI *TPluginInfos)(plugininfos_s *pPluginInfos);
typedef VOID (CCSAPI *TPluginFree)(void);
//======================================================================

//======================================================================
class CPlugins {
private:
	std::vector<plugin_s*> vPlugins;
public:
	CPlugins() { }
	~CPlugins() { }

	BOOL LoadPlugin(const char *szLibrary, bool bManually);
	BOOL RemovePlugin(PLUGINID dwPluginId); 

	PLUGINID GetPluginCount(void); 
	plugin_s *GetPluginById(PLUGINID dwPluginId);

	BOOL SetPauseStatus(PLUGINID dwPluginId, bool bValue);
};

class CClientToPluginMsg {
private:
	std::vector<ctpm_handler_s*> vHandlers;

	BOOL HandlerExists(char* cHandlerSig); 
public:
	BOOL AddHandler(ctpm_handler_s *pHandler); 
	BOOL RemoveHandler(char* cHandlerSig); 
	BOOL CallHandler(char* cHandlerSig, clientinfo_s* pClient, const void* lpvBuffer, unsigned int dwBufSize);
	VOID Clear(void);
};
//======================================================================

#endif