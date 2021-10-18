#include "vars.h"

/*
	CorvusChat Server (dnyCorvusChat) - Chatserver component

	Developed by Daniel Brendel

	Version: 0.5
	Visit: https://github.com/danielbrendel
	Mail dbrendel1988@gmail.com

	File: vars.cpp: Global variable declarations 
*/

//======================================================================
bool g_bProgramRunning; //Determines if the main loop is running

globalvars_s g_GlobalVars; //Global data

objects_s g_Objects; //Class object instances

sockevents_s g_SockEvents = { //Socket event function table
	Event_OnClientConnect,
	Event_OnClientDisconnect,
	Event_OnClientWrite,
	Event_OnClientUpdatePing,
	Event_OnErrorOccured
};

concommand_s g_ConCommands[CONCOMMAND_AMOUNT] = { //ConCommands
	{"help", "Shows this help text", Cmd_Help},
	{"version", "Show server version information", Cmd_Version},
	{"status", "Shows server status informations", Cmd_Status},
	{"readme", "Shows the readme file", Cmd_Readme},
	{"exec", "<filename> Executes the specified script", Cmd_Exec},
	{"globalmsg", "<message> Broadcasts a server notice message", Cmd_BroadcastServerNotice},
	{"channelmsg", "<channel> <message> Sends a server notice message to the channel", Cmd_ChannelServerNotice},
	{"clientmsg", "<client> <message> Sends a server notice message to the client", Cmd_ClientServerNotice},
	{"listusers", "Lists all connected clients", Cmd_ListClients},
	{"listchannels", "Lists all created channels", Cmd_ListChannels},
	{"listchannelusers", "<channel> Lists all users of a channel", Cmd_ListChanUsers},
	{"createchannel", "<name> <topic> <password> <auth_password> <max_users> Creates a new channel", Cmd_CreateChannel},
	{"deletechannel", "<name> Deletes a channel", Cmd_DeleteChannel},
	{"kickclient", "<name> Kicks a client out of the server", Cmd_KickClient},
	{"banclient", "<name> Bans a client from the server", Cmd_BanClient},
	{"kickchanuser", "<name> <channel> Kicks a user out of a channel", Cmd_KickChanUser},
	{"reloadmotd", "<opt:file> Reloads the MOTD content", Cmd_ReloadMotd},
	{"showmotd", "<opt:with_dynvars> Shows the MOTD content (with dynvars if desired)", Cmd_ShowMotd},
	{"listplugins", "Lists all loaded plugins", Cmd_ListPlugins},
	{"plugininfos", "<plugin> Lists informations of the specified plugin", Cmd_PluginInfos},
	{"loadplugin", "<plugin_file> Loads a plugin from file", Cmd_LoadPlugin},
	{"unloadplugin", "<plugin_id> Unloads a plugin", Cmd_UnloadPlugin},
	{"pluginpause", "<plugin_id> <status> Sets the pause status of a plugin", Cmd_PluginPause},
	{"update", "Update the server component", Cmd_Update},
	{"restart", "Restart the server program", Cmd_Restart},
	{"quit", "Quit Application", Cmd_QuitApplication}
};

cd_handler_s g_CdHandlers[CDHANDLERS_AMOUNT] = { //Client incoming data handlers
	{CDH_LOGIN, CDH_Login},
	{CDH_PING, CDH_Ping},
	{CDH_SETCLIENTINFO, CDH_SetClientInfo},
	{CDH_GETCLIENTCOUNT, CDH_GetClientCount},
	{CDH_GETCLIENTINFO, CDH_GetClientInfo},
	{CDH_CHANNELMSG, CDH_ChannelMsg},
	{CDH_PRIVATEMSG, CDH_PrivateMsg},
	{CDH_JOINCHANNEL, CDH_JoinChannel},
	{CDH_LEAVECHANNEL, CDH_LeaveChannel},
	{CDH_CHANNELAMOUNT, CDH_ChannelAmount},
	{CDH_CHANNELINFONAME, CDH_ChannelInfoByName},
	{CDH_CHANNELINFOID, CDH_ChannelInfoByID},
	{CDH_CHANNELUSERS, CDH_ChannelUsers},
	{CDH_AUTHASADMIN, CDH_AuthAsAdmin},
	{CDH_KICKUSER, CDH_KickUser},
	{CDH_BANUSER, CDH_BanUser},
	{CDH_CHANGETOPIC, CDH_ChangeTopic},
	{CDH_CREATECHAN, CDH_CreateChannel},
	{CDH_DELETECHAN, CDH_DeleteChannel},
	{CDH_KICKFROMCHAN, CDH_KickUserFromChannel},
	{CDH_SETGHOSTMODE, CDH_SetGhostMode},
	{CDH_CHANNELADMINAUTH, CDH_AuthAsChannelAdmin},
	{CDH_SETCHANNELDATA, CDH_SetChannelData},
	{CDH_QUIT, CDH_Quit},
	{CDH_PLUGIN, CDH_Plugin},
	{CDH_CONFIRMMSG, CDH_ConfirmMessage},
};

HANDLE g_hConCmdThread = NULL; //Console command thread handle
HANDLE g_hMSThread = NULL; //Masterserver thread handle

DWORD g_dwCurRunTimer, g_dwLastRunTimer; //Timers for calculating program runtime 

print_output_s g_PrintOutput = {
	PO_STDOUT,
	""
};

enginefunctions_s g_EngineFuncs;

valid_string_s g_VSData = {
		NAME_MINIMUM_LEN, NAME_MAXIMUM_LEN, //Min/Max string length
		NAME_ALLOW_SPACES, //Spaces allowed?
		NAME_SPACE_IDENT, //Taken from ASCII table
		NAME_MIN_ASCIICHAR, NAME_MAX_ASCIICHAR //Min/Max ASCII char range
	};
//======================================================================
