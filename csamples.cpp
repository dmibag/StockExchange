//---------------------------------------------------------------------------

#pragma hdrstop

#include "csamples.h"
#include "utils.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

using namespace std;

//////////////////////////////// BaseSample //////////////////////////////////
/// здесь рабоча€ область функций, вы€вл€ющих паттерны, максимальное количество экспериментов

optional<vector<SampleData>> BaseSample::Find(BasePatParams pat_params) const {
	switch(pat_params.id) {
		case 1: return P3DownLine(move(pat_params));
		case 2: return P3UpLine(move(pat_params));
		case 3: return P3DownLineSide(move(pat_params));
		case 4: return P3UpLineSide(move(pat_params));
		case 7: return P3NoLineNar(move(pat_params));
		case 8: return P3FollowUp(move(pat_params));
		case 9: return P3FollowDown(move(pat_params));
		case 10: return P2Up(move(pat_params));
		case 11: return P2Down(move(pat_params));

		case 12: return CostUp(move(pat_params));
		case 13: return CostDown(move(pat_params));
	}
	return std::nullopt;
}
optional<vector<SampleData>> BaseSample::P3DownLine(BasePatParams pat_params) const {

	const FrameBars& H = bars_.H();
	const FrameBars& D = bars_.D();
	Prc prc(10); // дл€ вычитани€ и сложени€ с процентом d = d - prc(25);


	SampleData ret;

	int loss_segment = SegmentV(pat_params.cur, {600,820,1060,1370}); // cur
	if (loss_segment < 1 || loss_segment > 3) return nullopt;

	bool indBuyHH = pat_params.plus + pat_params.cur > H.IndicatorAt("avg"s, 0);
	bool indBuyHL = pat_params.minus - pat_params.cur < H.IndicatorAt("avg"s, 0);
	int bw0_segment = SegmentV(H.BodySize(0)/H.WholeSize(0), {0.42, 0,69}); // cur

	ret.base_params = pat_params;
	ret.params.emplace_back("loss_segment", loss_segment);
	ret.params.emplace_back("loss", pat_params.cur);

	bool W0moreW1 = H.WholeSize(0) >  H.WholeSize(1);

	SampleData ret_buy{ret}, ret_sell{ret};
	bool ret_buy_true{true}, ret_sell_true{true};

	ret_buy.key = Key(indBuyHH, indBuyHL, bw0_segment);

if (
 ret_buy.key == "k112"s
) {
}
else ret_buy_true = false;

	ret_buy.buy = true;
	ret_buy.open_price = pat_params.plus;
	ret_buy.close_top = pat_params.plus + pat_params.cur; // good
	ret_buy.close_bottom = pat_params.plus - pat_params.cur;  // bad

	ret_sell.key = Key(indBuyHH, indBuyHL, bw0_segment);

if (
 ret_sell.key == "k110"s
) {
}
else ret_sell_true = false;

	ret_sell.buy = false;
	ret_sell.open_price = pat_params.minus;
	ret_sell.close_top = pat_params.minus + pat_params.cur; // good
	ret_sell.close_bottom = pat_params.minus - pat_params.cur;  // bad

	vector<SampleData> ret_result;
	if (ret_buy_true) ret_result.push_back(move(ret_buy));
	if (ret_sell_true)ret_result.push_back(move(ret_sell));

	if (ret_result.size() > 0) {
		return ret_result;
	}
	return nullopt;
}

