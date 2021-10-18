#ifndef _UPDATE_H
#define _UPDATE_H

#include "includes.h"

/*
	CorvusChat Server (dnyCorvusChat) - Chatserver component

	Developed by Daniel Brendel

	Version: 0.5
	Visit: https://github.com/danielbrendel
	Mail dbrendel1988@gmail.com

	File: update.h: Update manager interface 
*/

//======================================================================
#define UPDATE_MAX_SCRIPT_LINE_LENGTH 2048
#define UPDATE_SPECIAL_MAIN_EXECUTABLE "{main}" //Special expression: main executable identifier
#define UPDATE_SPECIAL_VERSION_STRING "#version" //Special expression: version identifier
#define UPDATE_SPECIAL_DLTYPE_IMPORTANT "important" //Special expression: This object must be downloaded, so abort update process if download has failed for this object
#define UPDATE_SPECIAL_DLTYPE_OPTIONAL "optional" //Special epression: This object is optional, so don't abort update process if download has failed for this object
#define UPDATE_WEBSPACE_PATH "/ccsupdate/"
#define UPDATE_LOCAL_INFO_FILE "update.txt"
#define UPDATE_WEBSPACE_INFO_FILE UPDATE_WEBSPACE_PATH UPDATE_LOCAL_INFO_FILE
#define UPDATE_BATCH_FILENAME "__updatebatch.bat"
#define UPDATE_BATCH_CONTENT "REM Terminate server process\r\ntaskkill /PID %d\r\nREM Overwrite server program file with the downloaded file\r\nxcopy \"%s\" \"%s\" /Y\r\nREM Delete the downloaded file\r\ndel \"%s\"\r\nREM Start the updated server program file\r\nstart %s\r\nREM Delete update script if exists\r\ndel %s\r\nREM Delete batch script file\r\ndel %s\r\nREM Exit the cmd.exe interface\r\nexit\r\n"
//======================================================================

//======================================================================
enum CB_RET {
	CBR_ABORT, //Abort further script parsing
	CBR_UNHANDLED,
	CBR_HANDLED
};
//======================================================================

//======================================================================
typedef CB_RET (*THandleUpdateExpressions)(LPCSTR lpszExpression);
//======================================================================

//======================================================================
class CUpdateScriptParser {
protected:
	CB_RET ExpressionCallback(LPCSTR lpszExpression);

	BOOL PrepareLine(LPCSTR lpszLine, LPSTR lpszOut, DWORD dwOutBufLen);
	BOOL ParseLine(LPCSTR lpszLine);

	std::vector<char*> vStrings;
	bool bFirstExpression;
	THandleUpdateExpressions pEventFunction;
public:
	CUpdateScriptParser() { bFirstExpression = false; pEventFunction = NULL; }
	~CUpdateScriptParser() { }

	VOID SetEventFunction(THandleUpdateExpressions pfnEventFunction);

	DWORD GetArgumentCount(VOID);
	CHAR* GetArgumentValue(DWORD dwId);

	BOOL ParseScript(LPCSTR lpszScriptFile);
};

class CUpdate : public CUpdateScriptParser {
private:
	HINTERNET hInet;

	BOOL ExecUpdateBatch(LPCSTR lpszMainProgramFile, LPCSTR lpszBackupFile);

	friend CB_RET frEventMethod(LPCSTR lpszExpression);
public:
	CUpdate() { hInet = NULL; }
	~CUpdate() { }

	BOOL OpenInternetHandle(void); 
	VOID CloseInternetHandle(void);

	BOOL QueryUpdateInfo(VOID);

	BOOL UpdateResource(LPCSTR pszResourceName, BOOL bIsProgramMainExecutable);
};
//======================================================================

#endif