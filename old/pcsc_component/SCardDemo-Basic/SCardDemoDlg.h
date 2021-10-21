// SCardDemoDlg.h : header file
//

#if !defined(AFX_SCARDDEMODLG_H__1457F09E_89D3_490B_BA62_E62FA7E974B6__INCLUDED_)
#define AFX_SCARDDEMODLG_H__1457F09E_89D3_490B_BA62_E62FA7E974B6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CSCardDemoDlg dialog

#include "SCardmgr.h"
#include "DialogHeader.h"

class CSCardDemoDlg : public CDialog
{
// Construction
public:
	CSCardDemoDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CSCardDemoDlg)
	enum { IDD = IDD_SCARDDEMO_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSCardDemoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void InitDlgHeader();
	void DisplayErr();
	CSCardmgr m_SCard;
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CSCardDemoDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnRefresh();
	afx_msg void OnOpen();
	afx_msg void OnClose();
	afx_msg void OnReset();
	afx_msg void OnCmd();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CDialogHeader		m_HeaderCtrl;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCARDDEMODLG_H__1457F09E_89D3_490B_BA62_E62FA7E974B6__INCLUDED_)
