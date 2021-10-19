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

#include <basepch.h>
#pragma hdrstop
#pragma package(smart_init)
//---------------------------------------------------------------------------

//   Package source.
//---------------------------------------------------------------------------


#pragma argsused
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void*)
{
	return 1;
}
//---------------------------------------------------------------------------
