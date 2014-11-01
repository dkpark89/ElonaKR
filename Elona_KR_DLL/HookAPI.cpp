// ------------------------------------- //
// 您如果要使用本文件，请不要删除本说明  //
// ------------------------------------- //
//             HOOKAPI v1.6              //
//   Copyright 2002 编程沙龙 Paladin     //
//       www.ProgramSalon.com            //
// ------------------------------------- //
// 
//  HookAPI.cpp
//  通过取得函数地址，并在函数代码前插入call myfunc来实现调用myfunc的程序
//  Last Modified: 2003.05.07, by Paladin
//

#include <windows.h>
#include <time.h>
#include <stdio.h>
#include "ApiInfo.h"
#include "HookApi.h"
#include "util.h"

#ifdef WINNT
#include "injlib.h"
#endif

#ifdef WIN95
#include "ring0.h"
#endif

#pragma check_stack(off)

#ifdef WIN95

#pragma code_seg("_INIT")
#pragma comment(linker,"/SECTION:.bss,RWS /SECTION:.data,RWS /SECTION:.rdata,RWS /SECTION:.text,RWS /SECTION:_INIT,RWS ")
#pragma comment(linker,"/BASE:0xBFF70000")

#endif


#define CALL_BYTES_SIZE		5		// CALL 0xnnnnnnnn		size=5

CAPIInfo g_api_info;

char g_szDllPath[128];

static int g_demo_count =0;
#ifdef WINNT
extern BOOL ObtainSeDebugPrivilege();
#endif


////////////////////////
CHookAPI::CHookAPI()
{
	g_szDllPath[0] =0;

	//WriteLog("debug: hooked");
	m_hMyDll =NULL;
	HMODULE hMod =GetModuleHandle(HOOKAPI_DLL_NAME);
	if(hMod)
	{
		char fname[128];
		GetModuleFileName(hMod, fname, sizeof(fname));
		GetFilePath(fname, g_szDllPath);
		WriteProfileString("Elona_KR", "DLL_PATH", g_szDllPath);
	}
	else GetProfileString("Elona_KR", "DLL_PATH", "", g_szDllPath, sizeof(g_szDllPath));

	LoadMyDll();

	char mod_name[256], mod_name2[100];
	GetModuleFileName(NULL, mod_name, sizeof(mod_name));
	GetProfileString("Elona_KR", "exe_name", "", mod_name2, sizeof(mod_name2));
	strupr(mod_name);
	strupr(mod_name2);

#ifdef WINNT
	if(mod_name2[0] ==0 || strstr(mod_name, mod_name2) ==NULL)
#endif
	{
		Init();
		HookAllAPI();
	}
}

CHookAPI::~CHookAPI()
{
	UnhookAllAPI();
	FreeMyDll();
}

int CHookAPI::LoadMyDll()
{
	if(GetModuleHandle(MY_DLL_NAME)) return 0;

	char fdll[128];
	wsprintf(fdll, "%s\\%s", g_szDllPath, MY_DLL_NAME);
	if((m_hMyDll =LoadLibrary(fdll)) ==NULL)
	{
		WriteLog("LoadMyDll %s failed!", fdll);
		return -1;
	}

	return 0;
}

void CHookAPI::FreeMyDll()
{
	if(m_hMyDll) FreeLibrary(m_hMyDll);

	m_hMyDll =NULL;
}

