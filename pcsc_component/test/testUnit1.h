//---------------------------------------------------------------------------

#ifndef testUnit1H
#define testUnit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "PCSCConnector.h"
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
	TPCSCConnector *PCSCConnector1;
	TButton *Button1;
	TMemo *Memo1;
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall PCSCConnector1CardActive(TObject *Sender);
	void __fastcall PCSCConnector1CardInserted(TObject *Sender);
	void __fastcall PCSCConnector1CardInvalid(TObject *Sender);
	void __fastcall PCSCConnector1CardRemoved(TObject *Sender);
	void __fastcall PCSCConnector1Error(TObject *Sender, TErrSource ErrSource, unsigned ErrCode);
	void __fastcall PCSCConnector1ReaderListChange(TObject *Sender);
	void __fastcall PCSCConnector1ReaderWaiting(TObject *Sender);

private:	// User declarations
public:		// User declarations
	__fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
