#include <windows.h>
#include <stdio.h>
#include "ProcessModule.h"
#include "util.h"

#ifdef WINNT

int WINAPI InjectLib(DWORD process_id, char *lib_name)
{
	PTHREAD_START_ROUTINE pfnRemote =(PTHREAD_START_ROUTINE)
			GetProcAddress(GetModuleHandle("Kernel32"), "LoadLibraryA");
	if(pfnRemote ==NULL)
		return -1;
	HANDLE hProcess =OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_id);
	if(hProcess ==NULL)
	{
		WriteLog("InjectLib:OpenProcess %d failed!", process_id);
		return -1;
	}

	int mem_size =strlen(lib_name)+1;
	void *premote_mem =VirtualAllocEx(hProcess, NULL, mem_size, MEM_COMMIT, PAGE_READWRITE);
	if(premote_mem ==NULL)
	{
		CloseHandle(hProcess);
		return -1;
	}

	//if(hThread) SuspendThread(hThread);

	int ret =WriteProcessMemory(hProcess, premote_mem, lib_name, mem_size,NULL);
	if(ret ==STATUS_ACCESS_VIOLATION || ret ==false)
	{
		//if(hThread) ResumeThread(hThread);
		VirtualFreeEx(hProcess, premote_mem, 0, MEM_RELEASE);
		CloseHandle(hProcess); 
		return -1;
	}
	//if(hThread) ResumeThread(hThread);

	HANDLE hThread =CreateRemoteThread(hProcess, NULL, 0, 
			pfnRemote, premote_mem, 0, NULL);

	if(hThread ==NULL)
	{
		VirtualFreeEx(hProcess, premote_mem, 0, MEM_RELEASE);
		CloseHandle(hProcess);
		return -1;
	}
	
	//WriteLog2("c:\\hookapi_debug.log", "CreateRemoteThread ok");

	WaitForSingleObject(hThread, INFINITE);

	VirtualFreeEx(hProcess, premote_mem, 0, MEM_RELEASE);
	CloseHandle(hProcess);
	CloseHandle(hThread);

	return 0;
}

int WINAPI InjectLib(HANDLE hProcess, char *lib_name)
{
	PTHREAD_START_ROUTINE pfnRemote =(PTHREAD_START_ROUTINE)
			GetProcAddress(GetModuleHandle("Kernel32"), "LoadLibraryA");
	if(pfnRemote ==NULL)
		return -1;

	if(hProcess ==NULL)
	{
		WriteLog("InjectLib:OpenProcess %d failed!", hProcess);
		return -1;
	}

	int mem_size =strlen(lib_name)+1;
	void *premote_mem =VirtualAllocEx(hProcess, NULL, mem_size, MEM_COMMIT, PAGE_READWRITE);
	if(premote_mem ==NULL)
	{
		CloseHandle(hProcess);
		return -1;
	}

	//if(hThread) SuspendThread(hThread);

	int ret =WriteProcessMemory(hProcess, premote_mem, lib_name, mem_size,NULL);
	if(ret ==STATUS_ACCESS_VIOLATION || ret ==false)
	{
		//if(hThread) ResumeThread(hThread);
		VirtualFreeEx(hProcess, premote_mem, 0, MEM_RELEASE);
		return -1;
	}
	//if(hThread) ResumeThread(hThread);

	HANDLE hThread =CreateRemoteThread(hProcess, NULL, 0, 
			pfnRemote, premote_mem, 0, NULL);

	if(hThread ==NULL)
	{
		VirtualFreeEx(hProcess, premote_mem, 0, MEM_RELEASE);
		return -1;
	}
	
	//WriteLog2("c:\\hookapi_debug.log", "CreateRemoteThread ok");

	WaitForSingleObject(hThread, INFINITE);

	VirtualFreeEx(hProcess, premote_mem, 0, MEM_RELEASE);
	CloseHandle(hThread);

	return 0;
}

int WINAPI EjectLib(DWORD process_id, char *lib_name)
{
	PTHREAD_START_ROUTINE pfnRemote =(PTHREAD_START_ROUTINE)
			GetProcAddress(GetModuleHandle("Kernel32"), "FreeLibrary");
	
	if(pfnRemote ==NULL)
	{
		WriteLog("EjectLib:GetProcAddress FreeLibrary failed!");
		return -1;
	}
	HANDLE hProcess =OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_id);
	if(hProcess ==NULL)
	{
		WriteLog("EjectLib:OpenProcess %d failed!", process_id);
		return -1;
	}

	CProcessModule pm;
	HANDLE hmod =pm.GetProcessModuleHandle(process_id, lib_name);
	if(hmod ==NULL)
	{
		CloseHandle(hProcess);
		//WriteLog("EjectLib(), not found lib:%s", lib_name);
		return 0;
	}
	//WriteLog("EjectLib(), found lib:%s, mod handle:0x%x", lib_name, hmod);

	HANDLE hThread =CreateRemoteThread(hProcess, NULL, 0, 
			pfnRemote, hmod, 0, NULL);

	if(hThread ==NULL)
	{
		WriteLog("EjectLib():CreateRemoteThread failed! process_id:%d", process_id);
		CloseHandle(hProcess);
		return -1;
	}
	WaitForSingleObject(hThread, INFINITE);

	CloseHandle(hProcess);
	CloseHandle(hThread);

	return 0;
}

#endif
