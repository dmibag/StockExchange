//---------------------------------------------------------------------------

#pragma hdrstop

#include "utils.h"

#include <fstream>
#include <sstream>

#pragma package(smart_init)

using namespace std;
using filesystem::path;


void PrepareDB(UnicodeString infile, UnicodeString outfile) {
	std::ifstream db_in(UniToStr(infile), std::ios::binary);
	std::ofstream db_out(UniToStr(outfile), std::ios::binary);

	Bar bar_minute;
	std::string line;
	while (std::getline(db_in, line)) {
		line >> bar_minute;
		Serialize(bar_minute, db_out);
	}
}

map<int, pair<TDate, TDate>> MakeStartStopDates(UnicodeString DbFile) {

	std::ifstream db_file(UniToStr(DbFile), ios::binary);
	if (db_file) {

		Bar bar;
		int pred_date{};
		int cur_date{};
		int pred_iperiod{};
		map<int, pair<TDate, TDate>> voc;
		while (Deserialize(db_file, bar)) {
			if (pred_iperiod != bar.iperiod) {
				if (pred_iperiod != 0) {
					if (voc.size()) {
						voc[pred_iperiod] = {voc.at(pred_iperiod).first, pred_date};
					}
					voc[bar.iperiod] = {cur_date, 0};
				}
				pred_iperiod = bar.iperiod;
				pred_date = cur_date = static_cast<int>(bar.datetime.Val);
			}
			else if (static_cast<int>(bar.datetime.Val) != cur_date) {
				pred_date = cur_date;
				cur_date = static_cast<int>(bar.datetime.Val);
			}
		}
		voc[pred_iperiod] = {voc[pred_iperiod].first, pred_date};
		return voc;
	}
    return {};
}

vector<path> GetFilesInDir(const path& p) {
	auto status = filesystem::status(p);
	if (status.type() == filesystem::file_type::directory) {
		vector<path> list;
		for (const auto& dir_entry: filesystem::directory_iterator(p)) {
			if (dir_entry.status().type() != filesystem::file_type::directory) {
				list.push_back(dir_entry.path());
			}
		}
		sort(list.begin(), list.end(), [](const path& lhs, const path& rhs){
			return lhs.filename() < rhs.filename();
		});
		return list;
	}
	return {};
}

void CreateDbFromTxt(vector<path> src_files, const path& dst_filename) {
	string t = dst_filename.string();
	ofstream(dst_filename, ios::trunc);
	ofstream dst(dst_filename, ios::app | ios::ate);
	for (const path& p: src_files) {
		ifstream src(p, ios::in);
		string ln;
		getline(src, ln);
		dst << src.rdbuf();
	}
}

map<int, pair<TDate, TDate>> GetCachedMarkers(UnicodeString FileName) {
	map<int, pair<TDate, TDate>> res;
	std::ifstream db_in(UniToStr(FileName), std::ios::binary);
	Deserialize(db_in, res);
	return res;
}

void SaveBarsToDB(std::string db_name, std::string save_path, double step) {
	Bars bars(step);

	std::ifstream db_file(db_name, ios::binary);
	if (db_file) {
		Bar bar;
		Bar next_bar;

		Deserialize(db_file, bar);
		while (Deserialize(db_file, next_bar)) {
			bars.Update(bar);
			if (next_bar.iperiod != bar.iperiod) {
				std::ofstream db_out(save_path + to_string(bar.iperiod) + ".db"s, std::ios::binary);
				Serialize(bars, db_out);
				bars.Clear();
			}
			std::swap(bar, next_bar);
		}
		bars.Update(next_bar);
		std::ofstream db_out(save_path + to_string(next_bar.iperiod) + ".db"s, std::ios::binary);
		Serialize(bars, db_out);
	}
	else {
		throw Exception("db_file opening error");
	}
}

Aggregate ExportExperiment(const Experiment & experiment, std::ostream& output) {
	stringstream str;
	str.imbue(std::locale(std::cout.getloc(), new comma_separator));

	if (experiment.data.size() > 0) {
		str << "count"s << SEPAR;
		experiment.agg.at(0).PrintHeader(str) << SEPAR;
		experiment.data.at(0).PrintHeader(str) << '\n';

		for (int i = 0; i < experiment.data.size(); ++i) {
			str << '1' << SEPAR << experiment.agg.at(i) << SEPAR << experiment.data.at(i) << '\n';
		}
		output << str.rdbuf();
		return experiment.result;
	}
	throw Exception("Empty returned experiment structure");
}

/*
double GetMiddle(const Bar& bar, double step) {
	double whole_size = bar.high - bar.low;
	double mod = std::fmod(whole_size/2.0, step);

	double ret;
	(IsZero(mod) == false) ? ret = bar.low + whole_size/2.0 + mod
						   : ret = bar.low + whole_size/2.0;
	return ret;
}
*/

double GetMiddle(const Bar& bar, double step) {
	return ClearToStep((bar.high - bar.low)/2 + bar.low, step);
}

double ClearToStep(double value, double step) {
		double mod = std::fmod(value, step);

		return mod > step/2
			? value + step - mod
			: ( (mod == 0.) ? value : value - mod );
}


//---------------------------------------------------------------------------

