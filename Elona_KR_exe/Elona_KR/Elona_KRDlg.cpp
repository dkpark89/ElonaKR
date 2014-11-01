// Elona_KRDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "Elona_KR.h"
#include "Elona_KRDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CElona_KRDlg 대화 상자




CElona_KRDlg::CElona_KRDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CElona_KRDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_initialized = FALSE;
	m_pWndElona = NULL;
}

void CElona_KRDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CUSTOM1, m_items);
	DDX_Control(pDX, IDC_STATIC_MOVE, m_CtrlStaticMove);
	DDX_Control(pDX, IDC_BUTTON_HIDE, m_CtrlBtnHide);
	DDX_Control(pDX, IDC_BUTTON1, m_CtrlBtn1);
	DDX_Control(pDX, IDC_CHECK1, m_CtrlCheck1);
}

BEGIN_MESSAGE_MAP(CElona_KRDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON1, &CElona_KRDlg::OnBnClickedButton1)
	ON_WM_SIZE()
	ON_WM_COPYDATA()
	ON_BN_CLICKED(IDC_BUTTON_HIDE, &CElona_KRDlg::OnBnClickedButtonHide)
	ON_STN_CLICKED(IDC_STATIC_MOVE, &CElona_KRDlg::OnStnClickedStaticTitle)
	ON_BN_CLICKED(IDC_CHECK1, &CElona_KRDlg::OnBnClickedCheck1)
END_MESSAGE_MAP()


// CElona_KRDlg 메시지 처리기
typedef int (WINAPI *FuncHookOneProcess2)(HWND hwndNotify, char *exe_name);
typedef int (WINAPI *FuncUnhookOneProcess2)(char *exe_name);
typedef int (WINAPI *FuncReloadDic)(void);
HINSTANCE hLib=NULL;
PROCESS_INFORMATION ProcessInfo; 
FuncHookOneProcess2 HookOneProcess2;
FuncUnhookOneProcess2 UnhookOneProcess2;
FuncReloadDic ReloadDic;
bool bHookOk=false;

BOOL CElona_KRDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	WriteProfileString(_T("Elona_KR"), _T("exe_name"), _T("Elona_KR.exe"));
	hLib =LoadLibrary(_T("Elona_KR.dll"));
	if(hLib ==NULL)
	{
		AfxMessageBox(_T("Elona_KR.dll을 찾지 못했습니다"));
		OnOK();
		return false;
	}
	HookOneProcess2 =(FuncHookOneProcess2)GetProcAddress(hLib, "HookOneProcess2");
	UnhookOneProcess2 =(FuncUnhookOneProcess2)GetProcAddress(hLib, "UnhookOneProcess2");
	ReloadDic =(FuncReloadDic)GetProcAddress(hLib, "ReloadDic");
	if(HookOneProcess2 ==NULL)
	{
		AfxMessageBox(_T("후킹 실패 - 1"));
		FreeLibrary(hLib);
		OnOK();
		return false;
	}
	
	if(UnhookOneProcess2 ==NULL)
	{
		AfxMessageBox(_T("후킹 실패 - 2"));
		FreeLibrary(hLib);
		OnOK();
		return false;
	}

	if(ReloadDic ==NULL)
	{
		AfxMessageBox(_T("후킹 실패 - 3"));
		FreeLibrary(hLib);
		OnOK();
		return false;
	}

	if(HookOneProcess2(NULL, "elona.exe") <0)
	{
		AfxMessageBox(_T("후킹 실패 -4"));
		UnhookOneProcess2("elona.exe");
		FreeLibrary(hLib);
		OnOK();
		return false;
	}
	STARTUPINFO StartupInfo = {0}; 
	if(!::CreateProcess(_T("elona.exe"), _T(""), NULL, NULL, FALSE, 0, NULL,NULL, &StartupInfo, &ProcessInfo))
	{
		AfxMessageBox(_T("elona.exe 를 실행하지 못했습니다. 종료합니다."));
		OnOK();
		return false;
	}
	bHookOk= true;
	CRect wr;
	GetWindowRect(wr);
	CRect cr;
	m_items.GetWindowRect(cr);
	m_edgeDelta.cx = wr.right - cr.right;
	m_edgeDelta.cy = wr.bottom - cr.bottom;
	m_initialized = TRUE;
	m_items.SetSize(1,NUM_RAW);
	m_items.SetColWidth(0,cr.Width()-20);
	m_items.SetColHeading(0, _T("Elona_KR Ver 0.6"));
	m_items.SetCellText(0,NUM_RAW-2,_T("  Elona 한글 패치 런쳐 Ver 0.6"));
	m_items.SetCellText(0,NUM_RAW-1,_T("  http://nethack.byus.net"));
	m_items.EnsureRowIsVisible(NUM_RAW-1);
	this->SetWindowText(L"Elona_KR Ver 0.6");

	m_bCheck = true;
	m_CtrlCheck1.SetCheck(m_bCheck);
	SetTimer(1234, 2000, NULL);
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CElona_KRDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CElona_KRDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


BOOL CElona_KRDlg::DestroyWindow()
{
	// TODO: Add your specialized code here and/or call the base class
	if(bHookOk){
		if(UnhookOneProcess2)
			UnhookOneProcess2("elona.exe");
	}
	if(hLib)
		FreeLibrary(hLib);

	return CDialog::DestroyWindow();
}

