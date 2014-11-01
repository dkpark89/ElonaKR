// APIInfo.h: interface for the CAPIInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APIINFO_H__48FBE38B_70DC_4906_A99D_4314ED371ECF__INCLUDED_)
#define AFX_APIINFO_H__48FBE38B_70DC_4906_A99D_4314ED371ECF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef void (WINAPI *APIFUNC)(void);

typedef struct tagAPIINFO
{
	char module_name[100];
	char api_name[50];
	char my_api_name[50];
	char my_friend_api_name[50];
	int param_count;
	int start_pos;
	APIFUNC old_api, my_api;
	int f_hooked;
	BYTE save_bytes[20];
#ifdef WIN95
	HANDLE hMutex, m_hMutex;
#else
	CRITICAL_SECTION cs;
	DWORD old_protection_flags;
#endif
	tagAPIINFO *pnext;
}APIINFO;

class CAPIInfo
{
public:
	CAPIInfo();
	virtual ~CAPIInfo();
	
	int m_count;
	APIINFO *m_pInfo;

	APIINFO *Add(char *module_name, char *api_name, char *my_api_name,
		int param_count, APIFUNC old_api=NULL, APIFUNC my_api =NULL,
		char *my_friend_api_name =NULL, int start_pos=0);
	int DeleteAll();
	
	APIINFO *FindByMyAPI(APIFUNC my_api);
	APIINFO *FindByOldAPI(APIFUNC old_api);
	APIINFO *FindByAPIName(char *api_name);
	APIINFO *FindByMyAPIName(char *my_api_name);

	int lock(APIINFO *pinfo);
	int unlock(APIINFO *pinfo);
};

#endif // !defined(AFX_APIINFO_H__48FBE38B_70DC_4906_A99D_4314ED371ECF__INCLUDED_)
