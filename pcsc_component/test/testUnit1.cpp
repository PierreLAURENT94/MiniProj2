//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "testUnit1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "PCSCConnector"
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button1Click(TObject *Sender)
{
	PCSCConnector1->Init();
	Memo1->Lines = PCSCConnector1->ReaderList;
	Memo1->Refresh();
	if(PCSCConnector1->NumReaders > 0)
	{
	  PCSCConnector1->UseReaderNum = 0; //the first reader
	  PCSCConnector1->Open();
	}

}
//---------------------------------------------------------------------------
void __fastcall TForm1::PCSCConnector1CardActive(TObject *Sender)
{
	ShowMessage("PCSCConnector1CardActive");
}
//---------------------------------------------------------------------------
void __fastcall TForm1::PCSCConnector1CardInserted(TObject *Sender)
{
	ShowMessage("PCSCConnector1CardInserted");
}
//---------------------------------------------------------------------------
void __fastcall TForm1::PCSCConnector1CardInvalid(TObject *Sender)
{
	ShowMessage("PCSCConnector1CardInvalid");
}
//---------------------------------------------------------------------------
void __fastcall TForm1::PCSCConnector1CardRemoved(TObject *Sender)
{
	ShowMessage("PCSCConnector1CardRemoved");
}
//---------------------------------------------------------------------------
void __fastcall TForm1::PCSCConnector1Error(TObject *Sender, TErrSource ErrSource,
					unsigned ErrCode)
{
	ShowMessage("PCSCConnector1Error");
}
//---------------------------------------------------------------------------
void __fastcall TForm1::PCSCConnector1ReaderListChange(TObject *Sender)
{
	ShowMessage("PCSCConnector1ReaderListChange");
}
//---------------------------------------------------------------------------
void __fastcall TForm1::PCSCConnector1ReaderWaiting(TObject *Sender)
{
	ShowMessage("PCSCConnector1ReaderWaiting");
}
//---------------------------------------------------------------------------
