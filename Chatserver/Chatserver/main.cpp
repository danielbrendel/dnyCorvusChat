#include "includes.h"
#include "vars.h"
#include "log.h"
#include "utils.h"
#include "callbacks.h"
#include "versionfuncs.h"

/*
	CorvusChat Server (dnyCorvusChat) - Chatserver component

	Developed by Daniel Brendel

	Version: 0.5
	Visit: https://github.com/danielbrendel
	Mail dbrendel1988@gmail.com

	File: main.cpp: Main function implementations 
*/

//======================================================================
using namespace std;
//======================================================================

//======================================================================
void ShutdownComponents(void);
void FatalError(const char* szErrMsg, ...)
{
	//Handle fatal errors here. Note: This function must force own program instance termination

	if (!szErrMsg)
		return;

	char szBuffer[250];

	//Format string
	va_list vaList;
	va_start(vaList, szErrMsg);
	vsprintf_s(szBuffer, szErrMsg, vaList);
	va_end(vaList);

	//Log to hard disc if desired
	long long ulValue = (g_GlobalVars.pLogToDisc) ? g_GlobalVars.pLogToDisc->iValue : 0;
	if (g_GlobalVars.pLogToDisc->iValue)
		LogError("%s", szBuffer);

	//Show message
	MessageBoxA(0, szBuffer, PROGRAM_NAME " :: Fatal Error", MB_ICONERROR);

	//Shutdown all components
	ShutdownComponents();

	//Exit application
	ExitProcess(EXIT_FAILURE);
}
//======================================================================

//======================================================================
DWORD WINAPI ConsoleThread(LPVOID lpvArguments)
{
	//Thread to handle console commands

	char szExpression[250];

	while (true) {
		//Read input from standard input
		std::cin.getline(szExpression, sizeof(szExpression), '\n');

		if (szExpression[0] == '#') //Pass to config manager
			g_Objects.oConfigInt.Parse(&szExpression[1]);
		else //Handle as default console command
			g_Objects.ConCommand.HandleCommand(szExpression);

		Sleep(10);
	}
}
//======================================================================

//======================================================================
DWORD WINAPI MasterserverThread(LPVOID lpvArguments)
{
	//Thread to handle Masterserver component

	//Just sleep a bit to not corrupt startup console output texts
	Sleep(5000);

	while (true) {
		if (g_Objects.MasterServer.IsConnected() == FALSE) {
			g_Objects.MasterServer.EstablishConnection(g_GlobalVars.pMSAddr->szValue, (WORD)g_GlobalVars.pMSPort->iValue);
		}

		Sleep(20000);
	}
}
//======================================================================

//======================================================================
BOOL WINAPI ConsoleControlHandler(DWORD dwCtrlType)
{
	//Handle console close event here

	if (dwCtrlType == CTRL_CLOSE_EVENT) {
		ShutdownComponents();

		return TRUE;
	}

	return FALSE;
}
//======================================================================

//======================================================================
void ConfigUnknownExpressionHandler(const std::string& szExpression)
{
	//Handle unknown expressions

	if (!szExpression.length())
		return;

	//Try to pass to console
	if (!g_Objects.ConCommand.HandleCommand(szExpression.c_str())) {
		ConsolePrint(FOREGROUND_RED, ("Unknown expression: " + szExpression + "\n").c_str());
	}
}
//======================================================================