// Init hooked api info
int CHookAPI::Init()
{
	char cur_mod[200];
	MYAPIINFO *pMyAPIInfo;
	tagGetMyAPIInfo GetMyAPIInfo;
	APIFUNC old_api, my_api;
	HMODULE hmod;
	HINSTANCE hMyDll;

	// 得到要截获的api信息
	if(m_hMyDll ==NULL)
	{
		hMyDll =GetModuleHandle(MY_DLL_NAME);
		if(hMyDll ==NULL)
		{
			WriteLog("Init()::m_hMyDll ==NULL");
			return false;
		}
	}
	else
		hMyDll =m_hMyDll;

	if((GetMyAPIInfo=(tagGetMyAPIInfo)GetProcAddress(hMyDll, "GetMyAPIInfo")) ==NULL)
	{
		WriteLog("GetProcAddress GetMyAPIInfo of %s failed!", MY_DLL_NAME);
		return false;
	}
	pMyAPIInfo =GetMyAPIInfo();
	if(pMyAPIInfo ==NULL)
	{
		WriteLog("Init()::GetMyAPIInfo failed!");
		return false;
	}

	int count =0;
	//int f_hook_process =false;

	// 分析并保存函数和参数
	GetModuleFileName(NULL, cur_mod, sizeof(cur_mod));
	APIINFO *pinfo;

	//WriteLog("debug: get api info...");
	while (pMyAPIInfo[count].module_name !=NULL)
	{
		if((hmod =GetModuleHandle(pMyAPIInfo[count].module_name)) ==NULL)
		{
			WriteLog("Error Get module %s, so not hook this module's api", pMyAPIInfo[count].module_name);
			count++;
			continue;
		}
		if((old_api =(APIFUNC)GetProcAddress(hmod, pMyAPIInfo[count].api_name)) ==NULL)
		{
			WriteLog("Error Get ProcAddress %s of module %s, so not hook this module's api", pMyAPIInfo[count].api_name, pMyAPIInfo[count].module_name);
			count++;
			continue;
		}
		//WriteLog("debug: GetProcAddress %s %s =%x", pMyAPIInfo[count].module_name, pMyAPIInfo[count].api_name, old_api);

		if((my_api =(APIFUNC)GetProcAddress(hMyDll, pMyAPIInfo[count].my_api_name)) ==NULL)
		{
			WriteLog("failed to GetrProcAddress:%s", pMyAPIInfo[count].my_api_name);
			return false;
		}

		//WriteLog("debug: add api info:%s", pMyAPIInfo[count].api_name);
		if((pinfo =g_api_info.Add(pMyAPIInfo[count].module_name, pMyAPIInfo[count].api_name,
			pMyAPIInfo[count].my_api_name, pMyAPIInfo[count].param_count,
			old_api, my_api, pMyAPIInfo[count].friend_my_api_name,
			pMyAPIInfo[count].start_pos)) ==NULL)
			break;

		count++;
	}

#ifdef WINNT
	if((hmod =GetModuleHandle("Kernel32.dll")) ==NULL)
	{
		WriteLog("Error Get module Kernel32.dll, can not hook CreateprocessA");
		return -1;
	}
	if((old_api =(APIFUNC)GetProcAddress(hmod, "CreateProcessA")) ==NULL)
	{
		WriteLog("Error Get ProcAddress CreateProcessA of module Kernel32.dll");
		return -1;
	}
	if((pinfo =g_api_info.Add("Kernel32.dll", "CreateProcessA", "myCreateprocessA2003", 10, old_api,
		(APIFUNC)myCreateProcessA2003)) ==NULL)
	{
		WriteLog("Error add APIINFO of CreateProcessA");
		return -1;
	}

	if((old_api =(APIFUNC)GetProcAddress(hmod, "CreateProcessW")) ==NULL)
	{
		WriteLog("Error Get ProcAddress CreateProcessW of module Kernel32.dll");
		return -1;
	}
	if((pinfo =g_api_info.Add("Kernel32.dll", "CreateProcessW", "myCreateprocessW2003", 10, old_api,
		(APIFUNC)myCreateProcessW2003)) ==NULL)
	{
		WriteLog("Error add APIINFO of CreateProcessW");
		return -1;
	}
#endif

	//WriteLog("debug: Init ok!");

	return 0;
}

