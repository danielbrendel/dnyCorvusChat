#include "channels.h"
#include "utils.h"

/*
	CorvusChat Server (dnyCorvusChat) - Chatserver component

	Developed by Daniel Brendel

	Version: 0.5
	Visit: https://github.com/danielbrendel
	Mail dbrendel1988@gmail.com

	File: channel.cpp: Channel manager implementation 
*/

//======================================================================
BOOL CChannels::CreateChannel(char *szName, char *szPassword, char *szAuthPassword, char *szTopic, DWORD dwMaxUsers, bool bRegistered)
{
	//Create a new channel

	if (!szName)
		return FALSE;

    //Check if channel limit is reached
	if ((CHANNELAMOUNT)vChannels.size() >= (CHANNELAMOUNT)g_GlobalVars.pMaxChannels->iValue)
        return FALSE;

	//Check string sizes
	if ((szName) && (!IsValidStrLen(szName)))
		return FALSE;

	if ((szPassword) && (!IsValidStrLen(szPassword)))
		return FALSE;

	if ((szTopic) && (!IsValidStrLen(szTopic)))
		return FALSE;

	//Check if a channel with this name already exists
	if (GetChannelByName(szName))
		return FALSE;

	//Allocate memory and set data

    channel_s* pNew = new channel_s;
	if (!pNew)
		return FALSE;

    strcpy_s(pNew->szName, szName);
    strcpy_s(pNew->szPassword, (szPassword) ? szPassword : DEFAULT_CHANNEL_PASSWORD);
	strcpy_s(pNew->szAdminPassword, (szAuthPassword) ? szAuthPassword : DEFAULT_CHANNEL_AUTHPASSWORD);
    strcpy_s(pNew->szTopic, (szTopic) ? szTopic : DEFAULT_CHANNEL_TOPIC);
    pNew->dwMaxUsers = (dwMaxUsers) ? dwMaxUsers : DEFAULT_MAXUSERAMOUNT;
    pNew->bRegistered = bRegistered;
    pNew->vUsers.clear();

    vChannels.push_back(pNew);

	return TRUE;
}
//======================================================================

//======================================================================
BOOL CChannels::DeleteChannel(CHANNELID chChannelId)
{
	//Delete a channel

	if (chChannelId >= (CHANNELID)vChannels.size())
		return FALSE;

	//Free user data of this channel
	for (CHANNELID i = 0; i < (CHANNELID)vChannels[chChannelId]->vUsers.size(); i++)
		delete vChannels[chChannelId]->vUsers[i];

	//Clear user list of this channel
	vChannels[chChannelId]->vUsers.clear();

	//Free channel memory and remove from list
	delete vChannels[chChannelId];
	vChannels.erase(vChannels.begin() + chChannelId);

	return TRUE;
}
//======================================================================

//======================================================================
CHANNELID CChannels::GetIdOfChannel(channel_s* pChannel)
{
	//Get the channel ID of a channel by channel data pointer

	if (!pChannel)
		return INVALID_CHANNEL_ID;

	//Loop trough list and check for pointer equality
	for (CHANNELID i = 0; i < (CHANNELID)vChannels.size(); i++) {
		if (vChannels[i] == pChannel)
			return i;
	}

	return INVALID_CHANNEL_ID;
}
//======================================================================

//======================================================================
channel_s *CChannels::GetChannelById(const CHANNELID chChannelId)
{
	//Get a channel data pointer by ID

	if (chChannelId < (CHANNELID)vChannels.size())
        return vChannels[chChannelId];

	return NULL;
}
//======================================================================

//======================================================================
channel_s *CChannels::GetChannelByName(const char *szName)
{
	//Get a channel data pointer by the channel name name. Names are converted to low chars to prevent having channels with same name but different char type.

    if (!szName) 
		return NULL;

	if (!IsValidStrLen(szName))
		return NULL;

	char szArg[MAX_NETWORK_STRING_LENGTH];
	char szFromList[MAX_NETWORK_STRING_LENGTH];

	//Convert search string to lower chars
	strcpy_s(szArg, szName);
	toLower(szArg);

	//Loop trough list
	for (DWORD i = 0; i < vChannels.size(); i++) {
		//Convert channel name of current entry
		strcpy_s(szFromList, vChannels[i]->szName);
		toLower(szFromList);

		//Check for string equality
        if (strcmp(szFromList, szArg)==0)
            return vChannels[i];
	}

	return NULL;
}
//======================================================================

