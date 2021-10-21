// SCardDemoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SCardDemo.h"
#include "SCardDemoDlg.h"

// Extension
#include "Atr.h"
#include "ApduProcesser.h"
#include "ConsoleWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


int Hex2Asc(char *Dest,char *Src,int SrcLen);
int Asc2Hex(char *Dest,char *Src,int SrcLen);
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
// CSCardDemoDlg dialog

CSCardDemoDlg::CSCardDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSCardDemoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSCardDemoDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSCardDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSCardDemoDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSCardDemoDlg, CDialog)
	//{{AFX_MSG_MAP(CSCardDemoDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDB_REFRESH, OnRefresh)
	ON_BN_CLICKED(IDB_OPEN, OnOpen)
	ON_BN_CLICKED(IDB_CLOSE, OnClose)
	ON_BN_CLICKED(IDB_RESET, OnReset)
	ON_BN_CLICKED(IDB_CMD, OnCmd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSCardDemoDlg message handlers

BOOL CSCardDemoDlg::OnInitDialog()
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
	
	// Refresh the Reader Lists
	OnRefresh();

	// Init the Dialog Header.
	InitDlgHeader();
	
	LoadApduHistory(TRUE);			// Load the text to registry.

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSCardDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CSCardDemoDlg::OnPaint() 
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
HCURSOR CSCardDemoDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


void CSCardDemoDlg::OnRefresh() 
{
	SCReader	stuReader;

	if(m_SCard.SCardGetPcscList())			// Get PC/SC Lists
	{
		CComboBox* pCtl = (CComboBox *)GetDlgItem(IDC_CMB_LIST);

		pCtl ->ResetContent();				// Clear the contents of List

		// List them
		for(int i = 0 ; i < m_SCard.m_arrReaders.GetSize(); i ++)
		{
			stuReader = m_SCard.m_arrReaders.GetAt(i);

			pCtl ->AddString(stuReader.szReaderName);
		}

		// Select the 1st
		if(m_SCard.m_arrReaders.GetSize() >0)
		{
			pCtl -> SetCurSel(0);

			// Enable "Open"
			GetDlgItem(IDB_OPEN) ->EnableWindow(TRUE);
		}
		else
		{
			// Disable "Open"
			GetDlgItem(IDB_OPEN) ->EnableWindow(FALSE);
		}
	}
	else		// Disable "Open"
	{
		GetDlgItem(IDB_OPEN) ->EnableWindow(FALSE);
	}

	// Reset the state of Other Controls
	GetDlgItem(IDB_CLOSE) ->EnableWindow(FALSE);
	GetDlgItem(IDB_RESET) ->EnableWindow(FALSE);
	GetDlgItem(IDB_CMD) ->EnableWindow(FALSE);

	GetDlgItem(IDE_APDU) ->EnableWindow(FALSE);
}

///////////////////////////////////////////////////////////////////////////////
// Open a Reader
void CSCardDemoDlg::OnOpen() 
{
	CComboBox* pCtl = (CComboBox *)GetDlgItem(IDC_CMB_LIST);

	int nSel = pCtl ->GetCurSel();

	if(nSel == -1)
		return;

	if(m_SCard.SCardOpen(nSel))					// Do Open
	{
		GetDlgItem(IDB_OPEN) ->EnableWindow(FALSE);
		GetDlgItem(IDB_CLOSE) ->EnableWindow(TRUE);
		GetDlgItem(IDB_RESET) ->EnableWindow(TRUE);
		GetDlgItem(IDB_CMD) ->EnableWindow(TRUE);

		GetDlgItem(IDE_APDU) ->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDB_OPEN) ->EnableWindow(TRUE);
		GetDlgItem(IDB_CLOSE) ->EnableWindow(FALSE);
		GetDlgItem(IDB_RESET) ->EnableWindow(FALSE);
		GetDlgItem(IDB_CMD) ->EnableWindow(FALSE);

		GetDlgItem(IDE_APDU) ->EnableWindow(FALSE);

		DisplayErr();
	}
}


///////////////////////////////////////////////////////////////////////////////
// Close the open Reader
void CSCardDemoDlg::OnClose() 
{
	if(!m_SCard.SCardClose())
	{
		// Close failed.
		SetDlgItemText(IDS_MSG, "Reader Close failed.");
	}

	// Reset the state of Controls
	GetDlgItem(IDB_OPEN) ->EnableWindow(TRUE);
	GetDlgItem(IDB_CLOSE) ->EnableWindow(FALSE);
	GetDlgItem(IDB_RESET) ->EnableWindow(FALSE);
	GetDlgItem(IDB_CMD) ->EnableWindow(FALSE);
	
	GetDlgItem(IDE_APDU) ->EnableWindow(FALSE);
}


