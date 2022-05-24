//---------------------------------------------------------------------------

#pragma hdrstop

#include "ckpi.h"
#include <tuple>

//---------------------------------------------------------------------------
#pragma package(smart_init)

using namespace std;
	bool Kpi::IsGoingUp(double open_price) const {
		return bars_.H().Close(0) < open_price;
	}

	Result Kpi::PowerGreen(BarTimeFrame type, int inspect_bars_count, int pattern_count) const {
		static const char* name = "power_green";
		const FrameBars *T;
		switch (type) {
			case BarTimeFrame::M: T = &bars_.M(); break;
			case BarTimeFrame::H: T = &bars_.H(); break;
			case BarTimeFrame::D: T = &bars_.D(); break;
		}
		double upbody{0};
		double downbody{0};
		for (int i = pattern_count; i < pattern_count + inspect_bars_count; i++) {
			if (T->Green(i) == 1) {
				upbody += T->BodySize(i);
			}
			else if (T->Green(i) == -1) {
				downbody += T->BodySize(i);
			}
		}
		double ret = upbody / (downbody + upbody);
		return {name, ret};
	}

	Result Kpi::PowerGreenCount(BarTimeFrame type, int inspect_bars_count, int pattern_count) const {
		static const char* name = "power_green_count";
		const FrameBars *T;
		switch (type) {
			case BarTimeFrame::M: T = &bars_.M(); break;
			case BarTimeFrame::H: T = &bars_.H(); break;
			case BarTimeFrame::D: T = &bars_.D(); break;
		}
		double upbody{0};
		double downbody{0};
		for (int i = pattern_count; i < pattern_count + inspect_bars_count; i++) {
			if (T->Green(i) == 1) {
				upbody ++;
			}
			else if (T->Green(i) == -1) {
				downbody ++;
			}
		}
		double ret = upbody / (downbody + upbody);
		return {name, ret};
	}

	Result Kpi::BodyOffset(BarTimeFrame type, int cur) const {
		static const char* name = "body_offset";
		const FrameBars *T;
		switch (type) {
			case BarTimeFrame::M: T = &bars_.M(); break;
			case BarTimeFrame::H: T = &bars_.H(); break;
			case BarTimeFrame::D: T = &bars_.D(); break;
		}
		double mid_body = T->BodyUp(cur) - (T->BodyUp(cur) - T->BodyDown(cur))/2;
		double mbtomw = (T->High(cur) - mid_body)/(T->High(cur) - T->Low(cur));
		return {name, mbtomw};
	}

	Result Kpi::BodyWhole(BarTimeFrame type, int cur) const {
		static const char* name = "body_whole";
		const FrameBars *T;
		switch (type) {
			case BarTimeFrame::M: T = &bars_.M(); break;
			case BarTimeFrame::H: T = &bars_.H(); break;
			case BarTimeFrame::D: T = &bars_.D(); break;
		}
		double btoa = T->BodySize(cur)/T->WholeSize(cur);
		return {name, btoa};
	}

	Result Kpi::PowerBar(BarTimeFrame type, int cur, int count) const {
		static const char* name = "power_bar";
		const FrameBars *T;
		switch (type) {
			case BarTimeFrame::M: T = &bars_.M(); break;
			case BarTimeFrame::H: T = &bars_.H(); break;
			case BarTimeFrame::D: T = &bars_.D(); break;
		}
		double mid = 0.0;
		for (int i = 0; i < count; i++) {
			mid += T->BodySize(cur + i + 1);
		}
		mid = mid / static_cast<double>(count);
		return {name, T->BodySize(cur)/mid};
	}

	Result Kpi::PowerUpper(int count_bars, double src, int pattern_count) const {
		static const char* name = "power_upper";
		double up{0};
		double down{0.0};
		for (int i = pattern_count; i < pattern_count + count_bars; ++i) {
			if (bars_.H().BodyDown(i) > src) {
				up += bars_.H().BodySize(i);
			}
			else if (bars_.H().BodyUp(i) < src) {
				down += bars_.H().BodySize(i);
			}
			else {
				up += bars_.H().BodyUp(i) - src;
				down +=  src - bars_.H().BodyDown(i);
			}
		}
		double ret = up / (down + up);
		return {name, ret};
	}

	Result Kpi::InBar(BarTimeFrame type, int count, double price) const {
		static const char* name = "in_bar";
		const FrameBars *T;
		switch (type) {
			case BarTimeFrame::M: T = &bars_.M(); break;
			case BarTimeFrame::H: T = &bars_.H(); break;
			case BarTimeFrame::D: T = &bars_.D(); break;
		}
		if (T->HasClosedBars(count)) {

			double ret{};
			double high{T->FromLast(0).high};
			double low{T->FromLast(0).low};

			for (int i = 1; i < count; ++i) {
				if (T->FromLast(i).high > high) {
					high = T->FromLast(i).high;
				}
				if (T->FromLast(i).low < low) {
					low = T->FromLast(i).low;
				}
			}
			double delta = high - low;
			ret = (price - low) / delta;

			return {name, ret};
        }
		return {name, -1};
	}

	Result Kpi::TrendBar(int days, double price) const {
		static const char* name = "trend_bar";
		if (bars_.D().HasClosedBars(days)) {
			double ret{};
			double high{bars_.D().FromLast(0).high};
			double low{bars_.D().FromLast(0).low};

			for (int i = 0; i < days; ++i) {
				if (bars_.D().FromLast(i).high > high) {
					high = bars_.D().FromLast(i).high;
				}
				if (bars_.D().FromLast(i).low < low) {
					low = bars_.D().FromLast(i).low;
				}
			}
			for (int i = 0; bars_.D().FromLast(0).datetimevals.day != bars_.H().FromLast(i).datetimevals.day; ++i ) {
				if (bars_.H().FromLast(i).high > high) {
					high = bars_.H().FromLast(i).high;
				}
				if (bars_.H().FromLast(i).low < low) {
					low = bars_.H().FromLast(i).low;
				}
			}
			double delta = high - low;
			ret = (price - low) / delta;

			return {name, ret};
		}
		return {name, -1};
	}
	Result Kpi::GetAvgDiff(int num) const {
		static const char* name = "avg_diff";
		return {name, bars_.D().IndicatorAt("avg"s, num) - bars_.D().IndicatorAt("avg"s, num + 1)};
	}

	Result Kpi::GetCost(BarTimeFrame type, int num) const {
		static const char* name = "cost";
		const FrameBars *T;
		switch (type) {
			case BarTimeFrame::M: T = &bars_.M(); break;
			case BarTimeFrame::H: T = &bars_.H(); break;
			case BarTimeFrame::D: T = &bars_.D(); break;
		}
		return {name, T->FromLast(num).vol / T->WholeSize(num)};
	}


