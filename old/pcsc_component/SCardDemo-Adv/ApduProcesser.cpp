// ApduProcesser.cpp: 
//          APDU Processor for APDU/APDUs/APDUS' file
//
//			by Y.Volta Tang (y.volta@gmail.com)
//          2008-01-18 22:06
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ApduProcesser.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CApduProcesser::CApduProcesser()
{	
	m_szErrMsg[0] = 0x0;
}

CApduProcesser::~CApduProcesser()
{

}

// Read APDUs from a file.
BOOL CApduProcesser::DoFile(LPCTSTR szPath)
{
	CFile			file;
	DWORD			dwLen = 0;
	char			*pBuf = NULL;
	BOOL			bRet = FALSE;
	char			szCtrl;

	if(!file.Open(szPath, CFile::modeRead | CFile::shareDenyWrite))
		return FALSE;
	
	dwLen = file.GetLength();
	pBuf = (char*)malloc(dwLen + 10);
	
	if(pBuf == NULL)
	{
		file.Close();
		return FALSE;
	}

	memset(pBuf, 0x0, dwLen);
	file.Read(pBuf, dwLen);

	char	*p = pBuf;
	char	*pcr = pBuf;
	for(UINT i=0; i < dwLen; i ++)
	{
		szCtrl = *p++;

		if(szCtrl == '\n' || szCtrl == '\r')
		{
			*(p-1) = '\0';

			// process this line.
			bRet = DoMultiAPDU(pcr);

// 			if(!bRet)		// sth. wrong.
// 				break;
// 			else
				pcr = p;
		}
	}


	free(pBuf);
	file.Close();

	return bRet;
}


//////////////////////////////////////////////////////////////////////////
// sth. like:
// 0084 0000 08;;;0084 0000 08;;
//
BOOL CApduProcesser::DoMultiAPDU(char *szMLBuf)
{
	char		*pBuf = szMLBuf;
	char		*pUnit = szMLBuf;
	BOOL		bRet = FALSE;
	int			nCnt = 0;

	int			nBufLen = strlen(szMLBuf);
	for(int i=0; i < nBufLen; i ++)
	{
		if(*pBuf++ == ';')
		{
			*(pBuf-1) = 0x0;
			
			bRet = DoSingleAPDU(pUnit);
			
			if(!bRet)
				break;
			else
			{
				nCnt ++;
				pUnit = pBuf;
			}
		}
	}

	// Not Mutlti-APDU found, so try the Single APDU.
	if(nCnt == 0)
		return DoSingleAPDU(pUnit);


	return bRet;
}

//////////////////////////////////////////////////////////////////////////
// sth. like:
// 0084 0000 08
//
BOOL CApduProcesser::DoSingleAPDU(char *szSLBuf)
{
	return ProcessSingleLine(szSLBuf);
}


BOOL CApduProcesser::ProcessInput(char *szInput)
{
	m_arrApdu.RemoveAll();

	if(szInput[1] == ':')
	{	// This is a plain script file.
		return DoFile(szInput);
	}
	else
	{	
		if(strchr(szInput, ';') != NULL)
		{	// multi-apdu specified.
			return DoMultiAPDU(szInput);
		}
		else
		{	// single-apdu specified.
			return DoSingleAPDU(szInput);
		}
	}
	
	return FALSE;
}

// Process a APDU Line.
BOOL CApduProcesser::ProcessSingleLine(char *szSLBuf)
{
	char	szString[1025] = {0};
	char	*pdest = NULL;
	int		result = 0;

	if(strlen(szSLBuf) > 1024)
		return FALSE;

	// expected response?
	pdest = strstr( szSLBuf, "# exp: " );
	result = pdest - szSLBuf + 1;

	if( pdest != NULL )
	{
		m_arrExpResp.Add(pdest+7);

		return TRUE;
	}


	// Eat the non-numeric chars.
	for(UINT i=0, j=0; i < strlen(szSLBuf); i++)
	{
		if(szSLBuf[i] >='0' && szSLBuf[i] <= '9')
		{
			szString[j] = szSLBuf[i];
			j ++;
		}
		else if(szSLBuf[i] >='A' && szSLBuf[i] <= 'F')
		{
			szString[j] = szSLBuf[i];
			j ++;
		}
		else if(szSLBuf[i] >='a' && szSLBuf[i] <= 'f')
		{
			szString[j] = szSLBuf[i];
			j ++;
		}
		else
			continue;
	}

	if(strlen(szString) == 0 ||
		strlen(szString) %2 != 0)
		return TRUE;

	// Insert it to the pool
	m_arrApdu.Add(szString);
	
	return TRUE;
}

// Get the APDU Count
INT CApduProcesser::GetLineCount()
{
	return m_arrApdu.GetSize();
}

// Get the Error Message of last error.
char * CApduProcesser::GetErrorMessage()
{
	return m_szErrMsg;
}


// Get one APDU from Buffer.
char * CApduProcesser::GetOneApdu(int nInx)
{
	CString			strTmp;

	if(nInx >= m_arrApdu.GetSize())
		return NULL;

	strTmp = m_arrApdu.GetAt(nInx);

	strTmp.MakeUpper();			// Convert to Upper Case.

	sprintf(m_szApdu, "%s", strTmp);

	return m_szApdu;
}

// Get one Expected Response from Buffer.
char* CApduProcesser::GetOneExpResp(int nInx)
{
	CString			strTmp;
	
	if(nInx >= m_arrExpResp.GetSize())
		return NULL;
	
	strTmp = m_arrExpResp.GetAt(nInx);
	
	strTmp.MakeUpper();			// Convert to Upper Case.
	
	sprintf(m_szExpResp, "%s", strTmp);
	
	return m_szExpResp;	
}


// Dump the APDUs from buffer.
void CApduProcesser::DumpApdus()
{
	CString			str;

	OutputDebugString("\n ---------------- APDU Dump +");
	for(int i=0; i < m_arrApdu.GetSize(); i ++)
	{
		str = m_arrApdu.GetAt(i);

		OutputDebugString("\n");
		OutputDebugString(str);
	}

	OutputDebugString("\n ---------------- APDU Dump -");
}


// end-of-file
//////////////////////////////////////////////////////////////////////////