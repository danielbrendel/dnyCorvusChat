# CorvusChat (dnyCorvusChat) - Chat Server component

Developer: Daniel Brendel\
Version: 0.5\
License: The MIT License\
Contact: dbrendel1988@gmail.com\
Visit: https://github.com/danielbrendel/

## Description:
This product is the chat server component of the CorvusChat service product. It allowes you
to create your own chat server for using in a network (LAN, Internet). You can control it
via the console interface, config scripting and RCON. Users can chat in channels and perform
private conversations. This component features all basic required stuff like MOTD, banlist
and general server management (server password, maximum allowed users/channels, force a
client version, client admin login, etc.). Also you can extend the server functionallity 
with the integrated plugin engine, which works via native DLLs. The component is also able
to update its own program executable and the readme.txt file. To get a general feature
overview, please have a look at the listing below.


## Features:
+ Server management
	- Name
	- Password
	- Forced client
	- Server notice messages (client, channel, broadcast)
+ Client management
	- Name (with validity check)
	- Clan Tag
	- Used Client
	- Messages from client-to-client
	- Maximum client amount
	- Force client login
	- User profile informations (E-Mail, Country, Full name)
	- Admin login via password
	- Admin actions (kick/ban users, create/delete/edit channels, switch ghost mode)
+ Network management
	- Server port
	- Limit Single-address connections
	- User messages system
	- Incoming client message system
	- Send system
	- Antiflood system
	- Send server infos via UDP
	- Define ping timeout value
+ Channel management
	- Join/Leave channels
	- Name (with validity check)
	- Password
	- Topic
	- Maximum users
	- Maximum channels
	- Messages from client-to-channel
	- Permanent or temporary channels
	- Auth as channel admin
	- Modify channel data as admin
+ Masterserver system
	- Handle Masterserver connection
	- Update specific server informations to Masterserver
+ Console interface management
	- Add console commands via manager (name, description, event function)
+ Daily server information management
	- Message of the Day manager
	- DynVars manager
	- Send Advertisment of the Day information to clients
+ IP Banlist management
	- IP banlist manager
+ Remote controlable server
	- Define RCON port
	- Define RCON password
	- Handle Console/Script expressions
	- Return output to the RCON user
+ Plugin Engine
	- Load/Unload plugins
	- Get plugin informations
	- Pause plugins
	- Provided C/C++ SDK
+ Update function
	- Update manager
	- Update and restart the server program file automatically
	- Update all resources by parsing a server update script
+ File logging
	- Log to disc
	- Format filename with time and date to have managed log files
	- Log normal messsages and errors
+ Misc
	- Restart server program
	- Colored console messages (client actions: yellow, client admin auth actions: light blue, private/channel chat messages: green, errors: red, config echo: pink, the rest default (commonly grey))


