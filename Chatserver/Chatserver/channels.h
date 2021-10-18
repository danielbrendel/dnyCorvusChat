#ifndef _CHANNELS_H
#define _CHANNELS_H

#include "includes.h"
#include "clientsock.h"

/*
	CorvusChat Server (dnyCorvusChat) - Chatserver component

	Developed by Daniel Brendel

	Version: 0.5
	Visit: https://github.com/danielbrendel
	Mail dbrendel1988@gmail.com

	File: channels.h: Channel manager interface 
*/

//======================================================================
#define INVALID_CHANNEL_ID -1
#define INVALID_CHUSER_ID INVALID_CHANNEL_ID

#define DEFAULT_CHANNEL_TOPIC "Default channel topic"
#define DEFAULT_CHANNEL_PASSWORD S_NO_PASSWORD
#define DEFAULT_CHANNEL_AUTHPASSWORD "password"
#define DEFAULT_MAXUSERAMOUNT 50
//======================================================================

//======================================================================
typedef int CHANNELID;
typedef CHANNELID CHANNELAMOUNT;
typedef int CHUSERID;
//======================================================================

//======================================================================
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
//======================================================================

//======================================================================
class CChannels {
private:
	std::vector<channel_s*> vChannels;
public:
	CChannels() { }
	~CChannels() { }

	BOOL CreateChannel(char *szName, char *szPassword, char *szAuthPassword, char *szTopic, DWORD dwMaxUsers, bool bRegistered); 
	BOOL DeleteChannel(CHANNELID chChannelId); 

	CHANNELID GetIdOfChannel(channel_s* pChannel); 
	channel_s *GetChannelById(const CHANNELID chChannelId); 
	channel_s *GetChannelByName(const char *szName); 
	const CHANNELID GetChannelCount(void);

	BOOL IsUserInChannel(const clientinfo_s *pClient, const channel_s *pChannel, chan_user_s** ppChanUser); 
	chan_user_s *GetUserById(const channel_s* pChannel, const CHUSERID dwUserId); 
	chan_user_s *GetUserByName(const channel_s* pChannel, const char *szName); 
	CHUSERID GetUserIdOfChannel(const channel_s* pChannel, const chan_user_s* pUser); 

	BOOL PutUserInChannel(clientinfo_s *pClient, channel_s *pChannel); 
	BOOL RemoveUserFromChannel(chan_user_s* pUser, channel_s *pChannel); 
};
//======================================================================

#endif