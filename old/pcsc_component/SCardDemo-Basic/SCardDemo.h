// CSCardDemo.h : main header file for the SCARDDEMO application
//

#if !defined(AFX_SCARDDEMO_H__51FDCB0C_6373_472A_B33E_44B13865A394__INCLUDED_)
#define AFX_SCARDDEMO_H__51FDCB0C_6373_472A_B33E_44B13865A394__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSCardDemoApp:
// See CSCardDemo.cpp for the implementation of this class
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

#endif // !defined(AFX_SCARDDEMO_H__51FDCB0C_6373_472A_B33E_44B13865A394__INCLUDED_)
