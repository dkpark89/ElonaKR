#ifndef __PROCESS_MODULE_H__
#define __PROCESS_MODULE_H__

/*#ifdef WINNT
typedef BOOL (WINAPI *ENUMPROCESSES_PROC)(DWORD *lpidProcess, DWORD cb, DWORD *cbNeeded);
typedef BOOL (WINAPI *ENUMPROCESSMODULES_PROC)(HANDLE hProcess, HMODULE *lphModule, DWORD cb, LPDWORD lpcbNeeded);
typedef DWORD (WINAPI *GETMODULEBASENAME_PROC)(HANDLE hProcess, HMODULE hModule, LPTSTR lpBaseName, DWORD nSize);
#else*/
#include <tlhelp32.h>
typedef HANDLE (WINAPI *CREATETOOLHELP32SNAPSHOT_PROC)(DWORD dwFlags, DWORD th32ProcessID);
typedef BOOL (WINAPI *PROCESS32FIRST_PROC)(HANDLE hSnapshot, LPPROCESSENTRY32 lppe);
typedef BOOL (WINAPI *PROCESS32NEXT_PROC)(HANDLE hSnapshot, LPPROCESSENTRY32 lppe);
typedef BOOL (WINAPI *MODULE32FIRST_PROC)(HANDLE hSnapshot, LPMODULEENTRY32 lpme);
typedef BOOL (WINAPI *MODULE32NEXT_PROC)(HANDLE hSnapshot, LPMODULEENTRY32 lpme);
//#endif

#define MAX_PROCESS_COUNT	512
#define MAX_MODULE_COUNT	500

class CProcessModule
{
public:
	CProcessModule();
	~CProcessModule();
	
private:
/*#ifdef WINNT
	HINSTANCE m_hLib;  // from LoadLibrary("PSAPI.dll")
	ENUMPROCESSES_PROC m_pEnumProcesses;  // api EnumProcesses()
	ENUMPROCESSMODULES_PROC m_pEnumProcessModules;  // api EnumProcessModules()
	GETMODULEBASENAME_PROC m_pGetModuleBaseName;  // api GetModuleBaseName()
	HANDLE m_hProcess;
#else*/
	char m_mod_name[MAX_MODULE_COUNT][100];  // max 100 modules in one process
	CREATETOOLHELP32SNAPSHOT_PROC pCreateToolhelp32Snapshot;
	PROCESS32FIRST_PROC pProcess32First;
	PROCESS32NEXT_PROC pProcess32Next;
	MODULE32FIRST_PROC pModule32First;
	MODULE32NEXT_PROC pModule32Next;
//#endif

public:
	DWORD m_dwProcessIDs[MAX_PROCESS_COUNT];  // 
	DWORD m_dwProcessCount;
	HMODULE m_hMods[MAX_MODULE_COUNT];
	int m_nModCount;

	int EnumProcess();
	int EnumProcessModules(DWORD process_id);
	int GetModuleBaseNameA(int num, char *mod_name);
	void EndEnumProcessModules();

	HANDLE GetProcessModuleHandle(DWORD process_id, char *mod_name);
	DWORD GetProcessID(char *exe_name);

#ifdef WINNT
	BOOL ObtainSeDebugPrivilege(HANDLE hProcess);
#endif
};


#endif
