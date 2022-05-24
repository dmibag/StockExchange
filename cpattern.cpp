//---------------------------------------------------------------------------

#pragma hdrstop

#include "cpattern.h"
#include "utils.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)


std::optional<BasePatParams> Pattern3DownLine::FindPattern() const {
	if (bars_.H().HasClosedBars(3)
		&& bars_.H().Low(2) > bars_.H().Low(1)
		&& bars_.H().Low(0) > bars_.H().Low(1)
		&& bars_.H().High(0) > bars_.H().High(1)

		&& bars_.H().WholeSize(0) > bars_.H().Step()*10
		&& bars_.H().WholeSize(1) > bars_.H().Step()*10
	) {
		BasePatParams ret;
		ret.cur = ClearToStep(  (bars_.H().High(0) - bars_.H().Low(1))  , bars_.H().Step());
		ret.plus = bars_.H().High(0) + bars_.H().Step()*2;
		ret.minus = bars_.H().Low(1) - bars_.H().Step()*2;

		ret.name = Name();
		ret.id = ID();
		ret.bar = bars_.H().FromLast(0);
		return ret;
	}
	return std::nullopt;
}

std::optional<BasePatParams> Pattern3UpLine::FindPattern() const {
	if (bars_.H().HasClosedBars(3)
		&& bars_.H().High(2) < bars_.H().High(1)
		&& bars_.H().Low(0) < bars_.H().Low(1)
		&& bars_.H().High(0) < bars_.H().High(1)

		&& bars_.H().WholeSize(0) > bars_.H().Step()*10
		&& bars_.H().WholeSize(1) > bars_.H().Step()*10
	) {
		BasePatParams ret;
		ret.cur = ClearToStep(  (bars_.H().High(1) - bars_.H().Low(0))*2  , bars_.H().Step());
		ret.plus = bars_.H().High(1) + bars_.H().Step()*2;
		ret.minus = bars_.H().Low(0) - bars_.H().Step()*2;

		ret.name = Name();
		ret.id = ID();
		ret.bar = bars_.H().FromLast(0);
		return ret;
	}
	return std::nullopt;
}

std::optional<BasePatParams> Pattern3DownLineSide::FindPattern() const {
	if (bars_.H().HasClosedBars(3)
		&& bars_.H().Low(2) > bars_.H().Low(1)
		&& bars_.H().Low(0) > bars_.H().Low(1)
		&& bars_.H().High(0) < bars_.H().High(1)
		&& bars_.H().WholeSize(0) > bars_.H().Step()*10
		&& bars_.H().WholeSize(1) > bars_.H().Step()*10
	) {
		BasePatParams ret;
		ret.cur = GetMiddle(bars_.H().FromLast(0), bars_.H().Step());
		ret.plus = bars_.H().WholeSize(0);
		ret.minus = ret.cur - (bars_.H().Low(1) - bars_.H().Step());

		ret.name = Name();
		ret.id = ID();
		ret.bar = bars_.H().FromLast(0);
		return ret;
	}
	return std::nullopt;
}

std::optional<BasePatParams> Pattern3UpLineSide::FindPattern() const {
	if (bars_.H().HasClosedBars(3)
		&& bars_.H().High(2) < bars_.H().High(1)
		&& bars_.H().Low(0) < bars_.H().Low(1)
		&& bars_.H().High(0) < bars_.H().High(1)
		&& bars_.H().WholeSize(0) > bars_.H().Step()*10
		&& bars_.H().WholeSize(1) > bars_.H().Step()*10
	) {
		BasePatParams ret;
//		ret.cur = GetMiddle(bars_.H().FromLast(0), bars_.H().Step());
//		ret.plus = (bars_.H().High(1) + bars_.H().Step()) - ret.cur;
//		ret.minus = bars_.H().WholeSize(0);

		ret.name = Name();
		ret.id = ID();
		ret.bar = bars_.H().FromLast(0);
		return ret;
	}
	return std::nullopt;
}

