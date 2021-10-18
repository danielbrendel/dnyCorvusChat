#include "plugins.h"
#include "versionfuncs.h"
#include "vars.h"

/*
	CorvusChat Server (dnyCorvusChat) - Chatserver component

	Developed by Daniel Brendel

	Version: 0.5
	Visit: https://github.com/danielbrendel
	Mail dbrendel1988@gmail.com

	File: plugins.cpp: Plugin manager implementation 
*/

//======================================================================
BOOL CPlugins::LoadPlugin(const char *szLibrary, bool bManually)
{
	//Load a plugin

	if (!szLibrary)
		return FALSE;

	//Allocate memory and set data

	plugin_s* pNew = new plugin_s;
	if (!pNew)
		return FALSE;

	pNew->bPaused = false;
	strcpy_s(pNew->name, szLibrary);

	pNew->hModule = LoadLibraryA(szLibrary); //Load DLL module into memory
	if (!pNew->hModule) {
		delete pNew;
		return FALSE;
	}
	
	//Get exported interface function pointer: CCS_PluginInit
	TPluginInit pfnInit = (TPluginInit)GetProcAddress(pNew->hModule, "CCS_PluginInit");
	if (!pfnInit) {
		delete pNew;
		FreeLibrary(pNew->hModule);
		return FALSE;
	}

	//Get exported interface function pointer: CCS_PluginInfos
	TPluginInfos pfnInfos = (TPluginInfos)GetProcAddress(pNew->hModule, "CCS_PluginInfos");
	if (!pfnInfos) {
		delete pNew;
		FreeLibrary(pNew->hModule);
		return FALSE;
	}

	//Call init callback function
	if (!pfnInit((PLUGINID)vPlugins.size(), IFACEVERSION(), &g_EngineFuncs, &pNew->evttable, &g_GlobalVars)) {
		delete pNew;
		FreeLibrary(pNew->hModule);
		return FALSE;
	}

	memset(&pNew->infos, 0x00, sizeof(pNew->infos));

	//Call infos callback function
	pfnInfos(&pNew->infos);

	//If loaded manually call server initialize event functions
	if (bManually) {
		if (pNew->evttable.events_pre.OnServerInitialize)
			pNew->evttable.events_pre.OnServerInitialize(NULL);

		if (pNew->evttable.events_post.OnServerInitialize)
			pNew->evttable.events_post.OnServerInitialize(NULL);
	}

	//Add data to list
	vPlugins.push_back(pNew);

	return TRUE;
}
//======================================================================

//======================================================================
BOOL CPlugins::RemovePlugin(PLUGINID dwPluginId)
{
    //Remove a plugin by ID

    if (dwPluginId < vPlugins.size()) {
		//Get unload callback function address and call it if available
		TPluginFree pfnFree = (TPluginFree)GetProcAddress(vPlugins[dwPluginId]->hModule, "CCS_PluginFree");
        if (pfnFree != NULL) {
            pfnFree();
        }

        //Free library from process
        FreeLibrary(vPlugins[dwPluginId]->hModule);

        //Free memory and remove from list
		delete vPlugins[dwPluginId];
        vPlugins.erase(vPlugins.begin() + dwPluginId);

		return TRUE;
    }

	return FALSE;
}
//======================================================================

//======================================================================
PLUGINID CPlugins::GetPluginCount(void)
{
    //Return the amount of loaded plugins

    return (PLUGINID)vPlugins.size();
}
//======================================================================

//======================================================================
plugin_s *CPlugins::GetPluginById(PLUGINID dwPluginId)
{
    //Return a pointer to plugin data by ID

    if (dwPluginId < vPlugins.size())
        return vPlugins[dwPluginId];

    return NULL;
}
//======================================================================

//======================================================================
BOOL CPlugins::SetPauseStatus(PLUGINID dwPluginId, bool bValue)
{
    //Set pause status of a plugin

    if (dwPluginId < vPlugins.size()) {
        vPlugins[dwPluginId]->bPaused = bValue;

		return TRUE;
	}

	return FALSE;
}
//======================================================================

//======================================================================
BOOL CClientToPluginMsg::HandlerExists(char* cHandlerSig)
{
	//Check if a handler with this signature exists

	if (!cHandlerSig)
		return FALSE;

	//Loop trough list
	for (size_t i = 0; i < vHandlers.size(); i++) {
		if ((vHandlers[i]) && (!strcmp(vHandlers[i]->cHandlerSig, cHandlerSig))) //Compare handler signatures
			return TRUE;
	}

	return FALSE;
}
//======================================================================

//======================================================================
BOOL CClientToPluginMsg::AddHandler(ctpm_handler_s *pHandler)
{
	//Add new client-to-plugin-msg handler

	if (!pHandler)
		return FALSE;

	//Check if already a handler with this signature exists
	if (HandlerExists(pHandler->cHandlerSig))
		return FALSE;

	//Allocate memory and set data

	ctpm_handler_s* pNew = new ctpm_handler_s;
	if (!pNew)
		return FALSE;

	memcpy(pNew, pHandler, sizeof(ctpm_handler_s));

	//Add the new handler to list
	vHandlers.push_back(pNew);

	return TRUE;
}
//======================================================================

//======================================================================
BOOL CClientToPluginMsg::RemoveHandler(char* cHandlerSig)
{
	//Remove an existing client-to-plugin-msg handler

	if (!cHandlerSig)
		return FALSE;

	//Check if handler exists
	if (!HandlerExists(cHandlerSig))
		return FALSE;

	//Loop trough list
	for (size_t i = 0; i < vHandlers.size(); i++) { 
		if ((vHandlers[i]) && (!strcmp(vHandlers[i]->cHandlerSig, cHandlerSig))) { //Compare handler signatures
			delete vHandlers[i]; //Free allocated memory

			vHandlers.erase(vHandlers.begin() + i); //Remove list entry

			return TRUE;
		}
	}

	return FALSE;
}
//======================================================================

//======================================================================
BOOL CClientToPluginMsg::CallHandler(char* cHandlerSig, clientinfo_s* pClient, const void* lpvBuffer, unsigned int dwBufSize)
{
	//Check if a handler for this signature exists and call event function

	if ((!cHandlerSig) || (!pClient))
		return FALSE;

	//Loop trough list
	for (unsigned int i = 0; i < vHandlers.size(); i++) {
		if ((vHandlers[i]) && (!strcmp(vHandlers[i]->cHandlerSig, cHandlerSig))) { //Compare handler signatures
			//Check function pointer
			if (!vHandlers[i]->TClientMsgHandler)
				return FALSE;

			//Call event function
			vHandlers[i]->TClientMsgHandler(pClient, lpvBuffer, dwBufSize);

			return TRUE;
		}
	}

	return FALSE;
}
//======================================================================

//======================================================================
VOID CClientToPluginMsg::Clear(void)
{
	//Clear all memory and data

	//Free memory
	for (unsigned int i = 0; i < vHandlers.size(); i++)
		delete vHandlers[i];

	//Clear list
	vHandlers.clear();
}
//======================================================================
