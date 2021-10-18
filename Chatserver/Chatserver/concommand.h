#ifndef _CONCOMMAND_H
#define _CONCOMMAND_H

#include "includes.h"
#include "conparser.h"

/*
	CorvusChat Server (dnyCorvusChat) - Chatserver component

	Developed by Daniel Brendel

	Version: 0.5
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

	size_t GetCmdAmount(void);
	BOOL GetConCommandData(size_t dwCmdId, concommand_s* pConCmdDataOut);

	bool AddCommand(const char* szCommand, const char* szDescription, TCmdFunction pfnFunction);
	bool DelCommand(const char* szCommand);

	void Clear(void);

	bool HandleCommand(const char* szExpression);
};
//======================================================================

#endif