#include <windows.h>
#include "hookapi.h"
#include "ProcessModule.h"
#include "util.h"

#ifdef WINNT
#include "injlib.h"
#endif

CHookAPI g_hook_api;
extern char g_szDllPath[128];

#ifdef WINNT
BOOL ObtainSeDebugPrivilege();
#endif

#ifdef WINNT
BOOL IfCanHook(DWORD process_id)
{
	DWORD cur_pid =GetCurrentProcessId();
	if(process_id ==0 || process_id ==cur_pid) return false;

	CProcessModule pm;

	if(pm.GetProcessModuleHandle(process_id, "smss.exe")
		//|| pm.GetProcessModuleHandle(process_id, "csrss.exe")
		//|| pm.GetProcessModuleHandle(process_id, "lsass.exe")
		)
		return false;

	return true;
}
#endif

int WINAPI HookOneProcess(DWORD process_id)
{
#ifdef WINNT
	char fname[128];
	ObtainSeDebugPrivilege();
	wsprintf(fname, "%s\\Elona_KR.dll", g_szDllPath);
	InjectLib(process_id, fname);
#endif
	return 0;
}

int WINAPI UnhookOneProcess(DWORD process_id)
{
#ifdef WINNT
	ObtainSeDebugPrivilege();
	EjectLib(process_id, "Elona_KR.dll");
#endif

	return 0;
}

char g_exe_to_hook[256];
int g_f_stop =0;
DWORD g_pid =0;
DWORD WINAPI hook_thread(void *lpvoid)
{
	HWND hwndNotify =(HWND)lpvoid;

	WriteLog("hook exe:%s", g_exe_to_hook);

#ifdef WINNT
	ObtainSeDebugPrivilege();
#endif

	CProcessModule pm;
	while(!g_f_stop && (g_pid=pm.GetProcessID(g_exe_to_hook)) ==0)
	{
		Sleep(1000);
	}
	WriteLog("hook....");
	if(!g_f_stop)
	{
		char fname[128];
		wsprintf(fname, "%s\\Elona_KR.dll", g_szDllPath);

		if(hwndNotify) SendMessage(hwndNotify, WM_APP+2004, g_pid, 0L);
		InjectLib(g_pid, fname);
	}

	return 0;
}

int WINAPI HookOneProcess2(HWND hwndNotify, char *exe_name)
{
#ifdef WINNT
	DWORD dw;
	strcpy(g_exe_to_hook, exe_name);
	HANDLE hThread =CreateThread(NULL, 0, hook_thread, hwndNotify, 0, &dw);
	if(hThread ==NULL)
		return -1;
#endif

	return 0;
}

int WINAPI UnhookOneProcess2(char *exe_name)
{
	g_f_stop =1;

#ifdef WINNT
	if(g_pid)
		EjectLib(g_pid, "Elona_KR.dll");
#endif

	return 0;
}

#include "../trans_kr.h"
#include <stdio.h>
int WINAPI ReloadDic(void)
{
	ko_ReloadDic();
	return 0;
}

int WINAPI HookAllProcess()
{
#ifdef WINNT
	char fname[128];
	wsprintf(fname, "%s\\Elona_KR.dll", g_szDllPath);

	ObtainSeDebugPrivilege();
	CProcessModule pm;
	pm.EnumProcess();
	for(int i =0; i<(int)pm.m_dwProcessCount; i++)
	{
		if(IfCanHook(pm.m_dwProcessIDs[i]))
			InjectLib(pm.m_dwProcessIDs[i], fname);
	}
#endif

	return 0;
}

int WINAPI UnhookAllProcess()
{
	g_f_stop =1;
#ifdef WINNT
	CProcessModule pm;
	pm.EnumProcess();
	for(int i =0; i<(int)pm.m_dwProcessCount; i++)
	{
		if(IfCanHook(pm.m_dwProcessIDs[i]))
			EjectLib(pm.m_dwProcessIDs[i], "Elona_KR.dll");
	}
#endif

	return 0;
}

#ifdef WINNT
BOOL ObtainSeDebugPrivilege()
{
	TOKEN_PRIVILEGES TokenPrivileges;
	TOKEN_PRIVILEGES PreviousTokenPrivileges;
	LUID luid;
	HANDLE hToken;
	DWORD dwPreviousTokenPrivilegesSize = sizeof(TOKEN_PRIVILEGES);

	if(!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		WriteLog("debug 001");
		return false;
	}

	if(!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid))
	{
		WriteLog("debug 002");
		return false;
	}
  
	TokenPrivileges.PrivilegeCount            = 1;
	TokenPrivileges.Privileges[0].Luid        = luid;
	TokenPrivileges.Privileges[0].Attributes  = 0;

	if(!AdjustTokenPrivileges(hToken, FALSE, &TokenPrivileges, sizeof(TOKEN_PRIVILEGES),
			&PreviousTokenPrivileges, &dwPreviousTokenPrivilegesSize))
	{
		WriteLog("debug 003");
		return false;
	}

	PreviousTokenPrivileges.PrivilegeCount             = 1;
	PreviousTokenPrivileges.Privileges[0].Luid         = luid;
	PreviousTokenPrivileges.Privileges[0].Attributes  |= SE_PRIVILEGE_ENABLED;

	if(!AdjustTokenPrivileges(hToken, FALSE, &PreviousTokenPrivileges,
			dwPreviousTokenPrivilegesSize, NULL, NULL))
	{
		WriteLog("debug 004");
		return false;
	}

	//WriteLog("debug ok");

	return true;
}
#endif

BOOL WINAPI DllMain(HINSTANCE hInstDLL, DWORD dwReason, PVOID pvReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		break;

	case DLL_PROCESS_DETACH:
		break;
	}

	return TRUE;
}
