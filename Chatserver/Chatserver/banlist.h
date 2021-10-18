#ifndef _BANLIST_H
#define _BANLIST_H

#include "includes.h"

/*
	CorvusChat Server (dnyCorvusChat) - Chatserver component

	Developed by Daniel Brendel

	Version: 0.5
	Visit: https://github.com/danielbrendel
	Mail dbrendel1988@gmail.com

	File: banlist.h: Banlist manager interface 
*/

//======================================================================
#define MAX_IPV4_LEN 15
#define BANLIST_DEFAULT_FILE "banlist.txt"
//======================================================================

//======================================================================
class CBanList {
private:
	std::vector<ULONG> vAddresses;

	VOID Clear(VOID);

	ULONG AddressToInteger(LPCSTR lpszAddress);
	LPCSTR AddressToString(ULONG ulAddress);

	INT GetListIdByAddress(ULONG ulIpAddr);
public:
	CBanList() { }
	~CBanList() { Clear(); }

	BOOL AddToList(LPCSTR lpszIpv4Address);
	BOOL AddToList(ULONG ulIpv4Address);

	BOOL RemoveFromList(LPCSTR lpszIpv4Address);
	BOOL RemoveFromList(ULONG ulIpv4Address);

	BOOL EntryExists(LPCSTR lpszIpv4Address);
	BOOL EntryExists(ULONG ulIpv4Address);

	BOOL SaveToFile(LPCSTR lpszFileName);
	BOOL LoadFromFile(LPCSTR lpszFileName);
};
//======================================================================

#endif