#include "events.h"
#include "vars.h"

//Event functions

//======================================================================
plugin_result CCSAPI OnServerInitialize_Post(bool* pbRetValue)
{
	//Called after engine has processed its stuff
	
	g_pEngFuncs->OUT_ConsolePrint("OnServerInitialize_Post called\n");

	return PLUGIN_CONTINUE;
}
//======================================================================
