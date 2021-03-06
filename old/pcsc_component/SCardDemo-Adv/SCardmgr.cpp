// SCardmgr.cpp: 
//          SmartCard's encapsulation.
//
//			by Y.Volta Tang (y.volta@gmail.com)
//          2008-01-18 22:06
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SCardmgr.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// string/hex conversion by Xian-Qiang Yu.
int Hex2Asc(char *Dest,char *Src,int SrcLen);
int Asc2Hex(char *Dest,char *Src,int SrcLen);
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSCardmgr::CSCardmgr()
{
	sprintf(m_szErrMsg, "%s", "No error.");

	memset(pResponseBuffer, 0x0, sizeof(pResponseBuffer));
	ResponseLength = MAX_RESPONSE;

	m_arrReaders.RemoveAll();
}

CSCardmgr::~CSCardmgr()
{
	m_arrReaders.RemoveAll();
}

///////////////////////////////////////////////////////////////////////////////
// Open a SCard Reader by its index
BOOL CSCardmgr::SCardOpen(int nInx)
{

	SCReader			stuReader;

	if(nInx > m_arrReaders.GetSize())
	{
		sprintf(m_szErrMsg, "%s", "Index out of bound.");
		return FALSE;
	}

	m_nInx = nInx;

	stuReader = m_arrReaders.GetAt(nInx);		// Get reader name from ReaderArray

    DWORD ret = SCardConnect(ContextHandle, 
						stuReader.szReaderName, 
						SCARD_SHARE_SHARED, 
						SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, 
						&CardHandle, 
						&ActiveProtocol);

	lErr = ret;
	if (ret == SCARD_S_SUCCESS)
    {
		return TRUE;
	}

	sprintf(m_szErrMsg, "Cannot connect to this reader(Err: 0x%08x", ret);
	return FALSE;
}


///////////////////////////////////////////////////////////////////////////////
// Close a SCard Reader connection
BOOL CSCardmgr::SCardClose()
{
	DWORD ret = SCardDisconnect(CardHandle, SCARD_EJECT_CARD);

	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// Get ATR from a open SCard Reader
BOOL CSCardmgr::SCardReset(LPCTSTR strResp)
{
    DWORD               dwATRLength = 40; 
    BYTE                pbyATR[40]; 
	SCReader			stuReader;
	DWORD               dwLength    = 300;
    DWORD               dwCardState = 0;
	DWORD               dwActiveProtocol = 0;

    // hCardHandle was set by a previous call to SCardConnect.
    DWORD ret = SCardReconnect(CardHandle,
                             SCARD_SHARE_SHARED,
                             SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1,
                             SCARD_UNPOWER_CARD,
                             &dwATRLength );
    if ( SCARD_S_SUCCESS != ret ){
			strResp = "";
			return 0;
	}

	stuReader = m_arrReaders.GetAt(m_nInx);		// Get reader name from ReaderArray

	ret = SCardStatus(CardHandle,				// Trick
					stuReader.szReaderName,
					&dwLength,
					&dwCardState,
					&dwActiveProtocol,
					pbyATR,
					&dwATRLength);

	ret = SCardStatus(CardHandle,				// Trick
					stuReader.szReaderName,
					&dwLength,
					&dwCardState,
					&dwActiveProtocol,
					pbyATR,
					&dwATRLength);

	if (ret != SCARD_S_SUCCESS){
		strResp = "";
		return FALSE;
	}

	// String convertion
	Hex2Asc((char*)strResp, (char*)pbyATR, dwATRLength );

	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// Transmit Apdu 
BOOL CSCardmgr::SCardTransmit(LPCTSTR strApdu, LPCTSTR strResp, UINT *nSW)
{	
	char                SendData[MAX_APDU_LEN], ReceiveData[MAX_RESPONSE];
	ULONG               nCmdLen, nResp;
	SCARD_IO_REQUEST	IO_Request;

    ZeroMemory(ReceiveData, MAX_APDU_LEN);
    nCmdLen = strlen(strApdu);

    Asc2Hex(SendData, (char*)strApdu, nCmdLen);			// string convertion

    nResp = MAX_RESPONSE;
	IO_Request.dwProtocol = ActiveProtocol;
	IO_Request.cbPciLength = (DWORD) sizeof(SCARD_IO_REQUEST);

	DWORD ret = ::SCardTransmit(CardHandle,				// SCard API
						&IO_Request, 
						(PUCHAR)SendData, 
						nCmdLen / 2, 
						0, 
						(PUCHAR)ReceiveData, 
						&nResp);
	
	if (ret != SCARD_S_SUCCESS){
		
		sprintf(m_szErrMsg, "SCardTransmit.Error(0x%08X): %s", ret, strApdu);
        return FALSE;
	}

    if(nResp != 0)
    {
		Hex2Asc((char*)strResp, ReceiveData, nResp - 2);
		*nSW = ((unsigned char)(ReceiveData[nResp - 2]) * 256) + 
			(unsigned char)ReceiveData[nResp - 1];

		return TRUE;
    }

	return FALSE;
}


///////////////////////////////////////////////////////////////////////////////
// Get SCard Reader's List
BOOL CSCardmgr::SCardGetPcscList()
{
	DWORD ret = SCardEstablishContext(SCARD_SCOPE_USER, 
										NULL, 
										NULL, 
										&ContextHandle);
	lErr = ret;

	if (ret != SCARD_S_SUCCESS) 
	{
		//MessageBuffer.Format("Function SCardEstablishContext returned 0x%X error code.", ret);
		sprintf(m_szErrMsg, "Function SCardEstablishContext returned 0x%08X error code.", ret);
		return FALSE;
	}

	ret = SCardListReaders(ContextHandle, 0, 
						(char *) pResponseBuffer, 
						&ResponseLength);

	lErr = ret;
	if (ret != SCARD_S_SUCCESS) 
	{
		sprintf(m_szErrMsg, "No reader available. (%x)", ret);
		AfxMessageBox(m_szErrMsg);
		return FALSE;
	}
	else 
	{

		// OK, Clear the Lists
		m_arrReaders.RemoveAll();

		UINT		StringLen = 0;
		SCReader	stuReader;
		
		// Build the Reader List
		while ( ResponseLength > StringLen+1) 
		{                    
			ZeroMemory(stuReader.szReaderName, sizeof(stuReader.szReaderName));
			
			strcpy(stuReader.szReaderName, (char*)pResponseBuffer+StringLen);
			m_arrReaders.Add(stuReader);

			StringLen += strlen((LPCTSTR) pResponseBuffer+StringLen+1);
			StringLen += 2;
		}
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// hex to asc: 0x22 -> "22"
int Hex2Asc(char *Dest,char *Src,int SrcLen)
{
	int i;
	for ( i = 0; i < SrcLen; i ++ )
	{
		sprintf(Dest + i * 2,"%02X",(unsigned char)Src[i]);
	}
	Dest[i * 2] = 0;
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// asc to hex: "22" -> 0x22
int Asc2Hex(char *Dest,char *Src,int SrcLen)
{
	int i;
	for ( i = 0; i < SrcLen / 2; i ++ )
	{
		sscanf(Src + i * 2,"%02X",(unsigned char *)&Dest[i]);
	}
	return TRUE;
}


// end-of-file
//////////////////////////////////////////////////////////////////////////