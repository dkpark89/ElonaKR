#ifndef __UTIL_H__
#define __UTIL_H__

void WriteLog(char *fmt,...);
void WriteLog2(char *fname, char *fmt,...);
char * GetErrString(char *msg, DWORD msg_size, DWORD err);
int GetFilePath(char *fname, char *path);

#endif
