#include <windows.h>
#include <string.h>
#ifdef WINNT
#include "psapi.h"
#endif
#include "ProcessModule.h"
#include "util.h"

CProcessModule::CProcessModule()
{
	m_dwProcessCount =0;
	m_nModCount =0;

/*#ifdef WINNT
	m_hProcess =NULL;
	m_hLib =NULL;
	m_pEnumProcesses =NULL;  // api EnumProcesses()
	m_pEnumProcessModules =NULL;  // api EnumProcessModules()
	m_pGetModuleBaseName =NULL;  // api GetModuleBaseName()
#else*/
	memset(&m_mod_name, 0, sizeof(m_mod_name));
//#endif

/*#ifdef WINNT
	m_hLib = LoadLibrary("psapi.dll");

	if (m_hLib == NULL)
		return;

	m_pEnumProcesses = (ENUMPROCESSES_PROC)GetProcAddress(m_hLib, "EnumProcesses");

	if (m_pEnumProcesses == NULL)
	{
		FreeLibrary(m_hLib);
		m_hLib =NULL;
	}

	m_pEnumProcessModules = (ENUMPROCESSMODULES_PROC)GetProcAddress(m_hLib, "EnumProcessModules");

	if (m_pEnumProcessModules == NULL)
	{
		FreeLibrary(m_hLib);
		m_hLib =NULL;
	}

	m_pGetModuleBaseName = (GETMODULEBASENAME_PROC)GetProcAddress(m_hLib, "GetModuleBaseNameA");

	if (m_pGetModuleBaseName == NULL)
	{
		FreeLibrary(m_hLib);
		m_hLib =NULL;
	}
#else*/
	HMODULE hModule = GetModuleHandle("kernel32.dll");

	if (hModule == NULL)
		return;

	pCreateToolhelp32Snapshot = (CREATETOOLHELP32SNAPSHOT_PROC)GetProcAddress(hModule, "CreateToolhelp32Snapshot");

	if (pCreateToolhelp32Snapshot == NULL)
		return;

	pProcess32First = (PROCESS32FIRST_PROC)GetProcAddress(hModule, "Process32First");

	if (pProcess32First == NULL)
		return;

	pProcess32Next = (PROCESS32NEXT_PROC)GetProcAddress(hModule, "Process32Next");

	if (pProcess32Next == NULL)
		return;

	pModule32First = (MODULE32FIRST_PROC)GetProcAddress(hModule, "Module32First");

	if (pModule32First == NULL)
		return;

	pModule32Next = (MODULE32NEXT_PROC)GetProcAddress(hModule, "Module32Next");

	if (pModule32Next == NULL)
		return;
//#endif
}

CProcessModule::~CProcessModule()
{
/*#ifdef WINNT
	if(m_hLib)
	{
		FreeLibrary(m_hLib);
		m_hLib =NULL;
	}
#endif*/
}

int CProcessModule::EnumProcess()
{
	m_dwProcessCount =0;
/*#ifdef WINNT
	DWORD dwBytesNeeded;

	if(!m_pEnumProcesses(&m_dwProcessIDs[0], sizeof(m_dwProcessIDs), &dwBytesNeeded))
		return -1;
	
	m_dwProcessCount = dwBytesNeeded / sizeof(DWORD);
#else*/
	HANDLE hSnapShot;
	PROCESSENTRY32 ProcessEntry32;
	//MODULEENTRY32 ModuleEntry32;
	BOOL Result;
	//char *pszExtension;

	hSnapShot = pCreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hSnapShot == (HANDLE)-1)
		return false;

	ProcessEntry32.dwSize = sizeof(PROCESSENTRY32);

	Result = pProcess32First(hSnapShot, &ProcessEntry32);

	if (Result != TRUE)
	{
		CloseHandle(hSnapShot);
		return false;
	}

	do
	{
		//HMODULE hMod =GetModuleHandle(ProcessEntry32.szExeFile);
		//WriteLog("process:%s, id:%x, module id:%x",
		//	ProcessEntry32.szExeFile, ProcessEntry32.th32ProcessID,
		//	ProcessEntry32.th32ModuleID);
		m_dwProcessIDs[m_dwProcessCount] = ProcessEntry32.th32ProcessID;
		m_dwProcessCount ++;
	} while (pProcess32Next(hSnapShot, &ProcessEntry32) && m_dwProcessCount < MAX_PROCESS_COUNT);

	CloseHandle(hSnapShot);

//#endif

	return m_dwProcessCount;
}

