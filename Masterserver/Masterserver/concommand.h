#ifndef _CONCOMMAND_H
#define _CONCOMMAND_H

#include "includes.h"
#include "conparser.h"

/*
	CorvusChat Server (dnyCorvusChat) - Masterserver component

	Developed by Daniel Brendel

	Version: 0.2
	Visit: https://github.com/danielbrendel
	Mail dbrendel1988@gmail.com

	File: concommand.h: Console command manager interface
*/

//======================================================================
#define CCMD_DEFAULT_DESCRIPTION "No description given"
//======================================================================

//======================================================================
typedef void (*TCmdFunction)(CConParser* pParser);
//======================================================================

//======================================================================
struct concommand_s {
	char name[250];
	char description[250];
	TCmdFunction pfn;
};
//======================================================================

//======================================================================
class CConCommand {
private:
	std::vector<concommand_s> vCommands;

	bool CommandExists(const char* szCommand, unsigned int *uiCmdID);
public:
	CConCommand() { }
	~CConCommand() { }

	bool AddCommand(const char* szCommand, const char* szDescription, TCmdFunction pfnFunction);
	void Clear(void);

	bool HandleCommand(const char* szExpression);
};
//======================================================================

#endif