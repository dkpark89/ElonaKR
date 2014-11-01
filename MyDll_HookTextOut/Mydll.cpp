#include "stdafx.h"
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mydll.h"
#include "util.h"
#include <comdef.h>
#include <CRTDBG.H>
#include <atlconv.h> 
#include "../trans_kr.h"
#include "usp10.h"

#pragma comment(lib, "atls.lib")
#ifdef WIN95
#pragma code_seg("_INIT")
#pragma comment(linker,"/SECTION:.bss,RWS /SECTION:.data,RWS /SECTION:.rdata,RWS /SECTION:.text,RWS /SECTION:_INIT,RWS ")
#pragma comment(linker,"/BASE:0xBFF70000")
#endif


BOOL WINAPI myExtTextOutW(HDC hDC, int x, int y, UINT options, const RECT *lprc, LPCWSTR str, UINT len, CONST INT *lpDx)
{
	char* tmpStr;
	WCHAR* transStr;
	int trans_len;
	USES_CONVERSION;
	tmpStr = W2A((LPWSTR)str);
	tmpStr[len]=0;
	transStr = A2W(ko_translate(tmpStr));
	trans_len = wcslen(transStr);
	return ExtTextOutW(hDC, x, y, options, lprc, transStr, trans_len, lpDx);
}

int WINAPI myDrawTextA(HDC hDC, LPCSTR str, int len,  LPRECT lprc, UINT uFormat)
{
	const char* transStr;
	int trans_len;
	transStr = ko_translate((char*)str);
	trans_len = strlen(transStr);
	return DrawTextA(hDC, transStr, trans_len, lprc, uFormat);
}

LPSTR WINAPI mylstrcpyA(LPSTR str, LPSTR str2)
{
	const char* transStr;
	int trans_len;
	transStr = ko_translate(str);
	trans_len = strlen(transStr);
	return lstrcpyA((LPSTR)transStr, str2);
}

LPWSTR WINAPI mylstrcpyW(LPWSTR str, LPWSTR str2)
{
	char* tmpStr;
	WCHAR* transStr;
	int trans_len;
	int len = wcslen(str);
	USES_CONVERSION;
	tmpStr = W2A((LPWSTR)str);
	tmpStr[len]=0;
	transStr = A2W(ko_translate(tmpStr));
	trans_len = wcslen(transStr);
	return NULL;
	return lstrcpyW((LPWSTR)transStr, str2);
}

LPSTR WINAPI mylstrcatA(LPSTR str, LPSTR str2)
{
	const char* transStr;
	int trans_len;
	transStr = ko_translate(str);
	trans_len = strlen(transStr);
	return lstrcatA((LPSTR)transStr, str2);
}

LPWSTR WINAPI mylstrcatW(LPWSTR str, LPWSTR str2)
{
	char* tmpStr;
	WCHAR* transStr;
	int trans_len;
	int len = wcslen(str);
	USES_CONVERSION;
	tmpStr = W2A((LPWSTR)str);
	tmpStr[len]=0;
	transStr = A2W(ko_translate(tmpStr));
	trans_len = wcslen(transStr);
	return NULL;
	return lstrcatW((LPWSTR)transStr, str2);
}

int WINAPI myDrawTextW(HDC hDC, LPCWSTR str, int len,  LPRECT lprc, UINT uFormat)
{
	char* tmpStr;
	WCHAR* transStr;
	int trans_len;
	USES_CONVERSION;
	tmpStr = W2A((LPWSTR)str);
	tmpStr[len]=0;
	transStr = A2W(ko_translate(tmpStr));
	trans_len = wcslen(transStr);
	return DrawTextW(hDC, transStr, trans_len, lprc, uFormat);
}

int WINAPI myDrawTextExA(HDC hDC, LPSTR str, int len, LPRECT lprc, UINT uFormat, LPDRAWTEXTPARAMS lpdtp)
{
	const char* transStr;
	int trans_len;
	transStr = ko_translate(str);
	trans_len = strlen(transStr);
	return DrawTextExA(hDC, (LPSTR)transStr, trans_len, lprc, uFormat, lpdtp);
}

int WINAPI myDrawTextExW(HDC hDC, LPWSTR str, int len, LPRECT lprc, UINT uFormat, LPDRAWTEXTPARAMS lpdtp)
{
	char* tmpStr;
	WCHAR* transStr;
	int trans_len;
	USES_CONVERSION;
	tmpStr = W2A((LPWSTR)str);
	tmpStr[len]=0;
	transStr = A2W(ko_translate(tmpStr));
	trans_len = wcslen(transStr);
	return DrawTextExW(hDC, transStr, trans_len, lprc, uFormat, lpdtp);
}

