/****************************************************************************
The PC/SC Connector Component for C++ BUILDER 2010
Helps you access a smartcard reader through Microsofts SmartCard API
based on Norbert Huettisch's PCSCConnector.pas   (nobbi(at)nobbi.com)
rewrited it with C++ BUILDER 2010

The component has the following properties, methods and events:
Methods:
			bool __fastcall Init(void);
			bool __fastcall Open(void);
			void __fastcall Close(void);
			bool __fastcall Connect(void);
			void __fastcall Disconnect(void);
			long __fastcall GetResponseFromCard(const byte* apdu, const DWORD apduLength, byte *Response, DWORD& RLen);
Properties:
			int UseReaderNum; //points to the Reader to use in ReaderList or is set to -1 if no reader is selected
			TStringList* ReaderList; //holds the list of available readers. It is filled by calling Init.
			int NumReaders; //Holds the number of available readers
			bool Connected; //TRUE if connected to the reader (the reader MAY BE empty)
			bool Opened; //TRUE if a reader is being watched
			unsigned ReaderState; //holds the last status of the reader
			DWORD AttrProtocol; //the protocol used to communicate with the current card
			String AttrICCType; //the ICC type of the current card
			String AttrCardATR; // the ATR of the current card
			String AttrVendorName; //the vendor name of the reader
			String AttrVendorSerial; //the serial number of the reader
Events:
			OnCardInserted: fires when a card is inserted in the reader
			OnCardActive: fires when a inserted card is recognised and may be accessed.
			OnCardRemoved: fires when a inserted card is removed from the reader
			OnCardInvalid: fires when a inserted card is not readable
			OnReaderWaiting: fires when the reader is ready and waiting for a card
			OnReaderListChange: fires when the reader list has been updated
			OnError: fires when a error message is received while connecting to the card,reading the status word or transmitting data to the card.
Hints:
			First, call INIT
			Then, set the reader you want to use
			Then, call OPEN
			Then, if a card is inserted, you may call GetResponse...
Version info:
			20100110 initial version
-----------------------------------------------------------------------------
		Copyright 2010 by cadenza.yu@gmail.com
		Please direct questions, bug reports, and contributions to
				cadenza.yu@gmail.com

		Permission is hereby granted, free of charge, to any person
		obtaining a copy of this software and associated documentation
		files (the "Software"), to deal in the Software without
		restriction, including without limitation the rights to use,
		copy, modify, merge, publish, distribute, sublicense, and/or
		sell copies of the Software, and to permit persons to whom the
		Software is furnished to do so, subject to the following
		conditions:

		The above copyright notice and this permission notice shall be
		included in all copies or substantial portions of the
		Software.

		THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND
		EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
		OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
		NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
		HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
		WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
		FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
		OTHER DEALINGS IN THE SOFTWARE.

****************************************************************************/
#include <string>
#include "PCSCConnector.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
// ValidCtrCheck is used to assure that the components created do not have
// any pure virtual functions.
//
static inline void ValidCtrCheck(TPCSCConnector *)
{
  new TPCSCConnector(NULL);
}
//---------------------------------------------------------------------------
namespace Pcscconnector
{

  bool ReaderOpen;
  HWND NotifyHandle;
  std::string SelectedReader;
  DWORD ActReaderState;
  DWORD LastReaderState;

