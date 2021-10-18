#include "vars.h"


/*
	CorvusChat Server (dnyCorvusChat) - Masterserver component

	Developed by Daniel Brendel

	Version: 0.2
	Visit: https://github.com/danielbrendel
	Mail dbrendel1988@gmail.com

	File: vars.cpp: Global variable declarations 
*/

//======================================================================
objects_s g_Objects;

bool g_bServerRunning = true;

unsigned long g_dwLocalAlias = 0;

BYTE g_ucLocalhostId = 0;

HANDLE g_hConThread = 0;
//======================================================================
