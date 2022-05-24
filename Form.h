//---------------------------------------------------------------------------

#ifndef FormH
#define FormH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>

#include <map>
#include <vector>
#include "structures.h"

//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
	TButton *ButtonExperiment;
	TEdit *Edit1;
	TButton *ButtonBuildBars;
	TButton *ButtonPrepareDB;
	TOpenDialog *OpenDialog;
	TMemo *Memo1;
	TDateTimePicker *DateTimePicker1;
	TDateTimePicker *DateTimePicker2;
	TButton *ButtonCombineFiles;
	TButton *ButtonTest;
	TComboBox *ComboPeriodStart;
	TComboBox *ComboPeriodEnd;
	TComboBox *ComboPeriod;
	TButton *ButtonOpenSVG;
	TButton *ButtonCombineDeals;
	TCheckBox *CheckBoxDaysOver;
	TButton *ButtonSaveBars;
	TBevel *Bevel1;
	TLabel *Label1;
	TEdit *Edit2;
	TLabel *Label2;
	TLabel *Label3;
	TEdit *Edit3;
	TLabel *Label4;
	TEdit *Edit4;
	TLabel *Label5;
	TEdit *Edit5;
	TEdit *Edit6;
	TLabel *Label6;
	TEdit *Edit7;
	TLabel *Label7;
	TCheckBox *CheckBoxGood;
	TCheckBox *CheckBoxBuy;
	TCheckBox *CheckBoxLines;
	TCheckBox *CheckBoxBlack;
	TEdit *Edit8;
	TLabel *Label8;
	TRadioButton *RadioGrid;
	TRadioButton *RadioSize;
	TEdit *Edit9;
	TEdit *Edit10;
	TButton *ButtonGraph;
	TCheckBox *CheckBoxGraph;
	TMemo *Memo2;
	TComboBox *ComboType;
	void __fastcall ButtonPrepareDBClick(TObject *Sender);
	void __fastcall ButtonBuildBarsClick(TObject *Sender);
	void __fastcall ButtonExperimentClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall ButtonCombineFilesClick(TObject *Sender);
	void __fastcall ButtonOpenSVGClick(TObject *Sender);
	void __fastcall ButtonCombineDealsClick(TObject *Sender);
	void __fastcall ComboPeriodChange(TObject *Sender);
	void __fastcall ButtonSaveBarsClick(TObject *Sender);
	void __fastcall ButtonGraphClick(TObject *Sender);
	void __fastcall ButtonTestClick(TObject *Sender);
private:
	std::pair<TDate, TDate> __fastcall DateRange();
	std::pair<int, int> __fastcall PeriodRange();

public:		// User declarations
	__fastcall TForm1(TComponent* Owner);

	const UnicodeString IniFileName = "d:\\new_era\\exe\\ify\\ify.ini";
	Experiment experiment_;

};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
