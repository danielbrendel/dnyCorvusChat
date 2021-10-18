#include "conparser.h"

/*
	CorvusChat Server (dnyCorvusChat) - Masterserver component

	Developed by Daniel Brendel

	Version: 0.2
	Visit: https://github.com/danielbrendel
	Mail dbrendel1988@gmail.com

	File: conparser.cpp: Console expression parser implementation 
*/

//======================================================================
bool CConParser::Parse(const char *szExpression)
{
	//Parses a given command line

	if ((!szExpression) || (!szExpression[0]))
		return false;

	bool bInQuotes = false; //Ignore quotes, because quotes summarizes one parameter even with spaces.
	char szCurWord[250]; //Current parsed word

	memset(szCurWord, 0x00, sizeof(szCurWord));

	for (unsigned int i = 0; i <= strlen(szExpression); i++) { //Enumerate trough each character
			//Check for quotes
			if (szExpression[i] == '\"') {
				bInQuotes = !bInQuotes; //Toggle boolean when quote char encountered

				continue; //Ignore this character
			}

			//Check if a space char is found and the next following char is not a space char (to allow multiplace space chars following) and if not in quotes or end of string is reached
			if ((((szExpression[i] == ' ') && (szExpression[i+1] != ' ')) && (!bInQuotes)) || (!szExpression[i])) {
				//Add word to list
				char* pszNew = new char [250];
				if (!pszNew)
					return false;

				strcpy_s(pszNew, 250, szCurWord);

				ConArguments.push_back(pszNew);
				
				memset(szCurWord, 0x00, sizeof(szCurWord)); //Clear buffer to begin with copying

				continue; //Ignore the current char
		}

		//Append current character to the word holder only if we would not copy a unneccessary space char
		if (!((szExpression[i] == ' ') && (szExpression[i+1] == ' ')))
			strncat_s(szCurWord, &szExpression[i], sizeof(char));
	}

	return ConArguments.size() > 0;
}
//======================================================================

//======================================================================
void CConParser::CleanUp(void)
{
	//Free memory of the argument list

	for (unsigned int i = 0; i < ConArguments.size(); i++)
		delete [] ConArguments[i];

	ConArguments.clear();
}
//======================================================================

//======================================================================

DWORD CConParser::GetAmount(void)
{
	//Get amount of parsed items

	return (DWORD)ConArguments.size();
}
//======================================================================

//======================================================================
const char* CConParser::GetArgument(const DWORD dwID)
{
	//Get an item by ID

	if (dwID < ConArguments.size())
		return ConArguments[dwID];

	return NULL;
}
//======================================================================

//======================================================================
const char* CConParser::GetAll(void)
{
	//Create a string with all arguments

	static char szAllArguments[1024];

	memset(szAllArguments, 0x00, sizeof(szAllArguments));

	//Loop trough each entry
	for (unsigned int i = 1; i < ConArguments.size(); i++) {
		strcat_s(szAllArguments, ConArguments[i]); //Append argument
		
		if (i < ConArguments.size()-1) 
			strcat_s(szAllArguments, " "); //Append a space character if this argument is not the last one
	}

	return szAllArguments;
}
//======================================================================
