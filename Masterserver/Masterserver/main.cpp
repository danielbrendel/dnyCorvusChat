#include "includes.h"
#include "vars.h"
#include "callbacks.h"

/*
	CorvusChat Server (dnyCorvusChat) - Masterserver component

	Developed by Daniel Brendel

	Version: 0.2
	Visit: https://github.com/danielbrendel
	Mail dbrendel1988@gmail.com

	File: main.cpp: Main function implementations 
*/

using namespace std;

//======================================================================
size_t GetHostEntAddrListLen(const hostent* pHostEnt)
{
	//Get amount of address list entries of a hostent

	if (!pHostEnt)
		return 0;

	size_t uiResult = 0;

	while (pHostEnt->h_addr_list[uiResult]) { //Walk through list until the end (NULL-terminated)
		uiResult++;
	}

	return uiResult;
}
//======================================================================

//======================================================================
ULONG GetLocalhostAddr(DWORD dwEntryId)
{
	//Get address of localhost

	//Get host data by localhost name
	hostent* pHostEnt = gethostbyname("localhost");
	if (!pHostEnt)
		return 0;
	
	//Perform check to ensure we are within the address list array
	if (dwEntryId >= GetHostEntAddrListLen(pHostEnt))
		return 0;

	return *(unsigned long*)pHostEnt->h_addr_list[dwEntryId]; //Take the desired entry
}
//======================================================================

//======================================================================
const char* AddressToString(const unsigned long ulIPAddress)
{
	//Setup string with the IP-Address

	static char str[15];
	memset(str, 0x00, sizeof(str));

	sprintf_s(str, "%d.%d.%d.%d", *(BYTE*)&ulIPAddress, *(BYTE*)((DWORD)&ulIPAddress + 1), *(BYTE*)((DWORD)&ulIPAddress + 2), *(BYTE*)((DWORD)&ulIPAddress + 3));

	return &str[0];
}
//======================================================================

//======================================================================
DWORD WINAPI CmdThread(LPVOID lpvArguments)
{
     //Command thread function

    char buf[1024];

    while (true) {
        cin.getline(buf, sizeof(buf), '\n');

        g_Objects.ConCommand.HandleCommand(buf);

		Sleep(10);
    }

    return TRUE;
}
//======================================================================

//======================================================================
void ShutdownComponents(void);
BOOL WINAPI ConsoleControlHandler(DWORD dwCtrlType)
{
	//Handle console close event here

	if (dwCtrlType == CTRL_CLOSE_EVENT) {
		ShutdownComponents();

		return TRUE;
	}

	return FALSE;
}
//======================================================================

//======================================================================
bool InitializeComponents(WORD wPort, TpfnOnRecieveData pfnCallbackFunc)
{
	//Initialize all components

	if ((!wPort) || (!pfnCallbackFunc))
		return false;

	//Add console commands
	#define ADD_COMMAND(name, info, pfn) if (!g_Objects.ConCommand.AddCommand(name, info, pfn)) return false;
	ADD_COMMAND("about", "Show about text", Cmd_About);
	ADD_COMMAND("trylocalhost", "<id> Try a localhost address", Cmd_TryLocalhost);
	ADD_COMMAND("localalias", "<IPv4 address> Set the alias for the localhost address", Cmd_Localalias);
	ADD_COMMAND("listclients", "Lists all active clients", Cmd_ListClients);
	ADD_COMMAND("quit", "Quit program", Cmd_Quit);

	//Add user message
	if (g_Objects.UserMsg.AddUserMsg("SendServerInfo", sizeof(serverinfo_s) + 1) == INVALID_USERMSG)
		return false;

	//Initialize client socket handler
	if (!g_Objects.Client.Initialize(wPort, pfnCallbackFunc))
		return false;

	//Add client handlers
	if (!g_Objects.ClientData.AddHandler(IDENT_GETSERVERLIST, CDH_ServerInfo))
		return false;

	if (!g_Objects.ClientData.AddHandler(IDENT_ADDASSERVER, CDH_AddToServerList))
		return false;

	g_hConThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&CmdThread, NULL, 0, NULL);
    if (g_hConThread == INVALID_HANDLE_VALUE)
		return false;

	return true;
}
//======================================================================

//======================================================================
void ProcessComponents(void)
{
	//Process all components

	//Process socket component
	g_Objects.Client.WaitForClients();
    g_Objects.Client.RecieveData();
    g_Objects.Client.CheckClientsAlive();
    g_Objects.Client.CheckServerUpdate();

	//Update console title
	char szConTitle[250];
	sprintf_s(szConTitle, CONSOLE_TITLE, g_Objects.Client.GetServerCount(), g_Objects.Client.GetClientCount());
	SetConsoleTitleA(szConTitle);
}
//======================================================================

//======================================================================
void ShutdownComponents(void)
{
	//Shutdown all components

	//Shutdown objects
	g_Objects.Client.Close();
	g_Objects.ClientData.Clear();
	g_Objects.ConCommand.Clear();
	g_Objects.UserMsg.Clear();

	//Terminate thread
	if (g_hConThread != INVALID_HANDLE_VALUE) {
		TerminateThread(g_hConThread, EXIT_SUCCESS);
		CloseHandle(g_hConThread);
	}
}
//======================================================================

//======================================================================
int main(int argc, char *argv[])
{
    //Main entry point of application

    SetConsoleTitleA(PROGRAM_NAME);

	//Handle mutex stuff

    HANDLE hMutex = CreateMutexA(0, FALSE, PROGRAM_NAME);
    if (!hMutex) {
        printf("Failed to create mutex\n");
        return -1;
    }

    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        MessageBoxA(0, "Program already running.", "Error", MB_ICONWARNING);
        return -1;
    }

	//Add console control handle
	if (!SetConsoleCtrlHandler(&ConsoleControlHandler, TRUE)) {
		printf("SetConsoleCtrlHandler failed: %d\n", GetLastError());
		return -1;
	}
	
	//Initialize components
	if (!InitializeComponents(PORT, &OnRecieveClientData)) {
		printf("InitializeComponents failed\n");
		ShutdownComponents();
		return -1;
	}

    printf(PROGRAM_NAME " started\n");
    printf("Running on port %d\n", PORT);
	printf("Waiting for clients...\n\n");

    while (g_bServerRunning) {
		ProcessComponents(); //Process all components

		Sleep(1);
	}

	//Shutdown components
	ShutdownComponents();

	//Close mutex handle
    if (hMutex)
        CloseHandle(hMutex);

    return EXIT_SUCCESS;
}
//======================================================================
