// ApduProcesser.h: interface for the CApduProcesser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APDUPROCESSER_H__D48287E5_5761_4963_8A03_862AD86B7583__INCLUDED_)
#define AFX_APDUPROCESSER_H__D48287E5_5761_4963_8A03_862AD86B7583__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef MAX_BUF_SIZE
	#define MAX_BUF_SIZE		1024
#endif

class CApduProcesser  
{
public:
	char* GetOneExpResp(int nInx);

	// Dump APDUs via OutputDebugString
	void DumpApdus();

	// Get an APDU from pool
	char * GetOneApdu(int nInx);

	// Get Error Message of last operation.
	char * GetErrorMessage();

	// Get the count of APDUs
	INT GetLineCount();

	// Process the user input: a file, one APDU, or APDUs.
	BOOL ProcessInput(char *szInput);

	// Core: Process one APDU: "0084000008"
	BOOL DoSingleAPDU(char *szSLBuf);

	// Core: Process Multi-APDU: "0084000008;0084000008;"
	BOOL DoMultiAPDU(char *szMLBuf);

	// Core: Process a APDUs' file.
	BOOL DoFile(LPCTSTR szPath);

	//
	CApduProcesser();
	virtual ~CApduProcesser();

protected:

	// Processer
	BOOL ProcessSingleLine(char *szSLBuf);

	CStringArray		m_arrApdu;
	CStringArray		m_arrExpResp;

	char				m_szErrMsg[MAX_BUF_SIZE];

	char				m_szExpResp[MAX_BUF_SIZE];

	char				m_szApdu[MAX_BUF_SIZE];
};

#endif // !defined(AFX_APDUPROCESSER_H__D48287E5_5761_4963_8A03_862AD86B7583__INCLUDED_)