optional<vector<SampleData>> BaseSample::P3UpLine(BasePatParams pat_params) const {
	const FrameBars& H = bars_.H();
	const FrameBars& D = bars_.D();
	Prc prc(10); // дл€ вычитани€ и сложени€ с процентом d = d - prc(25);


	SampleData ret;

	int loss_segment = SegmentV(pat_params.cur, {2000, 4000}); // cur
	if (loss_segment < 1 || loss_segment > 1) return nullopt;

	bool indBuyHH = pat_params.plus + pat_params.cur > H.IndicatorAt("avg"s, 0);
	bool indBuyHL = pat_params.plus - pat_params.cur < H.IndicatorAt("avg"s, 0);
	bool indSellHH = pat_params.minus + pat_params.cur > H.IndicatorAt("avg"s, 0);
	bool indSellHL = pat_params.minus - pat_params.cur < H.IndicatorAt("avg"s, 0);
	bool indBuyDH = pat_params.plus + pat_params.cur > D.IndicatorAt("avg"s, 0);
	bool indBuyDL = pat_params.plus - pat_params.cur < D.IndicatorAt("avg"s, 0);
	bool indSellDH = pat_params.minus + pat_params.cur > D.IndicatorAt("avg"s, 0);
	bool indSellDL = pat_params.minus - pat_params.cur < D.IndicatorAt("avg"s, 0);
	int bw0_segment = SegmentV(H.BodySize(0)/H.WholeSize(0), {0.42, 0,69}); // cur
	bool W0moreW1 = H.WholeSize(0) >  H.WholeSize(1);

	bool indDup = D.IndicatorAt("avg"s, 0) > D.IndicatorAt("avg"s, 1);
	bool indHup = H.IndicatorAt("avg"s, 0) > H.IndicatorAt("avg"s, 1);
	bool indBuyD = pat_params.plus > D.IndicatorAt("avg"s, 0);
	bool indBuyH = pat_params.plus > H.IndicatorAt("avg"s, 0);
	bool indSellD = pat_params.minus > D.IndicatorAt("avg"s, 0);
	bool indSellH = pat_params.minus > H.IndicatorAt("avg"s, 0);

	bool Green0 = H.Green(0) == 1;
	bool Green1 = H.Green(1) == 1;
	bool Red1 = H.Green(1) == -1;
	bool Green3 = H.Green(3) == 1;
	bool Red2 = H.Green(2) == -1;

	ret.base_params = pat_params;
	ret.params.emplace_back("loss_segment", loss_segment);
	ret.params.emplace_back("loss", pat_params.cur);


	SampleData ret_buy{ret}, ret_sell{ret};
	bool ret_buy_true{true}, ret_sell_true{true};

	ret_buy.key = Key(Red1, Green0,W0moreW1 );
	ret_buy.buy = true;
	ret_buy.open_price = pat_params.plus;
	ret_buy.close_top = pat_params.plus + pat_params.cur; // good
	ret_buy.close_bottom = pat_params.plus - pat_params.cur;  // bad
/*
if (
  ret_buy.key == "k1111"s
) {
}
else if (
  ret_buy.key == "k10100"s
) {
 ret_buy_true = false;
	ret_buy.buy = false;
}
else ret_buy_true = false;

 */

	ret_sell.key = Key(indSellHH, indSellHL, bw0_segment);
	ret_sell.buy = false;
	ret_sell.open_price = pat_params.minus;
	ret_sell.close_top = pat_params.minus + pat_params.cur; // good
	ret_sell.close_bottom = pat_params.minus - pat_params.cur;  // bad


	if (ret_sell.key == "k102"s	) {
	}
	else if ( ret_sell.key == "k012"s ) {
		ret_sell.buy = true;
	}
	else ret_sell_true = false;

	if (ret_buy_true && ret_sell_true) {
		if (ret_buy.buy != ret_sell.buy) {
	//		return nullopt;
		}
	}

	vector<SampleData> ret_result;
	if (ret_buy_true) ret_result.push_back(move(ret_buy));
//	if (ret_sell_true)ret_result.push_back(move(ret_sell));

	if (ret_result.size() > 0) {
		return ret_result;
	}
	return nullopt;
}
//////////////////////////////////////////////////////////////////////////////
optional<vector<SampleData>> BaseSample::P3DownLineSide(BasePatParams pat_params) const {

	const FrameBars& H = bars_.H();
	const FrameBars& D = bars_.D();
	const int inspect_bars_count = 9;
	if (H.HasClosedBars(inspect_bars_count) == false) return nullopt;

	SampleData ret;
	Prc prc(10); // дл€ вычитани€ и сложени€ с процентом d = d - prc(25);

	ret.buy = true;
	ret.open_price = pat_params.cur;
	pat_params.plus = pat_params.minus;

	ret.close_top = pat_params.cur + pat_params.plus; // good
	ret.close_bottom = pat_params.cur - pat_params.minus;  // bad
	ret.up = K.IsGoingUp(ret.open_price);

	int trend_month_segment = SegmentV(K.TrendBar(20, pat_params.bar.close).value, {.33,.67});
	int trend_week_segment = SegmentV(K.TrendBar(5, pat_params.bar.close).value, {.33,.67});
	int loss_segment = SegmentV(pat_params.minus, {300.,500.,1000.,1500.,3000.});
	int day_segment = SegmentV(pat_params.bar.datetimevals.hour, {10,14,19});

	int power_upper_20_segment = SegmentV(K.PowerUpper(20, ret.open_price).value, {0.25,0.5,0.75});
	int power_bar_20_bool_segment = SegmentV(K.PowerBar(BarTimeFrame::H, 0, 20).value, {1.});

	if (loss_segment == 0 || loss_segment == 5) return nullopt;
//	if (loss_segment != 4) return nullopt;

	int count_avg{1};
	if (D.IndicatorAt("avg"s, 0) - D.IndicatorAt("avg"s, 1) > 0) {
		while (count_avg < D.IndicatorDeq("avg"s).size() - D.Indicator("avg"s).Tail() && (D.IndicatorAt("avg"s, count_avg) - D.IndicatorAt("avg"s, count_avg + 1) > 0)) { count_avg++; }
	}
	else {
		while (count_avg < D.IndicatorDeq("avg"s).size() - D.Indicator("avg"s).Tail() && (D.IndicatorAt("avg"s, count_avg) - D.IndicatorAt("avg"s, count_avg + 1) < 0)) { count_avg++; }
		count_avg = -count_avg;
	}

	int avg_segment = SegmentV(count_avg, {-30,-15,-3,3,15,30});


	bool Green0 = H.Green(0) == 1;
	bool Green1 = H.Green(1) == 1;
	bool Green3 = H.Green(3) == 1;
	bool Red2 = H.Green(2) == -1;
	bool B0moreB1 = H.BodySize(0) >  H.BodySize(1);
	bool B2moreB0 = H.BodySize(2) >  H.BodySize(0);
	bool B2moreB1 = H.BodySize(2) >  H.BodySize(1);
	bool BU3moreCloseTop = H.BodyUp(3) > ret.close_top;


	ret.key = Key(Green0, Green1, Red2, Green3, power_bar_20_bool_segment, power_upper_20_segment, avg_segment);

	if (
 ret.key == "k0010034"s
|| ret.key == "k0110033"s
|| ret.key == "k0011034"s
|| ret.key == "k1001003"s
|| ret.key == "k0000031"s
	) {
	}
	else if (
 ret.key == "k1010033"s
|| ret.key == "k1010031"s
|| ret.key == "k1010035"s
|| ret.key == "k1011033"s
	)
	{
//		return nullopt;
		ret.buy = false;
	}
	else return nullopt;


	ret.params.emplace_back("profit", pat_params.plus);
	ret.params.emplace_back("loss", pat_params.minus);

	ret.params.emplace_back("month_seg", static_cast<double>(trend_month_segment));
	ret.params.emplace_back("week_seg", static_cast<double>(trend_week_segment));
	ret.params.emplace_back("loss_segment", static_cast<double>(loss_segment));
	ret.params.emplace_back("day_segment", static_cast<double>(day_segment));

//	ret.params.push_back(K.PowerGreen(20));

	ret.base_params = std::move(pat_params);
	vector<SampleData> retresult;
	retresult.push_back(move(ret));
	return retresult;
}
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
optional<vector<SampleData>> BaseSample::P3UpLineSide(BasePatParams pat_params) const {

	const FrameBars& H = bars_.H();
	const FrameBars& D = bars_.D();
	const int inspect_bars_count = 9;
	if (H.HasClosedBars(inspect_bars_count) == false) return nullopt;

	SampleData ret;
	Prc prc(10); // дл€ вычитани€ и сложени€ с процентом d = d - prc(25);

	ret.buy = true;
	ret.open_price = pat_params.cur;
	pat_params.minus = pat_params.plus;

	ret.close_top = pat_params.cur + pat_params.plus; // good
	ret.close_bottom = pat_params.cur - pat_params.minus;  // bad
	ret.up = K.IsGoingUp(ret.open_price);

	int trend_month_segment = SegmentV(K.TrendBar(20, pat_params.bar.close).value, {.33,.67});
	int trend_week_segment = SegmentV(K.TrendBar(5, pat_params.bar.close).value, {.33,.67});
	int loss_segment = SegmentV(pat_params.plus, {300.,500.,1000.,1500.,3000.});
	int day_segment = SegmentV(pat_params.bar.datetimevals.hour, {10,14,19});

	int power_upper_20_segment = SegmentV(K.PowerUpper(20, ret.open_price).value, {0.25,0.5,0.75});
	int power_bar_20_bool_segment = SegmentV(K.PowerBar(BarTimeFrame::H, 0, 20).value, {1.});

	if (loss_segment == 0 || loss_segment == 5) return nullopt;
//	if (loss_segment != 4) return nullopt;

	int count_avg{1};
	if (D.IndicatorAt("avg"s, 0) - D.IndicatorAt("avg"s, 1) > 0) {
		while (count_avg < D.IndicatorDeq("avg"s).size() - D.Indicator("avg"s).Tail() && (D.IndicatorAt("avg"s, count_avg) - D.IndicatorAt("avg"s, count_avg + 1) > 0)) { count_avg++; }
	}
	else {
		while (count_avg < D.IndicatorDeq("avg"s).size() - D.Indicator("avg"s).Tail() && (D.IndicatorAt("avg"s, count_avg) - D.IndicatorAt("avg"s, count_avg + 1) < 0)) { count_avg++; }
		count_avg = -count_avg;
	}

	int avg_segment = SegmentV(count_avg, {-30,-15,-3,3,15,30});


	bool Green0 = H.Green(0) == -1;
	bool Green1 = H.Green(1) == -1;
	bool Green3 = H.Green(3) == -1;
	bool Red2 = H.Green(2) == 1;
	bool B0moreB1 = H.BodySize(0) >  H.BodySize(1);
	bool B2moreB0 = H.BodySize(2) >  H.BodySize(0);
	bool B2moreB1 = H.BodySize(2) >  H.BodySize(1);

	ret.key = Key(Green0, Green1, Red2, Green3, power_bar_20_bool_segment, power_upper_20_segment, avg_segment);

	if (
 ret.key == "k0010002"s
|| ret.key == "k0010005"s
|| ret.key == "k1000005"s
|| ret.key == "k1011034"s

	) {
	}
	else if (
 ret.key == "k0011002"s
|| ret.key == "k1000002"s
|| ret.key == "k1011003"s
|| ret.key == "k1000003"s
|| ret.key == "k0010000"s


	)
	{
		ret.buy = false;
	}
	else return nullopt;


	ret.params.emplace_back("profit", pat_params.plus);
	ret.params.emplace_back("loss", pat_params.minus);

	ret.params.emplace_back("month_seg", static_cast<double>(trend_month_segment));
	ret.params.emplace_back("week_seg", static_cast<double>(trend_week_segment));
	ret.params.emplace_back("loss_segment", static_cast<double>(loss_segment));
	ret.params.emplace_back("day_segment", static_cast<double>(day_segment));

//	ret.params.push_back(K.PowerGreen(20));

	ret.base_params = std::move(pat_params);
	vector<SampleData> retresult;
	retresult.push_back(move(ret));
	return retresult;
}

