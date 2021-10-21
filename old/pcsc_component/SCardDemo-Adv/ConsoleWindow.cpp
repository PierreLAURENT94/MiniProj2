// ConsoleWindow.cpp: 
//          Console for debug output. 
//          for more information, please refer to:
//              http://www.cs.umu.se/kurser/TDBB09/VT01/console/readout.c
//
//			by Y.Volta Tang (y.volta@gmail.com)
//          2008-01-18 22:06
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ConsoleWindow.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define PERR
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CConsoleWindow::CConsoleWindow(CWnd *pParent/* = NULL*/)
{
	m_hConsole	= NULL;
	m_bCreate	= FALSE;

	if(AllocConsole())
	{
		m_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTitle("Dbg Console - SCard Demo (Advanced)");
		SetConsoleMode(m_hConsole,ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT); // ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT);
		m_bCreate=TRUE;
	}
	else{
		m_hConsole=GetStdHandle(STD_OUTPUT_HANDLE);
		if(m_hConsole==INVALID_HANDLE_VALUE)
			m_hConsole=NULL;
	}
	if(m_hConsole)
	{
		CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
		if(GetConsoleScreenBufferInfo(m_hConsole, &csbiInfo))
		{
			m_bAttrs=TRUE;
			m_OldColorAttrs = csbiInfo.wAttributes;      
		}
		else{
			m_bAttrs=FALSE;
			m_OldColorAttrs = 0;
		}
	}

	m_pParent = pParent;
}


CConsoleWindow::~CConsoleWindow()
{
	if(m_bCreate)
		FreeConsole();
}

// Set the Console Window
BOOL CConsoleWindow::SetTile(LPCTSTR lpTile)
{
	return SetConsoleTitle(lpTile);
}

// Output a string with current ATTR & pos.
BOOL CConsoleWindow::WriteString(LPCTSTR lpString)
{
	BOOL	ret=FALSE;
	DWORD	dwLen = 0;

	if(m_hConsole)
	{
		ret=WriteConsole(m_hConsole,lpString,_tcslen(lpString), &dwLen,NULL);
	}
	return ret;
}

// Output a string with specified ATTR at the current pos.
BOOL CConsoleWindow::WriteString(WORD Attrs,LPCTSTR lpString)
{
	BOOL ret=FALSE;
	if(m_hConsole)
	{
		// set the attribute.
		if(m_bAttrs)SetConsoleTextAttribute(m_hConsole,Attrs);

		// Output
		ret=WriteConsole(m_hConsole,lpString,_tcslen(lpString),NULL,NULL);
		
		// reset the attribute.
		if(m_bAttrs)SetConsoleTextAttribute(m_hConsole,m_OldColorAttrs);
	}

	return ret; 
}

// printf style output
BOOL CConsoleWindow::DBWindowWrite(LPCTSTR fmt,...)
{
	TCHAR   message[MAX_BUF_LEN] = {0};
	va_list cur_arg;
	va_start(cur_arg,fmt);
	_vsntprintf(message,MAX_BUF_LEN,fmt,cur_arg);
	va_end(cur_arg);

	return WriteString(message);
}

// printf style output with color.
BOOL CConsoleWindow::DBWindowWrite(WORD Attrs,LPCTSTR fmt,...)
{
	TCHAR   message[MAX_BUF_LEN] = {0};
	va_list cur_arg;
	va_start(cur_arg,fmt);
	_vsntprintf(message, MAX_BUF_LEN, fmt,cur_arg);
	va_end(cur_arg);

	return WriteString(Attrs,message);
}

// Wait for any-key input.
void CConsoleWindow::WaitForAnyKey()
{
	ULONG			BytesTransferred;
	INPUT_RECORD	Char;

	HANDLE hConsole = GetStdHandle(STD_INPUT_HANDLE);

	DWORD fdwMode = ENABLE_MOUSE_INPUT | ENABLE_PROCESSED_OUTPUT; 
	
	if (! SetConsoleMode(hConsole, fdwMode) ) 
	{	// Sth. wrong.
		return;
	}
	
	// keRecord.uChar
	FlushConsoleInputBuffer(hConsole);
	
	while(TRUE)
	{
	   BOOL Success = ReadConsoleInput(hConsole,
			&Char,
			1,
			&BytesTransferred
			);

		if(Char.EventType == KEY_EVENT)
			break;
	}

}