  static const ShortInt NOREADERSELECTED = -1;
  static const Word WM_CARDSTATE = 0x42a;

//---------------------------------------------------------------------------
  int __fastcall CardWatcherThread(void * PContext)
  {
    int Result = 0;
    long RetVar;
    SCARDCONTEXT RContext;
    SCARD_READERSTATE RStates[1];

    try
    {
      RContext = *(static_cast<SCARDCONTEXT *>(PContext));
      memset(RStates, sizeof(RStates), 0);
      RStates[0].szReader     = SelectedReader.c_str();
      RStates[0].pvUserData   = NULL;
      RStates[0].dwEventState = ActReaderState;

      while (ReaderOpen)
      {
        RStates[0].dwCurrentState = RStates[0].dwEventState;
        RetVar = SCardGetStatusChange(RContext, INFINITE , RStates, 1);
        ActReaderState = RStates[0].dwEventState;
        PostMessage(NotifyHandle, WM_CARDSTATE, RetVar, 0);
      }
    }

    __finally
    {
      Result = 0;
    }
    return Result;
  }
//---------------------------------------------------------------------------
  __fastcall TPCSCConnector::TPCSCConnector(TComponent* Owner)
      : TComponent(Owner)
  {
    FReaderList   = new TStringList();
    FContext      = 0;
    FCardHandle   = 0;
    FNumReaders   = 0;
    FUseReaderNum = -1;
    FConnected    = false;
    ActReaderState  = SCARD_STATE_UNAWARE;
    LastReaderState = SCARD_STATE_UNAWARE;
    ReaderOpen      = false;
    ClearReaderAttributes();

    if (!ComponentState.Contains(csDesigning))
    {
      NotifyHandle = AllocateHWnd(MessageWndProc);
    }
  }
//---------------------------------------------------------------------------
  __fastcall TPCSCConnector::~TPCSCConnector()
  {
    CloseAndDisconnect();
    SCardReleaseContext(FContext);
    delete FReaderList;

    if (!ComponentState.Contains(csDesigning))
    {
      DeallocateHWnd(NotifyHandle);
    }
  }
//---------------------------------------------------------------------------
/*
		 Init tries to establish a connection with the SmartCard API and to read the
	list of installed readers. If the connection is successfull, and a minimum
	of one reader is found, TRUE is returned
*/
 bool __fastcall TPCSCConnector::Init()
  {
    long RetVar;
    bool Result = false;
    LPTSTR ReaderList, Reader;
    DWORD ReaderListSize = SCARD_AUTOALLOCATE;
    int i;
    FNumReaders = 0;
    CloseAndDisconnect();

    if (SCardIsValidContext(FContext) == SCARD_S_SUCCESS)
    {
      SCardReleaseContext(FContext);
    }

    RetVar = SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &FContext);

    if (RetVar == SCARD_S_SUCCESS)
    { // Determine which readers are available.
      RetVar = SCardListReaders(FContext, NULL, (LPTSTR) & ReaderList, &ReaderListSize);

      if (RetVar == SCARD_S_SUCCESS)
      {  // Place the readers into FReaderList
        FReaderList->Clear();
        Reader = ReaderList;

        for (i = 0; i < MAXIMUM_SMARTCARD_READERS; i++)
        {
          if (*Reader == '\0')  break; // A double-null terminates the list of values.

          FReaderList->Add(Reader);
          Reader += strlen(Reader) + 1; // Do something with the multi string of readers.
        }

        SCardFreeMemory(FContext, ReaderList);  //release it;
        FNumReaders = FReaderList->Count;

        if (FNumReaders > 0)
        {
          if (Assigned(FOnReaderListChange))
          {
            FOnReaderListChange(this);
          }

          Result = true;
        }
      }
      else if (Assigned(FOnError))
      {
        FOnError(this, esInit, RetVar);
      }
    }
    else if (Assigned(FOnError))
    {
      FOnError(this, esInit, RetVar);
    }

    return Result;
  }
//---------------------------------------------------------------------------
/*
Open starts watching the selected reader for status changes
*/
  bool __fastcall TPCSCConnector::Open(void)
  {
    bool Result = false;
    unsigned ThreadID;
    CloseAndDisconnect();

    if ((FUseReaderNum > NOREADERSELECTED) &&
        (SCardIsValidContext(FContext) == SCARD_S_SUCCESS))
    {
      ReaderOpen      = true;
      ActReaderState  = SCARD_STATE_UNAWARE;
      LastReaderState = SCARD_STATE_UNAWARE;
      BeginThread(NULL, 0, CardWatcherThread, &FContext, 0, ThreadID);
      Result = true;
    }

    return Result;
	}
//---------------------------------------------------------------------------
/*
Close cancels all open requests and finishes watching the reader
*/
  void __fastcall TPCSCConnector::Close(void)
  {
    ReaderOpen = false;   //EndThread
    SCardCancel(FContext);

    if (FConnected)
    {
      Disconnect();
    }
  }

//---------------------------------------------------------------------------
/*
Connect tries to connect to the card in the selected reader.
      If successful, TRUE is returned
*/
  bool __fastcall TPCSCConnector::Connect(void)
  {
    bool Result = false;

    if (FConnected)  Disconnect();

    if (FUseReaderNum > NOREADERSELECTED)
    {
      if (ConnectSelectedReader())
      {
        FConnected = true;
      }
      else
      {
        FConnected = false;
      }

      Result = FConnected;
    }

    return Result;
  }

