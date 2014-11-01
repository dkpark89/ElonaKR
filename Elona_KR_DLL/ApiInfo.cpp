// APIInfo.cpp: implementation of the CAPIInfo class.
//
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#include "APIInfo.h"
#include "util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#ifdef WIN95
APIINFO g_apiinfo_buf[1000];
#endif

CAPIInfo::CAPIInfo()
{
	m_count =0;
	m_pInfo =NULL;
}

CAPIInfo::~CAPIInfo()
{
	DeleteAll();
}

APIINFO *CAPIInfo::Add(char *module_name, char *api_name, char *my_api_name,
		int param_count, APIFUNC old_api, APIFUNC my_api, char *my_friend_api_name, int start_pos)
{
#ifdef WIN95
	APIINFO *pinfo =(APIINFO *)&g_apiinfo_buf[m_count];
#else
	APIINFO *pinfo =(APIINFO *)malloc(sizeof(APIINFO));
#endif

	strcpy(pinfo->module_name, module_name);
	strcpy(pinfo->api_name, api_name);
	strcpy(pinfo->my_api_name, my_api_name);
	pinfo->param_count =param_count;

	pinfo->old_api =old_api;
	pinfo->my_api =my_api;

	if(my_friend_api_name)
		strcpy(pinfo->my_friend_api_name, my_friend_api_name);
	else pinfo->my_friend_api_name[0] =0;

	pinfo->start_pos =start_pos;
	pinfo->f_hooked =false;

#ifdef WIN95
	pinfo->hMutex =CreateMutex(NULL, FALSE, api_name);
#endif

#ifdef WINNT
	InitializeCriticalSection(&pinfo->cs);
#endif

	pinfo->pnext =NULL;
	APIINFO *pinfo2 =m_pInfo;
	while(pinfo2 !=NULL && pinfo2->pnext !=NULL)
		pinfo2 =pinfo2->pnext;
	if(pinfo2 ==NULL) m_pInfo =pinfo;             // this is head of m_pInfo
	else pinfo2->pnext =pinfo;

	m_count++;

	return pinfo;
}

int CAPIInfo::DeleteAll()
{
	APIINFO *pinfo =m_pInfo, *pinfo2;
	while(pinfo)
	{
		pinfo2 =pinfo;
		pinfo =pinfo->pnext;
#ifdef WIN95
		if (pinfo2->hMutex)
			CloseHandle(pinfo2->hMutex);
#else
		DeleteCriticalSection(&pinfo2->cs);
		free(pinfo2);
#endif
	}

	m_pInfo =NULL;
	m_count =0;

	return 0;
}

APIINFO *CAPIInfo::FindByMyAPI(APIFUNC my_api)
{
	APIINFO *pinfo =m_pInfo;

	while(pinfo)
	{
		if(pinfo->my_api ==my_api)
			break;
		pinfo =pinfo->pnext;
	}

	return pinfo;
}

APIINFO *CAPIInfo::FindByOldAPI(APIFUNC old_api)
{
	APIINFO *pinfo =m_pInfo;

	while(pinfo)
	{
		if((BYTE *)pinfo->old_api+pinfo->start_pos ==(BYTE *)old_api)
			break;
		pinfo =pinfo->pnext;
	}

	return pinfo;
}

APIINFO *CAPIInfo::FindByAPIName(char *api_name)
{
	APIINFO *pinfo =m_pInfo;

	while(pinfo)
	{
		if(!strcmp(pinfo->api_name, api_name))
			break;
		pinfo =pinfo->pnext;
	}

	return pinfo;
}

APIINFO *CAPIInfo::FindByMyAPIName(char *my_api_name)
{
	APIINFO *pinfo =m_pInfo;

	while(pinfo)
	{
		if(!strcmp(pinfo->my_api_name, my_api_name))
			break;
		pinfo =pinfo->pnext;
	}

	return pinfo;
}

int CAPIInfo::lock(APIINFO *pinfo)
{
#ifdef WIN95
	if((pinfo->m_hMutex =OpenMutex(MUTEX_ALL_ACCESS, FALSE, pinfo->api_name)) !=NULL)
		WaitForSingleObject(pinfo->m_hMutex, INFINITE);
#else
	EnterCriticalSection(&pinfo->cs);
#endif

	return 0;
}

int CAPIInfo::unlock(APIINFO *pinfo)
{
#ifdef WIN95
	if(pinfo->m_hMutex)
	{
		ReleaseMutex(pinfo->m_hMutex);
		CloseHandle(pinfo->m_hMutex);
		pinfo->m_hMutex =NULL;
	}
#else
	LeaveCriticalSection(&pinfo->cs);
#endif

	return 0;
}
