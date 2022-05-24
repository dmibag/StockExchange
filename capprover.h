//---------------------------------------------------------------------------

#ifndef capproverH
#define capproverH

#include "cbars.h"
#include "ckpi.h"
#include "cpattern.h"


class Approver {
public:
	Approver(const Bars& bars, BarTimeFrame type) : bars_(bars), patterns_(bars), type_(type) {}

	std::optional<BasePatParams> Execute(const Bar& from, ApproveParams & params) {
		if (	from.datetime >= params.date_range.first
			 && static_cast<int>(from.datetime.Val) < static_cast<int>(params.date_range.second.Val)
			 && params.new_day == false) {

			bool type_pass{false};
			switch (type_) {
				case BarTimeFrame::M: type_pass = params.new_min; break;
				case BarTimeFrame::H: type_pass = params.new_hour; break;
				case BarTimeFrame::D: type_pass = params.new_day; break;
			}

			if (type_pass) {
				if (auto pat = patterns_.FindPattern()) {
					return pat;
				}
				return std::nullopt;
			}
		}
		return std::nullopt;
	}

private:
	const Bars& bars_;
	Patterns patterns_;
	BarTimeFrame type_;

};
//---------------------------------------------------------------------------
#endif
