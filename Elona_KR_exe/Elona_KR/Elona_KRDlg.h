// Elona_KRDlg.h : ��� ����
//

#pragma once
#include "afxwin.h"
#include "multilinelist.h"


// CElona_KRDlg ��ȭ ����
class CElona_KRDlg : public CDialog
{
// �����Դϴ�.
public:
	CElona_KRDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ELONA_KR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	BOOL m_initialized;
	bool m_bCheck;
	CSize m_edgeDelta;
	void ChangePos(int mode);
	CWnd* m_pWndElona;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL DestroyWindow();
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
public:
	CMultilineList m_items;
public:
	afx_msg void OnBnClickedButton1();
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
public:
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
public:
	afx_msg void OnBnClickedButtonHide();
	afx_msg void OnStnClickedStaticTitle();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
	CStatic m_CtrlStaticMove;
public:
	CButton m_CtrlBtnHide;
public:
	CButton m_CtrlBtn1;
	CButton m_CtrlCheck1;
	afx_msg void OnBnClickedCheck1();
};
