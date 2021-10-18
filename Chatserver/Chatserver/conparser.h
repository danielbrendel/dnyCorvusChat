#ifndef _CONPARSER_H
#define _CONPARSER_H

#include "includes.h"

/*
	CorvusChat Server (dnyCorvusChat) - Chatserver component

	Developed by Daniel Brendel

	Version: 0.5
	Visit: https://github.com/danielbrendel
	Mail dbrendel1988@gmail.com

	File: conparser.h: Console expression parser interface 
*/

//======================================================================
class CConParser {
private:
	std::vector<char*> ConArguments;
public:
	bool Parse(const char *szExpression);
	void CleanUp(void);

	DWORD GetAmount(void);
	const char *GetArgument(const DWORD dwID);

	const char* GetAll(void);
};
//======================================================================

#endif