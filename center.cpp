//---------------------------------------------------------------------------

#pragma hdrstop

#include "center.h"
#include "utils.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

using namespace std;

const int WAITING_FRAMES = 0;

Enter::Enter(BarTimeFrame type, int id, string name)
	: type_(type), id_name_({id, move(name)}) {
}

void Enter::AddSample(vector<SampleData>&& sample) {
	for (int i = 0; i < sample.size(); ++i) {
		samples_.emplace_back(WAITING_FRAMES, move(sample[i]));
	}
}

pair<int, const string_view> Enter::GetName() const {
	return id_name_;
}

void Enter::UpdateExpireSamples() {
	for (auto it = samples_.begin(); it != samples_.end(); ) {
		if (it->first < 0) {
			it = samples_.erase(it);
		} else {
			it->first--;
			++it;
		}
	}
}

void Enter::CleanSamples() {
	samples_.clear();
}

bool Enter::HasSamples() const {
	return samples_.size() > 0;
}


std::vector<EnterData> && Enter::ExitWithResults(const Bar& bar) {
	if (Close(bar)) {
		results_.push_back(move(enter_.value()));
		enter_.reset();
	}
	return move(results_);
}

bool Enter::Pass(ApproveParams& params) const {
	bool type_pass{false};
	switch (type_) {
		case BarTimeFrame::M: type_pass = params.new_min; break;
		case BarTimeFrame::H: type_pass = params.new_hour; break;
		case BarTimeFrame::D: type_pass = params.new_day; break;
	}
	return type_pass;
}
int Enter::Key(const Bars& bars, BarTimeFrame type, double price) const {
	bool t1, t2, t3;
	const FrameBars *T;
	switch (type) {
		case BarTimeFrame::M: T = &bars.M(); break;
		case BarTimeFrame::H: T = &bars.H(); break;
		case BarTimeFrame::D: T = &bars.D(); break;
	}

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
	return t1*100 + t2*10 + t3;
}

bool Enter::Trend(const Bars& bars, BarTimeFrame type) const {
	const FrameBars *T;
	switch (type) {
		case BarTimeFrame::M: T = &bars.M(); break;
		case BarTimeFrame::H: T = &bars.H(); break;
		case BarTimeFrame::D: T = &bars.D(); break;
	}
	return T->IndicatorAt("avg"s, 0) > T->IndicatorAt("avg"s, 1);
}

void Enter::Update(const Bars& bars, const Bar& prev_typed_bar, const Bar& cur_typed_bar, const Bar& bar, ApproveParams& params) {
	if (params.new_day) { // сам проверяет ежедневную очистку
		CleanSamples();
	}

	if (Pass(params)) {
		UpdateExpireSamples();
	}

	if (enter_) { // в рынке
		if (TryClose(bar)) {
			results_.push_back(move(enter_.value()));
			enter_.reset();
			return;
		}
		bool buy = enter_->sample.buy;
// UNUSED PROFIT
		if (enter_->sample.follow == FOLLOW::NONE && Pass(params)) {
			if (bars.M().IndicatorAt("top_cost", 0) == -1.0)	{
//				enter_->close_bottom = enter_->close_bottom - 100;
			}
		}
		if (enter_->sample.follow == FOLLOW::PROFIT && Pass(params)) {
			// пытаемся сдвинуть стопы или закрыть сделку!
			if (buy) {
				if (cur_typed_bar.Green() && cur_typed_bar.close > enter_->offset) { // пытаемся сдвинуть стоп
					enter_->close_bottom = enter_->close_bottom + (cur_typed_bar.close - enter_->offset);
					enter_->close_top = enter_->close_top + (cur_typed_bar.close - enter_->offset);
					enter_->offset = cur_typed_bar.close;
				}
			}
			else {
				if (!cur_typed_bar.Green() && cur_typed_bar.close < enter_->offset) { // пытаемся сдвинуть стоп
					enter_->close_bottom = enter_->close_bottom - (enter_->offset - cur_typed_bar.close);
					enter_->close_top = enter_->close_top - (enter_->offset - cur_typed_bar.close);
					enter_->offset = cur_typed_bar.close;
				}
			}
		}
		else if (enter_->sample.follow == FOLLOW::LOSS && Pass(params)) {
			// пытаемся сдвинуть профиты ниже при покупке или закрыть сделку!
			if (!buy) {
				if (/*!cur_typed_bar.Green() && */cur_typed_bar.close < enter_->offset) { // пытаемся сдвинуть стоп
					enter_->close_bottom = enter_->close_bottom - (enter_->offset - cur_typed_bar.close);
					enter_->close_top = enter_->close_top - (enter_->offset - cur_typed_bar.close);  /* тут // */
					enter_->offset = cur_typed_bar.close;
				}
				else if (/*cur_typed_bar.Green() &&*/ cur_typed_bar.close > enter_->offset) { // пытаемся сдвинуть стоп
					enter_->close_bottom = enter_->close_bottom + (cur_typed_bar.close - enter_->offset);   ///////////////////
					if (enter_->close_top - enter_->sample.close_top > 2000) {
						CloseForced(bar);
					}
					enter_->close_top = enter_->close_top + (cur_typed_bar.close - enter_->offset);
					enter_->offset = cur_typed_bar.close;
				}
			}
			else {  // отработано!

				if (/*!cur_typed_bar.Green() &&*/ cur_typed_bar.close < enter_->offset) { // пытаемся сдвинуть стоп
//					enter_->story_txt = enter_->story_txt + std::to_string(Key(bars, BarTimeFrame::H, cur_typed_bar.close)) + "d,"s;
					enter_->close_bottom = enter_->close_bottom - (enter_->offset - cur_typed_bar.close);
					if (enter_->sample.close_bottom - enter_->close_bottom > 2000) {
						CloseForced(bar);
					}
					if (enter_->sample.close_bottom - enter_->close_bottom > static_cast<double>(enter_->story_double)) {
						enter_->story_double = static_cast<int>(enter_->sample.close_bottom - enter_->close_bottom);
				}
					enter_->close_top = enter_->close_top - (enter_->offset - cur_typed_bar.close);  /* тут // */

					enter_->offset = cur_typed_bar.close;
				}
				else if (/*cur_typed_bar.Green() &&*/ cur_typed_bar.close > enter_->offset) { // пытаемся сдвинуть стоп
//					enter_->story_txt = enter_->story_txt + std::to_string(Key(bars, BarTimeFrame::H, cur_typed_bar.close)) + "u,"s;

					enter_->close_bottom = enter_->close_bottom + (cur_typed_bar.close - enter_->offset);
					enter_->close_top = enter_->close_top + (cur_typed_bar.close - enter_->offset);
					enter_->offset = cur_typed_bar.close;
				}
			}
			enter_->ups.push_back(enter_->close_top);
			enter_->downs.push_back(enter_->close_bottom);
		}

	}
	else { // ожидает, не торгуется
		if (TryEnter(bar)) { // cross error during opening
			results_.push_back(move(enter_.value()));
			enter_.reset();
		}
	}
}