std::optional<BasePatParams> Pattern3NoLineNar::FindPattern() const {
	if (bars_.H().HasClosedBars(3)
		&& bars_.H().Low(2) < bars_.H().Low(1)
		&& bars_.H().Low(1) < bars_.H().Low(0)
		&& bars_.H().High(2) > bars_.H().High(1)
		&& bars_.H().High(1) > bars_.H().High(0)
		&& bars_.H().WholeSize(0) > bars_.H().Step()*10
		&& bars_.H().WholeSize(1) > bars_.H().Step()*10
	) {
		BasePatParams ret;
		ret.cur = GetMiddle(bars_.H().FromLast(2), bars_.H().Step());
		ret.minus = ret.cur - (bars_.H().Low(2) - bars_.H().Step());
		ret.plus = ret.minus;

		ret.name = Name();
		ret.id = ID();
		ret.bar = bars_.H().FromLast(0);
		return ret;
	}
	return std::nullopt;
}

std::optional<BasePatParams> Pattern3FollowUp::FindPattern() const {
	if (bars_.H().HasClosedBars(3)
		&& bars_.H().Low(2) < bars_.H().Low(1)
		&& bars_.H().Low(1) < bars_.H().Low(0)
		&& bars_.H().High(2) < bars_.H().High(1)
		&& bars_.H().High(1) < bars_.H().High(0)
		&& bars_.H().WholeSize(0) > bars_.H().Step()*10
		&& bars_.H().WholeSize(1) > bars_.H().Step()*10
	) {
		BasePatParams ret;
		ret.cur = GetMiddle(bars_.H().FromLast(0), bars_.H().Step());
		ret.minus = bars_.H().WholeSize(0);
		ret.plus = ret.minus;

		ret.name = Name();
		ret.id = ID();
		ret.bar = bars_.H().FromLast(0);
		return ret;
	}
	return std::nullopt;
}

std::optional<BasePatParams> Pattern3FollowDown::FindPattern() const {
	if (bars_.H().HasClosedBars(3)
		&& bars_.H().Low(2) > bars_.H().Low(1)
		&& bars_.H().Low(1) > bars_.H().Low(0)
		&& bars_.H().High(2) > bars_.H().High(1)
		&& bars_.H().High(1) > bars_.H().High(0)
		&& bars_.H().WholeSize(0) > bars_.H().Step()*10
		&& bars_.H().WholeSize(1) > bars_.H().Step()*10
	) {
		BasePatParams ret;
		ret.cur = GetMiddle(bars_.H().FromLast(0), bars_.H().Step());
		ret.minus = bars_.H().WholeSize(0);
		ret.plus = ret.minus;

		ret.name = Name();
		ret.id = ID();
		ret.bar = bars_.H().FromLast(0);
		return ret;
	}
	return std::nullopt;
}

std::optional<BasePatParams> Pattern2Up::FindPattern() const {
	if (bars_.M().HasClosedBars(3)
		&& bars_.M().Low(1) < bars_.M().Low(0)
		&& bars_.M().High(1) < bars_.M().High(0)
		&& bars_.M().WholeSize(0) > bars_.M().Step()*5
		&& bars_.M().WholeSize(1) > bars_.M().Step()*5
	) {
		BasePatParams ret;
//		if (bars_.M().Green(2) == -1) return std::nullopt;
//		ret.cur = ClearToStep(  (bars_.M().Close(0) - bars_.M().Open(1)), bars_.M().Step());
//		if (ret.cur <= 0) return std::nullopt;
		double low = bars_.M().Low(1);

		ret.cur = ClearToStep(  (bars_.M().High(0) - low), bars_.M().Step());
//		ret.cur = ClearToStep(  (bars_.M().High(0) - bars_.M().Low(0)), bars_.M().Step());
		ret.plus = bars_.M().High(0) + bars_.M().Step()*2;
		ret.minus = low - bars_.M().Step()*2;

		ret.name = Name();
		ret.id = ID();
		ret.bar = bars_.M().FromLast(0);
		ret.type = BarTimeFrame::M;
		return ret;
	}
	return std::nullopt;
}

