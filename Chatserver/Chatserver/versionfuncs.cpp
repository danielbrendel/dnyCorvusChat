#include "versionfuncs.h"
#include "vars.h"

/*
	CorvusChat Server (dnyCorvusChat) - Chatserver component

	Developed by Daniel Brendel

	Version: 0.5
	Visit: https://github.com/danielbrendel
	Mail dbrendel1988@gmail.com

	File: versionfuncs.cpp: Version function implementations 
*/

//======================================================================
DWORD SERVERVERSION(void)
{
	//Returns the current server version

	unsigned short sver[2] = {VF_SERVERVERSION};

	return *(DWORD*)sver;
}
//======================================================================

//======================================================================
DWORD IFACEVERSION(void)
{
	//Returns the current engine interface version

	unsigned short iver[2] = {VF_INTERFACEVERSION};

	return *(DWORD*)iver;
}
//======================================================================