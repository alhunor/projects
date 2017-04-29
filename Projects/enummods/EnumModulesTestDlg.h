// EnumModulesTestDlg.h : header file
//

#if !defined(AFX_ENUMMODULESTESTDLG_H__4FC28B26_116A_4667_837B_B2D130BA1D5B__INCLUDED_)
#define AFX_ENUMMODULESTESTDLG_H__4FC28B26_116A_4667_837B_B2D130BA1D5B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CEnumModulesTestDlg dialog

class CEnumModulesTestDlg : public CDialog
{
// Construction
public:
	CEnumModulesTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CEnumModulesTestDlg)
	enum { IDD = IDD_ENUMMODULESTEST_DIALOG };
	CListCtrl	m_lcVisible;
	CListCtrl	m_lcInvisible;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEnumModulesTestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void RefreshLists(void);
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CEnumModulesTestDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ENUMMODULESTESTDLG_H__4FC28B26_116A_4667_837B_B2D130BA1D5B__INCLUDED_)