std::optional<BasePatParams> Pattern2Down::FindPattern() const {
	if (bars_.M().HasClosedBars(3)
		&& bars_.M().Low(1) > bars_.M().Low(0)
		&& bars_.M().High(1) > bars_.M().High(0)
		&& bars_.M().WholeSize(0) > bars_.M().Step()*5
		&& bars_.M().WholeSize(1) > bars_.M().Step()*5
	) {
		BasePatParams ret;

//		ret.cur = ClearToStep(  (bars_.M().Close(0) - bars_.M().Open(1)), bars_.M().Step());
//		if (ret.cur <= 0) return std::nullopt;
		double low = bars_.M().Low(0);

		ret.cur = ClearToStep(  (bars_.M().High(1) - low), bars_.M().Step());
//		ret.cur = ClearToStep(  (bars_.M().High(0) - bars_.M().Low(0)), bars_.M().Step());
		ret.plus = bars_.M().High(1) + bars_.M().Step()*2;
		ret.minus = low - bars_.M().Step()*2;

		ret.name = Name();
		ret.id = ID();
		ret.bar = bars_.M().FromLast(0);
		ret.type = BarTimeFrame::M;
		return ret;
	}
	return std::nullopt;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

std::optional<BasePatParams> PatternCostUp::FindPattern() const {
	if (bars_.M().HasClosedBars(5)
		&& bars_.M().IndicatorAt("top_cost", 0) == 1.0
	) {
		BasePatParams ret;

		ret.cur = ClearToStep(  (bars_.M().High(0) - bars_.M().Low(0)), bars_.M().Step());
		ret.plus = bars_.M().High(0) + bars_.M().Step()*3;
		ret.minus = bars_.M().Low(0) - bars_.M().Step()*3;

		ret.name = Name();
		ret.id = ID();
		ret.bar = bars_.M().FromLast(0);
		ret.type = BarTimeFrame::M;
		return ret;
	}
	return std::nullopt;
}

std::optional<BasePatParams> PatternCostDown::FindPattern() const {
	if (bars_.M().HasClosedBars(5)
		&& bars_.M().IndicatorAt("top_cost", 0) == -1.0
	) {
		BasePatParams ret;

		ret.cur = ClearToStep(  (bars_.M().High(0) - bars_.M().Low(0)), bars_.M().Step());
		ret.plus = bars_.M().High(0) + bars_.M().Step()*3;
		ret.minus = bars_.M().Low(0) - bars_.M().Step()*3;

		ret.name = Name();
		ret.id = ID();
		ret.bar = bars_.M().FromLast(0);
		ret.type = BarTimeFrame::M;
		return ret;
	}
	return std::nullopt;
}

// Экспереминтеруем с паттернами

Patterns::Patterns(const Bars& bars) : bars_(bars) {
//	patterns_.push_back(std::make_unique<Pattern3DownLine>(bars_));
//	patterns_.push_back(std::make_unique<Pattern3UpLine>(bars_));
//	patterns_.push_back(std::make_unique<Pattern3DownLineSide>(bars_));
//	patterns_.push_back(std::make_unique<Pattern3UpLineSide>(bars_));
	//	patterns_.push_back(std::make_unique<Pattern3NoLineNar>(bars_)); // мало встречается, использую
//	patterns_.push_back(std::make_unique<Pattern3FollowUp>(bars_));
//	patterns_.push_back(std::make_unique<Pattern3FollowDown>(bars_));

//	patterns_.push_back(std::make_unique<Pattern2Up>(bars_));
//	patterns_.push_back(std::make_unique<Pattern2Down>(bars_));

	patterns_.push_back(std::make_unique<PatternCostUp>(bars_));
//	patterns_.push_back(std::make_unique<PatternCostDown>(bars_));

}
