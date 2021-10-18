#ifndef _DYNVARS_H
#define _DYNVARS_H

#include "includes.h"

/*
	CorvusChat Server (dnyCorvusChat) - Chatserver component

	Developed by Daniel Brendel

	Version: 0.5
	Visit: https://github.com/danielbrendel
	Mail dbrendel1988@gmail.com

	File: dynvars.h: Dynamic vars manager interface
*/

//======================================================================
#define DYNVAR_MAX_NAME_LEN 35
#define DYNVAR_MAX_VALUE_LEN 200

#define DYNVAR_PREFIX '$'

#define DYNVAR_ONLY_STRSTR false //Uses only strstr to check for dyn vars
#define DYNVAR_WITH_VALID_STREND true //Useful to prevent using a substring dyn var name in a dyn var name string
//======================================================================

//======================================================================
typedef void (*TpfnDynVarCallback)(char* szVarValue);
//======================================================================

//======================================================================
struct dynvar_s {
	char szVarName[DYNVAR_MAX_NAME_LEN + 1]; //Dynvar name (with '$' prefix)
	TpfnDynVarCallback pfnFunction; //Callback function to get the current value
};
//======================================================================

//======================================================================
class CDynVars {
private:
	std::vector<dynvar_s> vDynVars;

	bool bCheckType;

	BOOL DynvarExists(LPCSTR lpszName, DWORD *pdwDynVarId);
	BOOL DynvarIsInExpression(LPCSTR lpszExpression, DWORD dwDynVarId);
public:
	CDynVars() { }
	~CDynVars() { }

	void SetCheckType(bool bCheckType) { this->bCheckType = bCheckType; }

	BOOL AddDynVar(LPCSTR lpszName, const TpfnDynVarCallback pfnCallbackFunc);
	VOID Clear(void);

	std::string HandleExpression(LPCSTR lpszExpression);
};
//======================================================================

#endif