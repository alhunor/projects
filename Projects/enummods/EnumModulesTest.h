// EnumModulesTest.h : main header file for the ENUMMODULESTEST application
//

#if !defined(AFX_ENUMMODULESTEST_H__990ADB59_F3E8_41A9_A309_A6853FEDAD48__INCLUDED_)
#define AFX_ENUMMODULESTEST_H__990ADB59_F3E8_41A9_A309_A6853FEDAD48__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CEnumModulesTestApp:
// See EnumModulesTest.cpp for the implementation of this class
//

class CEnumModulesTestApp : public CWinApp
{
public:
	CEnumModulesTestApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEnumModulesTestApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CEnumModulesTestApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ENUMMODULESTEST_H__990ADB59_F3E8_41A9_A309_A6853FEDAD48__INCLUDED_)
