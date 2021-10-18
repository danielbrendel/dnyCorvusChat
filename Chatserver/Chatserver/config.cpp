#include "config.h"
#include "vars.h"
#include "log.h"

/*
	CorvusChat Server (dnyCorvusChat) - Chatserver component

	Developed by Daniel Brendel

	Version: 0.5
	Visit: https://github.com/danielbrendel
	Mail dbrendel1988@gmail.com

	File: config.cpp: Configuration specific implementations 
*/

//======================================================================
cce_eventtable_s ef = {&EchoEvent, &CmdEvent, &WriteVarValueEvent, &GetVarAmountEvent, &WriteVarNameByIdEvent, &FatalErrorOccuredEvent};

const char g_szCmdNameTable[][CCE_NAME_STRING_LENGTH + 1] = { //All server commands
	"createchannel",
	"loadplugin",
	"setbanner"
};
//======================================================================

//======================================================================
VOID ConsolePrint(WORD wConAttributes, LPCSTR lpszFmt, ...);
char* CreateRandomString(BYTE bAsciiStart, BYTE bAsciiEnd, unsigned short usStrSize);
void FatalError(const char* szErrMsg, ...);
void ShutdownComponents(void);
//======================================================================

//======================================================================
void EchoEvent(const char *lpszOutputText)
{
	//Called for text output
	
	if ((!lpszOutputText) || (!lpszOutputText[0]))
		return;

	if (g_PrintOutput.type == PO_STDOUT) { //Send text to stdout and log to hard disc (if desired)
		ConsolePrint(FOREGROUND_PINK, "%s\n", lpszOutputText);

		//long long ulValue = (g_GlobalVars.pLogToDisc) ? g_GlobalVars.pLogToDisc->iValue : 0;

		//if (ulValue)
			//LogMessage("%s", lpszOutputText);
	} else if (g_PrintOutput.type == PO_RCON) { //Send text to the rcon using client
		g_Objects.RCon.SendText(lpszOutputText);
	}
}
//======================================================================

//======================================================================
void CmdEvent(const char *lpszCommand)
{
    //Called for handling user cmds/vars

	if (lpszCommand[0] == '#') { //Pass command/var to console
		static char szConCmd[CCE_STRING_LENGTH];

		strcpy_s(szConCmd, &lpszCommand[1]);

		g_Objects.ConCommand.HandleCommand(szConCmd);

		return;
	}

	if (CCE_CmdGetCount() >= 1) { //Handle control variable sets
		char* szArg1 = CCE_CmdGetValue(0);
		if (szArg1) {
			int iArg = atoi(szArg1);

			if (strcmp(lpszCommand, g_szCmdNameTable[0])==0) {
				char* szArg2 = CCE_CmdGetValue(1);
				char* szArg3 = CCE_CmdGetValue(2);
				char* szArg4 = CCE_CmdGetValue(3);
				char* szArg5 = CCE_CmdGetValue(4);

				if ((szArg2) && (szArg3) && (szArg4) && (szArg5)) 
					ENG_CreateChannel(szArg1, szArg2, szArg3, szArg4, atoi(szArg5), true);

				return;
			} else if (strcmp(lpszCommand, g_szCmdNameTable[1])==0) {
				char* szArg2 = CCE_CmdGetValue(1);

				bool bManually;

				if (szArg2)
					bManually = (BOOL)atoi(szArg2) == TRUE;
				else
					bManually = false;

				ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, "Loading plugin \"%s\"... ", szArg1);

				(g_Objects.Plugins.LoadPlugin(szArg1, bManually)) ? ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, "Done\n") : ConsolePrint(FOREGROUND_RED, "Failed\n");

				return;
			} else if (strcmp(lpszCommand, g_szCmdNameTable[2])==0) {
				char* szArg2 = CCE_CmdGetValue(1);
				char* szArg3 = CCE_CmdGetValue(2);

				if ((szArg2) && (szArg3)) {
					strcpy_s(g_GlobalVars.aotd.szURL, szArg1);
					g_GlobalVars.aotd.x = atoi(szArg2);
					g_GlobalVars.aotd.y = atoi(szArg3);
				}

				return;
			}
			
			//Parse
			g_Objects.CVarMgr.Parse(lpszCommand);
		}

	}
}
//======================================================================

//======================================================================
void WriteVarValueEvent(char *lpszName)
{
	//Called for setting var values

	if (!lpszName)
		return;

	cvar_s* pCvar = g_Objects.CVarMgr.GetCVar(lpszName);
	if (pCvar) {
		if (pCvar->eType == CVAR_TYPE_STRING) {
			sprintf_s(lpszName, 100, "%s", pCvar->szValue);
		} else if (pCvar->eType == CVAR_TYPE_INTEGER) {
			sprintf_s(lpszName, 100, "%i", pCvar->iValue);
		} else if (pCvar->eType == CVAR_TYPE_FLOAT) {
			sprintf_s(lpszName, 100, "%lf", pCvar->dblValue);
		}
	}
}
//======================================================================

//======================================================================
unsigned int GetVarAmountEvent(void)
{
	//Called for returning the amount of used cvars

	return (unsigned int)g_Objects.CVarMgr.Count();
}
//======================================================================

//======================================================================
void WriteVarNameByIdEvent(unsigned int id, char* lpszValue)
{
	//Called for setting a cvar name by ID

	if (id < g_Objects.CVarMgr.Count()) {
		cvar_s* pCvar = g_Objects.CVarMgr.GetCVar(lpszValue);
		if (pCvar)
			strcpy_s(lpszValue, CCE_NAME_STRING_LENGTH, pCvar->szName);
	}
}
//======================================================================