// hook one api
int CHookAPI::HookOneAPI(APIINFO *pinfo)
{
	//WriteLog("debug: Hook one api:%s, start_pos:%d", pinfo->api_name, pinfo->start_pos);
	if(pinfo->f_hooked)
		return 0;

	BYTE *papi =(BYTE *)pinfo->old_api + pinfo->start_pos;  // insert call xxxxxxxx from start_pos. lgd 2003.03.01

	//将函数所指内容改为可写
	if(!RemoveProtection(pinfo))
	{
		WriteLog("RemoveProtection failed! %s", pinfo->api_name);
		return -1;
	}

	// 保存源函数前5个字节，因为后面将覆盖此处
	memcpy(pinfo->save_bytes, papi, 5/*sizeof(pinfo->save_bytes)*/);

	//在源函数前插入call ProcessCall命令
	papi[0] =0xE8;
	*(DWORD *)&papi[1] =(DWORD)ProcessCall -(DWORD)papi -CALL_BYTES_SIZE;

	pinfo->f_hooked =true;

	WriteLog("debug: Hook one api ok:hmod=%x, %s-%x(%x,%x,%x,%x), start_pos:%d", 
		GetModuleHandle(pinfo->module_name), pinfo->api_name,
		papi, papi[0], papi[1], pinfo->save_bytes[0], pinfo->save_bytes[1], pinfo->start_pos);
		
	return 0;
}

int CHookAPI::UnhookOneAPI(APIINFO *pinfo)
{
	if(!pinfo->f_hooked) return 0;

	// ligang, 2003.04.19
	// check if already unhooked in the first hook for same old api like that CreateProcessW are hooked in HookAPINT.dll and mydll.dll
	// or: unhook only once for api that hooked more than once.
	APIINFO *pinfo2 =g_api_info.m_pInfo;
	int f_already_unhooked =0;
	while(pinfo2)
	{
		if(pinfo2 ==pinfo)
			break;
		if(pinfo2->old_api ==pinfo->old_api)  // already unhooked
		{
			f_already_unhooked =true;
			break;
		}
		pinfo2 =pinfo2->pnext;
	}

	if(f_already_unhooked)  // only unhook first hook, because the next hook's save_bytes is not origion bytes of old api
	{
		pinfo->f_hooked =false;
		return 0;
	}

	BYTE *papi =(BYTE *)pinfo->old_api +pinfo->start_pos;

	//char *p =(char *)papi;
	//WriteLog("debug: unHook one api:%s(%x, %x,%x), start_pos:%d", pinfo->api_name, papi, papi[0], papi[1], pinfo->start_pos);

	if(RemoveProtection(pinfo))
		memcpy((PBYTE)papi, pinfo->save_bytes, CALL_BYTES_SIZE);  // 恢复原5个字节
	else WriteLog("UnhookAPIFunction %s RemoveProtection failed!", pinfo->save_bytes);

#ifdef WIN95
	SetPageProtection(papi);
#else
	DWORD dwScratch;
	VirtualProtect(papi, 20, pinfo->old_protection_flags, &dwScratch);
#endif
	pinfo->f_hooked =false;

	return 0;
}

int CHookAPI::HookAllAPI()
{
	APIINFO *pinfo =g_api_info.m_pInfo;

	while (pinfo !=NULL)
	{
		HookOneAPI(pinfo);
		pinfo =pinfo->pnext;
	}

	return 0;
}

void CHookAPI::UnhookAllAPI()
{
	APIINFO *pinfo =g_api_info.m_pInfo;
	//ObtainSeDebugPrivilege();

	while (pinfo !=NULL)
	{
		UnhookOneAPI(pinfo);
		pinfo =pinfo->pnext;
	}
}

