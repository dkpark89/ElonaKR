#include <windows.h>
#include <stdio.h>
#include <stdarg.h>

#include "util.h"

int GetFilePath(char *fname, char *path)
{
	*path =0;

	char *p =fname+strlen(fname);
	while(p > fname)
	{
		if(*p =='\\' || *p =='//')
		{
			strncpy(path,  fname, p-fname);
			path[p-fname] =0;
			return 0;
		}
		p--;
	}

	return -1;
}

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
	
	int pos = SetFilePointer (hFile, 0, NULL, FILE_END);
	
	if (pos != -1) // Test for failure
	{ 
		wsprintf(temp, "%s:", modname);
		DWORD dw;
		WriteFile(hFile, temp, strlen(temp), &dw, NULL);
		
		va_start(args,fmt);
		vsprintf(temp, fmt, args);
		va_end(args);
		
		WriteFile(hFile, temp, strlen(temp), &dw, NULL);
		
		wsprintf(temp, "\r\n");
		WriteFile(hFile, temp, strlen(temp), &dw, NULL);
	}

	CloseHandle(hFile);
}

void WriteLog2(char *fname, char *fmt,...)
{
	return;
	va_list args;
	char modname[200];

	char temp[5000];
	HANDLE hFile;

	GetModuleFileName(NULL, modname, sizeof(modname));

	if((hFile =CreateFile(fname, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL)) <0)
	{
		return;
	}
	
	_llseek((HFILE)hFile, 0, SEEK_END);

	wsprintf(temp, "%s:", modname);
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

char * GetErrString(char *msg, DWORD msg_size, DWORD err)
{
	*msg =0;

	if(FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, err, 0, msg, msg_size, NULL))
		return NULL;

	return msg;
}