## Communication protocol:
Each message is prefixed with a message ID that identificates the type of the message and a following information buffer
* Server messages (These messages are sent to clients by the server, either to a single user, a channel or broadcast)
	+ "RequestLogin": 0x01 (1 byte in size) A request from the server to the client that the client may now send his login data
	+ "ServerVersion": 0x02 (5 bytes in size) The current version of the server protocol which is the program version
		- Argument 1 (Integer): The server version
	+ "Ping": 0x03 (2 byte in size) This is send to ensure that the client is still online
		- Argument 1 (Byte): The current ping value which the client must send back (pong)
	+ "ClientCount": 0x04 (5 bytes in size) This message is used to tell the clients the current amount of connected clients
		- Argument 1 (Integer): The amount of current connected clients
	+ "ClientInfo": 0x05 (1 + sizeof(clinfo_s) bytes in size) This message contains client informations which a client wants to see
		- Argument 1 (clinfo_s): A structure containing the client informations
	+ "MOTD": 0x06 (1 + variable string length) This message contains the current MOTD information
		- Argument 1 (String): The MOTD text
	+ "ChanCount": 0x07 (5 bytes in size) This message contains the amount of existing channels
		- Argument 1 (Integer): The amount of existing channels
	+ "ChanInfo": 0x08 (1 + sizeof(chaninfo_s) bytes in size) This message contains informations about a channel
		- Argument 1 (chaninfo_s): The channel informations
	+ "ChanUsers": 0x09 (1 + MAX_NETWORK_STRING_LENGTH + MAX_CHANUSERS_STRING_LENGTH bytes in size) This message contains a list of channel users of a specific channel
		- Argument 1 (String): The channel name
		- Argument 2 (String): The list of users, each name delimited with a specific delimiter
	+ "ListChannels": 0x0A (1 + MAX_CHANNAMES_STRING_LENGTH bytes in size) A message containing a list of channel names
		- Argument 1 (String): The list of channel names, each name delimited with a specific delimiter
	+ "ServerNotice": 0x0B (1 + variable string length) A message containing an information string
		- Argument 1 (String): The information text
	+ "PrivMsg": 0x0C (1 + MAX_NETWORK_STRING_LENGTH * 2 bytes in size) A private message from a specific user to a client
		- Argument 1 (String): The sender of the message
		- Argument 2 (String): The chat message
	+ "ChanMsg": 0x0D (1 + MAX_NETWORK_STRING_LENGTH * 2 bytes in size) A channel message from a specific user to a channel
		- Argument 1 (String): The sender of the message
		- Argument 2 (String): The channel name
		- Argument 3 (String): The chat message
	+ "ClientJoin": 0x0E (1 + sizeof(CLIENTID) + MAX_NETWORK_STRING_LENGTH bytes in size) An information containing the user ID and name that has joined the chat server
		- Argument 1 (Integer): The user ID
		- Argument 2 (String): The user nickname
	+ "ClientLeave": 0x0F (1 + sizeof(CLIENTID) + MAX_NETWORK_STRING_LENTH*2 bytes in size) An information containing the user ID, name and quit reason of a user that has left the server
		- Argument 1 (Integer): The user ID
		- Argument 2 (String): The user nickname
		- Argument 3 (String): The reason why this user has left the server
	+ "ChannelJoin": 0x10 (1 + sizeof(CHANNELID) + MAX_NETWORK_STRING_LENGTH*2 bytes in size) An information containing data of a user that has joined a channel
		- Argument 1 (Integer): The channel ID
		- Argument 2 (String): The user nickname
		- Argument 3 (String): The channel name
	+ "ChannelLeave": 0x11 (1 + sizeof(CHANNELID) + MAX_NETWORK_STRING_LENGTH*2 bytes in size) An information containing data of a user that has left a channel
		- Argument 1 (Integer): The channel ID
		- Argument 2 (String): The user nickname
		- Argument 3 (String): The channel name
	+ "Topic": 0x12 (1 + sizeof(topic_s) bytes in size) The topic of a channel
		- Argument 1 (String): The channel name
		- Argument 2 (String): The channels' topic
	+ "OperationStatus": 0x13 (5 bytes in size) A status value of a current client operation
		- Argument 1 (Short): The general status value
		- Argument 2 (Short): The detailed status value belonging to the first one
	+ "AOTD": 0x14 (1 + 8 + MAX_NETWORK_STRING_LENGTH bytes in size) The advertisment of the day information
		- Argument 1 (Integer): The X-resolution of the banner
		- Argument 2 (Integer): The Y-resolution of the banner
		- Argument 3 (String): The URL to the advertisment banner