//======================================================================
void SetupEngineFunctions(enginefunctions_s& sEngFuncs)
{
	//Assign function pointers to engine structure members

	#define ASSIGN_ENGINEFUNCTION(pfn, prefix) sEngFuncs.##prefix##pfn = &ENG_##pfn

	ASSIGN_ENGINEFUNCTION(GetServerVersion, VER_);
	ASSIGN_ENGINEFUNCTION(GetInterfaceVersion, VER_);
	ASSIGN_ENGINEFUNCTION(ConsolePrint, OUT_);
	ASSIGN_ENGINEFUNCTION(FatalError, OUT_);
	ASSIGN_ENGINEFUNCTION(LogMessage, OUT_);
	ASSIGN_ENGINEFUNCTION(LogError, OUT_);
	ASSIGN_ENGINEFUNCTION(ClientPrivMsg, CHAT_);
	ASSIGN_ENGINEFUNCTION(ClientChanMsg, CHAT_);
	ASSIGN_ENGINEFUNCTION(GetClientCount, CL_);
	ASSIGN_ENGINEFUNCTION(GetClientById, CL_);
	ASSIGN_ENGINEFUNCTION(GetClientId, CL_);
	ASSIGN_ENGINEFUNCTION(CreateBot, CL_);
	ASSIGN_ENGINEFUNCTION(KickClient, CL_);
	ASSIGN_ENGINEFUNCTION(CreateChannel, CHAN_);
	ASSIGN_ENGINEFUNCTION(DeleteChannel, CHAN_);
	ASSIGN_ENGINEFUNCTION(GetIdOfChannel, CHAN_);
	ASSIGN_ENGINEFUNCTION(GetChannelById, CHAN_);
	ASSIGN_ENGINEFUNCTION(GetChannelByName, CHAN_);
	ASSIGN_ENGINEFUNCTION(GetChannelCount, CHAN_);
	ASSIGN_ENGINEFUNCTION(IsUserInChannel, CHAN_);
	ASSIGN_ENGINEFUNCTION(GetUserByIdChan, CHAN_);
	ASSIGN_ENGINEFUNCTION(GetUserByNameChan, CHAN_);
	ASSIGN_ENGINEFUNCTION(GetUserIdOfChannel, CHAN_);
	ASSIGN_ENGINEFUNCTION(PutUserInChannel, CHAN_);
	ASSIGN_ENGINEFUNCTION(RemoveUserFromChannel, CHAN_);
	ASSIGN_ENGINEFUNCTION(EstablishMSConnection, MS_);
	ASSIGN_ENGINEFUNCTION(CloseMSConnection, MS_);
	ASSIGN_ENGINEFUNCTION(UpdateMSData, MS_);
	ASSIGN_ENGINEFUNCTION(IsMSConnected, MS_);
	ASSIGN_ENGINEFUNCTION(RegUserMessage, UMSG_);
	ASSIGN_ENGINEFUNCTION(GetMessageByName, UMSG_);
	ASSIGN_ENGINEFUNCTION(MessageBegin, UMSG_);
	ASSIGN_ENGINEFUNCTION(WriteByte, UMSG_);
	ASSIGN_ENGINEFUNCTION(WriteChar, UMSG_);
	ASSIGN_ENGINEFUNCTION(WriteShort, UMSG_);
	ASSIGN_ENGINEFUNCTION(WriteInt, UMSG_);
	ASSIGN_ENGINEFUNCTION(WriteFloat, UMSG_);
	ASSIGN_ENGINEFUNCTION(WriteString, UMSG_);
	ASSIGN_ENGINEFUNCTION(WriteBuf, UMSG_);
	ASSIGN_ENGINEFUNCTION(MessageEnd, UMSG_);
	ASSIGN_ENGINEFUNCTION(AddConCommand, CMD_);
	ASSIGN_ENGINEFUNCTION(DeleteConCommand, CMD_);
	ASSIGN_ENGINEFUNCTION(ExecScript, CFG_);
	ASSIGN_ENGINEFUNCTION(ExecCode, CFG_);
	ASSIGN_ENGINEFUNCTION(RegisterCVar, CVAR_);
	ASSIGN_ENGINEFUNCTION(RemoveCVar, CVAR_);
	ASSIGN_ENGINEFUNCTION(GetCVar, CVAR_);
	ASSIGN_ENGINEFUNCTION(SetCVarValueString, CVAR_);
	ASSIGN_ENGINEFUNCTION(SetCVarValueInteger, CVAR_);
	ASSIGN_ENGINEFUNCTION(SetCVarValueFloat, CVAR_);
	ASSIGN_ENGINEFUNCTION(AddClientHandler, CMSG_);
	ASSIGN_ENGINEFUNCTION(RemoveClientHandler, CMSG_);
	ASSIGN_ENGINEFUNCTION(GetClientInterface, RETRV_);
	ASSIGN_ENGINEFUNCTION(GetCommandCount, RETRV_);
	ASSIGN_ENGINEFUNCTION(GetCommandArg, RETRV_);
	ASSIGN_ENGINEFUNCTION(IsDestinationReachable, UTIL_);
	ASSIGN_ENGINEFUNCTION(AddressToString, UTIL_);
	ASSIGN_ENGINEFUNCTION(FileExists, UTIL_);
	ASSIGN_ENGINEFUNCTION(DirectoryExists, UTIL_);
	ASSIGN_ENGINEFUNCTION(ExtractFilePath, UTIL_);
	ASSIGN_ENGINEFUNCTION(ExtractFileName, UTIL_);
	ASSIGN_ENGINEFUNCTION(GetCurrentDateTime, UTIL_);
	ASSIGN_ENGINEFUNCTION(UpdateServerComponent, UTIL_);
}
//======================================================================

