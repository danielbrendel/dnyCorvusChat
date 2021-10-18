#include "motd.h"
#include "utils.h"

/*
	CorvusChat Server (dnyCorvusChat) - Chatserver component

	Developed by Daniel Brendel

	Version: 0.5
	Visit: https://github.com/danielbrendel
	Mail dbrendel1988@gmail.com

	File: motd.cpp: MOTD manager implementation 
*/

//======================================================================
BOOL CMessageOfTheDay::AddLine(const char* szLine)
{
    //Add new line to the list

    if (!szLine)
		return FALSE;

	//Check for line string length
	if (!IsValidStrLen(szLine))
		return FALSE;

	//Allocate memory and set data

    char *pNew = new char[MAX_NETWORK_STRING_LENGTH];
	if (!pNew)
		return FALSE;

    strcpy_s(pNew, MAX_NETWORK_STRING_LENGTH, szLine);
    vLines.push_back(pNew);

	return TRUE;
}
//======================================================================

//======================================================================
DWORD CMessageOfTheDay::GetLineCount(void)
{
    //Return the amount of lines

    return (DWORD)vLines.size();
}
//======================================================================

//======================================================================
char* CMessageOfTheDay::GetLine(DWORD dwId)
{
    //Get a pointer to a line by ID

    if (dwId < vLines.size())
        return vLines[dwId];

    return NULL;
}
//======================================================================

//======================================================================
VOID CMessageOfTheDay::Clear(void)
{
	//Free memory and clear list

	//Free memory of each line
    for (DWORD i = 0; i < vLines.size(); i++)
        delete [] vLines[i];

	//Clear list
    vLines.clear();
}
//======================================================================

//======================================================================
BOOL CMessageOfTheDay::Reload(const char *szMotdFile)
{
    //Reload MOTD content from file

	bool bResult = false;

    std::ifstream hFile;
    hFile.open(szMotdFile); //Open file
    if (hFile.is_open()) { //File exists and has been opened
        char line[MAX_NETWORK_STRING_LENGTH];

        Clear(); //Clear old content

        while(!hFile.eof()) { //Walk trough lines
            hFile.getline(line, sizeof(line)); //Read current line
            if ((line[0] == '/') && (line[1] == '/')) //Ignore comments
                continue;

            AddLine(line); //Add new line
        }

        hFile.close(); //Done

		bResult = true;
    }

	return bResult;
}

//======================================================================