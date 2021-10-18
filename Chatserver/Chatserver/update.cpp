#include "update.h"
#include "vars.h"
#include "utils.h"
#include "versionfuncs.h"
#include <ShlObj.h>

/*
	CorvusChat Server (dnyCorvusChat) - Chatserver component

	Developed by Daniel Brendel

	Version: 0.5
	Visit: https://github.com/danielbrendel
	Mail dbrendel1988@gmail.com

	File: update.cpp: Update manager implementation 
*/

//======================================================================
CB_RET CUpdateScriptParser::ExpressionCallback(LPCSTR lpszExpression)
{
	//Root event function
	
	CB_RET retVal = pEventFunction(lpszExpression); //Pass to callback function

	if (strcmp(lpszExpression, UPDATE_SPECIAL_VERSION_STRING)) { //Check if first expression is not the version expression
		if (!bFirstExpression) { //Check if current expression is the first handled expression
			return CBR_ABORT; //Return a failure indicator since version expression must be the first handled expression
		}
	}

	//Set flag to true after the part where to handle the expressions
	if (!bFirstExpression)
		bFirstExpression = true;

	return retVal;
}
//======================================================================

//======================================================================
DWORD CUpdateScriptParser::GetArgumentCount(VOID)
{
	//Get amount of strings

	return (DWORD)vStrings.size();
}
//======================================================================

//======================================================================
CHAR* CUpdateScriptParser::GetArgumentValue(DWORD dwId)
{
	//Get string by ID

	if (dwId < vStrings.size())
		return vStrings[dwId];

	return NULL;
}
//======================================================================

//======================================================================
BOOL CUpdateScriptParser::PrepareLine(LPCSTR lpszLine, LPSTR lpszOut, DWORD dwOutBufLen)
{
	//Prepare a line for parsing: Remove multiple TABs/spaces and ignore comments

	if ((!lpszLine) || (!lpszOut) || (!dwOutBufLen))
		return FALSE;

	bool bInQuotes = false;

	//Clear buffer
	memset(lpszOut, 0x00, dwOutBufLen);

	for (DWORD i = 0; i < (DWORD)strlen(lpszLine); i++) { //Enumerate through each char
		//Check for quotes
		if (lpszLine[i] == '"') {
			bInQuotes = !bInQuotes; //Toggle flag
		}
		
		if (!bInQuotes) {
			//Check for multiple TAB/spaces between each expression and ignore them
			if ((lpszLine[i] == ' ') && ((lpszLine[i + 1] == ' ') || (lpszLine[i + 1] == 0x09))) {
				continue;
			} else if ((lpszLine[i] == 0x09) && ((lpszLine[i + 1] == 0x09) || (lpszLine[i + 1] == ' '))) {
				continue;
			}

			//Check for comments and break out if exist
			else if ((lpszLine[i] == '/') && (lpszLine[i + 1] == '/')) {
				break;
			} 
		}
		
		strncat_s(lpszOut, dwOutBufLen, &lpszLine[i], 1); //Append current char to buffer
	}

	return TRUE;
}
//======================================================================

//======================================================================
BOOL CUpdateScriptParser::ParseLine(LPCSTR lpszLine)
{
	//Parse script line

	if (!lpszLine)
		return FALSE;

	char szNewLine[UPDATE_MAX_SCRIPT_LINE_LENGTH] = {0};
	char szExpression[UPDATE_MAX_SCRIPT_LINE_LENGTH] = {0};
	bool bInQuotes = false;

	//Prepare line
	if (!PrepareLine(lpszLine, szNewLine, sizeof(szNewLine)))
		return FALSE;
	
	//Ignore empty lines
	if (!szNewLine[0])
		return TRUE;

	//Parse expressions
	for (DWORD i = 0; i <= (DWORD)strlen(szNewLine); i++) { //Enumerate through each char
		//Check for quotes
		if (szNewLine[i] == '"') {
			bInQuotes = !bInQuotes; //Toggle flag
			continue; //Ignore this char
		}

		if (((szNewLine[i] == ' ') || (szNewLine[i] == 0x09) || (szNewLine[i] == 0x00)) && (!bInQuotes)) { //Split by TABs/spaces/zt
			//Allocate new buffer
			char* pBuf = new char[UPDATE_MAX_SCRIPT_LINE_LENGTH];
			if (!pBuf)
				return FALSE;

			//Set data
			memset(pBuf, 0x00, UPDATE_MAX_SCRIPT_LINE_LENGTH);
			strcpy_s(pBuf, UPDATE_MAX_SCRIPT_LINE_LENGTH, szExpression);

			vStrings.push_back(pBuf); //Add current string to list

			memset(szExpression, 0x00, sizeof(szExpression)); //Clear buffer

			continue; //Ignore current char
		}

		strncat_s(szExpression, &szNewLine[i], 1); //Append current char to buffer
	}

	CB_RET retVal;

	//Call event function
	if (vStrings.size() > 0) {
		retVal = ExpressionCallback(vStrings[0]);

		if (retVal == CBR_UNHANDLED)
			ConsolePrint(FOREGROUND_RED, CMD_STDOUT_PREFIX "Unhandled download script expression: \"%s\"\n", vStrings[0]);
	}

	//Free memory and clear list

	for (DWORD i = 0; i < (DWORD)vStrings.size(); i++)
		delete [] vStrings[i];

	vStrings.clear();

	return retVal != CBR_ABORT;
}
//======================================================================

