#include "ccesdk.h"

/* 
   Corvus Configuration Engine - SDK

   Coder: Daniel Brendel
   Version: 5
   Contact: dbrendel1988@gmail.com

   File: ccesdk.cpp: Data declarations and module handling functions

*/

//======================================================================
TCCE_SetCurrentPath CCE_SetCurrentPath;
TCCE_SetEventFuncs CCE_SetEventFuncs;
TCCE_About CCE_About;
TCCE_CalcTimer CCE_CalcTimer;
TCCE_RegisterKeyBind CCE_RegisterKeyBind;
TCCE_GetBindInfoByKey CCE_GetBindInfoByKey;
TCCE_GetBindInfo CCE_GetBindInfo;
TCCE_ExecCode CCE_ExecCode;
TCCE_CmdGetCount CCE_CmdGetCount;
TCCE_CmdGetValue CCE_CmdGetValue;
TCCE_ExecScript CCE_ExecScript;
TCCE_GetVarOrConstant CCE_GetVarOrConstant;

HMODULE hLibrary = NULL;
//======================================================================

//======================================================================
/*

  CCE_InitLibrary: Attach engine module and setup function pointers

*/
BOOL WINAPI CCE_InitLibrary(const char *lpszDLLPath)
{
	hLibrary = LOAD_MODULE(lpszDLLPath);
	if (hLibrary) {
		
		CCE_SetCurrentPath = (TCCE_SetCurrentPath)GET_PROC_ADDR("CCE_SetCurrentPath");
		if (!CCE_SetCurrentPath) {
			FREE_MODULE(hLibrary);
			return FALSE;
		}
		
		CCE_SetEventFuncs = (TCCE_SetEventFuncs)GET_PROC_ADDR("CCE_SetEventFuncs");
		if (!CCE_SetEventFuncs) {
			FREE_MODULE(hLibrary);
			return FALSE;
		}

		CCE_About = (TCCE_About)GET_PROC_ADDR("CCE_About");
		if (!CCE_About) {
			FREE_MODULE(hLibrary);
			return FALSE;
		}

		CCE_CalcTimer = (TCCE_CalcTimer)GET_PROC_ADDR("CCE_CalcTimer");
		if (!CCE_CalcTimer) {
			FREE_MODULE(hLibrary);
			return FALSE;
		}

		CCE_RegisterKeyBind = (TCCE_RegisterKeyBind)GET_PROC_ADDR("CCE_RegisterKeyBind");
		if (!CCE_RegisterKeyBind) {
			FREE_MODULE(hLibrary);
			return FALSE;
		}

		CCE_GetBindInfoByKey = (TCCE_GetBindInfoByKey)GET_PROC_ADDR("CCE_GetBindInfoByKey");
		if (!CCE_GetBindInfoByKey) {
			FREE_MODULE(hLibrary);
			return FALSE;
		}

		CCE_GetBindInfo = (TCCE_GetBindInfo)GET_PROC_ADDR("CCE_GetBindInfo");
		if (!CCE_GetBindInfo) {
			FREE_MODULE(hLibrary);
			return FALSE;
		}

		CCE_ExecCode = (TCCE_ExecCode)GET_PROC_ADDR("CCE_ExecCode");
		if (!CCE_ExecCode) {
			FREE_MODULE(hLibrary);
			return FALSE;
		}

		CCE_CmdGetCount = (TCCE_CmdGetCount)GET_PROC_ADDR("CCE_CmdGetCount");
		if (!CCE_CmdGetCount) {
			FREE_MODULE(hLibrary);
			return FALSE;
		}

		CCE_CmdGetValue = (TCCE_CmdGetValue)GET_PROC_ADDR("CCE_CmdGetValue");
		if (!CCE_CmdGetValue) {
			FREE_MODULE(hLibrary);
			return FALSE;
		}

		CCE_ExecScript = (TCCE_ExecScript)GET_PROC_ADDR("CCE_ExecScript");
		if (!CCE_ExecScript) {
			FREE_MODULE(hLibrary);
			return FALSE;
		}
		
		CCE_GetVarOrConstant = (TCCE_GetVarOrConstant)GET_PROC_ADDR("CCE_GetVarOrConstant");
		if (!CCE_GetVarOrConstant) {
			FREE_MODULE(hLibrary);
			return FALSE;
		}
	
		return TRUE;
	}
	
	return FALSE;
}

/*

  CCE_FreeLibrary: Detach engine module

*/
BOOL WINAPI CCE_FreeLibrary(void)
{
	return FREE_MODULE(hLibrary);
}
//======================================================================