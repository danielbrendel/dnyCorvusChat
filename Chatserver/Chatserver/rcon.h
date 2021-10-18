#ifndef _RCON_H
#define _RCON_H

#include "includes.h"
#include "concommand.h"

/*
	CorvusChat Server (dnyCorvusChat) - Chatserver component

	Developed by Daniel Brendel

	Version: 0.5
	Visit: https://github.com/danielbrendel
	Mail dbrendel1988@gmail.com

	File: rcon.h: Remote control manager interface 
*/

//======================================================================
#define MAX_RCON_BUFFER 2048
#define MAX_PASSWORD_LEN 40
#ifndef SD_BOTH
#define SD_BOTH 2
#endif
//======================================================================

//======================================================================
enum r_po_type {
	R_PO_STDOUT,
	R_PO_RCON,
	R_PO_IRC
};
//======================================================================

//======================================================================
struct r_print_output_s {
	r_po_type type; //The printing type
	char szIRCUser[250]; //The user name of the IRC network
};

struct rcon_data_s {
	SOCKET hClient; //Socket descriptor of the client that has sent a rcon command
	sockaddr_in saddr; //Address information of the client
	r_print_output_s *pPrintOutput; //To set its print type for the view of ConsolePrint
};

struct serverinfo_udp_s {
	char ident[5];
    char name[250];
    WORD port;
    DWORD version;
    DWORD ifver;
    char fclient[250];
    DWORD maxclients;
    DWORD curclients;
    DWORD maxchans;
    DWORD curchanamount;
    bool bPassword;
    unsigned int uptime;
};
//======================================================================

//======================================================================
class CRemoteControl {
private:
	SOCKET hSocket;
	sockaddr_in saddr;
	bool bIsReady;
	rcon_data_s rcondata;
public:
	CRemoteControl() { hSocket = NULL; bIsReady = false; }
	~CRemoteControl() { }

	BOOL Initialize(WORD wPort, r_print_output_s* pPrintOutput);
	BOOL Clear(VOID);

	VOID Process(CConCommand* pConCommand);

	BOOL SendText(LPCSTR lpszRemoteText);
};
//======================================================================

#endif