* Client messages (These messages can be sent to the server by a client)
	+ Message confirmation: 0xFF (1 byte in size) Just to tell the server that this client has recieved a server message (only needed for message queueing system)
	+ Ping: 0x02 (2 bytes in size) Answer the server the last recieved ping value (pong)
		- Argument 1 (Byte): The recieved ping value
	+ Login: 0x01 (1 + sizeof(login_s) bytes in size) Tell the server the used login data
		- Argument 1 (String): The user nickname
		- Argument 2 (String): The used clan tag
		- Argument 3 (String): The used client program name
		- Argument 4 (String): The server password if required (however this argument must be sent, but can be empty if no server password is set)
	+ Channel message: 0x07 (1 + sizeof(msg_channelmsg_s) bytes in size) A message to a channel
		- Argument 1 (String): The channel name
		- Argument 2 (String): The chat message
	+ Private message: 0x08 (1 + sizeof(msg_privatemsg_s) bytes in size) A message to a chat user
		- Argument 1 (String): The user nickname
		- Argument 2 (String): The chat message
	+ Set Client Info: 0x04 (1 + sizeof(infodata_s) bytes in size) Set additional client information
		- Argument 1 (String): The E-Mail address
		- Argument 2 (String): The country
		- Argument 3 (String): The full name
	+ Get client count: 0x05 (1 byte in size) Query the current amount of connected clients
	+ Get client info: 0x06 (1 + MAX_NETWORK_STRING_LENGTH bytes in size) Query client information of a user
		- Argument 1 (String): The user nickname
	+ Join Channel: 0x09 (1 + sizeof(joinchan_s) bytes in size) Try to join a channel
		- Argument 1 (String): The channel name
		- Argument 2 (String): The channel password if required (however this argument must be sent, but can be empty if no channel password is set)
	+ Leave channel: 0x0A (1 + sizeof(MAX_NETWORK_STRING_LENGTH) bytes in size) Try to leave a channel
		- Argument 1 (String): The channel name
	+ Channel amount: 0x0B (1 byte in size) Query the amount of current existing channels
	+ Channel info by name: 0x0C (1 + MAX_NETWORK_STRING_LENGTH bytes in size) Query informations about a channel
		- Argument 1 (String): The channel name
	+ Channel info by ID: 0x1C (1 + sizeof(CHANNELID) bytes in size) Query informations about a channel
		- Argument 1 (Integer): The channel ID
	+ Channel users: 0x0D (1 + MAX_NETWORK_STRING_LENGTH bytes in size) Query user name list of a channel
		- Argument 1 (String): The channel name
	+ Admin authenication: 0x0E (1 + MAX_NETWORK_STRING_LENGTH bytes in size) Try to auth as server administrator
		- Argument 1 (String): The administrator password
	+ Kick user: 0x0F (1 + MAX_NETWORK_STRING_LENGTH bytes in size) Try to kick a user from server
		- Argument 1 (String): The user nickname
	+ Ban User: 0x10 (1 + MAX_NETWORK_STRING_LENGTH bytes in size) Try to ban a user from server
		- Argument 1 (String): The user nickname
	+ Change Topic: 0x11 (1 + sizeof(chantopic_s) bytes in size) Try to change a channel topic of a channel
		- Argument 1 (String): The channel name
		- Argument 2 (String): The new channel topic
	+ Create channel: 0x12 (1 + sizeof(createchan_s) bytes in size) Try to create a new registered channel
		- Argument 1 (Integer): The maximum amount of users joined in this channel
		- Argument 2 (String): The channel name
		- Argument 3 (String): The channel topic
		- Argument 4 (String): The channel password if desired (however this argument must be sent, use "#0" to allow joining without a password)
	+ Delete Channel: 0x13 (1 + MAX_NETWORK_STRING_LENGTH bytes in size) Try to delete a channel
		- Argument 1 (String): The channel name
	+ Kick user from channel: 0x18 (1 + sizeof(chanuser_s) bytes in size) Try to kick a user out of a channel
		- Argument 1 (String): The channel name
		- Argument 2 (String): The target user nickname
	+ Set ghost mode: 0x16 (2 bytes in size) Try to set the ghost mode value (if server allows this)
		- Argument 1 (Boolean): The status value
	+ Auth as channel admin: 0x1A (1 + sizeof(chanadmin_s) bytes in size) Authenticate as channel admin for a specific channel
		- Argument 1 (String): The channel name
		- Argument 2 (String): The channel authentication password
	+ Set channel data: 0x1B (1 + sizeof(chandata_s) bytes in size) Set channel data of a specified channel
		- Argument 1 (String): The channel name
		- Argument 2 (String): New channel topic (#0 to not modify it)
		- Argument 3 (String): New channel password (#0 to not modify it)
		- Argument 4 (String): New channel authentication password (#0 to not modify it)
		- Argument 5 (Integer): New channel maximum user amount (-1 to not modify it)
	+ Plugin message: 0x19 (1 + MAX_SIG_STRING + variable message size) Pass a buffer to a MSG-Listener (used by Plugins)
		- Argument 1 (String): The message signature to identify this message for the listener handler
		- Argument 2 (Buffer): A variable-sized buffer for the listener to handle
	+ Quit: 0x17 (1 + MAX_NETWORK_STRING_LENGTH bytes in size) Quit server with the given reason
		- Argument 1 (String): The quit message (reason why to quit for example)
RCon messages:
These messages are used to either get server information or send a remote control command to the server.
Note that every RCon command output will be sent back to the RCon client (which would normally be put to the console)
	+ Server information: 0x01 "CCINFO" 0xFF
		- Force server to send back specific server informations (see serverinfo_udp_s for details)
	+ Server rcon command: 0x02 "CCRCON" 0xFF <password (size: MAX_PASSWORD_LEN)> <cmd_ident: (size: 1 byte)> <expression (size: MAX_RCON_BUFFER)
		- Force server to handle RCon commands
		- password: The RCon password
		- cmd_ident: The command identifier ('c' = Console command, 's' = Script Expression)
		- expression: The expression to handle
		

## Quick start:
- Edit the config file(s) of the chat server
- Enable port forwarding by your router/firewall if needed
- Be sure you are connected with Internet or LAN (or just test it locally)
- Start the server program executable (x64 name postfix for native 64 bit build or x86 name postfix for native 32 bit build)
- Type 'help' into the server console window to get a list of commands
