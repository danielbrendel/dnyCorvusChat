#ifndef __CCE_SDK
#define __CCE_SDK

#include <stdlib.h>
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#endif

/* 
   Corvus Configuration Engine - SDK

   Coder: Daniel Brendel
   Version: 5
   Contact: dbrendel1988@gmail.com

   File: ccesdk.h: Data definitions

*/

//======================================================================
//Default DLL name
#ifdef _WIN64
#define CCE_FILE_NAME "CCE_x64.dll"
#elif _WIN32
#define CCE_FILE_NAME "CCE_x86.dll"
#endif

#if defined(_WIN32) || defined(_WIN64) //Windows OS compile
#define LOAD_MODULE(n) LoadLibraryA(n)
#define GET_PROC_ADDR(n) GetProcAddress(hLibrary, n)
#define FREE_MODULE(n) FreeLibrary(n)
#endif

#define CCE_STRING_LENGTH 2048
#define CCE_NAME_STRING_LENGTH 35
//======================================================================

//======================================================================
//Gets called when engine parsed an echo command
//lpszOutputText is a pointer to the string
typedef void (*TEchoEvent)(const char *lpszOutputText);

//Gets called when engine parsed a user defined command
//lpszCommand is a pointer to the name
typedef void (*TCmdEvent)(const char *lpszCommand);

//Gets called when engine needs cvar value by name.
//lpszName is the name. Copy the new value into this buffer
typedef void (*TWriteVarValueEvent)(char *lpszName);

//Gets called when the engine wants to know the amount of user vars
typedef unsigned int (*TGetVarAmountEvent)(void);

//Gets called when the engine wants to know the var name by id
//id is the var identifier, lpszValue is the pointer to the var name
//in which you have to write it.
typedef void (*TWriteVarNameByIdEvent)(unsigned int id, char* lpszValue);

//Gets called whenever a fatal error occurs. Clean up your stuff whenever
//this event function gets called, since the program will be terminated
typedef void (*TFatalErrorOccuredEvent)(void);
//======================================================================

//======================================================================
struct cce_alias_s {
	char name[CCE_NAME_STRING_LENGTH]; //Alias name
	char cmds[CCE_STRING_LENGTH]; //Alias commands
};

struct expressionholder_s {
	bool bConstant; //If shall be constant
	char szName[CCE_NAME_STRING_LENGTH]; //Variable name
	char szExpression[CCE_STRING_LENGTH]; //Variables' expression
};

struct cce_bindinfo_s {
	bool bActive; //Is key active to a cmd?
	DWORD vKeyCode; //The virtual key code
	char name[CCE_NAME_STRING_LENGTH]; //Key name
	char cmds[CCE_STRING_LENGTH]; //The bound cmds
};

struct cce_timer_s {
	DWORD dwSecs; //Execute commands after every defined secs (0 = disabled)
	char szCmds[CCE_STRING_LENGTH]; //The commands
};

struct cce_eventtable_s {
	TEchoEvent pfnEchoEvent; //Echo event func
	TCmdEvent pfnCmdEvent; //Cmd event func
	TWriteVarValueEvent pfnWriteVarValueEvent; //Get user variable value by name event func
	TGetVarAmountEvent pfnGetVarAmountEvent; //Get amount of user variables event func
	TWriteVarNameByIdEvent pfnWriteVarNameByIdEvent; //Get user variable name by id event func
	TFatalErrorOccuredEvent pfnFatalErrorOccuredEvent; //On fatal error occured event func
};
//======================================================================

//======================================================================
//Set the current working directory for the engine
//It will add the path to the lpszPath string
typedef void (WINAPI *TCCE_SetCurrentPath)(const char *lpszPath);

//Pass event function pointers to engine by a pointer to a structure
//containing the data. Specify the table size to let the engine confirm it
typedef bool (WINAPI *TCCE_SetEventFuncs)(cce_eventtable_s *pTable, unsigned int dwTableSize);

//This just returns an about string pointer
typedef const char* (WINAPI *TCCE_About)(void);

//Call this every frame. The engine calculates there
//the timer command specific stuff
typedef void (WINAPI *TCCE_CalcTimer)(void);

//Register a key alias name to a virtual key code.
//The key is first set to inactive. When the engine
//parses a bind command the command is bound to that key.
//It is then active. If the engine parses an unbind or
//unbindall command it is set to inactive. The VKey is
//not used by the engine itself, it does not check for
//keys. This is your job. For example you can enumerate
//trough all of your virtual key codes, get the key bin
//data, check if the key is active and then execute the
//commands if the key is pressed.
typedef cce_bindinfo_s *(WINAPI *TCCE_RegisterKeyBind)(DWORD vKey, char *lpszName);

//Get a pointer to key bind data by passing the virtual key code
typedef cce_bindinfo_s *(WINAPI *TCCE_GetBindInfoByKey)(DWORD vKey);

//Get a pointer to key bind data by passing the key alias name
typedef cce_bindinfo_s *(WINAPI *TCCE_GetBindInfo)(const char *lpszName);

//Execute a script code. lpszCode points to the code string
typedef void (WINAPI *TCCE_ExecCode)(char *lpszCode);

//Returns the amount of arguments for a parsed command or cvar
typedef DWORD (WINAPI *TCCE_CmdGetCount)(void);

//Returns a pointer to a string which is an argument of the command or cvar
typedef char* (WINAPI *TCCE_CmdGetValue)(DWORD dwIdent);

//Execute a script file.
typedef BOOL (WINAPI *TCCE_ExecScript)(const char *lpszScriptFile);

//Access data of a constant or variable
typedef expressionholder_s* (WINAPI *TCCE_GetVarOrConstant)(const char* lpszHolderName);
//======================================================================

//======================================================================
extern TCCE_SetCurrentPath CCE_SetCurrentPath;
extern TCCE_SetEventFuncs CCE_SetEventFuncs;
extern TCCE_About CCE_About;
extern TCCE_CalcTimer CCE_CalcTimer;
extern TCCE_RegisterKeyBind CCE_RegisterKeyBind;
extern TCCE_GetBindInfoByKey CCE_GetBindInfoByKey;
extern TCCE_GetBindInfo CCE_GetBindInfo;
extern TCCE_ExecCode CCE_ExecCode;
extern TCCE_CmdGetCount CCE_CmdGetCount;
extern TCCE_CmdGetValue CCE_CmdGetValue;
extern TCCE_ExecScript CCE_ExecScript;
extern TCCE_GetVarOrConstant CCE_GetVarOrConstant;

extern HMODULE hLibrary;
//======================================================================

//======================================================================
BOOL WINAPI CCE_InitLibrary(const char *lpszDLLPath);
BOOL WINAPI CCE_FreeLibrary(void);
//======================================================================

#endif