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

#ifndef PcscconnectorHPP
#define PcscconnectorHPP
#include <winscard.h>
#pragma delphiheader begin
#pragma option push
#pragma option -w-      // All warnings off
#pragma option -Vx      // Zero-length empty class member functions
#pragma pack(push,8)
#include <System.hpp> // Pascal unit
#include <Sysinit.hpp>  // Pascal unit
#include <Classes.hpp>  // Pascal unit
#include <Sysutils.hpp> // Pascal unit


//-- user supplied -----------------------------------------------------------

namespace Pcscconnector
{
//-- type declarations -------------------------------------------------------
#pragma option push -b-
  enum TErrSource { esInit, esConnect, esGetStatus, esTransmit };
#pragma option pop

  typedef void __fastcall(__closure *TPCSCErrorEvent)(System::TObject* Sender, TErrSource ErrSource, unsigned ErrCode);

  class PACKAGE TPCSCConnector : public Classes::TComponent
  {
      typedef Classes::TComponent inherited;

    protected:
      SCARDCONTEXT FContext;
      SCARDHANDLE FCardHandle;
      bool FConnected;
      int FNumReaders;
      int FUseReaderNum;
      Classes::TStringList* FReaderList;
      DWORD FAttrProtocol;
      System::UnicodeString FAttrICCType;
      System::UnicodeString FAttrCardATR;
      System::UnicodeString FAttrVendorName;
      System::UnicodeString FAttrVendorSerial;

      Classes::TNotifyEvent FOnReaderWaiting;
      Classes::TNotifyEvent FOnReaderListChange;
      Classes::TNotifyEvent FOnCardInserted;
      Classes::TNotifyEvent FOnCardActive;
      Classes::TNotifyEvent FOnCardRemoved;
      Classes::TNotifyEvent FOnCardInvalid;
      TPCSCErrorEvent FOnError;

      template <typename T>
      bool __fastcall Assigned(T event);
      void __fastcall SetReaderNum(int Value);
      void __fastcall MessageWndProc(Messages::TMessage &Msg);
      bool __fastcall ConnectSelectedReader(void);
      void __fastcall ProcessReaderState(const unsigned OldState, const unsigned NewState);
      void __fastcall GetReaderAttributes(void);
      void __fastcall ClearReaderAttributes(void);
      bool __fastcall IsReaderOpen(void);
      unsigned __fastcall GetReaderState(void);
      void __fastcall CloseAndDisconnect(void);
      void __fastcall CardInsertedAction(void);
      void __fastcall CardActiveAction(void);
      void __fastcall CardRemovedAction(void);

    public:
      __fastcall virtual TPCSCConnector(Classes::TComponent* AOwner);
      __fastcall virtual ~TPCSCConnector(void);
      bool __fastcall Init(void);
      bool __fastcall Open(void);
      void __fastcall Close(void);
      bool __fastcall Connect(void);
      void __fastcall Disconnect(void);
      long __fastcall GetResponseFromCard(const byte* apdu, const DWORD apduLength, byte *Response, DWORD& RLen);

    __published:
    __property int UseReaderNum = {read = FUseReaderNum, write = SetReaderNum, default=-1};
      __property Classes::TNotifyEvent OnCardInserted = {read = FOnCardInserted, write = FOnCardInserted};
      __property Classes::TNotifyEvent OnCardActive = {read = FOnCardActive, write = FOnCardActive};
      __property Classes::TNotifyEvent OnCardRemoved = {read = FOnCardRemoved, write = FOnCardRemoved};
      __property Classes::TNotifyEvent OnCardInvalid = {read = FOnCardInvalid, write = FOnCardInvalid};
      __property Classes::TNotifyEvent OnReaderWaiting = {read = FOnReaderWaiting, write = FOnReaderWaiting};
      __property Classes::TNotifyEvent OnReaderListChange = {read = FOnReaderListChange, write = FOnReaderListChange};
      __property TPCSCErrorEvent OnError = {read = FOnError, write = FOnError};
      __property Classes::TStringList* ReaderList = {read = FReaderList};
      __property int NumReaders = {read = FNumReaders, nodefault};
      __property bool Connected = {read = FConnected, nodefault};
      __property bool Opened = {read = IsReaderOpen, nodefault};
      __property unsigned ReaderState = {read = GetReaderState, nodefault};
      __property DWORD AttrProtocol = {read = FAttrProtocol, nodefault};
      __property System::UnicodeString AttrICCType = {read = FAttrICCType};
      __property System::UnicodeString AttrCardATR = {read = FAttrCardATR};
      __property System::UnicodeString AttrVendorName = {read = FAttrVendorName};
      __property System::UnicodeString AttrVendorSerial = {read = FAttrVendorSerial};
  };


//-- var, const, procedure ---------------------------------------------------
  extern PACKAGE void __fastcall Register(void);

} /* namespace Pcscconnector */
using namespace Pcscconnector;
#pragma pack(pop)
#pragma option pop

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif  // PcscconnectorHPP