bool Enter::IsEnter() const {
	return enter_.has_value();
}

bool Enter::TryClose(const Bar& bar) {
	if (enter_) {
			if (bar == enter_->close_top && bar == enter_->close_bottom) {
				enter_->correct_close = false;
			}
			else if (bar == enter_->close_top) {
				enter_->closed_price = enter_->close_top;
			}
			else if (bar == enter_->close_bottom) {
				enter_->closed_price = enter_->close_bottom;
			}
			else if (bar > enter_->close_top) {
				enter_->closed_price = bar.low;
			}
			else if (bar < enter_->close_bottom) {
				enter_->closed_price = bar.high;
			}
			else {
				return false;
			}

			enter_->close_bar = bar;
			return true;
	}
	return false;
}

bool Enter::Close(const Bar& bar) {
	if (enter_) {

		if (!TryClose(bar)) {
			enter_->closed_price = bar.close;
			enter_->close_bar = bar;
		}
		return true;
	}
	return false;
}

void Enter::CloseForced(const Bar& bar) {
	if (Close(bar)) {
		results_.push_back(move(enter_.value()));
		enter_.reset();
	}
//    CleanSamples();
}

bool Enter::TryEnter(const Bar& bar) {
	for (auto it = samples_.begin(); it !=  samples_.end(); ++it) { // можно добавить обратную итерацию, посмотреть что эффективнее, позже...
		auto & sample_data = it->second;
		if (bar == sample_data.open_price) { // добавить проверку при покупке или продаже на > <
			if (bar != sample_data.close_top && bar != sample_data.close_bottom) {
				EnterData ent;
				ent.opened_price = sample_data.open_price; // ну или high low при ><
				ent.offset = sample_data.open_price; // ну или high low при ><
				ent.close_top = sample_data.close_top;
				ent.close_bottom = sample_data.close_bottom;
				ent.open_bar = bar;
				ent.sample = move(it->second);
				enter_ = move(ent);
//				it = samples_.erase(it);
				samples_.clear();
				return false;
			}
			else {
				EnterData ent;
				ent.correct_open = false;
				ent.opened_price = sample_data.open_price;
				ent.offset = sample_data.open_price;
				ent.close_top = sample_data.close_top;
				ent.close_bottom = sample_data.close_bottom;
				ent.open_bar = bar;
				ent.sample = move(it->second);
				enter_ = move(ent);
//				it = samples_.erase(it);
				samples_.clear();
				return true;
			}
		}
	}
	return false;
}