//======================================================================
BOOL CUpdateScriptParser::ParseScript(LPCSTR lpszScriptFile)
{
	//Parse a script

	if (!lpszScriptFile)
		return FALSE;

	if (!pEventFunction)
		return FALSE;

	std::ifstream hFile;
	hFile.open(lpszScriptFile); //Open file
	if (hFile.is_open()) { //File stream is available
		char szLine[MAX_PATH];

		bFirstExpression = false; //This must be set to false before parsing the lines. It is used to determine the first event function call for the current script

		while (!hFile.eof()) { //While not at end of file
			hFile.getline(szLine, sizeof(szLine), '\n');

			if (!ParseLine(szLine)) {
				hFile.close();

				return FALSE;
			}
		}

		hFile.close(); //Close file

		return TRUE;
	}

	return FALSE;
}
//======================================================================

//======================================================================
VOID CUpdateScriptParser::SetEventFunction(THandleUpdateExpressions pfnEventFunction)
{
	//Set event function which to handle the expressions

	if (!pfnEventFunction)
		return;

	pEventFunction = pfnEventFunction;
}
//======================================================================

//======================================================================
CB_RET frEventMethod(LPCSTR lpszExpression)
{
	//Friend function as event function

	if (strcmp(lpszExpression, UPDATE_SPECIAL_VERSION_STRING)==0) { //Version identifier. Note: This must be the first expression
		if (!g_Objects.Update.bFirstExpression) { //Handle this expression only once for a script
			//Check for version mismatch
			
			char* szArg = g_Objects.Update.GetArgumentValue(1);
			if (szArg) {
				DWORD dwServerVersion = SERVERVERSION();

				char szVer[20];
				sprintf_s(szVer, "%d.%d", *(WORD*)&dwServerVersion, *(WORD*)((DWORD_PTR)&dwServerVersion + 2));

				if (strcmp(szVer, szArg)==0) {
					return CBR_ABORT;
				} else {
					ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "[Update] New version available: %s (old version: %s)\n", szArg, szVer);
				}
			}
		}

		return CBR_HANDLED;
	} else if (strcmp(lpszExpression, "download")==0) {
		if (!g_Objects.Update.bFirstExpression) {
			return CBR_ABORT; //Can't handle command as first command
		}

		char* szArg1 = g_Objects.Update.GetArgumentValue(1);
		char* szArg2 = g_Objects.Update.GetArgumentValue(2);

		if (szArg1) {
			//Download specified object

			ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "[Update] Downloading resource: \"%s\" (%s)... ", szArg1, szArg2);
			
			if (strcmp(szArg1, UPDATE_SPECIAL_MAIN_EXECUTABLE)==0) {
				if (!g_Objects.Update.UpdateResource(NULL, TRUE)) {
					ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, "Failed\n");
					return CBR_ABORT; //Main executable is important by default
				} else {
					ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, "Done\n");
				}
			} else {
				if ((szArg2) && ((strcmp(szArg2, UPDATE_SPECIAL_DLTYPE_IMPORTANT)==0) || (strcmp(szArg2, UPDATE_SPECIAL_DLTYPE_OPTIONAL)==0))) {
					if (!g_Objects.Update.UpdateResource(szArg1, FALSE)) {
						ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, "Failed\n");

						//Check if object is not optional
						if (strcmp(szArg2, UPDATE_SPECIAL_DLTYPE_IMPORTANT)==0)
							return CBR_ABORT;
					} else {
						ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, "Done\n");
					}
				}
			}
		}

		return CBR_HANDLED;
	} else if (strcmp(lpszExpression, "delete")==0) {
		if (!g_Objects.Update.bFirstExpression) {
			return CBR_ABORT; //Can't handle command as first command
		}

		char* szArg = g_Objects.Update.GetArgumentValue(1);
		if (szArg) {
			//Delete specified object

			ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, CMD_STDOUT_PREFIX "[Update] Deleting object: \"%s\"... ", szArg);

			char szFullDelPath[MAX_PATH];
			sprintf_s(szFullDelPath, "%s%s", g_GlobalVars.szAppPath, szArg);

			(DeleteFileObject(szFullDelPath)) ? ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, "Done\n"): ConsolePrint(CONSOLE_ATTRIBUTE_DEFAULT, "Failed\n");
		}

		return CBR_HANDLED;
	}

	return CBR_UNHANDLED;
}
//======================================================================

