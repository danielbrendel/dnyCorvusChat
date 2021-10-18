#include "utils.h"

/*
	CorvusChat Server (dnyCorvusChat) - Chatserver component

	Developed by Daniel Brendel

	Version: 0.5
	Visit: https://github.com/danielbrendel
	Mail dbrendel1988@gmail.com

	File: utils.cpp: Utility function implementations 
*/

//======================================================================
BOOL IsDestinationReachable(LPCSTR lpszServer)
{
	//Check if server is available. This function is used to check if a TeamSpeak 3 server is online. Note: It checks only for the internet address, not the port.

	if (!lpszServer)
		return FALSE;

	in_addr inaddr;
	inaddr.S_un.S_addr = inet_addr(lpszServer);

	return gethostbyaddr((char*)&inaddr, sizeof(in_addr), AF_INET) != NULL;
}
//======================================================================

//======================================================================
inline unsigned int GetHoursByMS(unsigned int dwMS)
{
	//Calculate the value of hours by milliseconds

	#define MS_HOUR 3600000

	if (dwMS < MS_HOUR) //A hour did not pass yet
		return 0;

	return dwMS / MS_HOUR; //Get the hours within the defined milliseconds
}
//======================================================================

//======================================================================
const char* AddressToString(const unsigned long ulIPAddress)
{
	//Setup string with the IP-Address

	if (!ulIPAddress)
		return NULL;

	static char str[15];
	memset(str, 0x00, sizeof(str));

	sprintf_s(str, "%d.%d.%d.%d", *(BYTE*)&ulIPAddress, *(BYTE*)((DWORD)&ulIPAddress + 1), *(BYTE*)((DWORD)&ulIPAddress + 2), *(BYTE*)((DWORD)&ulIPAddress + 3));

	return &str[0];
}
//======================================================================