// 主要函数ProcessCall，所有api函数的替换函数
void CHookAPI::ProcessCall()
{
	PBYTE pbAfterCall;
	PDWORD pdwParam;
	PDWORD pdwESP;
	DWORD dwParamSize;
	void *pvReturnAddr;
	DWORD dwReturnValue;
	BYTE cl_val;
	BYTE *papi;
	DWORD errcode =0;

#ifdef WINNT
	PROCESS_INFORMATION *pi;
	char fname[128];
#endif

	_asm
	{
		Mov [cl_val], CL   // 备份CL for xxTextOutx in WIN9X
		Mov EAX, [EBP + 4] //前面是被替换的call xxxxxxxx命令
		Mov [pbAfterCall], EAX

		Mov EAX, [EBP + 8]
		Mov [pvReturnAddr], EAX // 保存上次调用位置

		Lea EAX, [EBP + 12]
		Mov [pdwParam], EAX    //取参数
	}
	APIINFO *pinfo;

#ifdef DEMO_VERSION
	if(++g_demo_count > 200)
		MsgBox("This is demo version, max hook count: 20 exceed...");
#endif

	if((pinfo =g_api_info.FindByOldAPI((APIFUNC)(pbAfterCall-CALL_BYTES_SIZE))) ==NULL)
	{
		WriteLog("ProcessCall: can not found api");
		goto call_ret;
	}
	g_api_info.lock(pinfo);

	papi =(BYTE *)pinfo->old_api+pinfo->start_pos;

	dwParamSize =pinfo->param_count * 4;  // 32位地址使用4个字节

	errcode =0;
	
	if(!RemoveProtection(pinfo))   // add 2004.03.30 for some time other program changed api's write permission
	{
		WriteLog("RemoveProtection failed! %s", pinfo->api_name);
		goto call_ret;
	}

	memcpy(papi, pinfo->save_bytes, CALL_BYTES_SIZE);   //还原原api函数前5字节(恢复api原函数调用)

	if(pinfo->my_friend_api_name[0])			// 如果此api和其他api函数共用部分代码如98中的TextOutA和TextOutW，则先恢复用到的函数的被修改的内容
	{
		WriteLog("Restore my_friend_api_name:%s", pinfo->my_friend_api_name);
		RestoreAPICodes(pinfo->my_friend_api_name);
	}

	// 压参数入堆栈
	_asm
	{
		Sub ESP, [dwParamSize]
		Mov [pdwESP], ESP
	}

	memcpy(pdwESP, pdwParam, dwParamSize);

	_asm
	{
		Mov CL, [cl_val]
	}

	// 调用mydll里的替换函数
	pinfo->my_api();

	_asm
	{
		Push EAX
		Mov [dwReturnValue], EAX  //构造返回值
	}

	errcode =GetLastError();
//#endif
	//MsgBox(pAPIInfo->szAPIName);
#ifdef WINNT
	if(!strcmp(pinfo->api_name, "CreateProcessW") || !strcmp(pinfo->api_name, "CreateProcessA") )
	{
		pi =(PROCESS_INFORMATION *)pdwParam[9];
		if(pi->hProcess)
		{
			//if(g_process_to_hook.Find(process_name))
			wsprintf(fname, "%s\\Elona_KR.dll", g_szDllPath);
			//WaitForSingleObject(pi->hProcess, 0);
			//SuspendThread(pi->hThread);
#ifdef WINNT
			ObtainSeDebugPrivilege();
#endif
			InjectLib(pi->hProcess, fname);  // hook 新进程
			//ResumeThread(pi->hThread);
		}
		else WriteLog("hProcess ==NULL");
	}
#endif

	if(pinfo->my_friend_api_name[0])	// 再恢复上面修改的内容
	{
		WriteLog("Restore my_friend_api_name...");
		RestoreAPICodes(pinfo->my_friend_api_name);
	}

	// 恢复 Call xxxx
	papi[0] =0xE8;
	*(DWORD *)&papi[1] =(DWORD)ProcessCall - (DWORD)papi - CALL_BYTES_SIZE;

	g_api_info.unlock(pinfo);


call_ret:
	// 恢复错误码
	SetLastError(errcode);

	// 构造虚函数返回

	_asm
	{
		Pop EAX

		Mov ECX, [dwParamSize]
		Mov EDX, [pvReturnAddr]
		Pop EDI
		Pop ESI
		Pop EBX
		Mov ESP, EBP
		Pop EBP
		Add ESP, 8
		Add ESP, ECX  //堆栈指针加参数大小，因为ProcessCall没有参数，所以要如此
		Push EDX
		Ret
	}
}

