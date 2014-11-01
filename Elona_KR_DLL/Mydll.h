// ------------------------------------- //
// 您如果要使用本文件，请不要删除本说明  //
// ------------------------------------- //
//             HOOKAPI v1.0              //
//   Copyright 2002 编程沙龙 Paladin     //
//       www.ProgramSalon.com            //
// ------------------------------------- //

#ifndef __mydll_h__
#define __mydll_h__

typedef struct
{
	char *module_name;
	char *api_name;
	int param_count;
	char *my_api_name;
	int start_pos;
	char *friend_my_api_name;
}MYAPIINFO;

#endif
