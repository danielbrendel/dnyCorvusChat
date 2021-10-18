#include "log.h"
#include "vars.h"

/*
	CorvusChat Server (dnyCorvusChat) - Chatserver component

	Developed by Daniel Brendel

	Version: 0.5
	Visit: https://github.com/danielbrendel
	Mail dbrendel1988@gmail.com

	File: log.cpp: Logging function implementations 
*/

//======================================================================
FILE* pFile;
//======================================================================

//======================================================================
bool CreateLog(const char *lpszFileName, bool CreateNew)
{
    //CreateLog:
    //Create file on Disc to store logging informations

	errno_t ret;

    if (CreateNew)
        ret = fopen_s(&pFile, lpszFileName, "w");
    else
        ret = fopen_s(&pFile, lpszFileName, "a");

	if (ret)
		return false;

    //Initialize HTML file
    fprintf_s(pFile, "<html>\n");
    fprintf_s(pFile, "<head>\n");
    fprintf_s(pFile, "<title> " PROGRAM_NAME " </title>\n");
    fprintf_s(pFile, "</head>\n");
    fprintf_s(pFile, "<body>\n");

    return pFile != NULL;
}
//======================================================================

//======================================================================
void LogMessage(const char *lpszMessage, ...)
{
    //Log a normal message

	if (!pFile)
		return;

    char buf[4096];

	va_list list;
	va_start(list, lpszMessage);
	vsprintf_s(buf, lpszMessage, list);
	va_end(list);

    fprintf_s(pFile, "<b><font color=\"#0000FF\">[" PROGRAM_SHORTCUT "]</font></b> <font color=\"#000000\">Message:</font> <font color=\"#00AC00\">%s</font><br>\n", buf);
}
//======================================================================

//======================================================================
void LogError(const char *lpszError, ...)
{
    //Log an error message

	if (!pFile)
		return;

    char buf[250];

	va_list list;
	va_start(list, lpszError);
	vsprintf_s(buf, lpszError, list);
	va_end(list);

    fprintf_s(pFile, "<b><font color=\"#0000FF\">[" PROGRAM_SHORTCUT "]</font></b> <font color=\"#CACA00\">Error:</font> <font color=\"#FF0000\">%s</font><br>\n", buf);
}
//======================================================================

//======================================================================
void CloseLog(void)
{
    //Close log file

    if (pFile) {
        fprintf_s(pFile, "</body>\n");
        fprintf_s(pFile, "</html>\n");

        fclose(pFile);
    }
}
//======================================================================