int CHookAPI::RestoreAPICodes(char *my_api_name)
{
	APIINFO *pinfo =g_api_info.FindByMyAPIName(my_api_name);
	if(pinfo ==NULL || pinfo->f_hooked ==false) return 0;

	g_api_info.lock(pinfo);
	memcpy((PBYTE)pinfo->old_api+pinfo->start_pos, pinfo->save_bytes, CALL_BYTES_SIZE/*(5>bytes)?bytes:5*/);
	g_api_info.unlock(pinfo);

	return 0;
}

int CHookAPI::ChangeAPICodes(char *my_api_name)
{
	APIINFO *pinfo =g_api_info.FindByMyAPIName(my_api_name);
	if(pinfo==NULL) return 0;
	
	BYTE *papi =(BYTE *)pinfo->old_api+pinfo->start_pos;

	papi[0] =0xE8;
	*(DWORD *)&papi[1] =(DWORD)ProcessCall -(DWORD)papi -CALL_BYTES_SIZE;

	return 0;
}

// 使某块有写保护的内存地址（代码段内地址）为可写
BOOL CHookAPI::RemoveProtection(APIINFO *pinfo)
{
	BYTE *papi =(BYTE *)pinfo->old_api+pinfo->start_pos;
	BOOL ret;

#ifdef WIN95

	ret =RemovePageProtection(papi);

#endif

#ifdef WINNT

	MEMORY_BASIC_INFORMATION mbi;
	DWORD dwProtectionFlags;
	DWORD dwScratch;

	VirtualQuery(papi, &mbi, sizeof(mbi));

	dwProtectionFlags =mbi.Protect;
	pinfo->old_protection_flags =dwProtectionFlags;

	dwProtectionFlags =PAGE_READWRITE;

	ret =VirtualProtect(papi, 20, dwProtectionFlags, &dwScratch);

#endif

	if(ret ==FALSE)
	{
		char err[256];
		GetErrString(err, sizeof(err), GetLastError());
		WriteLog("Error VirtualProtect:%s", err);
		//WriteLog("VirtualProtect:%x,%x,%x", pvAddress, dwProtectionFlags, dwScratch);
		return false;
	}

	return ret;
}

#ifdef WINNT

// to hook new process
DWORD WINAPI CHookAPI::myCreateProcessW2003(
	LPCWSTR lpApplicationName,
	LPWSTR lpCommandLine, 
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL bInheritHandles,
	DWORD dwCreationFlags,
	LPVOID lpEnvironment,
	LPCWSTR lpCurrentDirectory,
	LPSTARTUPINFOW lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation
)
{
	//WriteLog("HookAPI:CreateProcessW");
	DWORD aaa= CreateProcessW(lpApplicationName,
		lpCommandLine, lpProcessAttributes,
		lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment,
		lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
	char fname[400];
	int len =WideCharToMultiByte( CP_ACP, 0, lpCommandLine, -1, fname, sizeof(fname),NULL,NULL);
	fname[len] =0;
	WriteLog("HookAPI:CreateProcessW:%s", fname);
	return aaa;

}

DWORD WINAPI CHookAPI::myCreateProcessA2003(
	LPCSTR lpApplicationName,
	LPSTR lpCommandLine, 
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL bInheritHandles,
	DWORD dwCreationFlags,
	LPVOID lpEnvironment,
	LPCSTR lpCurrentDirectory,
	LPSTARTUPINFO lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation
)
{
	DWORD aaa= CreateProcessA(lpApplicationName,
		lpCommandLine, lpProcessAttributes,
		lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment,
		lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
	WriteLog("HookAPI:CreateProcessA:%s", lpCommandLine);
	return aaa;
}
#endif