//======================================================================
BOOL SetupDynVars(void)
{
	//Setup all dynvars here

	#define ADD_DYNVAR(n, pfn) if (!g_Objects.DynVars.AddDynVar(n, DV_##pfn)) return FALSE;

	g_Objects.DynVars.SetCheckType(DYNVAR_WITH_VALID_STREND);

	ADD_DYNVAR("$author", Author);
	ADD_DYNVAR("$version", Version);
	ADD_DYNVAR("$platform", Platform);
	ADD_DYNVAR("$servername", ServerName);
	ADD_DYNVAR("$serverport", ServerPort);
	ADD_DYNVAR("$uptime", Uptime);
	ADD_DYNVAR("$datetime", DateTime);
	ADD_DYNVAR("$forcedclient", ForcedClient);
	ADD_DYNVAR("$currentusers", CurrentUsers);
	ADD_DYNVAR("$maxusers", MaximumUsers);
	ADD_DYNVAR("$currentchans", CurrentChans);
	ADD_DYNVAR("$maxchans", MaximumChans);
	ADD_DYNVAR("$usernames", UserNames);
	ADD_DYNVAR("$channames", ChanNames);
	ADD_DYNVAR("$updateurl", UpdateURL);
	ADD_DYNVAR("$masterserver", Masterserver);
	ADD_DYNVAR("$aotd", AOTD);
	ADD_DYNVAR("$ghostmode", GhostMode);
	ADD_DYNVAR("$pluginamount", Pluginamount);
	ADD_DYNVAR("$pluginnames", Plugins);

	return TRUE;
}
//======================================================================

//======================================================================
bool RegisterCVars(void)
{
	//Register all CVars

	#define RCV_REGISTER(n, t, d) g_Objects.oConfigInt.CCVar::Add(n, t, d)

	g_GlobalVars.pSendType = RCV_REGISTER("net_sendsystem", ConfigMgr::CCVar::CVAR_TYPE_INT, "1");
	if (!g_GlobalVars.pSendType)
		return false;

	g_GlobalVars.pSingleaddrAmount = RCV_REGISTER("net_singleaddramount", ConfigMgr::CCVar::CVAR_TYPE_INT, "5");
	if (!g_GlobalVars.pSingleaddrAmount)
		return false;

	g_GlobalVars.pClientPort = RCV_REGISTER("net_port", ConfigMgr::CCVar::CVAR_TYPE_INT, "35400");
	if (!g_GlobalVars.pClientPort)
		return false;

	g_GlobalVars.pRConPort = RCV_REGISTER("net_rconport", ConfigMgr::CCVar::CVAR_TYPE_INT, "35401");
	if (!g_GlobalVars.pRConPort)
		return false;

	g_GlobalVars.pServerName = RCV_REGISTER("sv_name", ConfigMgr::CCVar::CVAR_TYPE_STRING, "CorvusChat Chatserver");
	if (!g_GlobalVars.pServerName)
		return false;

	g_GlobalVars.pUpdateURL = RCV_REGISTER("upd_netresource", ConfigMgr::CCVar::CVAR_TYPE_STRING, "");
	if (!g_GlobalVars.pUpdateURL)
		return false;

	g_GlobalVars.pServerPW = RCV_REGISTER("sv_serverpw", ConfigMgr::CCVar::CVAR_TYPE_STRING, "#0");
	if (!g_GlobalVars.pServerPW)
		return false;

	g_GlobalVars.pRConPW = RCV_REGISTER("cnt_rconpw", ConfigMgr::CCVar::CVAR_TYPE_STRING, "password");
	if (!g_GlobalVars.pRConPW)
		return false;

	g_GlobalVars.pLogToDisc = RCV_REGISTER("fl_logtodisc", ConfigMgr::CCVar::CVAR_TYPE_INT, "0");
	if (!g_GlobalVars.pLogToDisc)
		return false;

	g_GlobalVars.pMaxChannels = RCV_REGISTER("sv_maxchans", ConfigMgr::CCVar::CVAR_TYPE_INT, "100");
	if (!g_GlobalVars.pMaxChannels)
		return false;

	g_GlobalVars.pMaxUsers = RCV_REGISTER("sv_maxuser", ConfigMgr::CCVar::CVAR_TYPE_INT, "100");
	if (!g_GlobalVars.pMaxUsers)
		return false;

	g_GlobalVars.pMSAddr = RCV_REGISTER("ms_address", ConfigMgr::CCVar::CVAR_TYPE_STRING, "");
	if (!g_GlobalVars.pMSAddr)
		return false;

	g_GlobalVars.pMSPort = RCV_REGISTER("ms_port", ConfigMgr::CCVar::CVAR_TYPE_INT, "35403");
	if (!g_GlobalVars.pMSPort)
		return false;

	g_GlobalVars.pGhostModeAllowed = RCV_REGISTER("sv_gmallowed", ConfigMgr::CCVar::CVAR_TYPE_INT, "0");
	if (!g_GlobalVars.pGhostModeAllowed)
		return false;

	g_GlobalVars.pForcedClient = RCV_REGISTER("sv_fclient", ConfigMgr::CCVar::CVAR_TYPE_STRING, "#0");
	if (!g_GlobalVars.pForcedClient)
		return false;

	g_GlobalVars.pAdminPW = RCV_REGISTER("cnt_adminpw", ConfigMgr::CCVar::CVAR_TYPE_STRING, "password");
	if (!g_GlobalVars.pAdminPW)
		return false;

	g_GlobalVars.pLogClientActions = RCV_REGISTER("out_logclientactions", ConfigMgr::CCVar::CVAR_TYPE_INT, "1");
	if (!g_GlobalVars.pLogClientActions)
		return false;

	g_GlobalVars.pOutputPrefix = RCV_REGISTER("out_setoutputprefix", ConfigMgr::CCVar::CVAR_TYPE_INT, "0");
	if (!g_GlobalVars.pOutputPrefix)
		return false;

	g_GlobalVars.pPingTimeout = RCV_REGISTER("net_pingtimeout", ConfigMgr::CCVar::CVAR_TYPE_INT, "3");
	if (!g_GlobalVars.pPingTimeout)
		return false;

	g_GlobalVars.pPingDelay = RCV_REGISTER("net_pingdelay", ConfigMgr::CCVar::CVAR_TYPE_INT, "5");
	if (!g_GlobalVars.pPingDelay)
		return false;

	RCV_REGISTER("sys_platform", ConfigMgr::CCVar::CVAR_TYPE_STRING, PLATFORM);

	return true;
}
//======================================================================