// Wait for any-key input, and user can use cursor to select texts
// and copy them to clipboard.
void CConsoleWindow::WaitForAnyKeyEx()
{
	BOOL bSuccess;
	INPUT_RECORD inputBuffer;
	DWORD dwStdInMode;
	HANDLE hStdIn;
	DWORD dwInputEvents;
	COORD coordStart, coordEnd, coordDest = {0, 0};
	BOOL bDragMode = FALSE; /* mode flag - selecting (dragging) or not? */
	COORD coordPrev; /* coordinates of the start of the selected area */
	CHAR_INFO *pCharInfo; /* buffer to store info from ReadConsoleOutput */
	COORD coordBufSize; /* the size of the buffer to read into */
	SMALL_RECT srReadRegion; /* source rectangle to read from */
	WORD top, left, right, bottom; /* sides of the source rectangle */
	
	HANDLE		hConOut = m_hConsole;

	hStdIn = GetStdHandle(STD_INPUT_HANDLE);
	PERR(hStdIn != INVALID_HANDLE_VALUE, "GetStdHandle");

	/* save the console mode */
	bSuccess = GetConsoleMode(hStdIn, &dwStdInMode);
	PERR(bSuccess, "GetConsoleMode");

	/* enable mouse input */
	bSuccess = SetConsoleMode(hStdIn, dwStdInMode | ENABLE_MOUSE_INPUT);
	PERR(bSuccess, "SetConsoleMode");
	for(;;)
    {
		/* get an input event */
		bSuccess = ReadConsoleInput(hStdIn, &inputBuffer, 1, &dwInputEvents);
		PERR(bSuccess, "ReadConsoleInput");
		switch (inputBuffer.EventType)
		{
		case KEY_EVENT:
			/* is it an ESC character? */
			if (inputBuffer.Event.KeyEvent.bKeyDown 
				//&& inputBuffer.Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE
				)
			{
				/* set input mode back to what it was originally and return */
				bSuccess = SetConsoleMode(hStdIn, dwStdInMode);
				PERR(bSuccess, "SetConsoleMode");
				return;
			}
			break;
		case MOUSE_EVENT:
			/* was this was a click event? */
			if (inputBuffer.Event.MouseEvent.dwEventFlags != MOUSE_MOVED)
				/* is only leftmost button down? */
			{
				if(inputBuffer.Event.MouseEvent.dwButtonState ==
					FROM_LEFT_1ST_BUTTON_PRESSED)
				{
					/* save the starting coordinates of the selected area */
					coordStart = coordPrev =
						inputBuffer.Event.MouseEvent.dwMousePosition;
					/* invert the single char attribute the cursor is on */
					InvertRectangle(hConOut, coordStart, coordStart);
					bDragMode = TRUE; /* we are now in 'drag' mode */
				}
				else /* button #1 has just been released */
				{
					/* if we're not in drag mode, break; we might get a mouse */
					/* button release when gaining the focus */
					if (!bDragMode)
						break;

					/* save the coordinates of the mouse release */
					coordEnd = inputBuffer.Event.MouseEvent.dwMousePosition;
					bDragMode = FALSE; /* we're not in 'drag' mode anymore */

					/* invert the rectangle back to normal */
					InvertRectangle(hConOut, coordStart, coordEnd);

					/* copy the buffer in the rectangle and output it. First convert */
					/* coordSart to upper left & coordCur to lower right */
					top = min(coordStart.Y, coordEnd.Y);
					left = min(coordStart.X, coordEnd.X);
					bottom = max(coordStart.Y, coordEnd.Y);
					right = max(coordStart.X, coordEnd.X);
					coordStart.X = srReadRegion.Left = left;
					coordStart.Y = srReadRegion.Top = top;
					coordEnd.X = srReadRegion.Right = right;
					coordEnd.Y = srReadRegion.Bottom = bottom;
					
					/* get the size of the selected rectangle */
					coordBufSize.X = coordEnd.X - coordStart.X + (SHORT) 1;
					coordBufSize.Y = coordEnd.Y - coordStart.Y + (SHORT) 1;
					
					/* allocate a buffer for the screen rectangle */
					pCharInfo = (PCHAR_INFO)malloc(coordBufSize.X * coordBufSize.Y *
						sizeof(CHAR_INFO));
					PERR(pCharInfo, "malloc");
					
					/* read the screen rectangle into the buffer */
					bSuccess = ReadConsoleOutput(hConOut, /* output buffer */
						pCharInfo, /* buffer that receives data */
						coordBufSize, /* col/row size of destination buffer */
						coordDest, /* upper-left cell to write data to in dest */
						&srReadRegion); /* screen buffer rectangle to read from */
					PERR(bSuccess, "ReadConsoleOutput");
					
					CopyToClipboard(pCharInfo, &coordBufSize);

// Copy text to the currsor position, commented for clipboard operation.
// 					/* get current cursor position */
// 					bSuccess = GetConsoleScreenBufferInfo(hConOut, &csbi);
// 					PERR(bSuccess, "GetConsoleScreenBufferInfo");
// 					
// 					/* set up an output destination rectangle for the buffer. */
// 					/* Put it just below the current cursor position. */
// 					srReadRegion.Top = csbi.dwCursorPosition.Y;
// 					srReadRegion.Bottom = csbi.dwCursorPosition.Y + (bottom - top);
// 					srReadRegion.Left = 0;
// 					srReadRegion.Right = right - left;
// 					
// 					bSuccess = WriteConsoleOutput(hConOut, /* output buffer */
// 						pCharInfo, /* buffer with data to write */
// 						coordBufSize, /* col/row size of source buffer */
// 						coordDest, /* upper-left cell to write data from in src */
// 						&srReadRegion); /* screen buffer rect to write data to */
// 					PERR(bSuccess, "WriteConsoleOutput");
					
					free(pCharInfo);
				}
			}
			/* is it a mouse movement while we are in drag mode? */
			if (inputBuffer.Event.MouseEvent.dwEventFlags == MOUSE_MOVED &&
				bDragMode)
			{
				/* if the cursor moves from the rectangle it was previously in, */
				/* invert the old rectangle then invert the new rectangle. */
				/* Compare the current coordinates with the previous coordinates */
				if (memcmp(&inputBuffer.Event.MouseEvent.dwMousePosition,
					&coordPrev, sizeof(COORD)))
				{
					/* You could be clever and only invert what needs to be */
					/* inverted rather than the entire two rectangles... */
					/* Invert the old rectangle */
					InvertRectangle(hConOut, coordStart, coordPrev);

					/* and invert the new rectangle */
					InvertRectangle(hConOut, coordStart,
						inputBuffer.Event.MouseEvent.dwMousePosition);

					/* save the current mouse position to the previous position */
					memcpy(&coordPrev,
						&inputBuffer.Event.MouseEvent.dwMousePosition,
						sizeof(COORD));
				}
			}
			break;
      } /* switch */
    } /* while */	
}