void CElona_KRDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if(nIDEvent==1234)
	{
		if(WaitForSingleObject(ProcessInfo.hProcess, 1)==0)
			OnOK();
	}
	if(nIDEvent==1235)
	{
		KillTimer(1235);
		ShowWindow(SW_SHOW);
	}
	CDialog::OnTimer(nIDEvent);
}

void CElona_KRDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	ReloadDic();
}

void CElona_KRDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (!m_initialized)
		return;
	CRect wr;
	GetWindowRect(wr);
	CRect cr;
	m_items.GetWindowRect(cr);
	cr.right = wr.right - m_edgeDelta.cx;
	cr.bottom = wr.bottom - m_edgeDelta.cy;
	ScreenToClient(cr);
	m_items.MoveWindow(cr);
	m_items.SetColWidth(0,cr.Width()-20);
	m_items.Invalidate(FALSE);
	m_CtrlCheck1.SetWindowPos(NULL, wr.Width()-85, 2, 40, 20, 0);
	m_CtrlStaticMove.SetWindowPos(NULL, wr.Width()-27-5, 0, 26, 22, 0);
	m_CtrlBtnHide.SetWindowPos(NULL, wr.Width()-(90+3)-2, 22, 90, 20, 0);
	m_CtrlBtn1.SetWindowPos(NULL, wr.Width()-(90+3)-2, 42, 90, 20, 0);

}

void CElona_KRDlg::ChangePos(int mode)
{
#define NUM_WIDTH	5
#define NUM_HEIGHT	4
	int width[NUM_WIDTH]={800, 1024, 1152, 1280, 1680};
	int height[NUM_WIDTH]={600, 696, 768, 864};
	int diff, i;
	static int gap=0;
	static CRect rect;
	static CRect viewRect;
	static int prevMode=-1;
	CRect view;
	if(m_pWndElona==NULL)
	{
		m_pWndElona = FindWindow(_T("hspwnd0"), NULL);
		if(m_pWndElona)
		{
			m_pWndElona->GetWindowRect(&rect);
			for(i=0; i<NUM_WIDTH; i++)
			{
				diff = rect.Width()-width[i];
				if(diff>=0)
				{
					gap = diff/2;
					viewRect.left = rect.left+gap;
					viewRect.right = viewRect.left+width[i];
				}
				else
					break;
			}
			for(i=0; i<NUM_HEIGHT; i++)
			{
				diff = rect.Height()-height[i];
				if(diff>=0)
				{
					viewRect.bottom = rect.bottom-gap;
					viewRect.top = viewRect.bottom-height[i];
				}
				else
					break;
			}
		}
	}
	if(m_pWndElona && mode!=prevMode)
	{
		prevMode = mode;
		switch(mode)
		{
		case 1:
			view.left = viewRect.left+122;
			view.right = viewRect.right;
			view.bottom = viewRect.bottom-18;
			view.top = view.bottom-68;
			MoveWindow(view);
			break;
		}
	}
}

#include "../../trans_kr.h"
TCHAR aa[NUM_RAW][1024];
BOOL CElona_KRDlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	int mode;
	switch(pCopyDataStruct->dwData) // 사용 용도 판단
	{
	case 1004: // 송신 프로그램에서 COPYDATASTRUCT 구조체에서 dwData에 설정했던 값
		mode = ((char*)pCopyDataStruct->lpData)[10]-'0';
		ChangePos(mode);
		USES_CONVERSION;
		for(int i=0; i<NUM_RAW-1;i++)
		{
			_tcsnccpy(aa[i], aa[i+1], 1024);
			m_items.SetCellText(0,i,aa[i]);
		}
#ifdef _DEBUG
//		_tcsnccpy(aa[NUM_RAW-1], A2T((LPSTR)ko_translate((LPSTR)pCopyDataStruct->lpData)), 1024);
		_tcsnccpy(aa[NUM_RAW-1], A2T((LPSTR)pCopyDataStruct->lpData), 1024);
#else
//		_tcsnccpy(aa[NUM_RAW-1], A2T((LPSTR)ko_translate((LPSTR)pCopyDataStruct->lpData+13)), 1024);
		_tcsnccpy(aa[NUM_RAW-1], A2T((LPSTR)pCopyDataStruct->lpData+13), 1024);
#endif
		m_items.SetCellText(0,NUM_RAW-1,aa[NUM_RAW-1]);
		m_items.EnsureRowIsVisible(NUM_RAW-1);
		m_items.SetSelRow(NUM_RAW-1);
		break;
	}

	return CDialog::OnCopyData(pWnd, pCopyDataStruct);
}

void CElona_KRDlg::OnBnClickedButtonHide()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	ShowWindow(SW_HIDE);
	SetTimer(1235, 3000, NULL);
}

BOOL CElona_KRDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(pMsg->message==WM_KEYDOWN){
		if(pMsg->wParam==VK_ESCAPE){
			return TRUE;
		}
		if(pMsg->wParam==VK_RETURN){
			return TRUE;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CElona_KRDlg::OnStnClickedStaticTitle()
{
	// TODO: Add your control notification handler code here
	PostMessage( WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM( 0, 0)); 
}

void CElona_KRDlg::OnBnClickedCheck1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(m_CtrlCheck1.GetCheck()==0)
	{
		TransOn(false);
	}
	else
	{
		TransOn(true);
	}
}
