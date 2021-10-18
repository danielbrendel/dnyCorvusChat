#include "vars.h"

//Global data objects

//======================================================================
plugininfos_s g_PluginInfos = {
	"Name",
	"Version info",
	"Author",
	"Description",
	"Contact/Support data",
};
//======================================================================

//======================================================================
enginefunctions_s *g_pEngFuncs; //A pointer to the engine provided function table
globalvars_s *g_pGlobalVars; //A pointer to global server variables
PLUGINID g_pidPluginId; //The ID number of this plugin
//======================================================================