//======================================================================
void FatalErrorOccuredEvent(void)
{
	//Called for fatal CCE errors

	ConsolePrint(FOREGROUND_RED, "Fatal CCE error occured, terminating program...\n");

	ShutdownComponents();
}
//======================================================================

//======================================================================
unsigned char InitCCE(void)
{
	//Initialize CCE component

	//Format the full file name of CCE module
	char szTemp[250];
	sprintf_s(szTemp, "%s\\%s", g_GlobalVars.szCCEPath, CCE_FILE_NAME);
	
	//Load CCE module
	if (!CCE_InitLibrary(szTemp)) {
		return CCE_EINIT;
	}

	//Pass event functions to CCE
	if (!CCE_SetEventFuncs(&ef, sizeof(cce_eventtable_s))) {
		return CCE_EEVENTFUNCS;
	}

	//Set CCE working directory
	CCE_SetCurrentPath(g_GlobalVars.szCCEScripts);

	return CCE_ENO;
}
//======================================================================

//======================================================================
cvar_s* CCVarMgr::RegisterCVar(const char* szName, const CVarType_e eType, const char* pszDefaultValue)
{
	//Register a cvar

	if (!szName)
		return NULL;

	//Check if already exists
	if (this->GetCVar(szName))
		return NULL;

	//Allocate memory for new cvar
	cvar_s* pCvar = new cvar_s;
	if (!pCvar)
		return NULL;

	//Clear memory
	memset(pCvar, 0x00, sizeof(cvar_s));

	//Store data

	strcpy_s(pCvar->szName, szName);
	pCvar->eType = eType;

	if (pszDefaultValue) {
		switch (eType) {
		case CVAR_TYPE_STRING:
			strcpy_s(pCvar->szValue, pszDefaultValue);
			break;
		case CVAR_TYPE_INTEGER:
			pCvar->iValue = atoi(pszDefaultValue);
			break;
		case CVAR_TYPE_FLOAT:
			pCvar->dblValue = atof(pszDefaultValue);
			break;
		default:
			delete pCvar;
			return NULL;
			break;
		}
	}

	//Add to list
	this->vCVars.push_back(pCvar);

	return pCvar; //Return pointer to this entry from on top
}
//======================================================================

//======================================================================
bool CCVarMgr::RemoveCVar(const char* szName)
{
	//Delete an existing cvar

	//Get cvar id
	size_t uiId = this->GetCVarId(szName);
	if (uiId == std::string::npos)
		return false;

	//Free memory
	delete this->vCVars[uiId];

	//Delete from list
	this->vCVars.erase(this->vCVars.begin() + uiId);

	return true;
}
//======================================================================

//======================================================================
bool CCVarMgr::Parse(const char* lpszIdentifier)
{
	//Parse cvar string set event

	if (!lpszIdentifier)
		return false;

	//Get argument string pointer
	char* pszArgument = CCE_CmdGetValue(0);
	if (!pszArgument)
		return false;

	//Find cvar
	cvar_s* pCvar = this->GetCVar(lpszIdentifier);
	if (!pCvar)
		return false;
	
	//Set value
	switch (pCvar->eType) {
	case CVAR_TYPE_STRING:
		strcpy_s(pCvar->szValue, pszArgument);
		break;
	case CVAR_TYPE_INTEGER:
		pCvar->iValue = atoi(pszArgument);
		break;
	case CVAR_TYPE_FLOAT:
		pCvar->dblValue = atof(pszArgument);
		break;
	default:
		return false;
		break;
	}
	
	return true;
}
//======================================================================

//======================================================================
void CCVarMgr::Clear(void)
{
	//Clear list

	this->vCVars.clear();
}
//======================================================================

//======================================================================
cvar_s* CCVarMgr::GetCVar(const char* pszName)
{
	//Get CVar data pointer by name

	if (!pszName)
		return NULL;

	//Search in list
	for (size_t i = 0; i < this->vCVars.size(); i++) {
		if (strcmp(this->vCVars[i]->szName, pszName)==0) { //Check strings for equality
			return this->vCVars[i]; //Return pointer to list entry data
		}
	}

	return NULL;
}
//======================================================================

//======================================================================
size_t CCVarMgr::GetCVarId(const char* pszName)
{
	//Get cvar id by name

	if (!pszName)
		return std::string::npos;

	//Search in list
	for (size_t i = 0; i < this->vCVars.size(); i++) {
		if (strcmp(this->vCVars[i]->szName, pszName)==0) { //Check strings for equality
			return i; //Return list entry id
		}
	}

	return std::string::npos;
}
//======================================================================

//======================================================================
bool CCVarMgr::SetCVarValue(const char* pszName, const char* szValue)
{
	//Set cvar value - string

	cvar_s* pCvar = this->GetCVar(pszName);
	if ((!pCvar) || (pCvar->eType != CVAR_TYPE_STRING))
		return false;

	strcpy_s(pCvar->szValue, szValue);

	return true;
}
//======================================================================

//======================================================================
bool CCVarMgr::SetCVarValue(const char* pszName, const int iValue)
{
	//Set cvar value - long integer

	cvar_s* pCvar = this->GetCVar(pszName);
	if ((!pCvar) || (pCvar->eType != CVAR_TYPE_INTEGER))
		return false;

	pCvar->iValue = iValue;

	return true;
}
//======================================================================

//======================================================================
bool CCVarMgr::SetCVarValue(const char* pszName, const double dblValue)
{
	//Set cvar value - double

	cvar_s* pCvar = this->GetCVar(pszName);
	if ((!pCvar) || (pCvar->eType != CVAR_TYPE_FLOAT))
		return false;

	pCvar->dblValue = dblValue;

	return true;
}
//======================================================================
