//---------------------------------------------------------------------------
#pragma hdrstop

#include "capprover.h"
#include "cbars.h"
#include "center.h"
#include "crun.h"
#include "csamples.h"
#include "Form.h"
#include "structures.h"
#include "utils.h"

#include <fstream>
#include <string>
#include <Vcl.Dialogs.hpp>

#pragma package(smart_init)

using namespace std;

Run::Run(Settings& settings, double step)
	: settings_(settings), step_(step)
{

}
//---------------------------------------------------------------------------

void UpdateAproveParapms(ApproveParams & params, Bar* bar, Bar* next_bar) {
	params.new_day = params.end_day;
	params.end_day = (next_bar->datetimevals.day != bar->datetimevals.day);
	params.new_hour = params.end_hour;
	params.end_hour = (next_bar->datetimevals.hour != bar->datetimevals.hour);
	params.new_min = params.end_min;
	params.end_min = ((next_bar->datetimevals.min/15)*15 != (bar->datetimevals.min/15)*15);
}

//---------------------------------------------------------------------------
bool SkipIperiod(std::ifstream& db_file, Bar& bar, int start_period) {
	bool flag{false};
	while (Deserialize(db_file, bar)) {
		if (bar.iperiod == start_period) {
			flag = true;
			break;
		}
	}
	return flag;
}

//---------------------------------------------------------------------------
vector<EnterData> Run::Execute(std::pair<int, int> range) const {
	auto [start_period, end_period] = range;

	BarTimeFrame type{BarTimeFrame::M};
	Bars bars(step_);
	Kpi kpi(bars);
	KpiLine kpi_line(bars);

	BaseSample sample(bars, kpi);
	Enter enter(BarTimeFrame::M, 1, "test");

	auto markers = GetCachedMarkers(settings_.GetString("db_db_path_name") + ".markers");

	std::ifstream db_file(UniToStr(settings_.GetString("db_db_path_name")), std::ios::binary);
	if (db_file) {
		Bar bar;

		if (SkipIperiod(db_file, bar, start_period)) {
			// здесь уже есть первый полезный бар в bar
			Bar next_bar;
			ApproveParams params;
			Approver approver(bars, type);

			params.date_range = markers.at(bar.iperiod);

			while (Deserialize(db_file, next_bar)) {
				if (next_bar.iperiod > end_period) break;

				UpdateAproveParapms(params, &bar, &next_bar);

				if (static_cast<int>(next_bar.datetime.Val) > static_cast<int>(params.date_range.second.Val) && params.end_day == true) {
					// закрываем все открытые сделки если они есть
					enter.CloseForced(bar);
				}

				if (next_bar.iperiod != bar.iperiod) {
					bars.Clear();
					params.date_range = markers.at(next_bar.iperiod);
				}
				else {
					bars.Update(bar);
					if (auto base_pat_params = approver.Execute(bar, params)) {
						if (auto sample_data = sample.Find(base_pat_params.value())) {
							enter.AddSample(move(sample_data.value()));
						}
					}

					const FrameBars* frame_bar;
					switch (type) {
						case BarTimeFrame::M : frame_bar = &bars.M(); break;
						case BarTimeFrame::H : frame_bar = &bars.H(); break;
						case BarTimeFrame::D : frame_bar = &bars.D(); break;
					}
					if (enter.HasSamples() || enter.IsEnter()) {
						enter.Update(bars, frame_bar->FromLast(1), frame_bar->FromLast(0), bar, params);
					}
				}
				std::swap(bar, next_bar);
			}
				std::ofstream db_out(UniToStr(settings_.GetString("bars_db_path")) + to_string(next_bar.iperiod) + ".db"s, std::ios::binary);
				Serialize(bars, db_out);
			return enter.ExitWithResults(bar);
		}
		else {
			throw Exception("no bar loaded");
		}
	}
	else {
		throw Exception("db_file opening error");
	}
}

