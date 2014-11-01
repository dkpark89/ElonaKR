#ifndef __INJLIB_H__
#define __INJLIB_H__

int WINAPI InjectLib(DWORD process_id, char *lib_name);
int WINAPI InjectLib(HANDLE hProcess, char *lib_name);
int WINAPI EjectLib(DWORD process_id, char *lib_name);

#endif
