#ifndef _USERMSGS_H
#define _USERMSGS_H

#include "includes.h"
#include "clientsock.h"

/*
	CorvusChat Server (dnyCorvusChat) - Chatserver component

	Developed by Daniel Brendel

	Version: 0.5
	Visit: https://github.com/danielbrendel
	Mail dbrendel1988@gmail.com

	File: usermsgs.h: User message manager interface 
*/

//======================================================================
#define MAX_USER_MSG_SIZE 10240
#define INVALID_USERMSG -1
#define MSG_NAME_LEN 250
//======================================================================

//======================================================================
typedef int GLOBALID; //Global ID is valid for CLIENTID and CHANNELID. Both must be defined as the same type!
//======================================================================

//======================================================================
enum SEND_TYPE {
	ST_USER = 1,
	ST_CHANNEL,
	ST_BROADCAST
};
//======================================================================

//======================================================================
struct usermsg_s {
	char name[MSG_NAME_LEN];
	unsigned int size;
};
//======================================================================

//======================================================================
class CUserMsg {
private:
	LPVOID pvuMsgBuf; //Pointer to a buffer with allocated memory.
	DWORD dwuMsgPos; //Current byte position of the buffer.
	SEND_TYPE CurRecvType; //Type of reciever. This can be either a client, a channel or broadcast.
	GLOBALID gidCurRecv; //ID of the current reciever entity
	DWORD dwStrLength; //Used for counting string chars
	usermsg_s *pCurMsg; //Current usermsg pointer.

	std::vector<usermsg_s*> vUserMsgs;
public:
	CUserMsg() { pvuMsgBuf = NULL; dwuMsgPos = 0; gidCurRecv = -1; dwStrLength = 0; pCurMsg = NULL;}
	~CUserMsg() { Clear(); }

	int AddUserMsg(char *szName, int iSize);
	void Clear(void);

	DWORD GetAmount(void);

	usermsg_s* GetMessageInfo(unsigned int dwID);
	usermsg_s* GetMessageInfo(char *szName);
	int GetMessageByName(char* szMsg);
	
	bool MessageBegin(int iMsgID, GLOBALID gidRecvID, SEND_TYPE SendType);
	bool MessageEnd(CClientSocket* pSocket);

	void WriteByte(BYTE bValue);
	void WriteChar(char cValue);
	void WriteShort(short sValue);
	void WriteInt(int iValue);
	void WriteFloat(float fValue);
	void WriteString(char *szString, DWORD dwStringSize);
	void WriteBuf(void* pBuf, DWORD dwBufSize);
};
//======================================================================

#endif