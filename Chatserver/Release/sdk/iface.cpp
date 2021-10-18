#include "sdk.h"
#include "vars.h"
#include "events.h"

//iface.cpp: Exported plugin interface functions

//======================================================================
BOOL CCSAPI CCS_PluginInit(PLUGINID pidPluginId, DWORD dwInterfaceVersion, struct enginefunctions_s *pEngineFunctions, struct plugin_eventtable_s *pEventTable, struct globalvars_s* pGlobalVariables)
{
	//Called when plugin is in loading progress

	g_pidPluginId = pidPluginId;

	//First check for important pointers
	if (pEngineFunctions == NULL) {
		//This is fatal!
		printf("Fatal Error: Engine functions pointer NULL: 0x%X\n", pEngineFunctions);
		return false;
	}

	if (pEventTable == NULL) {
		//This is fatal!
		printf("Fatal Error: Event table pointer NULL: 0x%X\n", pEventTable);
		return false;
	}

	if (pGlobalVariables == NULL) {
		//This isn't fatal, but nevertheless bad
		printf("Warning: Global Variables pointer NULL: 0x%X\n", pGlobalVariables);
	}

	printf("[%s] Interface version: v%d Plugin Version: %s\n", g_PluginInfos.plName, dwInterfaceVersion, g_PluginInfos.plVersion);
	if (dwInterfaceVersion != pEngineFunctions->VER_GetInterfaceVersion())
		printf("[%s] Interface version mismatch\n", g_PluginInfos.plName);

	//Assign address values
	g_pEngFuncs = pEngineFunctions;
	g_pGlobalVars = pGlobalVariables;

	//Assign event functions:
	memset(pEventTable, 0x00, sizeof(plugin_eventtable_s));
	pEventTable->events_post.OnServerInitialize = &OnServerInitialize_Post;
	
	return true;
}
//======================================================================

//======================================================================
VOID CCSAPI CCS_PluginInfos(plugininfos_s *pPluginInfos)
{
	//Called when plugin is loaded

	if (!pPluginInfos) {
		printf("Warning: plugin infos pointer NULL: 0x%X\n", pPluginInfos);
		return;
	}
	
	//Copy plugininfo buffer
	memcpy(pPluginInfos, &g_PluginInfos, sizeof(plugininfos_s));
}
//======================================================================

//======================================================================
VOID CCSAPI CCS_PluginFree(void)
{
	//Called when plugin gets unloaded
}
//======================================================================
