// ConsoleWindow.h: interface for the CConsoleWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONSOLEWINDOW_H__DAADB9D5_8B79_4D4B_8032_1AE48B5AC8EF__INCLUDED_)
#define AFX_CONSOLEWINDOW_H__DAADB9D5_8B79_4D4B_8032_1AE48B5AC8EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define MAX_BUF_LEN		4096


//////////////////////////////////////////////////////////////////////////
class CConsoleWindow  
{
public:
	void WaitForAnyKeyEx();
	void CopyToClipboard(PCHAR_INFO pInfo, PCOORD pCoord);
	void WaitForAnyKey();
	CConsoleWindow(CWnd *pParent = NULL);
	virtual ~CConsoleWindow();

	BOOL SetTile(LPCTSTR lpTile);
	BOOL WriteString(LPCTSTR lpString);
	BOOL WriteString(WORD Attrs,LPCTSTR lpString);

	//
	DBWindowWrite(LPCTSTR fmt,...);
	DBWindowWrite(WORD Attrs,LPCTSTR fmt,...);

private:
	HANDLE m_hConsole;
	BOOL   m_bCreate;
	BOOL   m_bAttrs;
	WORD   m_OldColorAttrs; 
protected:
	CWnd* m_pParent;

private:
	//
	SHORT GetConX(HANDLE hCon);

	// 
	SHORT GetConY(HANDLE hCon);

	//
	BOOL Puts(HANDLE hConsole, PCHAR s);

	//
	void InvertRectangle(HANDLE hConOut, COORD coordStart, COORD coordCur);
};

#endif // !defined(AFX_CONSOLEWINDOW_H__DAADB9D5_8B79_4D4B_8032_1AE48B5AC8EF__INCLUDED_)
