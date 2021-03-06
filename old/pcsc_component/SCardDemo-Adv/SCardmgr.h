// SCardmgr.h: interface for the CSCardmgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCARDMGR_H__3604BED0_8806_4F02_9732_5F52F877A357__INCLUDED_)
#define AFX_SCARDMGR_H__3604BED0_8806_4F02_9732_5F52F877A357__INCLUDED_

///////////////////////////////////////////////////////////////////////////////
// PCSC Support.
#include "winscard.h"
#pragma comment (lib, "winscard.lib")

#include <afxtempl.h>

#define			MAX_RESPONSE		1024
#define			MAX_APDU_LEN		1024

typedef		struct STU_Reader{
	char	szReaderName[MAX_PATH];
}SCReader, *PSCReader;

///////////////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSCardmgr  
{
public:
	BOOL SCardGetPcscList();
	BOOL SCardTransmit(LPCTSTR strApdu, LPCTSTR strResp, UINT *nSW);
	BOOL SCardReset(LPCTSTR strResp);
	BOOL SCardClose();
	BOOL SCardOpen(int nInx);
	CSCardmgr();
	virtual ~CSCardmgr();

	CArray<SCReader, SCReader>			m_arrReaders;
	char				m_szErrMsg[1024];
	long				lErr;
protected:
	int m_nInx;

	SCARD_IO_REQUEST	IO_Request;
	short			    m_actName;

	BYTE			    pResponseBuffer[MAX_RESPONSE];
	ULONG	            ResponseLength;
	DWORD				ActiveProtocol;
	SCARDCONTEXT		ContextHandle; // Context
    SCARDHANDLE			CardHandle;    // Handle of SCReader
};

#endif // !defined(AFX_SCARDMGR_H__3604BED0_8806_4F02_9732_5F52F877A357__INCLUDED_)