///////////////////////////////////////////////////////////////////////////////
// Get the ATR information of the open Reader
void CSCardDemoDlg::OnReset() 
{
	CHAR		szATR[ATR_MAX_SIZE * 2 +1] = {0};
	BYTE		byATR[ATR_MAX_SIZE] = {0};
	CHAR		szFullAtr[MAX_RESPONSE] = {0};

	UINT		np = 0;				// Prot.
	BYTE		protocol = ATR_PROTOCOL_TYPE_T0;
	
	ZeroMemory(szATR, sizeof(szATR));

	if(m_SCard.SCardReset(szATR))					// Do Reset it
	{
		int nLenAtr = strlen(szATR);

		Asc2Hex((char*)byATR, szATR, nLenAtr);				// Convert ATR to Hex format

		CAtr			objATR(byATR, nLenAtr/2);

		objATR.ATR_GetNumberOfProtocols(&np);		// get prots supported.
		INT nType = objATR.ATR_GetProtocolType(np, &protocol);	//

		sprintf(szFullAtr, "%s (T=%d).", szATR, protocol);

		SetDlgItemText(IDS_MSG, szFullAtr);			// Display the ATR Information
	}
}

///////////////////////////////////////////////////////////////////////////////
// Transmit APDU 
void CSCardDemoDlg::OnCmd() 
{
	CHAR				szApdu[MAX_RESPONSE] = {0};			// Input
	CHAR				szResp[MAX_RESPONSE] = {0};			// Response
	CHAR				szErr [MAX_RESPONSE] = {0};
	UINT				nSW;						// SW

	CApduProcesser		objApdu;					//
	BOOL				bDebug = FALSE;				// Step by step to run multi-apdus.

	
	ZeroMemory(szApdu, sizeof(szApdu));
	ZeroMemory(szResp, sizeof(szResp));

	// Get the APDU from user
	::GetDlgItemText(this->m_hWnd, IDE_APDU, szApdu, MAX_RESPONSE);
	if(strlen(szApdu) <=0)
	{
		SetDlgItemText(IDS_MSG, "Please Input Apdu.");
		GetDlgItem(IDE_APDU) ->SetFocus();
		return;							// no input, return
	}

	// Save the APDUs to registry.
	LoadApduHistory(FALSE);				

	// process the APDU.
	if(!objApdu.ProcessInput(szApdu))
	{
		SetDlgItemText(IDS_MSG, "Your Apdu is invalid.");
		return;
	}

	char		*pApdu = NULL;
	INT			nCounts = objApdu.GetLineCount();		// how many APDUs?

	// Does APDUs' count is more than 1?
	if(nCounts >=2)
	{	// Yes, use a console window to display the process.
		CConsoleWindow		objConsole(this);
		
		// Check for running mode.
		CButton *pBtn = (CButton*)GetDlgItem(IDK_DEBUG);
		if(pBtn ->GetCheck() == BST_CHECKED)
			bDebug = TRUE;
	
		// objApdu.DumpApdus();
		
		for(int i =0; i < nCounts; i++)
		{			
			// Get one APDU
			pApdu = objApdu.GetOneApdu(i);
			
			// Dump it to the console.
			objConsole.DBWindowWrite("\n\n --> %s.", pApdu);

			// Do Apdu Transmit
			if(!m_SCard.SCardTransmit(pApdu, szResp, &nSW))	
			{
				sprintf(szErr, "\n %s << ", m_SCard.m_szErrMsg);

				objConsole.DBWindowWrite(szErr);		
				break;
			}

			// Dump the Response and SWs to the console.
			objConsole.DBWindowWrite("\n <-- %s(%04X).", szResp, nSW);
			objConsole.DBWindowWrite("\n #-- %s.", objApdu.GetOneExpResp(i));

			// Any key for another APDU.
			if(bDebug)
				objConsole.WaitForAnyKeyEx();

		}
		
		// Before quit, prompt the user.
		objConsole.DBWindowWrite("\n\nPress any key to continue...");		
		objConsole.WaitForAnyKeyEx();
	}
	else// single APDU.
	{	// No, show the response to the edit window.
		pApdu = objApdu.GetOneApdu(0);
		
		if(pApdu == NULL)
		{
			MessageBox("Your APDU is error!     ", "Error", MB_ICONERROR | MB_OK);
			return;			
		}
		
		// Do Apdu Transmit
		if(!m_SCard.SCardTransmit(pApdu, szResp, &nSW))	
		{
			sprintf(szErr, "\n %s << ", m_SCard.m_szErrMsg);
		}
		else
		{
			sprintf(szErr, "%s(%04X)", szResp, nSW);
		}

		SetDlgItemText(IDS_MSG, szErr);			// Show it
	}
	
}


