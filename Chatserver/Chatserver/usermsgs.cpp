#include "usermsgs.h"
#include "vars.h"
#include "channels.h"

/*
	CorvusChat Server (dnyCorvusChat) - Chatserver component

	Developed by Daniel Brendel

	Version: 0.5
	Visit: https://github.com/danielbrendel
	Mail dbrendel1988@gmail.com

	File: usermsgs.cpp: User message manager implementation 
*/

//======================================================================
int CUserMsg::AddUserMsg(char *szName, int iSize)
{
	//Add a new usermsg to list

	if ((!szName) || (iSize == INVALID_USERMSG))
		return -1;

	//Check if message with name already exists
	if (GetMessageInfo(szName))
		return INVALID_USERMSG;

	//Allocate memory, set data and add to list
	usermsg_s *umsg = new usermsg_s;
	if (!umsg)
		return INVALID_USERMSG;

	strcpy_s(umsg->name, szName);
	umsg->size = iSize;
	
	vUserMsgs.push_back(umsg);

	return (int)vUserMsgs.size()-1;
}
//======================================================================

//======================================================================
void CUserMsg::Clear(void)
{
	//Cleanup all usermsg data

	for (unsigned int i = 0; i < vUserMsgs.size(); i++)
		delete vUserMsgs[i];

	vUserMsgs.clear();
}
//======================================================================

//======================================================================
DWORD CUserMsg::GetAmount(void) 
{ 
	//Get amount of registered user messages

	return (DWORD)vUserMsgs.size();
}
//======================================================================

//======================================================================
usermsg_s* CUserMsg::GetMessageInfo(unsigned int dwID)
{
	//Get usermsg data pointer by ID

	if (dwID < vUserMsgs.size())
		return vUserMsgs[dwID];

	return NULL;
}
//======================================================================

//======================================================================
usermsg_s* CUserMsg::GetMessageInfo(char *szName) 
{
	//Get usermsg data pointer by name

	//Search for name in list
	for (unsigned int i = 0; i < vUserMsgs.size(); i++) {
		if (strcmp(vUserMsgs[i]->name, szName)==0) //Check if name exists and return pointer
			return vUserMsgs[i];
	}

	return NULL;
}
//======================================================================

//======================================================================
int CUserMsg::GetMessageByName(char* szMsg)
{
	//Get the ID of a usermsg

	if (!szMsg)
		return INVALID_USERMSG;

	//Search for name in list
	for (unsigned int i = 0; i < vUserMsgs.size(); i++) {
		if (strcmp(vUserMsgs[i]->name, szMsg)==0)
			return (int)i;
	}

	return INVALID_USERMSG;
}
//======================================================================

//======================================================================
bool CUserMsg::MessageBegin(int iMsgID, GLOBALID gidRecvID, SEND_TYPE SendType)
{
	//Begin sending a usermsg

	//Check if old message has not been send / buffer has not be freed
	if ((pvuMsgBuf) || (dwuMsgPos > 0)) {
		ConsolePrint(FOREGROUND_RED, "[CUserMsg::MessageBegin] Old user message %d has not been finished (%d bytes in size)", *(BYTE*)pvuMsgBuf, dwuMsgPos);

		free(pvuMsgBuf);
		dwuMsgPos = 0;
	}

	//Check if the usermsg exists.
	usermsg_s *pMsg = GetMessageInfo(iMsgID);
	if (!pMsg)
		return false;

	pvuMsgBuf = malloc(MAX_USER_MSG_SIZE); //Allocate memory for the usermsg.
	if (!pvuMsgBuf)
		return false;

	dwuMsgPos = 0; //Start position is zero.
	gidCurRecv = gidRecvID; //ID of the reciever
	CurRecvType = SendType; //Send type
	dwStrLength = 0; //String length

	pCurMsg = pMsg; //Current usermsg pointer
	
	WriteByte(iMsgID + 1); //Write the ID of the usermsg

	return true;
}
//======================================================================

