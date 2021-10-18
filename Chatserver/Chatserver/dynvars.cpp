#include "dynvars.h"
#include "utils.h"

/*
	CorvusChat Server (dnyCorvusChat) - Chatserver component

	Developed by Daniel Brendel

	Version: 0.5
	Visit: https://github.com/danielbrendel
	Mail dbrendel1988@gmail.com

	File: dynvars.cpp: Dynamic vars manager implementation
*/

//======================================================================
BOOL CDynVars::DynvarExists(LPCSTR lpszName, DWORD *pdwDynVarId)
{
	//Check if a dyn var already exists

	if ((!lpszName) || (lpszName[0] != DYNVAR_PREFIX))
		return FALSE;

	//Search in list
	for (DWORD i = 0; i < vDynVars.size(); i++) {
		if (strcmp(vDynVars[i].szVarName, lpszName)==0) {
			//Set ID if desired
			if (pdwDynVarId)
				*pdwDynVarId = i;

			return TRUE;
		}
	}

	return FALSE;
}
//======================================================================

//======================================================================
BOOL CDynVars::DynvarIsInExpression(LPCSTR lpszExpression, DWORD dwDynVarId)
{
	//Check if a dyn var identifier is in the given expression string

	if ((!lpszExpression) || dwDynVarId >= vDynVars.size())
		return FALSE;

	//Check if the dyn var name is inside the string
	const char* szInStr = strstr(lpszExpression, vDynVars[dwDynVarId].szVarName);
	if (!szInStr)
		return FALSE;

	if (bCheckType == DYNVAR_ONLY_STRSTR) //If strstr is enough
		return TRUE;

	//Now check the last char of the identifier to be sure that a dynvar name is not a substring of another dyn var name
	szInStr += strlen(vDynVars[dwDynVarId].szVarName);

	return (!((szInStr[0] >= 0x41) && (szInStr[0] <= 0x5A) || (szInStr[0] >= 0x61) && (szInStr[0] <= 0x7B)));
}
//======================================================================

//======================================================================
BOOL CDynVars::AddDynVar(LPCSTR lpszName, const TpfnDynVarCallback pfnCallbackFunc)
{
	//Add a new dyn var

	if ((!lpszName) || (lpszName[0] != DYNVAR_PREFIX) || (!pfnCallbackFunc))
		return FALSE;

	//Check if already exists
	if (DynvarExists(lpszName, NULL))
		return FALSE;

	//Add to list
	dynvar_s dv;
	strcpy_s(dv.szVarName, lpszName);
	dv.pfnFunction = pfnCallbackFunc;
	vDynVars.push_back(dv);

	return TRUE;
}
//======================================================================

//======================================================================
VOID CDynVars::Clear(void)
{
	//Clear all dyn vars

	vDynVars.clear();
}
//======================================================================

//======================================================================
std::string CDynVars::HandleExpression(LPCSTR lpszExpression)
{
	//Handle an expression: Check for var names and replace them with the value

	std::string sRet(lpszExpression);

	if (!lpszExpression)
		return sRet;

	//Check for dyn var identifiers
	for (DWORD i = 0; i < vDynVars.size(); i++) {
		if (DynvarIsInExpression(lpszExpression, i)) {
			//Replace the string

			char szDVValueExpression[DYNVAR_MAX_VALUE_LEN + 1];
			
			vDynVars[i].pfnFunction(szDVValueExpression);
			szDVValueExpression[DYNVAR_MAX_VALUE_LEN] = 0;

			sRet = StringReplace(sRet, vDynVars[i].szVarName, szDVValueExpression);
		}
	}

	return sRet;
}
//======================================================================
