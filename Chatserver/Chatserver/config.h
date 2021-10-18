#ifndef _CONFIG_H
#define _CONFIG_H

#include "includes.h"
#include "ccesdk.h"

/*
	CorvusChat Server (dnyCorvusChat) - Chatserver component

	Developed by Daniel Brendel

	Version: 0.5
	Visit: https://github.com/danielbrendel
	Mail dbrendel1988@gmail.com

	File: config.h: Configuration specific interface 
*/

//======================================================================
#define CCE_ENO 0
#define CCE_EINIT 1
#define CCE_EEVENTFUNCS 2

#define CCE_VARVALUE_LEN 200
//======================================================================

//======================================================================
extern cce_eventtable_s ef;
//======================================================================

//======================================================================
enum CVarType_e {
	CVAR_TYPE_STRING = 0,
	CVAR_TYPE_INTEGER,
	CVAR_TYPE_FLOAT
};
//======================================================================

//======================================================================
struct bind_s {
	char name[250];
	int vk;
};

struct cvar_s {
	char szName[CCE_NAME_STRING_LENGTH];
	CVarType_e eType;
	union {
		char szValue[CCE_STRING_LENGTH];
		int iValue;
		double dblValue;
	};
};
//======================================================================

//======================================================================
class CCVarMgr {
private:
	std::vector<cvar_s*> vCVars;

	size_t GetCVarId(const char* pszName);
public:
	CCVarMgr() {}
	~CCVarMgr() {}

	size_t Count(void) { return this->vCVars.size(); }

	cvar_s* RegisterCVar(const char* szName, const CVarType_e eType, const char* pszDefaultValue);
	bool RemoveCVar(const char* szName);

	bool Parse(const char* lpszIdentifier);

	void Clear(void);

	cvar_s* GetCVar(const char* pszName);
	bool SetCVarValue(const char* pszName, const char* szValue);
	bool SetCVarValue(const char* pszName, const int iValue);
	bool SetCVarValue(const char* pszName, const double dblValue);
};
//======================================================================

//======================================================================
void WriteVarValueEvent(char *lpszName);
void EchoEvent(const char *lpszOutputText);
void CmdEvent(const char *lpszCommand);
unsigned int GetVarAmountEvent(void);
void WriteVarNameByIdEvent(unsigned int id, char* lpszValue);
void FatalErrorOccuredEvent(void);

unsigned char InitCCE(void);
//======================================================================

#endif