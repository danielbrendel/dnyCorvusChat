#include "banlist.h"
#include "utils.h"

/*
	CorvusChat Server (dnyCorvusChat) - Chatserver component

	Developed by Daniel Brendel

	Version: 0.5
	Visit: https://github.com/danielbrendel
	Mail dbrendel1988@gmail.com

	File: banlist.cpp: Banlist manager implementation 
*/

//======================================================================
VOID CBanList::Clear(VOID)
{
	//Clear list

	vAddresses.clear();
}
//======================================================================

//======================================================================
ULONG CBanList::AddressToInteger(LPCSTR lpszAddress)
{
	//Convert an IP address to an unsigned long

	if (!lpszAddress)
		return 0;

	return inet_addr(lpszAddress);
}
//======================================================================

//======================================================================
LPCSTR CBanList::AddressToString(ULONG ulAddress)
{
	//Convert an IP address to a string

	if (!ulAddress)
		return NULL;

	static char szAddress[MAX_IPV4_LEN];

	sprintf_s(szAddress, "%d.%d.%d.%d", *(BYTE*)&ulAddress, *(BYTE*)((DWORD)&ulAddress + 1), *(BYTE*)((DWORD)&ulAddress + 2), *(BYTE*)((DWORD)&ulAddress + 3));

	return szAddress;
}
//======================================================================

//======================================================================
BOOL CBanList::EntryExists(LPCSTR lpszIpv4Address)
{
	//Check if an address is already banned by string

	if (!lpszIpv4Address)
		return FALSE;

	//Convert to unsigned long
	ULONG ulAddr = AddressToInteger(lpszIpv4Address);

	//Loop trough list and check for entries
	for (unsigned int i = 0; i < vAddresses.size(); i++) {
		if (vAddresses[4] == ulAddr)
			return TRUE;
	}

	return FALSE;
}
//======================================================================

//======================================================================
BOOL CBanList::EntryExists(ULONG ulIpv4Address)
{
	//Check if an address is already banned by integer

	if (!ulIpv4Address)
		return FALSE;

	//Loop trough list and check for entries
	for (unsigned int i = 0; i < vAddresses.size(); i++) {
		if (vAddresses[i] == ulIpv4Address)
			return TRUE;
	}

	return FALSE;
}
//======================================================================

//======================================================================
INT CBanList::GetListIdByAddress(ULONG ulIpAddr)
{
	//Get the ID of an entry by IPv4 address

	if (!ulIpAddr)
		return -1;

	//search in list for entry
	for (int i = 0; i < (int)vAddresses.size(); i++) {
		if (vAddresses[i] == ulIpAddr)
			return i;
	}

	return -1;
}
//======================================================================

//======================================================================
BOOL CBanList::AddToList(LPCSTR lpszIpv4Address)
{
	//Add new entry to list by string

	if (!lpszIpv4Address)
		return FALSE;

	//Convert to integer
	ULONG ulAddr = AddressToInteger(lpszIpv4Address);
	if (!ulAddr)
		return FALSE;

	//Check if already exists
	if (EntryExists(ulAddr))
		return TRUE;

	//Add to list
	vAddresses.push_back(ulAddr);

	return TRUE;
}
//======================================================================

//======================================================================
BOOL CBanList::AddToList(ULONG ulIpv4Address)
{
	//Add new entry to list by integer

	if (!ulIpv4Address)
		return FALSE;

	//Check if already exists
	if (EntryExists(ulIpv4Address))
		return TRUE;

	//Add to list
	vAddresses.push_back(ulIpv4Address);

	return TRUE;
}
//======================================================================

//======================================================================
BOOL CBanList::RemoveFromList(LPCSTR lpszIpv4Address)
{
	//Remove an entry from list by string

	if (!lpszIpv4Address)
		return FALSE;

	//Convert to integer
	ULONG ulAddr = AddressToInteger(lpszIpv4Address);
	if (!ulAddr)
		return FALSE;

	//Check if entry exists
	if (!EntryExists(ulAddr))
		return TRUE;

	//Get entry ID
	INT iEntryId = GetListIdByAddress(ulAddr);
	if (iEntryId == -1)
		return FALSE;

	vAddresses.erase(vAddresses.begin() + iEntryId); //Remove entry

	return TRUE;
}
//======================================================================

//======================================================================
BOOL CBanList::RemoveFromList(ULONG ulIpv4Address)
{
	//Remove an entry from list by integer

	if (!ulIpv4Address)
		return FALSE;

	//Check if entry exists
	if (!EntryExists(ulIpv4Address))
		return TRUE;

	//Get entry ID
	INT iEntryId = GetListIdByAddress(ulIpv4Address);
	if (iEntryId == -1)
		return FALSE;

	vAddresses.erase(vAddresses.begin() + iEntryId); //Remove entry

	return TRUE;
}
//======================================================================

//======================================================================
BOOL CBanList::SaveToFile(LPCSTR lpszFileName)
{
	//Save all entries to a banlist file

	if (!lpszFileName)
		return FALSE;

	if (!vAddresses.size())
		return TRUE;

	char szDateTime[100];
	sprintf_s(szDateTime, "//Banlist creation time: %s\n\n", GetCurrentDateTime());

	//Create new file
	std::ofstream hFile;
	hFile.open(lpszFileName, std::ofstream::out);
	if (hFile.is_open()) { //If file has been opened
		hFile.write(szDateTime, strlen(szDateTime)); //Write information buffer 

		char szEntry[MAX_IPV4_LEN + 2];

		for (unsigned int i = 0; i < vAddresses.size(); i++) { //Loop trough entries and write to file
			//Format string
			sprintf_s(szEntry, "%s\n", this->AddressToString(vAddresses[i]));

			//Write to file at current file pointer position
			hFile.write(szEntry, strlen(szEntry));
		}

		hFile.close();

		return TRUE;
	}

	return FALSE;
}
//======================================================================

//======================================================================
BOOL CBanList::LoadFromFile(LPCSTR lpszFileName)
{
	//Load entries via a banlist file

	if (!lpszFileName)
		return FALSE;

	//Check if file exists
	if (!FileExists(lpszFileName))
		return FALSE;

	std::ifstream hFile;
	hFile.open(lpszFileName, std::ifstream::in);
	if (hFile.is_open()) { //If file has been opened
		char szLine[255];
		while (!hFile.eof()) { //While not file end is reached
			hFile.getline(szLine, sizeof(szLine), '\n'); //Get line at current file pointer

			if ((!szLine[0]) || (strncmp(szLine, "//", 2)==0)) //Filter empty line and comments
				continue;

			AddToList(szLine); //Add to list
		}

		hFile.close();

		return TRUE;
	}

	return FALSE;
}
//======================================================================
