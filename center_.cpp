//---------------------------------------------------------------------------

#pragma hdrstop

#include "center.h"
#include "utils.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

using namespace std;

const int WAITING_FRAMES = 0;

Enter::Enter(BarTimeFrame type, int id, string name, FOLLOW follow)
	: type_(type), id_name_({id, move(name)}), follow_(follow) {
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

void Enter::Update(const Bar& prev_typed_bar, const Bar& cur_typed_bar, const Bar& bar, ApproveParams& params) {
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
		if (follow_ == FOLLOW::PROFIT && Pass(params)) {
			// пытаемся сдвинуть стопы или закрыть сделку!
			if (buy) {
				if (cur_typed_bar.MidBodyPrice() > prev_typed_bar.MidBodyPrice()) { // пытаемся сдвинуть стоп
					if (enter_->close_bottom < prev_typed_bar.low) {
						enter_->close_bottom = prev_typed_bar.low;
					}
				}
				else { // закрываем сделку
					CloseForced(bar);
				}
			}
			else {
				if (cur_typed_bar.MidBodyPrice() < prev_typed_bar.MidBodyPrice()) {  // пытаемся сдвинуть стоп
					if (enter_->close_top > prev_typed_bar.high) {
						enter_->close_top = prev_typed_bar.high;
					}
				}
				else {
					CloseForced(bar);
				}
			}
		}
		else if (follow_ == FOLLOW::LOSS && Pass(params)) {
			// пытаемся сдвинуть профиты ниже при покупке или закрыть сделку!
			if (!buy) {
				if (cur_typed_bar.MidBodyPrice() > prev_typed_bar.MidBodyPrice()) { // пытаемся сдвинуть стоп
					if (enter_->close_bottom < prev_typed_bar.low) {
						enter_->close_bottom = prev_typed_bar.low;
//						enter_->close_bottom = ClearToStep(cur_typed_bar.low - (cur_typed_bar.low - enter_->close_bottom)/2.0, 10); // добавить Step
					}
				}
				else { // закрываем сделку
					CloseForced(bar);
				}
			}
			else {
				if (cur_typed_bar.MidBodyPrice() < prev_typed_bar.MidBodyPrice()) {  // пытаемся сдвинуть стоп
					if (enter_->close_top > prev_typed_bar.high) {
						enter_->close_top = prev_typed_bar.high;
//						enter_->close_top = ClearToStep(enter_->close_top - (enter_->close_top - cur_typed_bar.high)/2.0, 10); // добавить Step
					}
				}
				else {
					CloseForced(bar);
				}
			}
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
		if (follow_ == FOLLOW::NONE) { // loss и profit
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
		else { // loss only
			bool buy = enter_->sample.buy;
			if (follow_ == FOLLOW::PROFIT) {

				if (buy) {
					if (bar == enter_->close_bottom) {
						enter_->closed_price = enter_->close_bottom;
					}
					else if (bar < enter_->close_bottom) {
						enter_->closed_price = bar.high;
					}
					else {
						return false;
					}
				}
				else {
					if (bar == enter_->close_top) {
						enter_->closed_price = enter_->close_top;
					}
					else if (bar > enter_->close_top) {
						enter_->closed_price = bar.low;
					}
					else {
						return false;
					}
				}
			}
			else if (follow_ == FOLLOW::LOSS) {
				if (buy) {
					if (bar == enter_->close_top) {
						enter_->closed_price = enter_->close_top;
					}
					else if (bar > enter_->close_top) {
						enter_->closed_price = bar.low;
					}
					else {
						return false;
					}
				}
				else {
					if (bar == enter_->close_bottom) {
						enter_->closed_price = enter_->close_bottom;
					}
					else if (bar < enter_->close_bottom) {
						enter_->closed_price = bar.high;
					}
					else {
						return false;
					}
				}
			}
			enter_->close_bar = bar;
			return true;
		}
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
				ent.close_top = sample_data.close_top;
				ent.close_bottom = sample_data.close_bottom;
				ent.open_bar = bar;
				ent.sample = move(it->second);
				enter_ = move(ent);
				it = samples_.erase(it);
				return false;
			}
			else {
				EnterData ent;
				ent.correct_open = false;
				ent.opened_price = sample_data.open_price;
				ent.close_top = sample_data.close_top;
				ent.close_bottom = sample_data.close_bottom;
				ent.open_bar = bar;
				ent.sample = move(it->second);
				enter_ = move(ent);
				it = samples_.erase(it);
				return true;
			}
		}
	}
	return false;
}

/*
	std::optional<EnterData> enter_;
	std::list<std::pair<int, SampleData>> samples_;
	std::vector<EnterData> results_;
	std::pair<int, std::string> id_name_;
*/

