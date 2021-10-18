#include "concommand.h"

/*
	CorvusChat Server (dnyCorvusChat) - Masterserver component

	Developed by Daniel Brendel

	Version: 0.2
	Visit: https://github.com/danielbrendel
	Mail dbrendel1988@gmail.com

	File: concommand.cpp: Console command manager implementation  
*/

//======================================================================
bool CConCommand::CommandExists(const char* szCommand, unsigned int *uiCmdID)
{
	//Check if a command already exists

	if (!szCommand)
		return false;

	//Search in list for the given command
	for (unsigned int i = 0; i < vCommands.size(); i++) {
		if (strcmp(vCommands[i].name, szCommand)==0) { //Check if expression matches
			//Save registration ID if wanted
			if (uiCmdID)
				*uiCmdID = i;

			return true;
		}
	}

	return false;
}
//======================================================================

//======================================================================
bool CConCommand::AddCommand(const char* szCommand, const char* szDescription, TCmdFunction pfnFunction)
{
	//Add a new command to list
	
	if ((!szCommand) || (!pfnFunction))
		return false;

	//Check if command already exists
	if (CommandExists(szCommand, NULL))
		return false;

	//Set data
	concommand_s cmd;
	strcpy_s(cmd.name, szCommand);
	strcpy_s(cmd.description, (szDescription) ? szDescription : CCMD_DEFAULT_DESCRIPTION);
	cmd.pfn = pfnFunction;

	//Add to list
	vCommands.push_back(cmd);

	return true;
}
//======================================================================

//======================================================================
void CConCommand::Clear(void)
{
	//Clear list data

	vCommands.clear();
}
//======================================================================

//======================================================================
bool CConCommand::HandleCommand(const char* szExpression)
{
	//Parse a command expression

	if (!szExpression)
		return false;

	CConParser parser;

	//Parse expression
	if (!parser.Parse(szExpression))
		return false;

	//Check if a command with this name exists
	unsigned int uiCmdID;
	if (CommandExists(parser.GetArgument(0), &uiCmdID)) {
		//Call event function
		vCommands[uiCmdID].pfn(&parser);

		parser.CleanUp();

		return true;
	} else {
		printf("Unknown command: \'%s\'\n", szExpression);
	}

	parser.CleanUp();

	return false;
}
//======================================================================