//======================================================================
bool CUserMsg::MessageEnd(CClientSocket* pSocket)
{
	//Ends a user message and sends it if it's valid

	//Check for usermsg size missmatch to ensure the amount of send data matches with the registered usermsg size
	if ((dwuMsgPos - dwStrLength > pCurMsg->size) || (dwuMsgPos - dwStrLength < pCurMsg->size))
		return false;

	bool bResult = false;

	if (CurRecvType == ST_USER) { //If the message shall be sent to a client
		bResult = pSocket->SendBuf(gidCurRecv, pvuMsgBuf, dwuMsgPos, g_GlobalVars.pSendType->iValue == 0);
	} else if (CurRecvType == ST_CHANNEL) { //If the message shall be sent to all users of a channel
		channel_s* pChannel = g_Objects.Channels.GetChannelById(gidCurRecv);
		if (pChannel) {
			for (CHUSERID i = 0; i < (CHUSERID)pChannel->vUsers.size(); i++) {
				if ((pChannel->vUsers[i]) && (pChannel->vUsers[i]->pClient)) {
					bResult = g_Objects.ClientSock.SendBuf(pChannel->vUsers[i]->pClient, pvuMsgBuf, dwuMsgPos, g_GlobalVars.pSendType->iValue == 0);
				}
			}
		}
	} else if (CurRecvType == ST_BROADCAST) { //If the message shall be sent to all clients
		bResult = pSocket->BroadCast(pvuMsgBuf, dwuMsgPos, g_GlobalVars.pSendType->iValue == 0);
	}

	//Free and clear stuff.
	free(pvuMsgBuf);
	dwuMsgPos = 0;
	gidCurRecv = 0;
	pCurMsg = NULL;
	dwStrLength = 0;
	pvuMsgBuf = NULL;

	return bResult;
}
//======================================================================

//======================================================================
void CUserMsg::WriteByte(BYTE bValue)
{
	//Write byte to message buffer

	*(unsigned char*)((DWORD_PTR)pvuMsgBuf + dwuMsgPos) = bValue;
	dwuMsgPos += sizeof(BYTE);
}
//======================================================================

//======================================================================
void CUserMsg::WriteChar(char cValue)
{
	//Write char to message buffer

	*(char*)((DWORD_PTR)pvuMsgBuf + dwuMsgPos) = cValue;
	dwuMsgPos += sizeof(char);
}
//======================================================================

//======================================================================
void CUserMsg::WriteShort(short sValue)
{
	//Write short to message buffer

	*(short*)((DWORD_PTR)pvuMsgBuf + dwuMsgPos) = sValue;
	dwuMsgPos += sizeof(short);
}
//======================================================================

//======================================================================
void CUserMsg::WriteInt(int iValue)
{
	//Write int to message buffer

	*(int*)((DWORD_PTR)pvuMsgBuf + dwuMsgPos) = iValue;
	dwuMsgPos += sizeof(int);
}
//======================================================================

//======================================================================
void CUserMsg::WriteFloat(float fValue)
{
	//Write float to message buffer

	*(float*)((DWORD_PTR)pvuMsgBuf + dwuMsgPos) = fValue;
	dwuMsgPos += sizeof(float);
}
//======================================================================

//======================================================================
void CUserMsg::WriteString(char *str, DWORD stringSize)
{
	//Write string to message buffer

	for (DWORD i = 0; i < stringSize; i++) {
		WriteChar(str[i]);
	}

	dwStrLength += stringSize;
}
//======================================================================

//======================================================================
void CUserMsg::WriteBuf(void* pBuf, DWORD dwBufSize)
{
	//Copies a buffer to the message buffer

	memcpy((void*)((DWORD_PTR)pvuMsgBuf + dwuMsgPos), pBuf, dwBufSize);
	dwuMsgPos += dwBufSize;
}
//======================================================================