//======================================================================
bool FileExists(const char* szFileName)
{
	//Check if a file exists on disc

	if (!szFileName)
		return false;

	HANDLE hFile = CreateFileA(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
		return false;

	CloseHandle(hFile);

	return true;
}
//======================================================================

//======================================================================
void ExtractFilePath(char *szFileName)
{
	//Extract file path from a full file name

	if (!szFileName)
		return;

	for (unsigned int i = (unsigned int)strlen(szFileName); i > 3; i--) {
		if ((szFileName[i] == '\\') || (szFileName[i] == '/'))
			break;

		szFileName[i] = '\0';
	}
}
//======================================================================

//======================================================================
void WebDirToWinDir(char* szString)
{
	//Convert / chars to \ chars
	if (!szString)
		return;

	for (unsigned int i = 0; i < strlen(szString); i++) {
		if (szString[i] == '/')
			szString[i] = '\\';
	}
}
//======================================================================

//======================================================================
wchar_t *CharToWChar(char* szString)
{
	//Convert char string to wchar string
	if (!szString)
		return NULL;

    wchar_t* pWideString = new wchar_t[strlen(szString)+1];
	if (!pWideString)
		return NULL;

    mbstowcs(pWideString, szString, strlen(szString)+1);

    return pWideString;
}
//======================================================================

//======================================================================
char *ExtractFileName(char *filePath)
{
	//This function extracts the file name of a file path.

	if (!filePath)
		return NULL;

	static char fn[250], rev[250];
	memset(fn, 0x00, sizeof(fn));
	memset(rev, 0x00, sizeof(rev));

	strcpy_s(rev, "");

	for (unsigned int i = (unsigned int)strlen(filePath); i >= 0; i--) {
		if (filePath[i] == '\\') break;

		strncat_s(rev, &filePath[i], 1);
	}

	for (unsigned int g = 0; g <= strlen(rev); g++) {
		fn[g] = rev[strlen(rev)-1-g];
	}

	return fn;
}
//======================================================================

//======================================================================
const char* GetCurrentDateTime(void)
{
	//Get current time and date

	static char szTimeDate[100];

	time_t tim = time(NULL);
	if (tim != -1) {
		struct tm mTime;
		if (!localtime_s(&mTime, &tim)) {
			strftime(szTimeDate, sizeof(szTimeDate), "%Y-%m-%d %H:%M", &mTime);

			return szTimeDate;
		}
	}

	return NULL;
}
//======================================================================

//======================================================================
const char* GetCurrentDate(void)
{
	//Get current date

	static char szDate[100];

	time_t tim = time(NULL);
	if (tim != -1) {
		struct tm mTime;
		if (!localtime_s(&mTime, &tim)) {
			strftime(szDate, sizeof(szDate), "%Y-%m-%d", &mTime);

			return szDate;
		}
	}

	return NULL;
}
//======================================================================

//======================================================================
const char* GetCurrentTime(void)
{
	//Get current time

	static char szTime[100];

	time_t tim = time(NULL);
	if (tim != -1) {
		struct tm mTime;
		if (!localtime_s(&mTime, &tim)) {
			strftime(szTime, sizeof(szTime), "%H:%M", &mTime);

			return szTime;
		}
	}

	return NULL;
}
//======================================================================

//======================================================================
BOOL UpdateServerComponent(VOID)
{
	//Update the server component

	BOOL bResult = FALSE;

	if (g_Objects.Update.OpenInternetHandle()) { //Initialize update component
		bResult = g_Objects.Update.QueryUpdateInfo(); //Attempt to update

		g_Objects.Update.CloseInternetHandle(); //Clear handle
	}

	return bResult;
}
//======================================================================

//======================================================================
char* CreateRandomString(BYTE bAsciiStart, BYTE bAsciiEnd, unsigned short usStrSize)
{
	//Create a random string with specified length

	if (!usStrSize)
		return NULL;

	//Allocate memory
	char* pNewStr = new char[usStrSize + 1];
	if (!pNewStr)
		return NULL;
	
	srand((unsigned int)time(NULL)); //Initialize random seed

	//Set chars (only small alphabet letters)
	for (unsigned short i = 0; i < usStrSize; i++) 
		pNewStr[i] = bAsciiStart + rand()%bAsciiEnd;
	
	//Zero terminate it
	pNewStr[usStrSize] = 0;

	return pNewStr;
}
//======================================================================

//======================================================================
bool DirectoryExists(char* szDir)
{
	//Check if a directory exists

	if (!szDir)
		return false;

	//Try to get attributes of the directory
	DWORD dwAttributes = GetFileAttributesA(szDir);

	//Check if the function has succeeded (means at least the specified object exists)
	if (dwAttributes == INVALID_FILE_ATTRIBUTES)
		return false;

	//Compare if this object is a directory
	return (dwAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY;
}
//======================================================================

//======================================================================
bool IsValidStrLen(const char* szString, size_t uiMaxStrLen)
{
	//Check if a string has the proper length

	if ((!szString) || (!uiMaxStrLen))
		return false;

	//Loop trough string and check for termination char in the proper range
	for (size_t i = 0; i < uiMaxStrLen; i++) {
		if (szString[i] == 0)
			return true;
	}

	return false;
}
//======================================================================

//======================================================================
bool toLower(char* szString)
{
	//Convert all high chars to low chars
	
	if (!szString)
		return false;

	//Loop trough characters and convert them to lower
	for (unsigned int i = 0; i < strlen(szString); i++) { 
		szString[i] = tolower(szString[i]); 
	}

	return true;
}
//======================================================================

//======================================================================
bool IsValidString(const char* szString)
{
	//Check if a string is valid

	if (!szString)
		return false;

	CValidString vs;

	return vs.IsValidString(szString, &g_VSData);
}
//======================================================================

//======================================================================
bool ClientNameAlreadyUsed(const char* szClientName)
{
	//Check if a client name is already in use

	if (!szClientName)
		return false;

	//Search in client list
	for (CLIENTAMOUNT i = 0; i < ENG_GetClientCount(); i++) {
		clientinfo_s* pClient = ENG_GetClientById(i);
		if ((pClient) && (pClient->bHasLoggedIn) && (strcmp(pClient->userinfo.szName, szClientName)==0))
			return true;
	}

	return false;
}
//======================================================================

//======================================================================
std::string StringReplace(std::string& stdstrExpression, const char* lpszToReplace, const char* lpszNewString)
{
    //Replaces a string in a std string.

	if ((!lpszToReplace) || (!lpszNewString))
		return stdstrExpression;

    while (stdstrExpression.find(lpszToReplace) != -1) {
        stdstrExpression.replace(stdstrExpression.find(lpszToReplace), strlen(lpszToReplace), lpszNewString);
    }

    return stdstrExpression;
}
//======================================================================

//======================================================================
void FatalError(const char* szErrMsg, ...);
BOOL RestartProgram(VOID)
{
	//Perform program restart

	#define BATCH_FILE "temp_restarter_script.bat"
	#define BATCH_NAME_VAR "%0"

	static const char szBatchContent[] = {
		"rem CorvusChat Chatserver restart script\r\n" //Just a comment
		"@echo off\r\n" //Disable printing of the current path 
		"cls\r\n" //Clear console window content
		"taskkill /PID %d /F\r\n" //Terminate the owning process
		"start \"\" \"%sCorvusChat_%s.exe\"\r\n" //Restart the program
		"del %s\r\n" //Delete this batch script
	};

	char szTempDir[MAX_PATH] = {0};
	char g_szBatchFullFileName[MAX_PATH] = {0};

	//Get temp directory
	if (!GetTempPathA(sizeof(szTempDir), szTempDir))
		return FALSE;

	//Format full file name
	sprintf_s(g_szBatchFullFileName, "%s" BATCH_FILE, szTempDir);

	//Get process ID of owning process
	DWORD dwOwnerPid = GetCurrentProcessId();
	if (!dwOwnerPid)
		return FALSE;

	//Format script code
	char szFmtScript[2048] = {0};
	sprintf_s(szFmtScript, szBatchContent, dwOwnerPid, g_GlobalVars.szAppPath, PLATFORM, BATCH_NAME_VAR);

	//Create batch script on disc (current directory)

	std::ofstream hFile;
	hFile.open(g_szBatchFullFileName);
	if (!hFile.is_open())
		return FALSE;

	hFile.write(szFmtScript, strlen(szFmtScript));
	hFile.close();

	//Run it
	return (int)ShellExecuteA(0, "open", g_szBatchFullFileName, NULL, g_GlobalVars.szAppPath, SW_SHOWNORMAL) > 32;
}
//======================================================================

//======================================================================
BOOL DeleteFileObject(LPCSTR lpszFolder)
{
	//Delete a file object: files and folders

	if (!lpszFolder)
		return FALSE;

	char szFolder[MAX_PATH];
	
	//Setup string with folder name
	strcpy_s(szFolder, lpszFolder);
	memset(&szFolder[MAX_PATH-2], 0x00, 2); //Just to ensure the string is double-zero-terminated

	//Setup SH operation structure
	SHFILEOPSTRUCTA shOpStruct;
	
	memset(&shOpStruct, 0x00, sizeof(shOpStruct));

	shOpStruct.wFunc = FO_DELETE; //Delete operation
	shOpStruct.pFrom = szFolder; //Name string of object to delete
	shOpStruct.fFlags = FOF_NOERRORUI | FOF_SILENT | FOF_NOCONFIRMATION; //Don't show error GUI, deletion operation GUI and don't ask for confirmation

	return SHFileOperationA(&shOpStruct) == ERROR_SUCCESS; //Perform operation
}
//======================================================================
