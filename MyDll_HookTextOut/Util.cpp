#include "stdafx.h"
#include <stdio.h>
#include <io.h>
#include "resource.h"

#include "util.h"

void GetFileName(char *fname)
{
	char temp[200];

	GetModuleFileName(NULL, temp, sizeof(temp));
	int i =strlen(temp);
	while(i >0 && temp[i-1] !='\\' && temp[i-1] !=':') i--;

	strcpy(fname, &temp[i]);
}

// 如果是win9x，不能使用fopen函数
void WriteLog(char *fmt,...)
{
	return;
	va_list args;
	char modname[200];

	char temp[5000];
	HANDLE hFile;

	GetModuleFileName(NULL, modname, sizeof(modname));

	if((hFile =CreateFile("c:\\hookapi.log", GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL)) <0)
	{
		return;
	}
	
	_llseek((HFILE)hFile, 0, SEEK_END);

	wsprintf(temp, "mydll.dll:%s:", modname);
	DWORD dw;
	WriteFile(hFile, temp, strlen(temp), &dw, NULL);
	
	va_start(args,fmt);
	vsprintf(temp, fmt, args);
	va_end(args);

	WriteFile(hFile, temp, strlen(temp), &dw, NULL);

	wsprintf(temp, "\r\n");
	WriteFile(hFile, temp, strlen(temp), &dw, NULL);

	_lclose((HFILE)hFile);
}

int ipcmp(char *szip1, char *szip2)
{
	ULONG ip1 =GetIntIP(szip1);
	ULONG ip2 =GetIntIP(szip2);
	if(ip1 > ip2) return 1;
	else if(ip1 <ip2) return -1;
	else return 0;
}

ULONG GetIntIP(char *szip)
{
	char *p, *p1;
	char ip[16];
	int i;
	ULONG ii[4];
	i =0;
	strcpy(ip, szip);
	p =ip;
	while(*p && i<4)
	{
		p1 =p;
		while(*p && *p !='.') p++;
		*p =0;
		ii[i] =my_atoi(p1);
		p ++;
		i++;
	}
	
	return ii[0]*256*256*256+ii[1]*256*256+ii[2]*256+ii[3];
}

ULONG my_atoi(char *p)
{
	ULONG i;
	
	i =0;
	while(*p)
	{
		i =(i+*p-'0')*10;
		p++;
	}
	i =i/10;
	
	return i;
}