//======================================================================
bool AddCommands(void)
{
	//Add script commands

	#define AC_ADD(n, d, pfn) g_Objects.oConfigInt.CCommand::Add(n, d, pfn)

	if (!AC_ADD("echo", "Print text to output", &Cmd_Echo)) {
		return false;
	}

	if (!AC_ADD("createchannel", "Create a channel", &Cmd_CreateChannel)) {
		return false;
	}

	if (!AC_ADD("loadplugin", "Load a plugin", &Cmd_LoadPlugin)) {
		return false;
	}

	if (!AC_ADD("setbanner", "Set banner info", &Cmd_SetBanner)) {
		return false;
	}

	return true;
}
//======================================================================

//======================================================================
bool InitializeComponents(void)
{
	//Initialize all components

	//Setup config manager
	g_Objects.oConfigInt.SetUnknownExpressionInformer(&ConfigUnknownExpressionHandler);
	
	//Register CVars
	if (!RegisterCVars()) {
		FatalError("[Fatal Error] RegisterCVars failed");
		return false;
	}

	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, "CVars registered\n");

	//Register commands
	if (!AddCommands()) {
		FatalError("[Fatal Error] AddCommands failed");
		return false;
	}

	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, "Commands added\n");

	//Execute preinit config
	g_Objects.oConfigInt.Execute("scripts\\preinit.cfg");

	//Check for plugin event functions
	for (PLUGINID pid = 0; pid < g_Objects.Plugins.GetPluginCount(); pid++) {
        plugin_s* pPlugin = g_Objects.Plugins.GetPluginById(pid);
        if ((pPlugin) && (!pPlugin->bPaused) && (pPlugin->evttable.events_pre.OnServerInitialize)) { //Check for valid plugin pointer, if plugin is paused and if event function pointer is assigned
			plugin_result res = PLUGIN_CONTINUE;

			bool bRetValue;

			//Call event function
			res = pPlugin->evttable.events_pre.OnServerInitialize(&bRetValue);

            if (res == PLUGIN_BREAK) //Prevent all event function calls of the following plugins if desired
				break;
            else if (res == PLUGIN_RETURN) //Return from this function if desired
				return bRetValue;
        }
    }
	
	//Execute main config
	g_Objects.oConfigInt.Execute("scripts\\main.cfg");

	//Create log file if desired
	if (g_GlobalVars.pLogToDisc->iValue) {
		char szLogDir[250];
		char szLogFile[250];

		sprintf_s(szLogDir, "%slog", g_GlobalVars.szAppPath);
		sprintf_s(szLogFile, "%s\\logfile_%s.html", szLogDir, GetCurrentDateTime());

		//Just remove every ':' character
		for (unsigned int i = 3; i < strlen(szLogFile); i++) {
			if (szLogFile[i] == ':')
				szLogFile[i] = '-';
		}

		//Create log dir if not already exists
		if (!DirectoryExists(szLogDir))
			CreateDirectoryA(szLogDir, NULL);

		if (!CreateLog(szLogFile, true))
			ConsolePrint(FOREGROUND_RED, "Warning: Failed to create log file on hard disc\n");
		else
			ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, "Log file has been created: %s\n", szLogFile);
	}

	//Add client data handlers for incoming messages
	for (unsigned int i = 0; i < CDHANDLERS_AMOUNT; i++) {
		if (!g_Objects.ClientData.AddHandler(g_CdHandlers[i].ident, g_CdHandlers[i].pfnHandler)) {
			FatalError("[Fatal Error] CClientData::AddHandler failed for %d (%d)", g_CdHandlers[i].ident, i);
			return false;
		}
	}

	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, "Client data handlers registered\n");

	//Add console commands
	for (unsigned int i = 0; i < CONCOMMAND_AMOUNT; i++) {
		if (!g_Objects.ConCommand.AddCommand(g_ConCommands[i].name, g_ConCommands[i].description, g_ConCommands[i].pfn)) {
			FatalError("[Fatal Error] CConCommand::AddCommand failed for \'%s\' %d", g_ConCommands[i].name, i);
			return false;
		}
	}

	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, "Console commands registered\n");
	
	//Setup server socket component
	if (!g_Objects.ClientSock.Initialize((WORD)g_GlobalVars.pClientPort->iValue)) {
		FatalError("[Fatal Error] CClientSocket::Initialize failed (Port = %d)", g_GlobalVars.pClientPort->iValue);
		return false;
	}

	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, "Client socket handler initialized\n");

	g_Objects.ClientSock.SetEvents(&g_SockEvents);

	//Initialize RCon component instance
	if (!g_Objects.RCon.Initialize((WORD)g_GlobalVars.pRConPort->iValue, (r_print_output_s*)&g_PrintOutput)) {
		FatalError("[Fatal Error] CRemoteControl::Initialize failed (Port = %d)", g_GlobalVars.pRConPort->iValue);
		return false;
	}

	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, "RCon handler initialized\n");

	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, "Adding user messages...");

	//Setup outgoing UserMsgs
	g_Objects.UserMsg.AddUserMsg("RequestLogin", 1); //Request the client to login. Note: This is obsolete
	g_Objects.UserMsg.AddUserMsg("ServerVersion", sizeof(DWORD) + 1); //The server version
	g_Objects.UserMsg.AddUserMsg("Ping", sizeof(BYTE) + 1); //Send ping value
	g_Objects.UserMsg.AddUserMsg("ClientCount", sizeof(CLIENTID) + 1); //Send the amount of connected clients
	g_Objects.UserMsg.AddUserMsg("ClientInfo", sizeof(clinfo_s) + 1);
	g_Objects.UserMsg.AddUserMsg("MOTD", 1); //Send MOTD content
	g_Objects.UserMsg.AddUserMsg("ChanCount", sizeof(CHANNELID) + 1); //Send amount of existing channels
	g_Objects.UserMsg.AddUserMsg("ChanInfo", sizeof(chaninfo_s) + 1); //Send channel data information
	g_Objects.UserMsg.AddUserMsg("ChanUsers", 1 + MAX_NETWORK_STRING_LENGTH + MAX_CHANUSERS_STRING_LENGTH); //Send a list of all users of a channel
	g_Objects.UserMsg.AddUserMsg("ListChannels", 1); //Send a list of all existing channel names
	g_Objects.UserMsg.AddUserMsg("ServerNotice", 1); //Send an information containing string to a client
	g_Objects.UserMsg.AddUserMsg("PrivMsg", sizeof(privmsg_s) + 1); //Send  a private message from client to client
	g_Objects.UserMsg.AddUserMsg("ChanMsg", sizeof(chanmsg_s) + 1); //Send a message from client to channel
	g_Objects.UserMsg.AddUserMsg("ClientJoin", sizeof(CLIENTID) + MAX_NETWORK_STRING_LENGTH + 1); //A client has joined the server
	g_Objects.UserMsg.AddUserMsg("ClientLeave", sizeof(CLIENTID) + MAX_NETWORK_STRING_LENGTH*2 + 1); //A client has left the server
	g_Objects.UserMsg.AddUserMsg("ChannelJoin", sizeof(CHANNELID) + MAX_NETWORK_STRING_LENGTH*2 + 1); //A client has joined a channel
	g_Objects.UserMsg.AddUserMsg("ChannelLeave", sizeof(CHANNELID) + MAX_NETWORK_STRING_LENGTH*2 + 1); //A client has left a channel
	g_Objects.UserMsg.AddUserMsg("Topic", sizeof(topic_s) + 1); //Send topic of a channel
	g_Objects.UserMsg.AddUserMsg("OperationStatus", (sizeof(WORD)*2) + 1); //Send operation status (status value)
	g_Objects.UserMsg.AddUserMsg("AOTD", sizeof(aotd_s) + 1); //Send AOTD information

	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, "Done\n");

	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, "Adding DynVars...");

	//Add dynamic variables
	(SetupDynVars()) ? ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, "Done\n") : ConsolePrint(FOREGROUND_RED, "Failed\n");

	//Create console command handler thread
	g_hConCmdThread = CreateThread(NULL, 0, &ConsoleThread, NULL, 0, NULL);
	if (!g_hConCmdThread) {
		FatalError("[Fatal Error] Could not create thread for ConsoleThread function");
		return false;
	}

	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, "Console command thread created: %d -> 0x%p\n", g_hConCmdThread, &ConsoleThread); 

	//Create masterserver thread
	g_hMSThread = CreateThread(NULL, 0, &MasterserverThread, NULL, 0, NULL);
	if (!g_hMSThread) {
		FatalError("[Fatal Error] Could not create thread for MasterserverThread function");
		return false;
	}

	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, "Masterserver thread created: %d -> 0x%p\n", g_hMSThread, &MasterserverThread);

	g_GlobalVars.dwElapsedHours = 0;
	g_dwCurRunTimer = g_dwLastRunTimer = GetTickCount(); //Initialize timers

	//Load banlist
	(g_Objects.BanList.LoadFromFile(g_GlobalVars.szIPBanList)) ? ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, "Banlist content loaded from: %s\n", g_GlobalVars.szIPBanList) : ConsolePrint(FOREGROUND_RED, "Could not load banlist content from: %s\n", g_GlobalVars.szIPBanList);
	
	//Load MOTD
	(g_Objects.MOTD.Reload(g_GlobalVars.szMOTD)) ? ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, "MOTD content loaded from: %s\n", g_GlobalVars.szMOTD) : ConsolePrint(FOREGROUND_RED, "Could not load MOTD content from: %s\n", g_GlobalVars.szMOTD);

	//Execute other script files
	g_Objects.oConfigInt.Execute("scripts\\channels.cfg"); //For channel specific issues
	g_Objects.oConfigInt.Execute("scripts\\plugins.cfg"); //For plugin issues
	g_Objects.oConfigInt.Execute("scripts\\userconfig.cfg"); //For all other configurations of the admin

	//Check for plugin event functions
	for (PLUGINID pid = 0; pid < g_Objects.Plugins.GetPluginCount(); pid++) {
        plugin_s* pPlugin = g_Objects.Plugins.GetPluginById(pid);
        if ((pPlugin) && (!pPlugin->bPaused) && (pPlugin->evttable.events_post.OnServerInitialize)) { //Check for valid plugin pointer, if plugin is paused and if event function pointer is assigned
			plugin_result res = PLUGIN_CONTINUE;

			bool bRetValue;

			//Call event function
			res = pPlugin->evttable.events_post.OnServerInitialize(&bRetValue);

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
void ProcessComponents(void)
{
	//Process components here which don't need an extra thread

	//Check for plugin event functions
	for (PLUGINID pid = 0; pid < g_Objects.Plugins.GetPluginCount(); pid++) {
        plugin_s* pPlugin = g_Objects.Plugins.GetPluginById(pid);
        if ((pPlugin) && (!pPlugin->bPaused) && (pPlugin->evttable.events_pre.OnStartFrame)) { //Check for valid plugin pointer, if plugin is paused and if event function pointer is assigned
			plugin_result res = PLUGIN_CONTINUE;

			//Call event function
			res = pPlugin->evttable.events_pre.OnStartFrame();

            if (res == PLUGIN_BREAK) //Prevent all event function calls of the following plugins if desired
				break;
            else if (res == PLUGIN_RETURN) //Return from this function if desired
				return;
        }
    }

	//Handle socket component
	g_Objects.ClientSock.CalculateStuff();
	g_Objects.ClientSock.ProcessMessages();
	g_Objects.ClientSock.WaitForClients();
	g_Objects.ClientSock.RecieveData();

	//Handle RCon component
	g_Objects.RCon.Process(&g_Objects.ConCommand);

	//Calculate elapsed hours since server start
	#define MS_HOUR 3600000
	g_dwCurRunTimer = GetTickCount();
	if (g_dwCurRunTimer >= g_dwLastRunTimer + MS_HOUR) {
		g_GlobalVars.dwElapsedHours++;

		g_dwLastRunTimer = GetTickCount(); //Refresh to count again
	}

	//Update masterserver informations
	static serverinfo_s msinfo;
	*(DWORD*)&msinfo.ucVer = SERVERVERSION();
	msinfo.usPort = (WORD)g_GlobalVars.pClientPort->iValue;
	strcpy_s(msinfo.szName, g_GlobalVars.pServerName->szValue);
	strcpy_s(msinfo.szForcedClient, g_GlobalVars.pForcedClient->szValue);
	msinfo.uiCurClients = ENG_GetClientCount();
	msinfo.uiMaxclients = (unsigned int)g_GlobalVars.pMaxUsers->iValue;
	msinfo.bPassword = (!(strcmp(g_GlobalVars.pServerPW->szValue, "#0")==0));
	msinfo.uiUptime = g_GlobalVars.dwElapsedHours;
	g_Objects.MasterServer.UpdateData(&msinfo);
	g_Objects.MasterServer.RecieveData();

	//Update console title
	static char szConTitle[250];
	sprintf_s(szConTitle, CONSOLE_TITLE, g_GlobalVars.pServerName->szValue, g_Objects.ClientSock.GetClientCount(), g_GlobalVars.pMaxUsers->iValue);
	SetConsoleTitleA(szConTitle);

	//Check for plugin event functions
	for (PLUGINID pid = 0; pid < g_Objects.Plugins.GetPluginCount(); pid++) {
        plugin_s* pPlugin = g_Objects.Plugins.GetPluginById(pid);
        if ((pPlugin) && (!pPlugin->bPaused) && (pPlugin->evttable.events_post.OnStartFrame)) { //Check for valid plugin pointer, if plugin is paused and if event function pointer is assigned
			plugin_result res = PLUGIN_CONTINUE;

			//Call event function
			res = pPlugin->evttable.events_post.OnStartFrame();

            if (res == PLUGIN_BREAK) //Prevent all event function calls of the following plugins if desired
				break;
            else if (res == PLUGIN_RETURN) //Return from this function if desired
				return;
        }
    }
}
//======================================================================

//======================================================================
void ShutdownComponents(void)
{
	//Shutdown all components

	//Check for plugin event functions
	for (PLUGINID pid = 0; pid < g_Objects.Plugins.GetPluginCount(); pid++) {
        plugin_s* pPlugin = g_Objects.Plugins.GetPluginById(pid);
        if ((pPlugin) && (!pPlugin->bPaused) && (pPlugin->evttable.events_pre.OnServerShutdown)) { //Check for valid plugin pointer, if plugin is paused and if event function pointer is assigned
			plugin_result res = PLUGIN_CONTINUE;

			//Call event function
			res = pPlugin->evttable.events_pre.OnServerShutdown();

            if (res == PLUGIN_BREAK) //Prevent all event function calls of the following plugins if desired
				break;
            else if (res == PLUGIN_RETURN) //Return from this function if desired
				return;
        }
    }

	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, "Shutting down components...");

	//Terminate console command thread
	TerminateThread(g_hConCmdThread, EXIT_SUCCESS);
	g_hConCmdThread = NULL;

	//Terminate Masterserver thread
	TerminateThread(g_hMSThread, EXIT_SUCCESS);
	g_hMSThread = NULL;

	//Shutdown components
	g_Objects.ClientData.Clear();
	g_Objects.ConCommand.Clear();
	g_Objects.UserMsg.Clear();
	g_Objects.ClientSock.Clear();
	g_Objects.RCon.Clear();
	g_Objects.MasterServer.CloseConnection();
	g_Objects.DynVars.Clear();
	g_Objects.CTPM.Clear();

	g_Objects.BanList.SaveToFile(g_GlobalVars.szIPBanList);

	if (g_GlobalVars.pLogToDisc->iValue)
		CloseLog();

	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, "Done\n");

	//Check for plugin event functions
	for (PLUGINID pid = 0; pid < g_Objects.Plugins.GetPluginCount(); pid++) {
        plugin_s* pPlugin = g_Objects.Plugins.GetPluginById(pid);
        if ((pPlugin) && (!pPlugin->bPaused) && (pPlugin->evttable.events_post.OnServerShutdown)) { //Check for valid plugin pointer, if plugin is paused and if event function pointer is assigned
			plugin_result res = PLUGIN_CONTINUE;

			//Call event function
			res = pPlugin->evttable.events_post.OnServerShutdown();

            if (res == PLUGIN_BREAK) //Prevent all event function calls of the following plugins if desired
				break;
            else if (res == PLUGIN_RETURN) //Return from this function if desired
				return;
        }
    }
}
//======================================================================

