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
    fprintf_s(pFile, "<!doctype html>\n");
    fprintf_s(pFile, "<html lang=\"en\">\n");
    fprintf_s(pFile, "<head>\n");
    fprintf_s(pFile, "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"/>\n");
    fprintf_s(pFile, "<title> " PROGRAM_NAME " </title>\n");
    fprintf_s(pFile, "<style>\n");
    fprintf_s(pFile, "html, body { background-color: rgb(30, 30, 30); }\n");
    fprintf_s(pFile, "h1 { color: rgb(250, 250, 250); }\n");
    fprintf_s(pFile, ".log { position: relative; padding: 5px; }\n");
    fprintf_s(pFile, ".entry { position: relative; }\n");
    fprintf_s(pFile, ".apptag { display: inline-block; color: rgb(132, 178, 245); font-weight: bold; }\n");
    fprintf_s(pFile, ".logtype { display: inline-block; color: rgb(250, 250, 250); }\n");
    fprintf_s(pFile, ".message { display: inline-block; color: rgb(111, 255, 230); }\n");
    fprintf_s(pFile, ".success { display: inline-block; color: rgb(123, 230, 90); }\n");
    fprintf_s(pFile, ".warning { display: inline-block; color: rgb(255, 251, 5); }\n");
    fprintf_s(pFile, ".error { display: inline-block; color: rgb(234, 163, 153); }\n");
    fprintf_s(pFile, "</style>\n");
    fprintf_s(pFile, "</head>\n");
    fprintf_s(pFile, "<body>\n");
    fprintf_s(pFile, "<h1>" PROGRAM_NAME " log file</h1>\n");
    fprintf_s(pFile, "<div class=\"log\">\n");

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

    fprintf_s(pFile, "<div class=\"entry\">\n");
    fprintf_s(pFile, "<div class=\"apptag\">[" PROGRAM_SHORTCUT "]</div>\n");
    fprintf_s(pFile, "<div class=\"logtype\">Message:</div>\n");
    fprintf_s(pFile, "<div class=\"message\">%s</div>\n", buf);
    fprintf_s(pFile, "</div>\n");
}
//======================================================================

//======================================================================
void LogSuccess(const char* lpszMessage, ...)
{
    //Log a success message

    if (!pFile)
        return;

    char buf[4096];

    va_list list;
    va_start(list, lpszMessage);
    vsprintf_s(buf, lpszMessage, list);
    va_end(list);

    fprintf_s(pFile, "<div class=\"entry\">\n");
    fprintf_s(pFile, "<div class=\"apptag\">[" PROGRAM_SHORTCUT "]</div>\n");
    fprintf_s(pFile, "<div class=\"logtype\">Success:</div>\n");
    fprintf_s(pFile, "<div class=\"success\">%s</div>\n", buf);
    fprintf_s(pFile, "</div>\n");
}
//======================================================================

//======================================================================
void LogWarning(const char* lpszWarning, ...)
{
    //Log a warning message

    if (!pFile)
        return;

    char buf[250];

    va_list list;
    va_start(list, lpszWarning);
    vsprintf_s(buf, lpszWarning, list);
    va_end(list);

    fprintf_s(pFile, "<div class=\"entry\">\n");
    fprintf_s(pFile, "<div class=\"apptag\">[" PROGRAM_SHORTCUT "]</div>\n");
    fprintf_s(pFile, "<div class=\"logtype\">Warning:</div>\n");
    fprintf_s(pFile, "<div class=\"warning\">%s</div>\n", buf);
    fprintf_s(pFile, "</div>\n");
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

    fprintf_s(pFile, "<div class=\"entry\">\n");
    fprintf_s(pFile, "<div class=\"apptag\">[" PROGRAM_SHORTCUT "]</div>\n");
    fprintf_s(pFile, "<div class=\"logtype\">Error:</div>\n");
    fprintf_s(pFile, "<div class=\"error\">%s</div>\n", buf);
    fprintf_s(pFile, "</div>\n");
}
//======================================================================

//======================================================================
void CloseLog(void)
{
    //Close log file

    if (pFile) {
        fprintf_s(pFile, "</div>\n");
        fprintf_s(pFile, "</body>\n");
        fprintf_s(pFile, "</html>\n");

        fclose(pFile);
    }
}
//======================================================================
