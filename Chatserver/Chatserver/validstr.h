#ifndef _VALIDSTR_H
#define _VALIDSTR_H

#include "includes.h"

/*
	CorvusChat Server (dnyCorvusChat) - Chatserver component

	Developed by Daniel Brendel

	Version: 0.5
	Visit: https://github.com/danielbrendel
	Mail dbrendel1988@gmail.com

	File: validstr.h: Valid string manager interface 
*/

//======================================================================
struct valid_string_s {
	unsigned short usMinLength, usMaxLength; //Allowed minimum/maximum length
	bool bSpacesAllowed; //Space characters allowed?
	unsigned char ucSpaceChar; //Space character
	unsigned char ucAsciiMin, ucAsciiMax; //Minimum/maximum value of possible ASCII chars (range)
};
//======================================================================

//======================================================================
class CValidString {
private:
	const valid_string_s *pValidString;

	inline bool IsValidLength(const char* szString);
	bool HasNotOnlySpaces(const char* szString);
	bool CheckForSpaces(const char* szString);
	bool HasPrintableChars(const char* szString);
public:
	bool IsValidString(const char* szString, const valid_string_s* pValidString);
};
//======================================================================

#endif