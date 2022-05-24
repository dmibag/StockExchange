//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop


#include "Form.h"
#include "cbars.h"
#include "crun.h"
#include "utils.h"
#include "test.h"
#include "printsvg.h"

#include <fstream>
#include <map>
#include <vector>
#include <sstream>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;

using namespace std;

static double FEE  = 10.;

class ButCapt {
public:
	ButCapt(TObject *Sender) : sender_(Sender), caption_(((TButton*)sender_)->Caption) {
		((TButton*)sender_)->Caption = "Working...";

	}
	~ButCapt () {
		((TButton*)sender_)->Caption = caption_;
	}

private:
	TObject *sender_;
	UnicodeString caption_;
};

//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
	Test();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ButtonPrepareDBClick(TObject *Sender)
{
	ButCapt bc(Sender);

	Settings settings(IniFileName);
	UnicodeString CsvFile, DbFile, Key;

	Key = "last_db_csv_path";
	OpenDialog->InitialDir = settings.GetString(Key);
	if (OpenDialog->Execute()) {
		settings.SetString(Key, ExtractFilePath(OpenDialog->FileName));
		CsvFile = OpenDialog->FileName;
	}
	else return;

	Key = "last_db_db_path";
	OpenDialog->InitialDir = settings.GetString(Key);
	if (OpenDialog->Execute()) {
		settings.SetString(Key, ExtractFilePath(OpenDialog->FileName));
		DbFile = OpenDialog->FileName;
	}
	else return;

    // конвертация TXT в DB
	PrepareDB(CsvFile, DbFile);
	settings.SetString("db_db_path_name", OpenDialog->FileName);

	// создание маркеров
	auto markers = MakeStartStopDates(OpenDialog->FileName);

	if (markers.size()) {
		// сохранение маркеров в БД
		std::ofstream db_out(UniToStr(OpenDialog->FileName + ".markers"), std::ios::binary);
		Serialize(markers, db_out);
		// обновление маркеров в интерфейсе
		ComboPeriodStart->Items->Clear();
		ComboPeriodEnd->Items->Clear();
		ComboPeriod->Items->Clear();
		for (const auto & marker : markers) {
			ComboPeriodStart->Items->Append(marker.first);
			ComboPeriodEnd->Items->Append(marker.first);
			ComboPeriod->Items->Append(marker.first);
		}
	}

}
//---------------------------------------------------------------------------
std::pair<TDate, TDate> __fastcall TForm1::DateRange() {
	return {DateTimePicker1->Date, DateTimePicker2->Date};
}
//---------------------------------------------------------------------------

std::pair<int, int> __fastcall TForm1::PeriodRange() {
	return {StrToInt(ComboPeriodStart->Text), StrToInt(ComboPeriodEnd->Text)};
}
//---------------------------------------------------------------------------