//---------------------------------------------------------------------------
/*
Disconnect disconnects from the card in the selected reader
*/
  void __fastcall TPCSCConnector::Disconnect(void)
  {
    if (FConnected)
    {
      SCardDisconnect(FCardHandle, SCARD_RESET_CARD);
      FConnected  = false;
      FCardHandle = 0;
    }
  }
//---------------------------------------------------------------------------
/*
		 Let you send APDUs to the card. Provide a complete APDU and get the
			resulting APDU as a byte[], SCardTransmit's result as result.

*/
  long __fastcall TPCSCConnector::GetResponseFromCard(const byte* apdu, const DWORD apduLength, byte *Response, DWORD& RLen)
  {
    long RetVar;
    SCARD_IO_REQUEST* IO_Request = NULL;

    switch (FAttrProtocol)
    {
      case SCARD_PROTOCOL_T0:
        IO_Request = SCARD_PCI_T0;
        break;
      case SCARD_PROTOCOL_T1:
        IO_Request = SCARD_PCI_T1;
        break;
      default:
        ;
    }

		RetVar = SCardTransmit(FCardHandle, IO_Request, apdu, apduLength, NULL, Response, &RLen);

    if (RetVar != SCARD_S_SUCCESS)
    {
      if (Assigned(FOnError))
      {
        FOnError(this, esTransmit, RetVar);
      }
    }

    return RetVar;
  }
//---------------------------------------------------------------------------
  void __fastcall TPCSCConnector::MessageWndProc(Messages::TMessage &Msg)
  {
    if (Msg.Msg == WM_CARDSTATE)  //
    {
      if (Msg.WParam != SCARD_S_SUCCESS)
      {
        if (Assigned(FOnError))
        {
          FOnError(this, esGetStatus, Msg.WParam);
        }
      }

      if (ActReaderState != LastReaderState)
      {
        ProcessReaderState(LastReaderState, ActReaderState);
      }
    }
    else
    {
      Msg.Result = DefWindowProc(NotifyHandle, Msg.Msg, Msg.WParam, Msg.LParam);
    }
  }
//---------------------------------------------------------------------------
  template <typename T>
  bool __fastcall TPCSCConnector::Assigned(T event)
  {
    return event != NULL;
  }
//---------------------------------------------------------------------------
  void __fastcall TPCSCConnector::SetReaderNum(int Value)
  {
    if (Value != FUseReaderNum)
    {
      CloseAndDisconnect();

      if (Value < FReaderList->Count)
      {
        SelectedReader = (AnsiString(FReaderList->Strings[Value])).c_str();
        FUseReaderNum   = Value;
      }
      else
      {
        SelectedReader = "";
        FUseReaderNum  = -1;
      }
    }
  }
//---------------------------------------------------------------------------
  void __fastcall TPCSCConnector::CloseAndDisconnect(void)
  {
    if (FConnected)  Disconnect();

    if (ReaderOpen) Close();
  }
//---------------------------------------------------------------------------
  bool __fastcall TPCSCConnector::IsReaderOpen(void)
  {
    return ReaderOpen;
  }
//---------------------------------------------------------------------------
  void __fastcall TPCSCConnector::ProcessReaderState(const unsigned OldState, const unsigned NewState)
  {
    bool CardInOld, CardInNew;
    bool ReaderEmOld, ReaderEmNew;
    bool CardMuteOld, CardMuteNew;
    bool CardIgnore;
    CardInOld   = (OldState & SCARD_STATE_PRESENT) > 0;
    CardInNew   = (NewState & SCARD_STATE_PRESENT) > 0;
    ReaderEmOld = (OldState & SCARD_STATE_EMPTY) > 0;
    ReaderEmNew = (NewState & SCARD_STATE_EMPTY) > 0;
    CardMuteOld = (OldState & SCARD_STATE_MUTE) > 0;
    CardMuteNew = (NewState & SCARD_STATE_MUTE) > 0;
    CardIgnore  = (NewState & SCARD_STATE_IGNORE) > 0;

    if (CardMuteNew && (!CardMuteOld))
    {
      if (Assigned(FOnCardInvalid)) FOnCardInvalid(this);
    }

    if (CardInNew   &&
        !CardInOld   &&
        !CardMuteNew &&
        !CardIgnore)  CardInsertedAction();

    if (CardInOld  && !CardInNew) CardRemovedAction();

    if (ReaderEmNew && !ReaderEmOld)
    {
      if (Assigned(FOnReaderWaiting)) FOnReaderWaiting(this);
    }

    LastReaderState = NewState;
  }
