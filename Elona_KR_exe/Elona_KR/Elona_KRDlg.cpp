// Elona_KRDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "Elona_KR.h"
#include "Elona_KRDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CElona_KRDlg ��ȭ ����




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


// CElona_KRDlg �޽��� ó����
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

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	WriteProfileString(_T("Elona_KR"), _T("exe_name"), _T("Elona_KR.exe"));
	hLib =LoadLibrary(_T("Elona_KR.dll"));
	if(hLib ==NULL)
	{
		AfxMessageBox(_T("Elona_KR.dll�� ã�� ���߽��ϴ�"));
		OnOK();
		return false;
	}
	HookOneProcess2 =(FuncHookOneProcess2)GetProcAddress(hLib, "HookOneProcess2");
	UnhookOneProcess2 =(FuncUnhookOneProcess2)GetProcAddress(hLib, "UnhookOneProcess2");
	ReloadDic =(FuncReloadDic)GetProcAddress(hLib, "ReloadDic");
	if(HookOneProcess2 ==NULL)
	{
		AfxMessageBox(_T("��ŷ ���� - 1"));
		FreeLibrary(hLib);
		OnOK();
		return false;
	}
	
	if(UnhookOneProcess2 ==NULL)
	{
		AfxMessageBox(_T("��ŷ ���� - 2"));
		FreeLibrary(hLib);
		OnOK();
		return false;
	}

	if(ReloadDic ==NULL)
	{
		AfxMessageBox(_T("��ŷ ���� - 3"));
		FreeLibrary(hLib);
		OnOK();
		return false;
	}

	if(HookOneProcess2(NULL, "elona.exe") <0)
	{
		AfxMessageBox(_T("��ŷ ���� -4"));
		UnhookOneProcess2("elona.exe");
		FreeLibrary(hLib);
		OnOK();
		return false;
	}
	STARTUPINFO StartupInfo = {0}; 
	if(!::CreateProcess(_T("elona.exe"), _T(""), NULL, NULL, FALSE, 0, NULL,NULL, &StartupInfo, &ProcessInfo))
	{
		AfxMessageBox(_T("elona.exe �� �������� ���߽��ϴ�. �����մϴ�."));
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
	m_items.SetCellText(0,NUM_RAW-2,_T("  Elona �ѱ� ��ġ ���� Ver 0.6"));
	m_items.SetCellText(0,NUM_RAW-1,_T("  http://nethack.byus.net"));
	m_items.EnsureRowIsVisible(NUM_RAW-1);
	this->SetWindowText(L"Elona_KR Ver 0.6");

	m_bCheck = true;
	m_CtrlCheck1.SetCheck(m_bCheck);
	SetTimer(1234, 2000, NULL);
	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CElona_KRDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
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
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	int mode;
	switch(pCopyDataStruct->dwData) // ��� �뵵 �Ǵ�
	{
	case 1004: // �۽� ���α׷����� COPYDATASTRUCT ����ü���� dwData�� �����ߴ� ��
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	ShowWindow(SW_HIDE);
	SetTimer(1235, 3000, NULL);
}

BOOL CElona_KRDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if(m_CtrlCheck1.GetCheck()==0)
	{
		TransOn(false);
	}
	else
	{
		TransOn(true);
	}
}