// Init the Dialog Header.
void CSCardDemoDlg::InitDlgHeader()
{
	// Set the Title and description.
	m_HeaderCtrl.SetTitleText("Smartcard Demo");
	m_HeaderCtrl.SetDescText("PC/SC Reader/Card operation: Open/Close/Reset/Transmit.");

	// Set the Icon
	m_HeaderCtrl.SetIconHandle(AfxGetApp()->LoadIcon(IDR_MAINFRAME));

	// Set the Header Image
	m_HeaderCtrl.SetBMPRes(IDB_HEADER_BAR);

	// Init the Control
	m_HeaderCtrl.Init(this);
	m_HeaderCtrl.MoveCtrls(this);

}


// Drop an APDU files to Edit.
void CSCardDemoDlg::OnDropFiles(HDROP hDropInfo) 
{
    unsigned int cFile = 0, i = 0;
    CHAR szFileNname[MAX_PATH] = {'\0'};
	
    cFile = ::DragQueryFile( hDropInfo, 0xFFFFFFFF, NULL, MAX_PATH );
	
    for(i = 0; i < cFile; i++)
    {
        DWORD fileAttrib = 0x0;
        ::DragQueryFile( hDropInfo, i, szFileNname, MAX_PATH );
		
        // bCanExit = FALSE;
        fileAttrib = GetFileAttributes(szFileNname);
		
        if (0x10 == (fileAttrib & FILE_ATTRIBUTE_DIRECTORY))
        {
            // Folder? we do not support.            
        }
        else
        {
            // a file.
			SetDlgItemText(IDE_APDU, szFileNname);
						
            break;
        }
    }	
	
	CDialog::OnDropFiles(hDropInfo);
}


// Load last APDU(a file or apdus) from registry if bLoad = TRUE.
// Save APDU(a file or apdus) to registry if   bLoad = FALSE.
void CSCardDemoDlg::LoadApduHistory(BOOL bDoLoad)
{
	CWinApp* pApp = AfxGetApp();
	
	if(bDoLoad)
	{
		
		CString strValue = pApp->GetProfileString("Apdu-File", "URL");
		SetDlgItemText(IDE_APDU, strValue);
	}
	else
	{
		CString strValue = "";
		GetDlgItemText(IDE_APDU, strValue);
		
		pApp->WriteProfileString("Apdu-File", "URL", strValue);	
	}
}