optional<vector<SampleData>> BaseSample::P3NoLineNar(BasePatParams pat_params) const {

	const FrameBars& H = bars_.H();
	const FrameBars& D = bars_.D();
	const int inspect_bars_count = 9;
	if (H.HasClosedBars(inspect_bars_count) == false) return nullopt;

	SampleData ret;
	Prc prc(10); // дл€ вычитани€ и сложени€ с процентом d = d - prc(25);

	ret.buy = true;
	ret.open_price = pat_params.cur;
//	pat_params.plus = pat_params.minus;

	ret.close_top = pat_params.cur + pat_params.plus; // good
	ret.close_bottom = pat_params.cur - pat_params.minus;  // bad
	ret.up = K.IsGoingUp(ret.open_price);

	int trend_month_segment = SegmentV(K.TrendBar(20, pat_params.bar.close).value, {.33,.67});
	int trend_week_segment = SegmentV(K.TrendBar(5, pat_params.bar.close).value, {.33,.67});
	int loss_segment = SegmentV(pat_params.minus, {300.,500.,1000.,1500.,3000.});
	int day_segment = SegmentV(pat_params.bar.datetimevals.hour, {10,14,19});

	int power_upper_20_segment = SegmentV(K.PowerUpper(20, ret.open_price).value, {0.25,0.5,0.75});
	int power_bar_20_bool_segment = SegmentV(K.PowerBar(BarTimeFrame::H, 0, 20).value, {1.});

	if (loss_segment == 0 || loss_segment == 5) return nullopt;
//	if (loss_segment != 4) return nullopt;

	int count_avg{1};
	if (D.IndicatorAt("avg"s, 0) - D.IndicatorAt("avg"s, 1) > 0) {
		while (count_avg < D.IndicatorDeq("avg"s).size() - D.Indicator("avg"s).Tail() && (D.IndicatorAt("avg"s, count_avg) - D.IndicatorAt("avg"s, count_avg + 1) > 0)) { count_avg++; }
	}
	else {
		while (count_avg < D.IndicatorDeq("avg"s).size() - D.Indicator("avg"s).Tail() && (D.IndicatorAt("avg"s, count_avg) - D.IndicatorAt("avg"s, count_avg + 1) < 0)) { count_avg++; }
		count_avg = -count_avg;
	}

	int avg_segment = SegmentV(count_avg, {-30,-15,-3,3,15,30});


	bool Green0 = H.Green(0) == 1;
	bool Green1 = H.Green(1) == 1;
	bool Green3 = H.Green(3) == 1;
	bool Red2 = H.Green(2) == -1;
	bool B0moreB1 = H.BodySize(0) >  H.BodySize(1);
	bool B2moreB0 = H.BodySize(2) >  H.BodySize(0);
	bool B2moreB1 = H.BodySize(2) >  H.BodySize(1);
	bool BU3moreCloseTop = H.BodyUp(3) > ret.close_top;


	ret.key = Key(Green0, Green1, Red2, Green3, power_bar_20_bool_segment, power_upper_20_segment, avg_segment);
	ret.params.emplace_back("profit", pat_params.plus);
	ret.params.emplace_back("loss", pat_params.minus);

	ret.params.emplace_back("month_seg", static_cast<double>(trend_month_segment));
	ret.params.emplace_back("week_seg", static_cast<double>(trend_week_segment));
	ret.params.emplace_back("loss_segment", static_cast<double>(loss_segment));
	ret.params.emplace_back("day_segment", static_cast<double>(day_segment));


	ret.base_params = std::move(pat_params);
	vector<SampleData> retresult;
	retresult.push_back(move(ret));
	return retresult;
}
//////////////////////////////////////////////////////////////////////////////
optional<vector<SampleData>> BaseSample::P3FollowUp(BasePatParams pat_params) const {

	const FrameBars& H = bars_.H();
	const FrameBars& D = bars_.D();
	const int inspect_bars_count = 9;
	if (H.HasClosedBars(inspect_bars_count) == false) return nullopt;

	SampleData ret;
	Prc prc(10); // дл€ вычитани€ и сложени€ с процентом d = d - prc(25);

	ret.buy = true;
	ret.open_price = pat_params.cur;
//	pat_params.plus = pat_params.minus;

	ret.close_top = pat_params.cur + pat_params.plus; // good
	ret.close_bottom = pat_params.cur - pat_params.minus;  // bad
	ret.up = K.IsGoingUp(ret.open_price);

	int trend_month_segment = SegmentV(K.TrendBar(20, pat_params.bar.close).value, {.33,.67});
	int trend_week_segment = SegmentV(K.TrendBar(5, pat_params.bar.close).value, {.33,.67});
	int loss_segment = SegmentV(pat_params.minus, {300.,500.,1000.,1500.,3000.});
	int day_segment = SegmentV(pat_params.bar.datetimevals.hour, {10,14,19});

	int power_upper_20_segment = SegmentV(K.PowerUpper(20, ret.open_price).value, {0.25,0.5,0.75});
	int power_bar_20_bool_segment = SegmentV(K.PowerBar(BarTimeFrame::H, 0, 20).value, {1.});

	if (loss_segment == 0 || loss_segment == 5) return nullopt;
//	if (loss_segment != 4) return nullopt;

	int count_avg{1};
	if (D.IndicatorAt("avg"s, 0) - D.IndicatorAt("avg"s, 1) > 0) {
		while (count_avg < D.IndicatorDeq("avg"s).size() - D.Indicator("avg"s).Tail() && (D.IndicatorAt("avg"s, count_avg) - D.IndicatorAt("avg"s, count_avg + 1) > 0)) { count_avg++; }
	}
	else {
		while (count_avg < D.IndicatorDeq("avg"s).size() - D.Indicator("avg"s).Tail() && (D.IndicatorAt("avg"s, count_avg) - D.IndicatorAt("avg"s, count_avg + 1) < 0)) { count_avg++; }
		count_avg = -count_avg;
	}

	int avg_segment = SegmentV(count_avg, {-30,-15,-3,3,15,30});


	bool Green0 = H.Green(0) == 1;
	bool Green1 = H.Green(1) == 1;
	bool Green3 = H.Green(3) == 1;
	bool Red2 = H.Green(2) == -1;
	bool B0moreB1 = H.BodySize(0) >  H.BodySize(1);
	bool B2moreB0 = H.BodySize(2) >  H.BodySize(0);
	bool B2moreB1 = H.BodySize(2) >  H.BodySize(1);
	bool BU3moreCloseTop = H.BodyUp(3) > ret.close_top;


	ret.key = Key(Green0, Green1, Red2, Green3, power_bar_20_bool_segment, power_upper_20_segment, avg_segment);
	if (
		 ret.key == "k1100003"s
		|| ret.key == "k1101103"s
		|| ret.key == "k1100102"s
		|| ret.key == "k0100003"s
		|| ret.key == "k0101003"s
		|| ret.key == "k1110102"s
		|| ret.key == "k1100014"s
		|| ret.key == "k1100124"s
		|| ret.key == "k1100101"s
		|| ret.key == "k1100135"s
		|| ret.key == "k0101005"s
		|| ret.key == "k1100015"s
		|| ret.key == "k0100033"s
		|| ret.key == "k1100035"s
		|| ret.key == "k0101004"s
		|| ret.key == "k1110104"s
		|| ret.key == "k1110103"s
		|| ret.key == "k1100114"s

	) {
	}
	else if (
		 ret.key == "k1110005"s
		|| ret.key == "k0111004"s
		|| ret.key == "k1100033"s
		|| ret.key == "k1100022"s
		|| ret.key == "k0100032"s
		|| ret.key == "k1100105"s
		|| ret.key == "k1111102"s
	)
	{
// return nullopt;
		ret.buy = false;
	}
	else return nullopt;

// return nullopt;

	ret.params.emplace_back("profit", pat_params.plus);
	ret.params.emplace_back("loss", pat_params.minus);

	ret.params.emplace_back("month_seg", static_cast<double>(trend_month_segment));
	ret.params.emplace_back("week_seg", static_cast<double>(trend_week_segment));
	ret.params.emplace_back("loss_segment", static_cast<double>(loss_segment));
	ret.params.emplace_back("day_segment", static_cast<double>(day_segment));

//	ret.params.push_back(K.PowerGreen(20));

	ret.base_params = std::move(pat_params);
	vector<SampleData> retresult;
	retresult.push_back(move(ret));
	return retresult;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
optional<vector<SampleData>> BaseSample::P3FollowDown(BasePatParams pat_params) const {

	const FrameBars& H = bars_.H();
	const FrameBars& D = bars_.D();
	const int inspect_bars_count = 9;
	if (H.HasClosedBars(inspect_bars_count) == false) return nullopt;

	SampleData ret;
	Prc prc(10); // дл€ вычитани€ и сложени€ с процентом d = d - prc(25);

	ret.buy = true;
	ret.open_price = pat_params.cur;
//	pat_params.plus = pat_params.minus;

	ret.close_top = pat_params.cur + pat_params.plus; // good
	ret.close_bottom = pat_params.cur - pat_params.minus;  // bad
	ret.up = K.IsGoingUp(ret.open_price);

	int trend_month_segment = SegmentV(K.TrendBar(20, pat_params.bar.close).value, {.33,.67});
	int trend_week_segment = SegmentV(K.TrendBar(5, pat_params.bar.close).value, {.33,.67});
	int loss_segment = SegmentV(pat_params.minus, {300.,500.,1000.,1500.,3000.});
	int day_segment = SegmentV(pat_params.bar.datetimevals.hour, {10,14,19});

	int power_upper_20_segment = SegmentV(K.PowerUpper(20, ret.open_price).value, {0.25,0.5,0.75});
	int power_bar_20_bool_segment = SegmentV(K.PowerBar(BarTimeFrame::H, 0, 20).value, {1.});

	if (loss_segment == 0 || loss_segment == 5) return nullopt;
//	if (loss_segment != 4) return nullopt;

	int count_avg{1};
	if (D.IndicatorAt("avg"s, 0) - D.IndicatorAt("avg"s, 1) > 0) {
		while (count_avg < D.IndicatorDeq("avg"s).size() - D.Indicator("avg"s).Tail() && (D.IndicatorAt("avg"s, count_avg) - D.IndicatorAt("avg"s, count_avg + 1) > 0)) { count_avg++; }
	}
	else {
		while (count_avg < D.IndicatorDeq("avg"s).size() - D.Indicator("avg"s).Tail() && (D.IndicatorAt("avg"s, count_avg) - D.IndicatorAt("avg"s, count_avg + 1) < 0)) { count_avg++; }
		count_avg = -count_avg;
	}

	int avg_segment = SegmentV(count_avg, {-30,-15,-3,3,15,30});


	bool Green0 = H.Green(0) == -1;
	bool Green1 = H.Green(1) == -1;
	bool Green3 = H.Green(3) == -1;
	bool Red2 = H.Green(2) == 1;
	bool B0moreB1 = H.BodySize(0) >  H.BodySize(1);
	bool B2moreB0 = H.BodySize(2) >  H.BodySize(0);
	bool B2moreB1 = H.BodySize(2) >  H.BodySize(1);
	bool BU3moreCloseTop = H.BodyUp(3) > ret.close_top;


	ret.key = Key(Green0, Green1, Red2, Green3, power_bar_20_bool_segment, power_upper_20_segment, avg_segment);
	if (
		 ret.key == "k1100134"s
		|| ret.key == "k1101114"s
		|| ret.key == "k1100025"s
		|| ret.key == "k1110002"s
		|| ret.key == "k1111134"s
		|| ret.key == "k0100022"s
		|| ret.key == "k1100113"s
		|| ret.key == "k0100024"s
		|| ret.key == "k0100035"s
		|| ret.key == "k1110132"s
		|| ret.key == "k1110005"s
		|| ret.key == "k1110032"s
		|| ret.key == "k0111032"s

	) {

	}
	else if (
		 ret.key == "k1110004"s
		|| ret.key == "k1100104"s
		|| ret.key == "k1111133"s
		|| ret.key == "k1100002"s
		|| ret.key == "k1100023"s
		|| ret.key == "k0100030"s
		|| ret.key == "k1100014"s
		|| ret.key == "k0101031"s
		|| ret.key == "k1001134"s
		|| ret.key == "k1100131"s
		|| ret.key == "k1101136"s
		|| ret.key == "k1100133"s
		|| ret.key == "k0100032"s
		|| ret.key == "k1101133"s
		|| ret.key == "k1111031"s
		|| ret.key == "k1101132"s

	)
	{
		ret.buy = false;
	}
	else return nullopt;

//	return nullopt;

	ret.params.emplace_back("profit", pat_params.plus);
	ret.params.emplace_back("loss", pat_params.minus);

	ret.params.emplace_back("month_seg", static_cast<double>(trend_month_segment));
	ret.params.emplace_back("week_seg", static_cast<double>(trend_week_segment));
	ret.params.emplace_back("loss_segment", static_cast<double>(loss_segment));
	ret.params.emplace_back("day_segment", static_cast<double>(day_segment));

//	ret.params.push_back(K.PowerGreen(20));

	ret.base_params = std::move(pat_params);
	vector<SampleData> retresult;
	retresult.push_back(move(ret));
	return retresult;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

optional<vector<SampleData>> BaseSample::P2Up(BasePatParams pat_params) const {
	const FrameBars& M = bars_.M();
	const FrameBars& H = bars_.H();
	const FrameBars& D = bars_.D();
	Prc prc(10); // дл€ вычитани€ и сложени€ с процентом d = d - prc(25);

	SampleData ret;

	int loss_segment = SegmentV(pat_params.cur, {200, 520, 750, 1500, 3000});
	if (loss_segment < 1 || loss_segment > 3) return nullopt;

	ret.base_params = pat_params;

	int day_segment = SegmentV(pat_params.bar.datetimevals.hour, {10, 12, 14, 16, 19});
	if (day_segment < 1 || day_segment > 4) return nullopt;

	int trend200 = SegmentV(K.InBar(BarTimeFrame::M, 200, pat_params.plus - pat_params.cur).value, {0.13,0.32,0.53,0.73,0.85});
	ret.params.emplace_back("loss", pat_params.cur);
	ret.params.emplace_back("trend200", trend200);

	int back{};
	for (int i = 2; i <= 10; i++) {
		if (M.FromLast(i).low > pat_params.plus - pat_params.cur - 50) {
			back++;
		}
		else break;
	}
	if (back != 0) return nullopt;
	ret.params.emplace_back("back", back);

	int pb0 = SegmentV(K.PowerBar(BarTimeFrame::M, 0, 100).value, {1});
	int pb1 = SegmentV(K.PowerBar(BarTimeFrame::M, 1, 100).value, {1});
//	if (pb0 > 1 || pb1 > 1)return nullopt;
	ret.params.emplace_back("pb0", pb0);
	ret.params.emplace_back("pb1", pb1);


	bool Green0 = M.Green(0) == 1;
	bool Green1 = M.Green(1) == 1;

	SampleData ret_buy{ret}, ret_sell{ret};
	bool ret_buy_true{true}, ret_sell_true{true};

	if (!(Green0 && Green1)) return nullopt;
	if (!(M.BodySize(0) > M.BodySize(1))) return nullopt;


	bool t1, t2, t3;
	const FrameBars *T = &bars_.H();
	double price = pat_params.plus;
	if (T->IndicatorAt("avg"s, 0) < T->IndicatorAt("avg"s, 1)) {
		t1 = false;
		if (T->IndicatorAt("avg"s, 1) - T->IndicatorAt("avg"s, 0) < T->IndicatorAt("avg"s, 2) - T->IndicatorAt("avg"s, 1)) {
			t2 = true;
		}
		else {
			t2 = false;
		}
	}
	else {
		t1 = true;
		if (T->IndicatorAt("avg"s, 0) - T->IndicatorAt("avg"s, 1) > T->IndicatorAt("avg"s, 1) - T->IndicatorAt("avg"s, 2)) {
			t2 = true;
		}
		else t2 = false;
	}
	t3 = price > T->IndicatorAt("avg"s, 0);

if (!(M.Green(2)==1 && M.Green(3)==1)) return nullopt;
//	ret_buy.key = Key(back);
	ret_buy.key = Key(t1, t2, t3);
	ret_buy.buy = true;

//	ret_buy.follow = FOLLOW::LOSS;
	ret_buy.follow = FOLLOW::NONE;
	ret_buy.open_price = pat_params.plus;
	ret_buy.close_top = pat_params.plus + pat_params.cur; // good
	ret_buy.close_bottom = pat_params.plus - pat_params.cur;  // bad
if (
   ret_buy.key == "k101"s
//|| ret_buy.key == "k111"s
) {
}
else ret_buy_true = false;

	ret_sell.key = Key(t1, t2, t3);
//	ret_sell.buy = false;
	ret_sell.buy = false;
	ret_sell.follow = FOLLOW::LOSS;
	ret_sell.open_price = pat_params.minus;
	ret_sell.close_top = pat_params.minus + pat_params.cur; // good
	ret_sell.close_bottom = pat_params.minus - pat_params.cur;  // bad

	vector<SampleData> ret_result;
	if (ret_buy_true) ret_result.push_back(move(ret_buy));
//	if (ret_sell_true)ret_result.push_back(move(ret_sell));

	if (ret_result.size() > 0) {
		return ret_result;
	}
	return nullopt;
}


optional<vector<SampleData>> BaseSample::P2Down(BasePatParams pat_params) const {
	const FrameBars& M = bars_.M();
	const FrameBars& H = bars_.H();
	const FrameBars& D = bars_.D();
	Prc prc(10); // дл€ вычитани€ и сложени€ с процентом d = d - prc(25);

	SampleData ret;

	int loss_segment = SegmentV(pat_params.cur, {200, 520, 750, 1500, 3000});
	if (loss_segment < 1 || loss_segment > 3) return nullopt;

	ret.base_params = pat_params;

	int day_segment = SegmentV(pat_params.bar.datetimevals.hour, {10, 12, 14, 16, 19});
	if (day_segment < 1 || day_segment > 4) return nullopt;

	int trend200 = SegmentV(K.InBar(BarTimeFrame::M, 200, pat_params.plus - pat_params.cur).value, {0.13,0.32,0.53,0.73,0.85});
	ret.params.emplace_back("loss", pat_params.cur);
	ret.params.emplace_back("trend200", trend200);

	int pb0 = SegmentV(K.PowerBar(BarTimeFrame::M, 0, 100).value, {1});
	int pb1 = SegmentV(K.PowerBar(BarTimeFrame::M, 1, 100).value, {1});
//	if (pb0 > 1 || pb1 > 1)return nullopt;
	ret.params.emplace_back("pb0", pb0);
	ret.params.emplace_back("pb1", pb1);


	bool Red0 = M.Green(0) == -1;
	bool Red1 = M.Green(1) == -1;

	SampleData ret_buy{ret}, ret_sell{ret};
	bool ret_buy_true{true}, ret_sell_true{true};

	if (!(Red0 && Red1)) return nullopt;
	if (!(M.BodySize(0) < M.BodySize(1))) return nullopt;


	bool t1, t2, t3;
	const FrameBars *T = &bars_.H();
	double price = pat_params.minus;
	if (T->IndicatorAt("avg"s, 0) < T->IndicatorAt("avg"s, 1)) {
		t1 = false;
		if (T->IndicatorAt("avg"s, 1) - T->IndicatorAt("avg"s, 0) < T->IndicatorAt("avg"s, 2) - T->IndicatorAt("avg"s, 1)) {
			t2 = true;
		}
		else {
			t2 = false;
		}
	}
	else {
		t1 = true;
		if (T->IndicatorAt("avg"s, 0) - T->IndicatorAt("avg"s, 1) > T->IndicatorAt("avg"s, 1) - T->IndicatorAt("avg"s, 2)) {
			t2 = true;
		}
		else t2 = false;
	}
	t3 = price > T->IndicatorAt("avg"s, 0);

	ret_buy.key = Key(t1, t2, t3);
	ret_buy.buy = true;
//	ret_buy.buy = false;
//	ret_buy.follow = FOLLOW::PROFIT;
	ret_buy.follow = FOLLOW::LOSS;
//	ret_buy.follow = FOLLOW::NONE;
	ret_buy.open_price = pat_params.plus;
	ret_buy.close_top = pat_params.plus + pat_params.cur; // good
	ret_buy.close_bottom = pat_params.plus - pat_params.cur;  // bad


	ret_sell.key = Key(t1, t2, t3);
	ret_sell.buy = false;
	ret_sell.follow = FOLLOW::LOSS;
	ret_sell.open_price = pat_params.minus;
	ret_sell.close_top = pat_params.minus + pat_params.cur; // good
	ret_sell.close_bottom = pat_params.minus - pat_params.cur;  // bad

	vector<SampleData> ret_result;
//	if (ret_buy_true) ret_result.push_back(move(ret_buy));
	if (ret_sell_true)ret_result.push_back(move(ret_sell));

	if (ret_result.size() > 0) {
		return ret_result;
	}
	return nullopt;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

optional<vector<SampleData>> BaseSample::CostUp(BasePatParams pat_params) const {
	const FrameBars& M = bars_.M();
	const FrameBars& H = bars_.H();
	const FrameBars& D = bars_.D();
	Prc prc(10); // дл€ вычитани€ и сложени€ с процентом d = d - prc(25);

	SampleData ret;

//	int loss_segment = SegmentV(pat_params.cur, {200, 520, 750, 1500, 3000});
//	if (loss_segment < 1 || loss_segment > 3) return nullopt;

	ret.base_params = pat_params;

	int day_segment = SegmentV(pat_params.bar.datetimevals.hour, {10, 12, 14, 16, 19});
	if (day_segment < 1 || day_segment > 4) return nullopt;

//	int trend200 = SegmentV(K.InBar(BarTimeFrame::M, 200, pat_params.plus - pat_params.cur).value, {0.13,0.32,0.53,0.73,0.85});
	ret.params.emplace_back("loss", pat_params.cur);
	ret.params.emplace_back("day_segment", day_segment);
//	ret.params.emplace_back("trend200", trend200);

//	int pb0 = SegmentV(K.PowerBar(BarTimeFrame::M, 0, 100).value, {1});
//	int pb1 = SegmentV(K.PowerBar(BarTimeFrame::M, 1, 100).value, {1});
//	if (pb0 > 1 || pb1 > 1)return nullopt;
//	ret.params.emplace_back("pb0", pb0);
//	ret.params.emplace_back("pb1", pb1);


//	bool Red0 = M.Green(0) == -1;
//	bool Red1 = M.Green(1) == -1;

	SampleData ret_buy{ret}, ret_sell{ret};
	bool ret_buy_true{true}, ret_sell_true{true};

//	if (!(Red0 && Red1)) return nullopt;
//	if (!(M.BodySize(0) < M.BodySize(1))) return nullopt;


	bool t1, t2, t3;
	const FrameBars *T = &bars_.M();
	double price = pat_params.minus;
	if (T->IndicatorAt("avg"s, 0) < T->IndicatorAt("avg"s, 1)) {
		t1 = false;
		if (T->IndicatorAt("avg"s, 1) - T->IndicatorAt("avg"s, 0) < T->IndicatorAt("avg"s, 2) - T->IndicatorAt("avg"s, 1)) {
			t2 = true;
		}
		else {
			t2 = false;
		}
	}
	else {
		t1 = true;
		if (T->IndicatorAt("avg"s, 0) - T->IndicatorAt("avg"s, 1) > T->IndicatorAt("avg"s, 1) - T->IndicatorAt("avg"s, 2)) {
			t2 = true;
		}
		else t2 = false;
	}
	t3 = price > T->IndicatorAt("avg"s, 0);

	bool dtrend = D.IndicatorAt("avg"s, 0) > D.IndicatorAt("avg"s, 1);

	ret_buy.key = Key(dtrend, t1);
//	if (ret_buy.key != "k00"s) return nullopt;
//	if (!t1) return nullopt;
	ret_buy.buy = true;
//	ret_buy.follow = FOLLOW::PROFIT;
//	ret_buy.follow = FOLLOW::LOSS;
	ret_buy.follow = FOLLOW::NONE;
	ret_buy.open_price = pat_params.plus;
	ret_buy.close_top = pat_params.plus + pat_params.cur*4; // good
	ret_buy.close_bottom = pat_params.plus - pat_params.cur*2;  // bad
//	ret_buy.close_top = pat_params.plus + 1000; // good
//	ret_buy.close_bottom = pat_params.plus - 500;  // bad


	ret_sell.key = Key(dtrend, t1, t2, t3);
	ret_sell.buy = true;
//	ret_sell.follow = FOLLOW::LOSS;
	ret_sell.follow = FOLLOW::NONE;
	ret_sell.open_price = pat_params.minus;
	ret_sell.close_top = pat_params.minus + pat_params.cur*4; // good
	ret_sell.close_bottom = pat_params.minus - pat_params.cur*2;  // bad

	vector<SampleData> ret_result;
	if (ret_buy_true) ret_result.push_back(move(ret_buy));
//	if (ret_sell_true)ret_result.push_back(move(ret_sell));

	if (ret_result.size() > 0) {
		return ret_result;
	}
	return nullopt;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

optional<vector<SampleData>> BaseSample::CostDown(BasePatParams pat_params) const {
	const FrameBars& M = bars_.M();
	const FrameBars& H = bars_.H();
	const FrameBars& D = bars_.D();
	Prc prc(10); // дл€ вычитани€ и сложени€ с процентом d = d - prc(25);

	SampleData ret;

//	int loss_segment = SegmentV(pat_params.cur, {200, 520, 750, 1500, 3000});
//	if (loss_segment < 1 || loss_segment > 3) return nullopt;

	ret.base_params = pat_params;

	int day_segment = SegmentV(pat_params.bar.datetimevals.hour, {10, 12, 14, 16, 19});
	if (day_segment < 1 || day_segment > 4) return nullopt;

//	int trend200 = SegmentV(K.InBar(BarTimeFrame::M, 200, pat_params.plus - pat_params.cur).value, {0.13,0.32,0.53,0.73,0.85});
	ret.params.emplace_back("loss", pat_params.cur);
//	ret.params.emplace_back("trend200", trend200);

//	int pb0 = SegmentV(K.PowerBar(BarTimeFrame::M, 0, 100).value, {1});
//	int pb1 = SegmentV(K.PowerBar(BarTimeFrame::M, 1, 100).value, {1});
//	if (pb0 > 1 || pb1 > 1)return nullopt;
//	ret.params.emplace_back("pb0", pb0);
//	ret.params.emplace_back("pb1", pb1);


//	bool Red0 = M.Green(0) == -1;
//	bool Red1 = M.Green(1) == -1;

	SampleData ret_buy{ret}, ret_sell{ret};
	bool ret_buy_true{true}, ret_sell_true{true};

//	if (!(Red0 && Red1)) return nullopt;
//	if (!(M.BodySize(0) < M.BodySize(1))) return nullopt;


	bool t1, t2, t3;
	const FrameBars *T = &bars_.M();
	double price = pat_params.minus;
	if (T->IndicatorAt("avg"s, 0) < T->IndicatorAt("avg"s, 1)) {
		t1 = false;
		if (T->IndicatorAt("avg"s, 1) - T->IndicatorAt("avg"s, 0) < T->IndicatorAt("avg"s, 2) - T->IndicatorAt("avg"s, 1)) {
			t2 = true;
		}
		else {
			t2 = false;
		}
	}
	else {
		t1 = true;
		if (T->IndicatorAt("avg"s, 0) - T->IndicatorAt("avg"s, 1) > T->IndicatorAt("avg"s, 1) - T->IndicatorAt("avg"s, 2)) {
			t2 = true;
		}
		else t2 = false;
	}
	t3 = price > T->IndicatorAt("avg"s, 0);

	ret_buy.key = Key(t1, t2, t3);
//	if (!t1) return nullopt;
	ret_buy.buy = true;
//	ret_buy.follow = FOLLOW::PROFIT;
//	ret_buy.follow = FOLLOW::LOSS;
	ret_buy.follow = FOLLOW::NONE;
	ret_buy.open_price = pat_params.plus;
	ret_buy.close_top = pat_params.plus + pat_params.cur*4; // good
	ret_buy.close_bottom = pat_params.plus - pat_params.cur*2;  // bad
//	ret_buy.close_top = pat_params.plus + 1000; // good
//	ret_buy.close_bottom = pat_params.plus - 500;  // bad

	bool dtrend = D.IndicatorAt("avg"s, 0) > D.IndicatorAt("avg"s, 1);
	ret_sell.key = Key(dtrend, t1);
	if (ret_sell.key != "k00"s) return nullopt;
	ret_sell.buy = false;
//	ret_sell.follow = FOLLOW::LOSS;
	ret_sell.follow = FOLLOW::NONE;
	ret_sell.open_price = pat_params.minus;
	ret_sell.close_top = pat_params.minus + pat_params.cur*2; // good
	ret_sell.close_bottom = pat_params.minus - pat_params.cur*4;  // bad

	vector<SampleData> ret_result;
//	if (ret_buy_true) ret_result.push_back(move(ret_buy));
	if (ret_sell_true)ret_result.push_back(move(ret_sell));

	if (ret_result.size() > 0) {
		return ret_result;
	}
	return nullopt;
}

