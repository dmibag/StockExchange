//---------------------------------------------------------------------------

#pragma hdrstop

#include "cbars.h"
#include "utils.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
using namespace std;

void UpdateBar(const Bar& from, Bar& to) {
	to.close = from.close;
	to.vol += from.vol;
	if (from.high > to.high) {
		to.high = from.high;
	}
	if (from.low < to.low) {
		to.low = from.low;
	}
}

bool FrameBars::Update(const Bar& bar) {
	double new_elem;
	double cur_elem;
	if (deq_.size()) {
		switch (type_) {
			case BarTimeFrame::M: new_elem = (bar.datetimevals.min/15)*15; cur_elem = (deq_.back().datetimevals.min/15)*15; break;
			case BarTimeFrame::H: new_elem = bar.datetimevals.hour; cur_elem = deq_.back().datetimevals.hour; break;
			case BarTimeFrame::D: new_elem = bar.datetimevals.day; cur_elem = deq_.back().datetimevals.day; break;
		}
	}

	if (deq_.size() && new_elem == cur_elem) {
		UpdateBar(bar, deq_.back());
		return false;
	}

	deq_.push_back(bar);

	switch (type_) {
		case BarTimeFrame::M: deq_.back().CutToMin(); break;
		case BarTimeFrame::H: deq_.back().CutToHour(); break;
		case BarTimeFrame::D: deq_.back().CutToDay(); break;
	}

	for (auto & ind : indicators_) {
		ind.second->Update(*this);
	}
	return true;
}

void IndicatorAvg::Update(const FrameBars& frame_bars) {
	const auto& bars_deq = frame_bars.Deq();
	if (bars_deq.size() > Tail()) {
		double ret{0.0};
		for (auto it = bars_deq.rbegin() + 1; it != bars_deq.rbegin() + 1 + Tail(); ++it) {
			double low = it->open > it->close ? it->close : it->open;
			double high = it->open > it->close ? it->open : it->close;
			ret += ((high - low)/2 + low)/static_cast<double>(Tail());
		}
		Deq().push_back(ClearToStep(ret, frame_bars.Step()));
	}
	else {
		Deq().push_back(0.0);
	}
}

void IndicatorAvgSpeed::Update(const FrameBars& frame_bars) {
	const auto& ind = frame_bars.Indicator("avg"s);
	if (ind.Deq().size() > 3) {
		double d0 = ind.Indicator(0) - ind.Indicator(1);
		double d1 = ind.Indicator(1) - ind.Indicator(2);
		if ((d0 < 0 && d1 > 0) || (d0 > 0 && d1 < 0)) {
			Deq().push_back(0);
		}
		else {
			double res = (d0 == 0 ? 10 : d0) / (d1 == 0 ? 10 : d1);
			if (d1 < 0) res = -res;
			Deq().push_back(res);
		}
	}
	else {
		Deq().push_back(0.0);
	}
}

void IndicatorTopCost::Update(const FrameBars& frame_bars) {
	const auto& bars_deq = frame_bars.Deq();
	double max{1.0};
	if (bars_deq.size() > Tail()) {
		if (frame_bars.High(0) - frame_bars.Low(0) == 0.0) {
			Deq().push_back(0.0);
		}
		else {
			double cur_cost = frame_bars.Vol(0) / (frame_bars.High(0) - frame_bars.Low(0));
			for (auto it = bars_deq.rbegin() + 1; it != bars_deq.rbegin() + 1 + Tail(); ++it) {
				if (it->high - it->low == 0.0) {
					continue;
				}
				double cost = it->vol / (it->high - it->low);
				if (cost > cur_cost) {
					max = 0.0;
					break;
				}
			}
			if (max != 0.0) {
				double ret{0.0};
				for (int i = 5; i > 0; --i) {
					ret += ((frame_bars.FromLast(i).high - frame_bars.FromLast(i).low)/2 + frame_bars.FromLast(i).low)/5.0;
				}

				if (ret < GetMiddle(frame_bars.FromLast(0), frame_bars.Step())) {
						Deq().push_back(1.0);
				}
				else {
						Deq().push_back(-1.0);
				}

			}
			else {
				Deq().push_back(0.0);
			}
		}
	}
	else {
		Deq().push_back(0.0);
	}
}

///////////////////            BaseIndicator

void Serialize(const BaseIndicator& val, std::ostream& out) {
	Serialize(val.Tail(), out);
	Serialize(val.Deq(), out);
}

std::istream& Deserialize(std::istream& in, BaseIndicator& val) {
	Deserialize(in, val.Tail());
	Deserialize(in, val.Deq());
	return in;
}

///////////////////            FrameBars

void Serialize(const FrameBars& val, std::ostream& out) {
	Serialize(val.Step(), out);
	Serialize(val.Frame(), out);
	Serialize(val.Deq(), out);
	Serialize(val.Indicators(), out);
}

std::istream& Deserialize(std::istream& in, FrameBars& val) {
	Deserialize(in, val.Step());
	Deserialize(in, val.Frame());
	Deserialize(in, val.Deq());
	Deserialize(in, val.Indicators());
	return in;
}

///////////////////            Bars

void Serialize(const Bars& val, std::ostream& out) {
	Serialize(val.Step(), out);
	Serialize(val.M(), out);
	Serialize(val.H(), out);
	Serialize(val.D(), out);
}

std::istream& Deserialize(std::istream& in, Bars& val) {
	Deserialize(in, val.Step());
	Deserialize(in, val.M());
	Deserialize(in, val.H());
	Deserialize(in, val.D());
	return in;
}