template <typename CONT>
size_t __fastcall ExportContainer(const CONT & cont, std::string file) {
	stringstream str;
	str.imbue(std::locale(std::cout.getloc(), new comma_separator));

	if (cont.size()) {
		cont[0].PrintHeader(str) << '\n';

		for (auto& el : cont) {
			str << el << '\n';
		}
		ofstream(file) << str.str();
		return cont.size();
	}
	throw std::exception("Empty returned container");
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ButtonBuildBarsClick(TObject *Sender)
{
	ButCapt bc(Sender);
	Settings settings(IniFileName);
	Run run(settings, 10);

	auto bars = run.BuildBars(StrToInt(ComboPeriod->Text), DateRange());              ////////////////////////////////////////////////////////////////////////////////////////////
	const deque<Bar> &deq = (ComboType->ItemIndex == 0) ? bars.M().Deq() : ((ComboType->ItemIndex == 1) ? bars.H().Deq() : bars.D().Deq() );

	ExportContainer(deq, UniToStr(settings.GetString("export_bars_path_name")));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void __fastcall TForm1::ButtonExperimentClick(TObject *Sender)
{
	{
		ButCapt bc(Sender);
		Settings settings(IniFileName);
		Run run(settings, 10);

		experiment_ = run.AggregateSamples(run.Execute(PeriodRange()), FEE);

	///////////// serialization
		{
			std::ofstream db_out(UniToStr(Settings(IniFileName).GetString("exp_path_name")), std::ios::binary);
			Serialize(experiment_, db_out);
		}
	/////////////
		stringstream str;
		str.imbue(std::locale(std::cout.getloc(), new comma_separator));

		std::ofstream csv_ostream(UniToStr(Settings(IniFileName).GetString("export_experiment_path_name")));
		str << ExportExperiment(experiment_, csv_ostream);
		Memo1->Lines->Text = str.str().c_str();
		if (CheckBoxGraph->Checked) {
			ButtonGraphClick(Sender);
		}
	}
	ButtonExperiment->Caption = "Experiment UPDATED";
}
//---------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& output, const std::pair<std::string, Run::DealsStat>& par) {
	stringstream str;
	str.imbue(std::locale(std::cout.getloc(), new comma_separator));
	str << par.first << "\t"s << par.second.count << "\t"s << par.second.sum
		<< "\t"s  << std::setprecision(3) << ((double)par.second.good / (double)par.second.count)*100;
	output << str.rdbuf();
	return output;
}

void __fastcall TForm1::ButtonTestClick(TObject *Sender)
{
	{
		ButCapt bc(Sender);
		Settings settings(IniFileName);
		Run run(settings, 10);
		auto result = run.CalcKeys(PeriodRange(), FEE);

		{
			Memo2->Lines->Clear();
			stringstream str;
			for (const auto& p : result)  { str << p << "\t|| ret.key == \""s << p.first << "\"s"s << "\r\n"s; }
			Memo2->Lines->Append(str.str().c_str());
		}

		Run::VecKeys buy_keys;
		Run::VecKeys sell_keys;
		std::copy_if(result.begin(), result.end(), std::back_inserter(buy_keys), [](const auto& stat) {
			return (static_cast<double>(stat.second.good) / static_cast<double>(stat.second.count)) > 0.7 && stat.second.count >= 10 && stat.second.sum > 3000;
		});

		std::copy_if(result.begin(), result.end(), std::back_inserter(sell_keys), [](const auto& stat) {
			return (static_cast<double>(stat.second.good) / static_cast<double>(stat.second.count)) < 0.3 && stat.second.count >= 10 && stat.second.sum < -3000;
		});

		{
			Memo1->Lines->Clear();
			stringstream str;
			for (const auto& p : buy_keys)  { str << p << "\t|| ret.key == \""s << p.first << "\"s"s << "\r\n"s; }
			str << "----------------------------------\r\n"s;
			for (const auto& p : sell_keys) { str << p << "\t|| ret.key == \""s << p.first << "\"s"s << "\r\n"s; }
			Memo1->Lines->Append(str.str().c_str());
		}
	}
	ButtonExperiment->Caption = "Experiment UPDATED";
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormShow(TObject *Sender)
{
	Settings settings(IniFileName);
	// загрузка маркеров в интерфейс
	{
		auto markers = GetCachedMarkers(settings.GetString("db_db_path_name") + ".markers");

		if (markers.size()) {
			ComboPeriodStart->Items->Clear();
			ComboPeriodEnd->Items->Clear();
			ComboPeriod->Items->Clear();
			for (const auto & marker : markers) {
				ComboPeriodStart->Items->Append(marker.first);
				ComboPeriodEnd->Items->Append(marker.first);
				ComboPeriod->Items->Append(marker.first);
			}
		}
	}
    // загрузка эксперемента (результаты)
	{
		std::ifstream db_file(UniToStr(settings.GetString("exp_path_name")), std::ios::binary);
		if (db_file) {
			Deserialize(db_file, experiment_);
		}
	}

	DateTimePicker1->Date = settings.GetDate("start_date");
	DateTimePicker2->Date = settings.GetDate("end_date");
	ComboPeriodStart->ItemIndex = settings.GetInteger("period_start");
	ComboPeriodEnd->ItemIndex = settings.GetInteger("period_end");
	ComboPeriod->ItemIndex = settings.GetInteger("period");
	ComboType->ItemIndex = settings.GetInteger("type");

	Edit2->Text = settings.GetString("edit_screen_width");
	Edit3->Text = settings.GetString("edit_screen_height");
	Edit4->Text = settings.GetString("edit_left_bars");
	Edit5->Text = settings.GetString("edit_right_bars");
	Edit6->Text = settings.GetString("edit_top_level");
	Edit7->Text = settings.GetString("edit_bottom_level");
	Edit8->Text = settings.GetString("edit_opacity");
	Edit9->Text = settings.GetString("edit_top_x");
	Edit10->Text = settings.GetString("edit_bottom_x");
	CheckBoxDaysOver->Checked = settings.GetBool("check_box_days_over");
	CheckBoxBuy->Checked = settings.GetBool("check_box_buy");
	CheckBoxGood->Checked = settings.GetBool("check_box_good");
	CheckBoxLines->Checked = settings.GetBool("check_box_lines");
	CheckBoxBlack->Checked = settings.GetBool("check_box_black");
	RadioGrid->Checked = settings.GetBool("radio_grid");
	RadioSize->Checked = settings.GetBool("radio_size");
	CheckBoxGraph->Checked = settings.GetBool("check_box_graph");
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
	Settings settings(IniFileName);
	settings.SetDate("start_date", DateTimePicker1->Date);
	settings.SetDate("end_date", DateTimePicker2->Date);
	settings.SetInteger("period_start", ComboPeriodStart->ItemIndex);
	settings.SetInteger("period_end", ComboPeriodEnd->ItemIndex);
	settings.SetInteger("type", ComboType->ItemIndex);
	settings.SetInteger("period", ComboPeriod->ItemIndex);

	settings.SetString("edit_screen_width", Edit2->Text);
	settings.SetString("edit_screen_height", Edit3->Text);
	settings.SetString("edit_left_bars", Edit4->Text);
	settings.SetString("edit_right_bars", Edit5->Text);
	settings.SetString("edit_top_level", Edit6->Text);
	settings.SetString("edit_bottom_level", Edit7->Text);
	settings.SetString("edit_opacity", Edit8->Text);
	settings.SetString("edit_top_x", Edit9->Text);
	settings.SetString("edit_bottom_x", Edit10->Text);
	settings.SetBool("check_box_days_over", CheckBoxDaysOver->Checked);
	settings.SetBool("check_box_buy", CheckBoxBuy->Checked);
	settings.SetBool("check_box_good", CheckBoxGood->Checked);
	settings.SetBool("check_box_lines", CheckBoxLines->Checked);
	settings.SetBool("check_box_black", CheckBoxBlack->Checked);
	settings.SetBool("radio_grid", RadioGrid->Checked);
	settings.SetBool("radio_size", RadioSize->Checked);
	settings.SetBool("check_box_graph", CheckBoxGraph->Checked);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ButtonCombineFilesClick(TObject *Sender)
{
	ButCapt bc(Sender);
	Settings settings(IniFileName);
	UnicodeString TxtFilesPath, TxtFile, Key;

	Key = "src_db_path";
	OpenDialog->InitialDir = settings.GetString(Key);
	if (OpenDialog->Execute()) {
		settings.SetString(Key, ExtractFilePath(OpenDialog->FileName));
		TxtFilesPath = ExtractFilePath(OpenDialog->FileName);
	}
	else return;

	Key = "dst_db_pathname";
	OpenDialog->InitialDir = ExtractFilePath(settings.GetString(Key));
	if (OpenDialog->Execute()) {
		settings.SetString(Key, OpenDialog->FileName);
		TxtFile = OpenDialog->FileName;
	}
	else return;

	CreateDbFromTxt(GetFilesInDir(UniToStr(TxtFilesPath)), UniToStr(TxtFile));
}
//---------------------------------------------------------------------------


void __fastcall TForm1::ButtonOpenSVGClick(TObject *Sender)
{
	ButCapt bc(Sender);
	if (experiment_.data.size() == 0) {
		ShowMessage("Run Experiment prior");
		return;
	}

	Settings settings(IniFileName);
	Run run(settings, 10);

	auto bars = run.BuildBars(StrToInt(ComboPeriod->Text), DateRange());
	const deque<Bar> &deq = (ComboType->ItemIndex == 0) ? bars.M().Deq() : ((ComboType->ItemIndex == 1) ? bars.H().Deq() : bars.D().Deq() );
	const deque<double> &avg = (ComboType->ItemIndex == 0) ? bars.M().IndicatorDeq("avg"s) : ((ComboType->ItemIndex == 1) ? bars.H().IndicatorDeq("avg"s) : bars.D().IndicatorDeq("avg"s) );
	const deque<double> &top_cost = (ComboType->ItemIndex == 0) ? bars.M().IndicatorDeq("top_cost"s) : ((ComboType->ItemIndex == 1) ? bars.H().IndicatorDeq("top_cost"s) : bars.D().IndicatorDeq("top_cost"s) );
// save csv
//	ExportContainer(deq, UniToStr(settings.GetString("export_bars_path_name")));

// save svg
	ostringstream str_svg;
	render::RenderMap(deq, avg, top_cost, experiment_.data, StrToInt(Edit2->Text), StrToInt(Edit3->Text)).Render(str_svg);
	auto name = UniToStr(settings.GetString("export_svg_path_name") + ComboPeriod->Text + ".svg");
	ofstream(name) << str_svg.str();

// run chrome with svg
	system(std::string("start chrome "s + name).c_str());
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ButtonCombineDealsClick(TObject *Sender)
{

	ButCapt bc(Sender);
	if (experiment_.data.size() == 0) {
		ShowMessage("Run Experiment prior");
		return;
	}

	Settings settings(IniFileName);
	Run run(settings, 10);

// save svg
	bool buy = CheckBoxBuy->Checked;
	bool grid = RadioGrid->Checked;
	bool good{true};
	for (int i = 0; i < 2; i++) {

		ostringstream str_svg;
		vector<EnterData> sam;

		std::copy_if(experiment_.data.begin(), experiment_.data.end(), std::back_inserter(sam), [&](EnterData& src){
			return
				   src.good() == good
				&& src.sample.buy == buy;
		});

		render::RenderCombine(run, sam,
			StrToInt(Edit2->Text), // width
			StrToInt(Edit3->Text), // height
			grid ? StrToInt(Edit6->Text) : StrToInt(Edit9->Text), // top level
			grid ? StrToInt(Edit7->Text) : StrToInt(Edit10->Text), // bottom level
			StrToInt(Edit4->Text), // left bars
			StrToInt(Edit5->Text), // right bars
			CheckBoxLines->Checked,
			CheckBoxBlack->Checked,
			StrToFloat(Edit8->Text),
			grid,
			buy,
            good,
			5000).Render(str_svg);
		auto name = UniToStr(settings.GetString("export_svg_combine_path_name")) + std::to_string(good) + ".svg"s;
		ofstream(name) << str_svg.str();

	// run chrome with svg
		system(std::string("start chrome "s + name).c_str());
		good = false;
	}

}
//---------------------------------------------------------------------------


void __fastcall TForm1::ComboPeriodChange(TObject *Sender)
{
	Settings settings(IniFileName);
	auto markers = GetCachedMarkers(settings.GetString("db_db_path_name") + ".markers");
	DateTimePicker1->Date = markers.at(StrToInt(ComboPeriod->Text)).first;
	DateTimePicker2->Date = markers.at(StrToInt(ComboPeriod->Text)).second;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ButtonSaveBarsClick(TObject *Sender)
{
	ButCapt bc(Sender);
	Settings settings(IniFileName);

	SaveBarsToDB(UniToStr(settings.GetString("db_db_path_name")), UniToStr(settings.GetString("bars_db_path")), 10);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ButtonGraphClick(TObject *Sender)
{
	ButCapt bc(Sender);
	if (experiment_.data.size() == 0) {
		ShowMessage("Run Experiment prior");
		return;
	}

	Settings settings(IniFileName);

	// save svg
	ostringstream str_svg;
	render::RenderGraph(experiment_, StrToInt(Edit2->Text)/1.5, StrToInt(Edit3->Text)/1.5).Render(str_svg);
	auto name = UniToStr(settings.GetString("export_svg_graph_path_name"));
	ofstream(name) << str_svg.str();

// run chrome with svg
	system(std::string("start chrome "s + name).c_str());

}
//---------------------------------------------------------------------------


