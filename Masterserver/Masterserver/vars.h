#ifndef _VARS_H
#define _VARS_H

#include "includes.h"
#include "clients.h"
#include "concommand.h"
#include "usermsgs.h"
#include "clientdata.h"

/*
	CorvusChat Server (dnyCorvusChat) - Masterserver component

	Developed by Daniel Brendel

	Version: 0.2
	Visit: https://github.com/danielbrendel
	Mail dbrendel1988@gmail.com

	File: vars.h: Global variables interface and general definitions
*/

//======================================================================
#define PROGRAM_NAME "CorvusChat Masterserver"
#define PROGRAM_SHORTCUT "CCM"
#define PROGRAM_VERSION "0.2"
#define PROGRAM_AUTHOR "Daniel Brendel"
#define PROGRAM_CONTACT "dbrendel1988@gmail.com"
#define PROGRAM_WEBSITE "https://github.com/danielbrendel/"

#define CMD_STDOUT_PREFIX "\t"

#define CONSOLE_TITLE PROGRAM_NAME " (%d/%d)"

#define PORT 4002

#define IDENT_GETSERVERLIST 0x01
#define IDENT_ADDASSERVER 0x02
//======================================================================

//======================================================================
struct objects_s {
	CClient Client;
	CClientData ClientData;
	CConCommand ConCommand;
	CUserMsg UserMsg;
};
//======================================================================

//======================================================================
extern objects_s g_Objects;
extern bool g_bServerRunning;
extern DWORD g_dwLocalAlias;
extern BYTE g_ucLocalhostId;
extern HANDLE g_hConThread;
//======================================================================

#endif