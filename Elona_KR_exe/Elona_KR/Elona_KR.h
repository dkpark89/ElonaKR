// Elona_KR.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.

#define NUM_RAW	20

// CElona_KRApp:
// �� Ŭ������ ������ ���ؼ��� Elona_KR.cpp�� �����Ͻʽÿ�.
//

class CElona_KRApp : public CWinApp
{
public:
	CElona_KRApp();

// �������Դϴ�.
	public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CElona_KRApp theApp;