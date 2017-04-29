// EnumModulesTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EnumModulesTest.h"
#include "EnumModulesTestDlg.h"
#include "EnumWinModules.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEnumModulesTestDlg dialog

CEnumModulesTestDlg::CEnumModulesTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEnumModulesTestDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEnumModulesTestDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CEnumModulesTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEnumModulesTestDlg)
	DDX_Control(pDX, IDC_VISLIST, m_lcVisible);
	DDX_Control(pDX, IDC_INVISLIST, m_lcInvisible);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEnumModulesTestDlg, CDialog)
	//{{AFX_MSG_MAP(CEnumModulesTestDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEnumModulesTestDlg message handlers

BOOL CEnumModulesTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here

	// setup list control headers
	m_lcVisible.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_lcVisible.InsertColumn(0, "Window Title", LVCFMT_LEFT, 150);
	m_lcVisible.InsertColumn(1, "Class Name", LVCFMT_LEFT, 100);
	m_lcVisible.InsertColumn(2, "Module Name", LVCFMT_LEFT, 241);

	m_lcInvisible.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_lcInvisible.InsertColumn(0, "Window Title", LVCFMT_LEFT, 150);
	m_lcInvisible.InsertColumn(1, "Class Name", LVCFMT_LEFT, 100);
	m_lcInvisible.InsertColumn(2, "Module Name", LVCFMT_LEFT, 241);

	RefreshLists();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CEnumModulesTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CEnumModulesTestDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CEnumModulesTestDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CEnumModulesTestDlg::RefreshLists()
{	
	CEnumWinModules wm;

	// show visible windows only
	wm.Process(FILTER_VISIBLEONLY);

	for (int i=0;i<wm.GetWindowCount();i++)
	{
		m_lcVisible.InsertItem(i, wm.GetWindowTitle(i));
		m_lcVisible.SetItemText(i, 1, wm.GetClassName(i));
		m_lcVisible.SetItemText(i, 2, wm.GetModuleName(i));
	}


	// show invisible windows only
	wm.Clear();
	wm.Process(FILTER_NONVISIBLEONLY);

	for (i=0;i<wm.GetWindowCount();i++)
	{
		m_lcInvisible.InsertItem(i, wm.GetWindowTitle(i));
		m_lcInvisible.SetItemText(i, 1, wm.GetClassName(i));
		m_lcInvisible.SetItemText(i, 2, wm.GetModuleName(i));
	}
}
