// ------------------------------------- //
// �����Ҫʹ�ñ��ļ����벻Ҫɾ����˵��  //
// ------------------------------------- //
//             HOOKAPI v1.0              //
//   Copyright 2002 ���ɳ�� Paladin     //
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