//======================================================================
int main(int argc, char* argv[])
{
	//Application entry point

	//Set console title
	SetConsoleTitleA(PROGRAM_NAME " v" PROGRAM_VERSION " Server Console");

	//Print initial information
	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, PROGRAM_NAME " v" PROGRAM_VERSION " developed by " PROGRAM_AUTHOR " (" PROGRAM_CONTACT ")\n\n");

	memset(&g_GlobalVars, 0x00, sizeof(globalvars_s));

	SetupEngineFunctions(g_EngineFuncs);

	//Try to create mutex since only one program instance is allowed
	HANDLE hMutex = CreateMutex(NULL, FALSE, TEXT(PROGRAM_NAME));
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		FatalError("[Fatal Error] Only one program instance is allowed");
	}

	//Load default program icon
	#define ID_CCICON 101
	HICON hIcon = (HICON)LoadImageA(0, MAKEINTRESOURCEA(ID_CCICON), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE);
	if (!hIcon) {
		ConsolePrint(FOREGROUND_YELLOW, "[Warning] LoadImageA failed: %d", GetLastError());
	}

	//Get application path
	if (!GetModuleFileNameA(0, g_GlobalVars.szAppPath, 250)) {
		FatalError("[Fatal Error] GetModuleFileNameA failed: %d", GetLastError());
	}

	for (unsigned short i = (WORD)strlen(g_GlobalVars.szAppPath); i > 3; i--) {
		if (g_GlobalVars.szAppPath[i] == '\\')
			break;

		g_GlobalVars.szAppPath[i] = 0;
	}

	sprintf_s(g_GlobalVars.szScripts, "%sscripts", g_GlobalVars.szAppPath);
	sprintf_s(g_GlobalVars.szIPBanList, "%stxt\\banlist.txt", g_GlobalVars.szAppPath);
	sprintf_s(g_GlobalVars.szMOTD, "%stxt\\motd.txt", g_GlobalVars.szAppPath);
	sprintf_s(g_GlobalVars.szPluginDir, "%splugins\\", g_GlobalVars.szAppPath);

	//Add console control handler
	if (!SetConsoleCtrlHandler(&ConsoleControlHandler, TRUE)) {
		FatalError("[Fatal Error] SetConsoleCtrlHandler failed: %d", GetLastError());
	}

	//Initialize WinSock
	WSADATA wsadata;
	if (WSAStartup(MAKEWORD(2, 2), &wsadata) == SOCKET_ERROR) {
		FatalError("[Fatal Error] WSAStartup failed: %d", WSAGetLastError());
	}

	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, "Initializing components...\n");

	//Initialize all components
	if (!InitializeComponents()) {
		return EXIT_FAILURE;
	}

	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, "Done\n\n");

	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, "Entering main loop\n\n");
	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, "Type \'help' for a list of commands\n\n");
	
	//Process main loop
	g_bProgramRunning = true;
	while (g_bProgramRunning) {
		ProcessComponents();
		
		Sleep(1);
	}

	//Shutdown all components
	ShutdownComponents();

	//Cleanup WinSock data
    WSACleanup();

	SetConsoleCtrlHandler(NULL, FALSE);

	if (hMutex)
		CloseHandle(hMutex);

	if (hIcon)
		DestroyIcon(hIcon);

	ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, "Server shutdown\n");

	Sleep(1000);

	return 0;
}
//======================================================================