//======================================================================
BOOL CUpdate::OpenInternetHandle(void)
{
	//Create internet handle

	return ((hInet = InternetOpenA(PROGRAM_NAME, 0, NULL, 0, 0)) != NULL);
}
//======================================================================

//======================================================================
VOID CUpdate::CloseInternetHandle(void)
{
	//Closes internet handle

	if (hInet) {
		//Close handle and reset variable

		InternetCloseHandle(hInet);

		hInet = NULL;
	}
}
//======================================================================

//======================================================================
void FatalError(const char* szErrMsg, ...);
BOOL CUpdate::ExecUpdateBatch(LPCSTR lpszMainProgramFile, LPCSTR lpszBackupFile)
{
	//Write a batch file, which performs the last update steps of updating the main executable program file

	if ((!lpszMainProgramFile) || (!lpszBackupFile))
		return FALSE;

	char szWriteContent[1024] = {0};
	char szBatFile[MAX_PATH];
	char szUpdateScript[MAX_PATH];
	
	static char caBatSelfDel[] = {'%', '0', 0}; //%0 stands for the current executed batch script file

	//Get own process ID
	DWORD dwOwnPID = GetCurrentProcessId();
	if (!dwOwnPID)
		FatalError("Fatal: GetCurrentProcessId failed");

	//Create the file
	sprintf_s(szBatFile, "%s" UPDATE_BATCH_FILENAME, g_GlobalVars.szAppPath);
	HANDLE hBatchFile = CreateFileA(szBatFile, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0);
	if (hBatchFile == INVALID_HANDLE_VALUE)
		return FALSE;

	//Format update script file name
	sprintf_s(szUpdateScript, "%s%s", g_GlobalVars.szAppPath, UPDATE_LOCAL_INFO_FILE);

	//Format batch code string
	sprintf_s(szWriteContent, UPDATE_BATCH_CONTENT, dwOwnPID, lpszBackupFile, lpszMainProgramFile, lpszBackupFile, ExtractFileName((char*)lpszMainProgramFile), szUpdateScript, caBatSelfDel);

	szWriteContent[sizeof(szWriteContent)-1] = 0;

	//Write batch script content to file
	DWORD dwBytesWritten = 0;
	if ((!WriteFile(hBatchFile, szWriteContent, (DWORD)strlen(szWriteContent), &dwBytesWritten, NULL)) || (dwBytesWritten != strlen(szWriteContent))) {
		CloseHandle(hBatchFile);
		return FALSE;
	}

	CloseHandle(hBatchFile);
	
	//Run batch file
	return (DWORD)ShellExecuteA(0, "open", szBatFile, NULL, g_GlobalVars.szAppPath, SW_SHOWNORMAL) > 32;
}
//======================================================================

//======================================================================
BOOL CUpdate::QueryUpdateInfo(VOID)
{
	//Check if a new program version is available

	BOOL bResult = FALSE;

	char szUpdateFile[MAX_PATH];
	char szLocalUpdateFile[MAX_PATH];

	if (!hInet)
		return FALSE;

	//Format pathes
	sprintf_s(szUpdateFile, "%s" UPDATE_WEBSPACE_INFO_FILE, g_GlobalVars.pUpdateURL->szValue);
	sprintf_s(szLocalUpdateFile, "%s" UPDATE_LOCAL_INFO_FILE, g_GlobalVars.szAppPath);

	WebDirToWinDir(szLocalUpdateFile);

	//Open connection to URL with path
	HINTERNET hUrl = InternetOpenUrlA(hInet, szUpdateFile, NULL, 0, INTERNET_FLAG_NO_CACHE_WRITE, 0);
	if (hUrl) {
		//Create file on disc
		HANDLE hFile = CreateFileA(szLocalUpdateFile, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0);
		if (hFile != INVALID_HANDLE_VALUE) {
			DWORD dwBytesRead = -1;
			DWORD dwBytesWritten = 0;

			BYTE ucBuffer[1024];

			//Read file from web resource

			while (dwBytesRead != 0) { //End of web resource data is reached if InternetReadFile returns TRUE and dwBytesRead is set to zero
				memset(ucBuffer, 0x00, sizeof(ucBuffer));

				if (!InternetReadFile(hUrl, ucBuffer, sizeof(ucBuffer), &dwBytesRead)) {
					CloseHandle(hFile);
					InternetCloseHandle(hUrl);
					return bResult;
				} else {
					if (!WriteFile(hFile, ucBuffer, sizeof(ucBuffer), &dwBytesWritten, NULL)) {
						CloseHandle(hFile);
						InternetCloseHandle(hUrl);
						return bResult;
					}
				}
			}
			
			CloseHandle(hFile);

			//Attempt to parse script
			SetEventFunction(&frEventMethod);
			bResult = ParseScript(szLocalUpdateFile);
			
			//Delete update script file
			DeleteFileA(szLocalUpdateFile);
		}

		InternetCloseHandle(hUrl); //Close connection
	}

	return bResult;
}
//======================================================================