int WINAPI mySetWindowTextA(HWND hWnd, LPSTR str)
{
	const char* transStr;
	int trans_len;
	USES_CONVERSION;
	transStr = ko_translate(str);
	trans_len = strlen(transStr);
	return SetWindowTextA(hWnd, (LPSTR)transStr);
}

char gBuffer[1024]={0,};
int W, H;
HWND m_hWndElona=NULL;
void GetWH(void)
{
#define NUM_WIDTH	5
#define NUM_HEIGHT	4
	int width[NUM_WIDTH]={800, 1024, 1152, 1280, 1680};
	int height[NUM_WIDTH]={600, 696, 768, 864};
	int diff, i;
	int gap=0;
	tagRECT rect;
	if(m_hWndElona==NULL)
	{
		m_hWndElona = FindWindow("hspwnd0", NULL);
		if(m_hWndElona)
		{
			GetWindowRect(m_hWndElona, &rect);
			for(i=0; i<NUM_WIDTH; i++)
			{
				diff = (rect.right-rect.left)-width[i];
				if(diff>=0)
				{
					W = width[i];
				}
				else
					break;
			}
			for(i=0; i<NUM_HEIGHT; i++)
			{
				diff = (rect.bottom-rect.top)-height[i];
				if(diff>=0)
				{
					H = height[i];
				}
				else
					break;
			}
		}
	}
}

int StatusWnd(int x, int y)
{
	GetWH();
	if(x>143 && y>H-84 && x<W && y<H-20) return 1;
//	if(x>100 && y>100 && x<685 && y<470) return 2;
	return 0;
}

int WINAPI myTabbedTextOutA(HDC hDC, int X, int Y, LPSTR str, int len, int nTabPositions, LPINT lpnTabStopPositions, int nTabOrigin)
{
	const char* transStr;
	int trans_len;
	COPYDATASTRUCT cds;
    cds.dwData = 1004; // 사용 목적에 따른 식별 값
	HWND hWnd = FindWindow("#32770","Elona_KR Ver 0.6");
	int mode;
	char ch;
	static bool bQuata = false; // 인용부호 추리기
	static bool bFirst = true; 

	mode = StatusWnd(X,Y);

	if(len>1 && mode)
	{
		if(str[len-1]==' ')
		{
			if(str[len-2]==' ')
			{
				ch=str[len-3];
			}
			else
			{
				ch = str[len-2];
			}
			switch(ch)
			{
			case '\"':
				if(!bQuata && str[0]!='\"')
				{
					break;
				}
				else
				{
					bQuata = false;
				}
				break;
			case '.':
			case '!':
			case '?':
			case '*':
				if(!bQuata && str[0]!='\"')
				{
					break;
				}
			default:
				if(bFirst)
				{
					sprintf(gBuffer, "[%04d,%03d,%d] ", X, Y, mode);
					if(str[0]=='\"')
					{
						bQuata = true;
					}
					bFirst = false;
				}
				strcat(gBuffer, str);
				return TabbedTextOutA(hDC, X, Y, str, len, nTabPositions, lpnTabStopPositions, nTabOrigin);
			}
		}
	}
	if(!bFirst && gBuffer[0])
	{
//		if(mode==prevMode) strcat(gBuffer, str);
		if(mode) strcat(gBuffer, str);
		const char* gBuffer2;
		gBuffer2 = ko_translate(gBuffer+13);
		strcpy(gBuffer+13, gBuffer2);
		cds.cbData = strlen(gBuffer)+1; 
		cds.lpData = (LPSTR)(LPCSTR)gBuffer; 
		if(hWnd) SendMessage(hWnd, WM_COPYDATA, 0, (LPARAM)&cds);

		gBuffer[0]=0;
		bFirst = true;
		return TabbedTextOutA(hDC, X, Y, str, len, nTabPositions, lpnTabStopPositions, nTabOrigin);
	}
	bFirst = true;
	bQuata = false;
	transStr = ko_translate(str);
	trans_len = strlen(transStr);
	return TabbedTextOutA(hDC, X, Y, (LPSTR)transStr, trans_len, nTabPositions, lpnTabStopPositions, nTabOrigin);
}

