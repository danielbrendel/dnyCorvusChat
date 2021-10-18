#ifndef _CorvusChat_SDK
#define _CorvusChat_SDK

#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <windows.h>
#include <winsock.h>

//CorvusChat Server Plugin SDK header (C/C++)
//Current interface version: v0.5

//======================================================================
#define CMD_STDOUT_PREFIX "\t" //Use this for user feedback of console commands as prefix

#define MAX_NETWORK_STRING_LENGTH 250 //A networked string buffer shall not be larger than this value
#define MAX_CLANTAG_STR_LEN 20 //Same rule as for networked strings but for clan tag
#define MAX_SIG_STRING 100 //Maximum amount of a signature chars for client usermsg handlers

#define CCSAPI __cdecl //Calling convention for exported functions, event functions and engine functions

#define INVALID_USERMSG -1
#define INVALID_CLIENT_ID -1
#define INVALID_CHANNEL_ID -1
//======================================================================

//======================================================================
typedef DWORD PLUGINID;

typedef int CLIENTID;
typedef CLIENTID CLIENTAMOUNT;

typedef int CHANNELID;
typedef CHANNELID CHANNELAMOUNT;
typedef int CHUSERID;

typedef int GLOBALID;
//======================================================================

//======================================================================
enum SEND_TYPE {
	ST_USER = 1, //Send to a single user
	ST_CHANNEL, //Send to all users of a channel
	ST_BROADCAST //Send to all users connected to the server
};

enum plugin_result {
    PLUGIN_CONTINUE, //Let engine continue with plugin enumeration
	PLUGIN_BREAK,  //Force engine to break out of plugin enumeration
	PLUGIN_RETURN //Force engine to return out of the engine function
};

enum CVarType_e {
	CVAR_TYPE_STRING = 0,
	CVAR_TYPE_INTEGER,
	CVAR_TYPE_FLOAT
};
//======================================================================

//======================================================================
typedef void (CCSAPI *TCmdFunction)(class CConParser* pParser);
//======================================================================

//======================================================================
struct cvar_s {
	char szName[CCE_NAME_STRING_LENGTH];
	CVarType_e eType;
	union {
		char szValue[CCE_STRING_LENGTH];
		int iValue;
		double dblValue;
	};
};

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

struct chan_user_s {
	clientinfo_s *pClient; //Pointer to clientsocket clientinfo data
	bool bIsChannelAdmin; //Determines if this user is a channel admin
};

struct channel_s {
	char szName[MAX_NETWORK_STRING_LENGTH]; //Channel name
	char szTopic[MAX_NETWORK_STRING_LENGTH]; //Channel topic
	char szPassword[MAX_NETWORK_STRING_LENGTH]; //Password. "#0" if not password protected
	char szAdminPassword[MAX_NETWORK_STRING_LENGTH]; //Password for administrator authentication
	CHUSERID dwMaxUsers; //Maximum amount of users
	bool bRegistered; //If this channel is registered or temporary (temp channels get deleted if the last user has left the channel)
	std::vector<chan_user_s*> vUsers; //List of users who have joined the channel
};

struct aotd_s {
    int x; //Banner Res X
    int y; //Banner Res Y
    char szURL[MAX_NETWORK_STRING_LENGTH]; //Banner URL
};

struct globalvars_s {
	cvar_s* pSendType; //Direct sending or adding to message buffer
	cvar_s* pSingleaddrAmount; //Amount of allowed connections from a single IP-Address
	cvar_s* pClientPort; //Port for client listeners
	cvar_s* pRConPort; //Port for rcon
	cvar_s* pServerName; //Chatserver name
	cvar_s* pUpdateURL; //URL where to retrieve program updates
	cvar_s* pServerPW; //Password to login to server. '#0' means no password
	cvar_s* pRConPW; //Remote control password
	cvar_s* pLogToDisc; //Used to determine if console outputs shall be logged to hard disc
	cvar_s* pMaxChannels; //Maximum amount of existing channels
	cvar_s* pMaxUsers; //Maximum amount of users
	cvar_s* pMSAddr; //Masterserver address
	cvar_s* pMSPort; //Masterserver port
	cvar_s* pGhostModeAllowed; //If ghostmode is allowed
	cvar_s* pForcedClient; //The forced client
	cvar_s* pAdminPW; //Admin password
	cvar_s* pLogClientActions; //Log some client actions (Connect, Join, Disconnect, Private/Channel messages, (Chan-)Admin authentication)
	cvar_s* pOutputPrefix; //Determine output (console, log, RCON) prefix: 0 = none, 1 = date, 2 = time, 3 = date and time
	cvar_s* pPingTimeout; //Specify ping timeout value: minimum 1 minute and maximum 5 minutes
	char szAppPath[MAX_PATH]; //Programs' running path
	char szCCEPath[MAX_PATH]; //Path to CCE module
	char szCCEScripts[MAX_PATH]; //Path to scripts dir
	char szPluginDir[MAX_PATH]; //Path for plugins
	char szMOTD[MAX_PATH]; //MOTD file
	char szIPBanList[MAX_PATH]; //Banlist file
	DWORD dwElapsedHours; //Programs' runtime in hours
	aotd_s aotd; //Advertisement of the day
};

struct ctpm_handler_s {
	char cHandlerSig[MAX_SIG_STRING];
	void (*TClientMsgHandler)(clientinfo_s* pClient, const void* lpvBuffer, unsigned int dwBufSize);
};

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
	int (CCSAPI *UMSG_RegUserMessage)(char* szName, int iSize);
	int (CCSAPI *UMSG_GetMessageByName)(char* szMsg);
	bool (CCSAPI *UMSG_MessageBegin)(int iMsgID, CLIENTID dwRecvID, BYTE SendType);
	void (CCSAPI *UMSG_WriteByte)(BYTE bValue);
	void (CCSAPI *UMSG_WriteChar)(char cValue);
	void (CCSAPI *UMSG_WriteShort)(short sValue);
	void (CCSAPI *UMSG_WriteInt)(int iValue);
	void (CCSAPI *UMSG_WriteFloat)(float fValue);
	void (CCSAPI *UMSG_WriteString)(char *szString, DWORD dwStringSize);
	void (CCSAPI *UMSG_WriteBuf)(void* pBuf, DWORD dwBufSize);
	bool (CCSAPI *UMSG_MessageEnd)(class CClientSocket* pSocket);

	//User interface and config functions
	void (CCSAPI *CMD_AddConCommand)(char *szCmdName, char *szCmdDescription, const void* pCmdProc);
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
	DWORD (CCSAPI *RETRV_GetCommandCount)(CConParser *pParser);
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
    eventfuncs_s events_pre; //Called before engine processes its stuff
    eventfuncs_s events_post; //called after engine processes its stuff
};

struct plugininfos_s {
    char plName[250];
    char plVersion[250];
    char plAuthor[250];
    char plDescription[250];
    char plContact[250];
};
//======================================================================

#endif