///////////////////////////////////////////////////////////////////////////////
// Error handler of SCard operation
void CSCardDemoDlg::DisplayErr()
{

	switch (m_SCard.lErr) {
		case SCARD_E_CANCELLED:
							SetDlgItemText(IDS_MSG,"The action was cancelled by an SCardCancel request.");
							break;

		case SCARD_E_CANT_DISPOSE:
							SetDlgItemText(IDS_MSG,"The system could not dispose of the media in the requested manner.");
							break;
		case SCARD_E_CARD_UNSUPPORTED:
							SetDlgItemText(IDS_MSG,"The smart card does not meet minimal requirements for support.");
							break;
		case SCARD_E_DUPLICATE_READER:
							SetDlgItemText(IDS_MSG,"The reader driver didn't produce a unique reader name.");
							break;
		case SCARD_E_INSUFFICIENT_BUFFER:
							SetDlgItemText(IDS_MSG,"The data buffer to receive returned data is too small for the returned data.");
							break;
		case SCARD_E_INVALID_ATR:
							SetDlgItemText(IDS_MSG,"An ATR obtained from the registry is not a valid ATR string.");
							break;
		case SCARD_E_INVALID_HANDLE:
							SetDlgItemText(IDS_MSG,"The supplied handle was invalid.");
							break;
		case SCARD_E_INVALID_PARAMETER:
							SetDlgItemText(IDS_MSG,"One or more of the supplied parameters could not be properly interpreted.");
							break;
		case SCARD_E_INVALID_TARGET:
							SetDlgItemText(IDS_MSG,"Registry startup information is missing or invalid.");
							break;
		case SCARD_E_INVALID_VALUE:
							SetDlgItemText(IDS_MSG,"One or more of the supplied parameters’ values could not be properly interpreted.");
							break;
		case SCARD_E_NOT_READY:
							SetDlgItemText(IDS_MSG,"The reader or card is not ready to accept commands.");
							break;
		case SCARD_E_NOT_TRANSACTED:
							SetDlgItemText(IDS_MSG,"An attempt was made to end a non-existent transaction.");
							break;
		case SCARD_E_NO_MEMORY:
							SetDlgItemText(IDS_MSG,"Not enough memory available to complete this command.");
							break;
		case SCARD_E_NO_SERVICE:
							SetDlgItemText(IDS_MSG,"The Smart card resource manager is not running.");
							break;
		case SCARD_E_NO_SMARTCARD:
							SetDlgItemText(IDS_MSG,"The operation requires a smart card but no smart card is currently in the device.");
							break;
		case SCARD_E_PCI_TOO_SMALL:
							SetDlgItemText(IDS_MSG,"The PCI Receive buffer was too small.");
							break;
		case SCARD_E_PROTO_MISMATCH:
							SetDlgItemText(IDS_MSG,"The requested protocols are incompatible with the protocol currently in use with the card.");
							break;
		case SCARD_E_READER_UNAVAILABLE:
							SetDlgItemText(IDS_MSG,"The specified reader is not currently available for use.");
							break;
		case SCARD_E_READER_UNSUPPORTED:
							SetDlgItemText(IDS_MSG,"The reader driver does not meet minimal requirements for support.");
							break;
		case SCARD_E_SERVICE_STOPPED:
							SetDlgItemText(IDS_MSG,"The Smart card resource manager has shut down.");
							break;
		case SCARD_E_SHARING_VIOLATION:
							SetDlgItemText(IDS_MSG,"The card cannot be accessed because of other connections outstanding.");
							break;
		case SCARD_E_SYSTEM_CANCELLED:
							SetDlgItemText(IDS_MSG,"The action was cancelled by the system presumably to log off or shut down.");
							break;
		case SCARD_E_TIMEOUT:
							SetDlgItemText(IDS_MSG,"The user-specified timeout value has expired.");
							break;
		case SCARD_E_UNKNOWN_CARD:
							SetDlgItemText(IDS_MSG,"The specified card name is not recognized.");
							break;
		case SCARD_E_UNKNOWN_READER:
							SetDlgItemText(IDS_MSG,"The specified reader name is not recognized.");
							break;
		case SCARD_F_COMM_ERROR:
							SetDlgItemText(IDS_MSG,"An internal communications error has been detected.");
							break;
		case SCARD_F_INTERNAL_ERROR:
							SetDlgItemText(IDS_MSG,"An internal consistency check failed.");
							break;
		case SCARD_F_UNKNOWN_ERROR:
							SetDlgItemText(IDS_MSG,"An internal error has been detected but the source is unknown.");
							break;
		case SCARD_F_WAITED_TOO_LONG:
							SetDlgItemText(IDS_MSG,"An internal consistency timer has expired.");
							break;
		case SCARD_S_SUCCESS:
							SetDlgItemText(IDS_MSG,"OK");
							break;
		case SCARD_W_REMOVED_CARD:
							SetDlgItemText(IDS_MSG,"The card has been removed so that further communication is not possible.");
							break;
		case SCARD_W_RESET_CARD:
							SetDlgItemText(IDS_MSG,"The card has been reset so any shared state information is invalid.");
							break;
		case SCARD_W_UNPOWERED_CARD:
							SetDlgItemText(IDS_MSG,"Power has been removed from the card so that further communication is not possible.");
							break;
		case SCARD_W_UNRESPONSIVE_CARD:
							SetDlgItemText(IDS_MSG,"The card is not responding to a reset.");
							break;
		case SCARD_W_UNSUPPORTED_CARD:
							SetDlgItemText(IDS_MSG,"The reader cannot communicate with the card due to ATR configuration conflicts.");
							break;
		default:
			SetDlgItemText(IDS_MSG, "Unknown error code.");
			break;
	}
}

// end-of-file
//////////////////////////////////////////////////////////////////////////
