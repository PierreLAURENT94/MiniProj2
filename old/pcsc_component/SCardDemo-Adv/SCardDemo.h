// SCardDemo.h : main header file for the SCARDDEMO application
//

#if !defined(AFX_SCARDDEMO_H__52077E4B_2C21_472E_9CD4_D6033C82CCE7__INCLUDED_)
#define AFX_SCARDDEMO_H__52077E4B_2C21_472E_9CD4_D6033C82CCE7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSCardDemoApp:
// See SCardDemo.cpp for the implementation of this class
//

class CSCardDemoApp : public CWinApp
{
public:
	CSCardDemoApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSCardDemoApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSCardDemoApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCARDDEMO_H__52077E4B_2C21_472E_9CD4_D6033C82CCE7__INCLUDED_)