LONG WINAPI myTabbedTextOutW(HDC hDC, int X, int Y, LPWSTR str, int len, int nTabPositions, LPINT lpnTabStopPositions, int nTabOrigin)
{
	char* tmpStr;
	WCHAR* transStr;
	int trans_len;
	USES_CONVERSION;
	tmpStr = W2A((LPWSTR)str);
	tmpStr[len]=0;
	transStr = A2W(ko_translate(tmpStr));
	trans_len = wcslen(transStr);
	return TabbedTextOutW(hDC, X, Y, transStr, trans_len, nTabPositions, lpnTabStopPositions, nTabOrigin);
}

HRESULT myScriptStringOut
 (
  SCRIPT_STRING_ANALYSIS ssa,
  int                    iX,
  int                    iY,
  UINT                   uOptions,
  const RECT*            prc,
  int                    iMinSel,
  int                    iMaxSel,
  BOOL                   fDisabled
 )
{
	HRESULT ret;
	ret = ScriptStringOut(ssa, iX, iY, uOptions, prc, iMinSel, iMaxSel, fDisabled);
//	g_flush = true;
	return ret;
}

HRESULT WINAPI myScriptStringAnalyse(
  HDC hdc,
  const void* pString,
  int cString,
  int cGlyphs,
  int iCharset,
  DWORD dwFlags,
  int iReqWidth,
  SCRIPT_CONTROL* psControl,
  SCRIPT_STATE* psState,
  const int* piDx,
  SCRIPT_TABDEF* pTabdef,
  const BYTE* pbInClass,
  SCRIPT_STRING_ANALYSIS* pssa
)
{
	char* tmpStr;
	WCHAR* transStr;
	int trans_len;
	USES_CONVERSION;
	tmpStr = W2A((LPWSTR)pString);
	tmpStr[cString]=0;
	transStr = A2W(ko_translate(tmpStr));
	trans_len = wcslen(transStr);
	return ScriptStringAnalyse(hdc, transStr, trans_len, cGlyphs, iCharset, dwFlags, iReqWidth, psControl, psState, piDx, pTabdef, pbInClass, pssa);
}

HRESULT WINAPI myScriptTextOut(const HDC hdc, SCRIPT_CACHE *psc, int x, int y, UINT fuOptions, 
                             const RECT *lprc, const SCRIPT_ANALYSIS *psa, const WCHAR *pwcReserved, 
                             int iReserved, const WORD *pwGlyphs, int cGlyphs, const int *piAdvance,
                             const int *piJustify, const GOFFSET *pGoffset)
{
	char* tmpStr;
	WCHAR* transStr;
	int trans_len;
	USES_CONVERSION;
	tmpStr = W2A((LPWSTR)pwGlyphs);
	tmpStr[cGlyphs]=0;
	transStr = A2W(ko_translate(tmpStr));
	trans_len = wcslen(transStr);

	return ScriptTextOut(hdc, psc, x, y, fuOptions, 
                             lprc, psa, pwcReserved, 
                             iReserved, (const WORD*)transStr, trans_len, piAdvance,
                             piJustify, pGoffset);
}

MYAPIINFO myapi_info[] =
{
	{"user32.DLL", "TabbedTextOutA", 8, "myTabbedTextOutA"},

//	{"kernel32.dll", "lstrcpyA", 2, "mylstrcpyA"},
//	{"kernel32.dll", "lstrcpyW", 2, "mylstrcpyW"},
//	{"kernel32.dll", "lstrcatA", 2, "mylstrcatA"},
//	{"kernel32.dll", "lstrcatW", 2, "mylstrcatW"},

//	{"user32.DLL", "DrawTextW", 5, "myDrawTextW"},
//	{"user32.DLL", "DrawTextExW", 6, "myDrawTextExW"}, //ActiveMovie Window, Elona ver 1.14
//	{"GDI32.DLL", "ExtTextOutW", 8, "myExtTextOutW"}, // 한글자씩 쓴다
//	{"USP10.DLL", "ScriptStringOut", 13, "myScriptStringOut"},
//	{"USP10.DLL", "ScriptStringAnalyse", 13, "myScriptStringAnalyse"}, // 이거도 좀. 한글자씩 쓰기도 하고 .. ㄷㄷ
//	{"user32.DLL", "SetWindowTextA", 2, "mySetWindowTextA"},
	{NULL, NULL, NULL}
};

// 苟죗코휭헝꼇狼錦맣
MYAPIINFO *GetMyAPIInfo()
{
	return &myapi_info[0];
}


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	if(ul_reason_for_call =DLL_PROCESS_ATTACH)
	{
		//GetProfileString("HookAPI", "dll_path", "", g_szDllPath, sizeof(g_szDllPath));
	}
	return TRUE;
}

