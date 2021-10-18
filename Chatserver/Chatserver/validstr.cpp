#include "validstr.h"

/*
	CorvusChat Server (dnyCorvusChat) - Chatserver component

	Developed by Daniel Brendel

	Version: 0.5
	Visit: https://github.com/danielbrendel
	Mail dbrendel1988@gmail.com

	File: validstr.cpp: Valid String manager implementation 
*/

//======================================================================
inline bool CValidString::IsValidLength(const char* szString)
{
	//Check for valid string length

	if ((!pValidString) || (!szString))
		return false;

	int iLen = (int)strlen(szString);

	return (iLen >= pValidString->usMinLength) && (iLen <= pValidString->usMaxLength);
}
//======================================================================

//======================================================================
bool CValidString::HasNotOnlySpaces(const char* szString)
{
	//Check if at least one character is not a space

	if ((!szString) || (!pValidString) || (!pValidString->bSpacesAllowed))
		return false;

	//Check string and try to find at least one non-space char
	for (unsigned short i = 0; i < strlen(szString); i++) {
		if (szString[i] != pValidString->ucSpaceChar) {
			return true;
		}
	}

	return false;
}
//======================================================================

//======================================================================
bool CValidString::CheckForSpaces(const char* szString)
{
	//Check if spaces are allowed and if not then check for them. Returns true if everything is fine depending on the rule

	if ((!szString) || (!pValidString))
		return false;

	//If spaces are allowed then check if at least one non-space char exists
	if (pValidString->bSpacesAllowed)
		return HasNotOnlySpaces(szString);

	//Spaces are not allowed, check if at least one exists
	for (unsigned short i = 0; i < strlen(szString); i++) {
		if (szString[i] == pValidString->ucSpaceChar) {
			return false;
		}
	}

	return true;
}
//======================================================================

//======================================================================
bool CValidString::HasPrintableChars(const char* szString)
{
	//Check if string has printable characters. This includes spaces

	if ((!szString) || (!pValidString))
		return false;

	//Check every char for its range
	for (unsigned short i = 0; i < strlen(szString); i++) {
		if (!((szString[i] >= pValidString->ucAsciiMin) && (szString[i] <= pValidString->ucAsciiMax))) {
			return false;
		}
	}

	return true;
}
//======================================================================

//======================================================================
bool CValidString::IsValidString(const char* szString, const valid_string_s* pValidString)
{
	//Check if the given string is valid depending on the rules

	if ((!szString) || (!pValidString))
		return false;

	this->pValidString = pValidString;

	//Check for valid string length
	if (!IsValidLength(szString))
		return false;

	//Check for printable chars
	if (!HasPrintableChars(szString))
		return false;

	//Handle space character cases
	if (!CheckForSpaces(szString))
		return false;

	return true;
}
//======================================================================