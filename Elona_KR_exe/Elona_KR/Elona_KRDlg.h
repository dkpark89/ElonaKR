// Elona_KRDlg.h : 헤더 파일
//

#pragma once
#include "afxwin.h"
#include "multilinelist.h"


// CElona_KRDlg 대화 상자
class CElona_KRDlg : public CDialog
{
// 생성입니다.
public:
	CElona_KRDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ELONA_KR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	BOOL m_initialized;
	bool m_bCheck;
	CSize m_edgeDelta;
	void ChangePos(int mode);
	CWnd* m_pWndElona;

	// 생성된 메시지 맵 함수
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