//---------------------------------------------------------------------------
  void __fastcall TPCSCConnector::CardInsertedAction(void)
  {
    if (Assigned(FOnCardInserted)) FOnCardInserted(this);

    if (FConnected)  CardActiveAction();
  }
//---------------------------------------------------------------------------
  void __fastcall TPCSCConnector::CardActiveAction(void)
  {
    GetReaderAttributes();

    if (FAttrProtocol != SCARD_PROTOCOL_UNDEFINED)
    {
      if (Assigned(FOnCardActive)) FOnCardActive(this);
    }
  }
//---------------------------------------------------------------------------
  void __fastcall TPCSCConnector::CardRemovedAction(void)
  {
    ClearReaderAttributes();

    if (Assigned(FOnCardRemoved)) FOnCardRemoved(this);

    Disconnect();
  }
//---------------------------------------------------------------------------
  void __fastcall TPCSCConnector::ClearReaderAttributes(void)
  {
    FAttrCardATR      = "";
    FAttrVendorName   = "";
    FAttrVendorSerial = "";
    FAttrProtocol     = 0;
    FAttrICCType      = "";
  }

//---------------------------------------------------------------------------
  unsigned __fastcall TPCSCConnector::GetReaderState(void)
  {
    return ActReaderState;
  }
//---------------------------------------------------------------------------
  bool __fastcall TPCSCConnector::ConnectSelectedReader(void)
  {
    long RetVar;
    bool Result = false;
    RetVar = SCardConnectA(FContext,
                           SelectedReader.c_str(),
                           SCARD_SHARE_EXCLUSIVE,
                           SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1,
                           &FCardHandle,
                           &FAttrProtocol);

    switch (RetVar)
    {
      case  SCARD_S_SUCCESS      :
        CardActiveAction();
        Result = true;
        break;
      case  SCARD_W_REMOVED_CARD :
        Result = true;
        break;
      default:
        Result = false;

        if (Assigned(FOnError)) FOnError(this, esConnect, RetVar);
    }

    return Result;
  }

//---------------------------------------------------------------------------
  void __fastcall TPCSCConnector::GetReaderAttributes(void)
  {
    long RetVar;
    byte ABuf[128];
    int AIBuf;
    DWORD ALen = 128;
    RetVar = SCardGetAttrib(FCardHandle, SCARD_ATTR_ATR_STRING, ABuf, &ALen);

    if (RetVar == SCARD_S_SUCCESS)
    {
      ABuf[ALen] = 0;
      FAttrCardATR = (char *)ABuf;
    }
    else FAttrCardATR = "";

    ALen = sizeof(ABuf);
    RetVar = SCardGetAttrib(FCardHandle, SCARD_ATTR_VENDOR_NAME, ABuf, &ALen);

    if (RetVar == SCARD_S_SUCCESS)
    {
      ABuf[ALen] = 0;
      FAttrVendorName = (char *)ABuf;
    }
    else FAttrVendorName = "";

    ALen   = sizeof(ABuf);
    RetVar = SCardGetAttrib(FCardHandle, SCARD_ATTR_VENDOR_IFD_SERIAL_NO, ABuf, &ALen);

    if (RetVar == SCARD_S_SUCCESS)
    {
      ABuf[ALen] = 0;
      FAttrVendorSerial = (char *)ABuf;
    }
    else FAttrVendorSerial = "";

    ALen = sizeof(AIBuf);
    RetVar = SCardGetAttrib(FCardHandle, SCARD_ATTR_CURRENT_PROTOCOL_TYPE, (byte *) & AIBuf, &ALen);

    if (RetVar == SCARD_S_SUCCESS)
      FAttrProtocol = AIBuf;
    else FAttrProtocol = 0;

    ALen = sizeof(AIBuf);
    AIBuf = 0;
    RetVar = SCardGetAttrib(FCardHandle, SCARD_ATTR_ICC_TYPE_PER_ATR, (byte *) & AIBuf, &ALen);

    if (RetVar == SCARD_S_SUCCESS)
    {
      switch (AIBuf)
      {
        case 1  :
          FAttrICCType = "ISO7816A";
          break;
        case 2  :
          FAttrICCType = "ISO7816S";
          break;
        default:
          FAttrICCType = "UNKNOWN";
      }
    }
    else FAttrICCType = "";
  }
//---------------------------------------------------------------------------
  void __fastcall PACKAGE Register()
  {
    TComponentClass classes[1] = {__classid(TPCSCConnector)};
    RegisterComponents(L"Samples", classes, 0);
  }

} //namespace Pcscconnector