// Copy Data to Clipboard.
void CConsoleWindow::CopyToClipboard(PCHAR_INFO ppInfo, PCOORD pCoord)
{
	INT				nLen = (pCoord->X + 2) * pCoord->Y + 1;
	PCHAR			pChar = (PCHAR)malloc(nLen);
	PCHAR			pLine = pChar;
	PCHAR			pLinend = pChar;
	
	memset(pChar, 0x0, nLen);

	int		j=0;			// One-Line-Length, so, j <= pCoord->X
	int		k=0;			// To Clipboard's index.

	// i, total units' index.
	for(int i =0; i < pCoord->X * pCoord->Y; i ++)
	{
		// save this char to buffer.
		pChar[k++] = ppInfo[j].Char.AsciiChar;

		j ++;		// offset per line.
		if(j % pCoord->X == 0)		// we should add CRLF to the end of this line?
		{
			// trim the space of right on each line.
			while(pChar[k-1] == ' ')
			{
				if(k <=0 )
					break;
				k --;
			}

			// pending the CRLF to the line.
			pChar[k ++] = '\r';
			pChar[k ++] = '\n';
		}
	}

	if((nLen == 4) && (pChar[0] == ' '))
	{
		return;
	}

	pChar[k] = 0x0;

	CString				str(pChar);

	// Open the clipboard
	if (OpenClipboard(m_pParent ->GetSafeHwnd()))			
	{
		
		HANDLE hClip;
		char *pBuf;
		EmptyClipboard();
		
		hClip = GlobalAlloc(GMEM_MOVEABLE, str.GetLength()+1);
		pBuf = (char*)GlobalLock(hClip);
		strcpy(pBuf, str);						// copy data to buffer.
		
		GlobalUnlock(hClip);
		
		SetClipboardData(CF_TEXT, hClip);		// set clipboard data
		
		CloseClipboard();						// close the clipboard
		
	}
}



/********************************************************************
* FUNCTION: GetConX(HANDLE hCon)                                    *
*                                                                   *
* PURPROSE: to get the current width of the console output buffer   *
*                                                                   *
* INPUT: the handle to get the information for                      *
*                                                                   *
* RETURNS: the width of the current console output buffer, in chars *
********************************************************************/
SHORT CConsoleWindow::GetConX(HANDLE hCon)
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	BOOL bSuccess;
	
	bSuccess = GetConsoleScreenBufferInfo(hCon, &csbi);
	PERR(bSuccess, "GetConsoleScreenBufferInfo");
	return(csbi.dwSize.X);
}


