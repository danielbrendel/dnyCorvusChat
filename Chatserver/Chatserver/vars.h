#ifndef _VARS_H
#define _VARS_H

#include "clientdata.h"
#include "concommand.h"
#include "usermsgs.h"
#include "callbacks.h"
#include "update.h"
#include "rcon.h"
#include "banlist.h"
#include "plugins.h"
#include "masterserver.h"
#include "motd.h"
#include "validstr.h"
#include "dynvars.h"
#include "config.h"

/*
	CorvusChat Server (dnyCorvusChat) - Chatserver component

	Developed by Daniel Brendel

	Version: 0.5
	Visit: https://github.com/danielbrendel
	Mail dbrendel1988@gmail.com

	File: vars.h: Global variables interface and general definitions
*/

//======================================================================
#define PROGRAM_NAME "CorvusChat Chatserver"
#define PROGRAM_SHORTCUT "CCS"
#define PROGRAM_VERSION "0.5 RC5"
#define PROGRAM_AUTHOR "Daniel Brendel"
#define PROGRAM_CONTACT "dbrendel1988@gmail.com"
#define PROGRAM_WEBSITE "https://github.com/danielbrendel/"

#if defined(_WIN64)
#define PLATFORM "x64"
#elif defined(_WIN32)
#define PLATFORM "x86"
#endif

#define VF_SERVERVERSION 0, 5
#define VF_INTERFACEVERSION 0, 5

#define CONSOLE_TITLE PROGRAM_NAME " \"%s\" (%d/%d)"

#define CONCOMMAND_AMOUNT 26 //Amount of commands, must be updated when changing g_ConCommands
#define CDHANDLERS_AMOUNT 26 //Amount of client data handlers, must be updated when changing g_CdHandlers

#define CMD_STDOUT_PREFIX "\t"

#define S_NO_PASSWORD "#0"
#define S_NO_FORCED_CLIENT "#0"
#define S_DEFAULT_CLIENTNAME "<unknown>"
#define S_DEFAULT_CLIENTLEAVEMSG "Client disconnect"
#define S_DEFAULT_KICKMSG "Kicked"
#define S_DEFAULT_BANMSG "Banned"

#define CHANNAMES_DELIMITER "\x04"
#define CHANUSERS_DELIMITER "\x04"

#define MAX_NETWORK_STRING_LENGTH 250
#define MAX_CHANNAMES_STRING_LENGTH 1024
#define MAX_CHANUSERS_STRING_LENGTH 1024

#define NAME_MINIMUM_LEN 3
#define NAME_MAXIMUM_LEN 15
#define NAME_ALLOW_SPACES false
#define NAME_SPACE_IDENT 0x20
#define NAME_MIN_ASCIICHAR NAME_SPACE_IDENT
#define NAME_MAX_ASCIICHAR 0x7E
//======================================================================

//======================================================================
enum { //General operation status
	OS_LOGIN,
	OS_CHANNELMSG,
	OS_PRIVATEMSG,
	OS_SETINFODATA,
	OS_CLIENTCOUNT,
	OS_GETINFODATA,
	OS_JOINCHAN,
	OS_LEAVECHAN,
	OS_CHANAMOUNT,
	OS_CHANINFO,
	OS_CHANUSERS,
	OS_ADMINAUTH,
	OS_KICKUSER,
	OS_BANUSER,
	OS_CHANGETOPIC,
	OS_CREATECHAN,
	OS_DELETECHAN,
	OS_KICKCHANUSER,
	OS_GHOSTMODE,
	OS_AUSASCHANADMIN,
	OS_SETCHANNELDATA,
	OS_PLUGINMSG,
	OS_QUIT
};

enum { //Detailed operation status
	OS_SUCCESSFUL,
	OS_INVALIDBUFFER,
	OS_BAD_PASSWORD,
	OS_NOTALLOWED,
	OS_BADUSER,
	OS_ENGFAIL,
	OS_ALREADYEXISTS
};

enum {
	CDH_LOGIN = 0x01, 
	CDH_PING, 
	CDH_SETCLIENTINFO = 0x04, 
	CDH_GETCLIENTCOUNT, 
	CDH_GETCLIENTINFO, 
	CDH_CHANNELMSG, 
	CDH_PRIVATEMSG = 0x08, 
	CDH_JOINCHANNEL,
	CDH_LEAVECHANNEL,
	CDH_CHANNELAMOUNT, 
	CDH_CHANNELINFONAME, 
	CDH_CHANNELUSERS, 
	CDH_AUTHASADMIN, 
	CDH_KICKUSER,
	CDH_BANUSER, 
	CDH_CHANGETOPIC, 
	CDH_CREATECHAN,
	CDH_DELETECHAN, 
	CDH_KICKFROMCHAN = 0x18,
	CDH_SETGHOSTMODE = 0x16, 
	CDH_QUIT, 
	CDH_PLUGIN = 0x19,
	CDH_CONFIRMMSG = 0xFF,
	CDH_CHANNELADMINAUTH = 0x1A,
	CDH_SETCHANNELDATA = 0x1B,
	CDH_CHANNELINFOID
};

enum po_type {
	PO_STDOUT,
	PO_RCON
};
//======================================================================

//======================================================================
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

struct objects_s {
	CClientData ClientData;
	CClientSocket ClientSock;
	CChannels Channels;
	CConCommand ConCommand;
	CUserMsg UserMsg;
	CRemoteControl RCon;
	CUpdate Update;
	CBanList BanList;
	CMessageOfTheDay MOTD;
	CPlugins Plugins;
	CClientToPluginMsg CTPM;
	CValidString ValidStr;
	CMasterServer MasterServer;
	CDynVars DynVars;
	CCVarMgr CVarMgr;
};

struct print_output_s {
	po_type type; //The printing type
	char szIRCUser[250]; //The user name of the IRC network
};
//======================================================================

//======================================================================
extern bool g_bProgramRunning;
extern globalvars_s g_GlobalVars;
extern objects_s g_Objects;
extern sockevents_s g_SockEvents;
extern concommand_s g_ConCommands[CONCOMMAND_AMOUNT];
extern cd_handler_s g_CdHandlers[CDHANDLERS_AMOUNT];
extern HANDLE g_hConCmdThread;
extern HANDLE g_hMSThread;
extern DWORD g_dwCurRunTimer;
extern DWORD g_dwLastRunTimer;
extern print_output_s g_PrintOutput;
extern enginefunctions_s g_EngineFuncs;
extern valid_string_s g_VSData;
//======================================================================

#endif