//---------------------------------------------------------------------------
vector<BasePatParams> Run::GetBasePatterns(std::pair<int, int> range) const {
	auto [start_period, end_period] = range;
	BarTimeFrame type{BarTimeFrame::H};
	Bars bars(step_);
	vector<BasePatParams> ret;

	auto markers = GetCachedMarkers(settings_.GetString("db_db_path_name") + ".markers");

	std::ifstream db_file(UniToStr(settings_.GetString("db_db_path_name")), std::ios::binary);
	if (db_file) {
		Bar bar;

		if (SkipIperiod(db_file, bar, start_period)) {
			// здесь уже есть первый полезный бар в bar
			Bar next_bar;
			ApproveParams params;
			Approver approver(bars, type);

			params.date_range = markers.at(bar.iperiod);

			while (Deserialize(db_file, next_bar)) {
				if (next_bar.iperiod > end_period) break;

				UpdateAproveParapms(params, &bar, &next_bar);

				if (next_bar.iperiod != bar.iperiod) {

					bars.Clear();
					params.date_range = markers.at(next_bar.iperiod);
				}
				else {
					bars.Update(bar);
					if (auto base_pat_params = approver.Execute(bar, params)) {
						ret.push_back(base_pat_params.value());
					}
				}
				std::swap(bar, next_bar);
			}

			return ret;
		}
		else {
			throw Exception("no bar loaded");
		}
	}
	else {
		throw Exception("db_file opening error");
	}
}
//---------------------------------------------------------------------------
bool Skip(std::ifstream& db_file, Bar& bar, TDate start_date) {
	bool flag = false;
	while (Deserialize(db_file, bar)) {
		if (bar.datetime > start_date) {
			flag = true;
			break;
		}
	}
	return flag;
}
//---------------------------------------------------------------------------

Bars Run::BuildBars(int period, std::pair<TDate, TDate> range) const {
	auto [start_date, end_date] = range;
	Bars bars(step_);

	std::ifstream db_in(UniToStr(settings_.GetString("bars_db_path")) + to_string(period) + ".db"s, std::ios::binary);
	Deserialize(db_in, bars);

	if (range.first.Val == 0 && range.second == TDate(2099,1,1)) {
		;
	}
	else bars.CutToRange(range);

	return bars;
}

Experiment Run::AggregateSamples(const std::vector<EnterData>& cont, double fee) const  {
	Experiment experiment;

	Aggregate result;
	vector<Agg> agg;
	vector<EnterData> data;

	double max{0};
	double slump{0};
	int spend_together{0};

	if (cont.size()) {
		for (const auto& el : cont) {
			if (el.correct()) {
				++result.correct_count;
				result.sum += el.result() - fee;
				if (result.sum > max) {
					slump = 0;
					max = result.sum;
				}
				else {
					if ( (max - result.sum) > slump ) {
						slump = max - result.sum;
					}
				}
				if (slump > result.max_slump) {
					result.max_slump = slump;
				}
				el.good() ? result.earned_count++ : result.spend_count++;
				el.good() ? spend_together = 0 : ++ spend_together;

				if (result.max_spend_together < spend_together) {
					result.max_spend_together = spend_together;
				}
				agg.emplace_back(el.open_bar.datetimevals.year, el.open_bar.datetime, result.sum, slump, el.result());
				data.push_back(el);
			}
			else {
				++result.incorrect_count;
			}
		}
		return {result, agg, data};
	}
	throw Exception("Empty returned enters data container");

}

Run::VecKeys Run::CalcKeys(std::pair<int, int> range, double fee) const {
	auto experiment = AggregateSamples(Execute(range), fee);
	std::map<std::string, DealsStat> stats;
	for (const auto& deal : experiment.data) {
		auto& stat = stats[deal.sample.key];
		stat.count++;
		stat.good += deal.good();
		stat.sum += deal.result();
	}
	VecKeys result(make_move_iterator(stats.begin()), make_move_iterator(stats.end()));
	sort(result.begin(), result.end(), [](const auto& lhs, const auto& rhs){
		return rhs.second.sum < lhs.second.sum; // по убыванию
	});
	return result;
}