/*********************************************************************
* FUNCTION: GetConY(HANDLE hCon)                                     *
*                                                                    *
* PURPROSE: to get the current height of the console output buffer   *
*                                                                    *
* INPUT: the handle to get the information for                       *
*                                                                    *
* RETURNS: the height of the current console output buffer, in chars *
*********************************************************************/
SHORT CConsoleWindow::GetConY(HANDLE hCon)
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	BOOL bSuccess;
	
	bSuccess = GetConsoleScreenBufferInfo(hCon, &csbi);
	PERR(bSuccess, "GetConsoleScreenBufferInfo");
	return(csbi.dwSize.Y);
}

/*************************************************************************
* FUNCTION: Puts(HANDLE hConsole, PCHAR s)                             *
*                                                                        *
* PURPOSE: write a string to the given console buffer, appending a cr/lf *
*                                                                        *
* INPUT: the console to write to, and the string to write                *
*                                                                        *
* RETURNS: TRUE if success, FALSE if an error occured                    *
*************************************************************************/
BOOL CConsoleWindow::Puts(HANDLE hConsole, PCHAR s)
{
	BOOL bSuccess;
	DWORD cCharsWritten;
	const PCHAR crlf = "\n";
	BOOL retflag = TRUE;
	
	/* write the string to the console */
	bSuccess = WriteConsole(hConsole, s, strlen(s), &cCharsWritten, NULL);
	PERR(bSuccess, "WriteConsole");
	retflag = bSuccess;
	/* put a carriage return & line feed after the string */
	bSuccess = WriteConsole(hConsole, crlf, strlen(crlf), &cCharsWritten, NULL);
	PERR(bSuccess, "WriteConsole");
	if (!bSuccess)
		retflag = FALSE;
	return(retflag);
}

/********************************************************************
* FUNCTION: InvertRectangle(HANDLE hConOut, COORD coordStart,       *
*                           COORD coordCur)                         *
*                                                                   *
* PURPOSE: invert the attributes of the rectangle defined as the    *
*          area between two input COORD parameters                  *
*                                                                   *
* INPUT: the output handle of the console to make the changes to,   *
*        and two sets of COORD structures defining the rectangle to *
*        invert                                                     *
********************************************************************/
void CConsoleWindow::InvertRectangle(HANDLE hConOut, COORD coordStart, COORD coordCur)
{
	WORD *awLineAttr;
	unsigned i;
	DWORD dwBoxWidth;
	WORD top, left, right, bottom;
	BOOL bSuccess;
	DWORD dwNumAttrsRead;
	
	/* convert coordSart to upper left & coordCur to lower right */
	top = min(coordStart.Y, coordCur.Y);
	bottom = max(coordStart.Y, coordCur.Y);
	left = min(coordStart.X, coordCur.X);
	right = max(coordStart.X, coordCur.X);
	coordStart.X = left;
	coordStart.Y = top;
	coordCur.X = right;
	coordCur.Y = bottom;
	
	/* get the width of the selected area */
	dwBoxWidth = right - left + 1;
	
	/* allocate a buffer for the attributes read from the console */
	awLineAttr = (PWORD) malloc(GetConX(hConOut) * sizeof(DWORD));
	PERR(awLineAttr, "malloc");
	
	/* for each line in the selected area */
	while (coordStart.Y <= coordCur.Y)
    {
		/* read the attributes on that line */
		bSuccess = ReadConsoleOutputAttribute(hConOut, awLineAttr,
			dwBoxWidth, coordStart, &dwNumAttrsRead);
		
		PERR(bSuccess, "ReadConsoleOutputAttribute");
		
		
		/* for each attribute in the line, invert the color */
		for (i = 0; i < dwBoxWidth; i++)
		{
			awLineAttr[i] = ~awLineAttr[i];
			awLineAttr[i] &= 0x00FF;
		}
		
		/* put the inverted color back on the line */
		bSuccess = WriteConsoleOutputAttribute(hConOut, awLineAttr, dwBoxWidth,
			coordStart, &dwNumAttrsRead);
		PERR(bSuccess, "WriteConsoleOutputAttribute");
		coordStart.Y++; /* go to the next line */
    }
	free(awLineAttr); /* free allocated attribute buffer */
	return;
}

// end-of-file
//////////////////////////////////////////////////////////////////////////