//======================================================================
BOOL CUpdate::UpdateResource(LPCSTR pszResourceName, BOOL bIsProgramMainExecutable)
{
	//Update a specific resource

	if (!hInet)
		return FALSE;

	//Get full file name of own process
	char szProgramFile[MAX_PATH];
	if (!GetModuleFileNameA(NULL, szProgramFile, sizeof(szProgramFile)))
		return FALSE;

	//Format resource path
	char szResourcePath[MAX_PATH];
	sprintf_s(szResourcePath, "%s" UPDATE_WEBSPACE_PATH "%s", g_GlobalVars.pUpdateURL->szValue, (bIsProgramMainExecutable) ? ExtractFileName(szProgramFile) : pszResourceName);

	if (bIsProgramMainExecutable)
		strcat_s(szResourcePath, ".dat");

	//Open connection to resource
	HINTERNET hUrl = InternetOpenUrlA(hInet, szResourcePath, NULL, 0, 16, 0);
	if (hUrl) {
		char szLocalFile[MAX_PATH];
		char szLocalTmpFile[MAX_PATH];
		char szLocalDir[MAX_PATH];
		char szResBuf[MAX_PATH];
		char szReadBuf[2048];

		DWORD dwBytesRead;
		DWORD dwBytesWritten;

		if (pszResourceName) { //Do this only if not the main program executable is desired
			strcpy_s(szResBuf, pszResourceName);
			WebDirToWinDir(szResBuf);
			ExtractFilePath(szResBuf);
			sprintf_s(szLocalDir, "%s%s", g_GlobalVars.szAppPath, szResBuf);

			//Create folder structure if not already exists

			if (!DirectoryExists(szLocalDir)) {
				wchar_t* pWC = CharToWChar(szLocalDir);
				if (!pWC) {
					InternetCloseHandle(hUrl);
					return FALSE;
				}

				if (SHCreateDirectory(NULL, pWC) != ERROR_SUCCESS) {
					delete [] pWC;
					InternetCloseHandle(hUrl);
					return FALSE;
				}

				delete [] pWC;
			}
		}

		sprintf_s(szLocalTmpFile, "%s%s__.BAK", g_GlobalVars.szAppPath, (bIsProgramMainExecutable) ? ExtractFileName(szProgramFile) : pszResourceName);
		WebDirToWinDir(szLocalTmpFile);

		//Create local file
		HANDLE hFile = CreateFileA(szLocalTmpFile, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0);
		if (hFile == INVALID_HANDLE_VALUE) {
			InternetCloseHandle(hUrl);
			return FALSE;
		}

		//Read every next buffer from web resource until the end of the resource
		while (InternetReadFile(hUrl, szReadBuf, sizeof(szReadBuf), &dwBytesRead)) {
			if (dwBytesRead == 0)
				break;

			//Write current buffer to local file at current file pointer position
			WriteFile(hFile, szReadBuf, dwBytesRead, &dwBytesWritten, NULL);
		}

		//Close handles
		CloseHandle(hFile);
		InternetCloseHandle(hUrl);

		//Do renames or handle if resource is the program main executable
		if (!bIsProgramMainExecutable) {
			sprintf_s(szLocalFile, "%s%s", g_GlobalVars.szAppPath, pszResourceName);
			WebDirToWinDir(szLocalFile);

			DeleteFileA(pszResourceName);
			MoveFileA(szLocalTmpFile, szLocalFile);
		} else {
			return ExecUpdateBatch(szProgramFile, szLocalTmpFile); //The batch script will continue the update process of the main executable program
		}

		DeleteFileA(szLocalTmpFile);

		return TRUE;
	}

	return FALSE;
}
//======================================================================
