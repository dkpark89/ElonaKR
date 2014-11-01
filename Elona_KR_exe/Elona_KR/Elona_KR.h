// Elona_KR.h : PROJECT_NAME 응용 프로그램에 대한 주 헤더 파일입니다.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "resource.h"		// 주 기호입니다.

#define NUM_RAW	20

// CElona_KRApp:
// 이 클래스의 구현에 대해서는 Elona_KR.cpp을 참조하십시오.
//

class CElona_KRApp : public CWinApp
{
public:
	CElona_KRApp();

// 재정의입니다.
	public:
	virtual BOOL InitInstance();

// 구현입니다.

	DECLARE_MESSAGE_MAP()
};

extern CElona_KRApp theApp;