//======================================================================
const CHANNELID CChannels::GetChannelCount(void)
{ 
	//Return the amount of existing channels

	return (CHANNELID)vChannels.size(); 
}
//======================================================================

//======================================================================
BOOL CChannels::IsUserInChannel(const clientinfo_s *pClient, const channel_s *pChannel, chan_user_s** ppChanUser)
{
	//Check if a user has joined a channel

	if ((!pClient) || (!pChannel)) 
		return false;

	//Check if users are inside the channel
	if (pChannel->vUsers.size() == 0) 
		return false;

	//Loop trough list
	for (CHANNELID i = 0; i < (CHANNELID)pChannel->vUsers.size(); i++) {
        if (pChannel->vUsers[i]->pClient == pClient) { //Compare client pointer
			//Set user pointer if desired
			if (ppChanUser)
				*ppChanUser = pChannel->vUsers[i];

			return true;
		}
	}

	return false;
}
//======================================================================

//======================================================================
chan_user_s *CChannels::GetUserById(const channel_s* pChannel, const CHUSERID dwUserId)
{
	//Get a pointer to the user structure data by the user channel ID

    if (!pChannel)
		return NULL;

	if (dwUserId < (CHUSERID)pChannel->vUsers.size())
        return pChannel->vUsers[dwUserId];

	return NULL;
}
//======================================================================

//======================================================================
chan_user_s *CChannels::GetUserByName(const channel_s* pChannel, const char *szName)
{
	//Get a pointer to the user structure data by the user name

	if ((!pChannel) || (!szName)) 
		return NULL;

	//Loop trough list and compare names
	for (CHANNELID i = 0; i < (CHANNELID)pChannel->vUsers.size(); i++) {
        if (strcmp(pChannel->vUsers[i]->pClient->userinfo.szName, szName)==0)
            return pChannel->vUsers[i];
	}

	return NULL;
}
//======================================================================

//======================================================================
CHUSERID CChannels::GetUserIdOfChannel(const channel_s* pChannel, const chan_user_s* pUser)
{
	//Get the ID of a user by user data pointer

	if ((!pChannel) || (!pUser)) 
		return INVALID_CHUSER_ID;

	//Search in list and compare data pointers
	for (CHUSERID i = 0; i < (CHUSERID)pChannel->vUsers.size(); i++) {
		if (pChannel->vUsers[i] == pUser)
			return i;
	}

	return -1;
}
//======================================================================

//======================================================================
BOOL CChannels::PutUserInChannel(clientinfo_s *pClient, channel_s *pChannel)
{
	//Put a user into a channel

    if ((!pClient) || (!pChannel)) 
		return FALSE;;

    //Check for user limit

    if ((CHUSERID)pChannel->vUsers.size() >= pChannel->dwMaxUsers)
        return FALSE;

	//Check if user is already inside
	if (IsUserInChannel(pClient, pChannel, NULL))
		return FALSE;

	//Allocate memory and set data

    chan_user_s* pNew = new chan_user_s;
	if (!pNew)
		return FALSE;

    pNew->pClient = pClient;
	pNew->bIsChannelAdmin = false;

    pChannel->vUsers.push_back(pNew);

	return TRUE;
}
//======================================================================

//======================================================================
BOOL CChannels::RemoveUserFromChannel(chan_user_s* pUser, channel_s *pChannel)
{
	//Remove a user from a channel

	if ((!pUser) || (!pChannel)) 
		return FALSE;

	//Get ID of this user
    CHUSERID chuId = GetUserIdOfChannel(pChannel, pUser);
    if (chuId == INVALID_CHUSER_ID)
		return FALSE;

	//Free memory and remove from list
    delete pChannel->vUsers[chuId];
    pChannel->vUsers.erase(pChannel->vUsers.begin() + chuId);

	return TRUE;
}
//======================================================================