int CProcessModule::EnumProcessModules(DWORD process_id)
{
	m_nModCount =0;
/*#ifdef WINNT
	DWORD cbNeeded;

	if(m_hProcess) CloseHandle(m_hProcess);

	m_hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, process_id);

	if (m_hProcess == NULL)
		return -1;
	if(!m_pEnumProcessModules(m_hProcess, m_hMods, sizeof(m_hMods), &cbNeeded))
	{
		char err_msg[256];
		DWORD err=GetLastError();
		GetErrString(err_msg, sizeof(err_msg), err);
		WriteLog("failed to EnumProcessModules, err=%d-%s", err, err_msg);
		CloseHandle(m_hProcess);
		return 0;
	}
	m_nModCount =cbNeeded / sizeof(HMODULE);
#else*/
	HANDLE hSnapShot;
	MODULEENTRY32 ModuleEntry32;
	BOOL Result;
 
	hSnapShot = pCreateToolhelp32Snapshot(TH32CS_SNAPMODULE, process_id);

	if (hSnapShot == (HANDLE)-1)
		return -1;

	ModuleEntry32.dwSize = sizeof(MODULEENTRY32);

	Result = pModule32First(hSnapShot, &ModuleEntry32);

	if (Result != TRUE)
	{
		CloseHandle(hSnapShot);
		return -1;
	}

	do
	{
		strcpy(m_mod_name[m_nModCount], ModuleEntry32.szModule);
		m_hMods[m_nModCount] =ModuleEntry32.hModule;
		m_nModCount++;

	} while (pModule32Next(hSnapShot, &ModuleEntry32) && m_nModCount <MAX_MODULE_COUNT);

	CloseHandle(hSnapShot);
//#endif

	return m_nModCount;
}

int CProcessModule::GetModuleBaseNameA(int num, char *mod_name)
{
/*#ifdef WINNT
	if ( m_pGetModuleBaseName( m_hProcess, m_hMods[num], mod_name,100))
		return -1;
#else*/
	strcpy(mod_name, m_mod_name[num]);
//#endif
	return 0;
}

void CProcessModule::EndEnumProcessModules()
{
/*#ifdef WINNT
	if(m_hProcess) CloseHandle(m_hProcess);
	m_hProcess =NULL;
#endif*/
}

HANDLE CProcessModule::GetProcessModuleHandle(DWORD process_id, char *mod_name)
{
	int mod_count =EnumProcessModules(process_id);
	char mod_base_name[100];

	for(int i =0; i<mod_count; i++)
	{
		GetModuleBaseNameA(i, mod_base_name);
		if(strcmpi(mod_base_name, mod_name) ==0)
		{
			EndEnumProcessModules();
			return m_hMods[i];
		}
	}

	EndEnumProcessModules();

	return NULL;
}

DWORD CProcessModule::GetProcessID(char *exe_name)
{
	WriteLog("GetProcessID...");
	EnumProcess();
	WriteLog("GetProcessID m_dwProcessCount=%d...", m_dwProcessCount);
	for(int i =0; i<(int)m_dwProcessCount; i++)
	{
		int mod_count =EnumProcessModules(m_dwProcessIDs[i]);
		char mod_base_name[100];
		//WriteLog("process num=%s", i);
		for(int j =0; j<mod_count; j++)
		{
			GetModuleBaseNameA(j, mod_base_name);
			if(strcmpi(mod_base_name, exe_name) ==0)
			{
				WriteLog("found mod_base_name %s", mod_base_name);
				EndEnumProcessModules();
				return m_dwProcessIDs[i];
			}
		}
		//WriteLog("\r\n");
	}


	EndEnumProcessModules();

	return NULL;
}

#ifdef WINNT
BOOL CProcessModule::ObtainSeDebugPrivilege(HANDLE hProcess)
{
	BOOL Result;
	TOKEN_PRIVILEGES TokenPrivileges;
	TOKEN_PRIVILEGES PreviousTokenPrivileges;
	LUID luid;
	HANDLE hToken;
	DWORD dwPreviousTokenPrivilegesSize = sizeof(TOKEN_PRIVILEGES);

	Result = OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);

	if (Result == FALSE)
		return false;

	Result = LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid);

	if (Result == FALSE)
		return false;
  
	TokenPrivileges.PrivilegeCount            = 1;
	TokenPrivileges.Privileges[0].Luid        = luid;
	TokenPrivileges.Privileges[0].Attributes  = 0;

	AdjustTokenPrivileges(hToken, FALSE, &TokenPrivileges, sizeof(TOKEN_PRIVILEGES),
				&PreviousTokenPrivileges, &dwPreviousTokenPrivilegesSize);

	if (GetLastError() != ERROR_SUCCESS)
		return false;

	PreviousTokenPrivileges.PrivilegeCount             = 1;
	PreviousTokenPrivileges.Privileges[0].Luid         = luid;
	PreviousTokenPrivileges.Privileges[0].Attributes  |= SE_PRIVILEGE_ENABLED;

	AdjustTokenPrivileges(hToken, FALSE, &PreviousTokenPrivileges,
		dwPreviousTokenPrivilegesSize, NULL, NULL);

	if (GetLastError() != ERROR_SUCCESS)
		return false;

	CloseHandle(hToken);
	return true;
}
#endif
