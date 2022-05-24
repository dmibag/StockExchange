//---------------------------------------------------------------------------

#ifndef projectorsH
#define projectorsH
//---------------------------------------------------------------------------

#include "svg.h"
#include "structures.h"
#include "cbars.h"
#include "crun.h"

#include <algorithm>
#include <deque>
#include <cmath>
#include <set>
#include <string>
#include <unordered_map>

namespace render {

using namespace svg;
using namespace std::literals;


class Projector {
protected:
	Projector(double width, double height, double off_start, double off_end, double off_inner, double grid_step)
			: width_(width), height_(height), off_start_(off_start), off_end_(off_end), off_inner_(off_inner), grid_step_(grid_step) {
	}

	double width_;
	double height_;
	double high_line_;
	double low_line_;
	double zoom_;
	double off_start_;
	double off_end_;
	double off_inner_;
	double block_width_;
	double grid_step_;

public:

	virtual ~Projector() = default;

	double GetObjWidth() const { return block_width_ - off_inner_; }

	template <typename T>
	double GetOffset(T it_first, T it_cur, size_t offset = 0) const {
		return off_start_ + block_width_*(std::distance(it_first, it_cur) + offset) + off_inner_;
	}



	double GetY(double src) const { return (high_line_ - src)*zoom_; }
	double GetWidth() const { return width_; }
	double GetHeight() const { return height_; }
	double GetOffStart() const { return off_start_; }
	double GetOffEnd() const { return off_end_; }
	double GetHighLine() const { return high_line_; }
	double GetLowLine() const { return low_line_; }
	double GetGridStep() const { return grid_step_; }


};

class ProjectorBars : public Projector {
public:

	template <typename T>
	BarShowProps GetBarProps(T it_bar) const {
		const Bar& bar = *it_bar;
		BarShowProps props;
		props.y1 = (high_line_ - bar.high)*zoom_;
		props.y2 = (high_line_ - ((bar.open > bar.close) ? bar.open : bar.close))*zoom_;
		props.y3 = (high_line_ - ((bar.open < bar.close) ? bar.open : bar.close))*zoom_;
		props.y4 = (high_line_ - bar.low)*zoom_;
		props.w = GetObjWidth();

		std::ostringstream str;
		str
		<< bar.iperiod
		<< ": "sv << std::setfill('0') << std::right << std::setw(2) << bar.datetimevals.day << '.'
		<< std::setfill('0') << std::right << std::setw(2) << bar.datetimevals.month << '.'
		<< bar.datetimevals.year
		<< ' ' << std::setfill('0') << std::right << std::setw(2) << bar.datetimevals.hour << ':'
		<< std::setfill('0') << std::right << std::setw(2) << bar.datetimevals.min;

		props.title = str.str();
		using Type = BarShowProps::Type;
		props.type = (bar.open > bar.close) ? Type::RED : Type::GREEN;
		return props;
	}

protected:
	const std::deque<Bar>& deq_;

	ProjectorBars(const std::deque<Bar>& deq, double width, double height, double off_start, double off_end, double off_inner, double grid_step)
			: Projector(width, height, off_start, off_end, off_inner, grid_step), deq_(deq) {
	}
};

class ProjectorAllInOne : public ProjectorBars {
public:
	ProjectorAllInOne(const std::deque<Bar>& deq, double width, double height, double off_start, double off_end, double off_inner, double grid_step)
			: ProjectorBars(deq, width, height, off_start, off_end, off_inner, grid_step) {

		auto low_it = std::min_element(deq_.begin(), deq_.end(), [](auto& lhs, auto& rhs) {
			return lhs.low < rhs.low;
		});
		auto high_it = std::max_element(deq_.begin(), deq_.end(), [](auto& lhs, auto& rhs) {
			return lhs.high < rhs.high;
		});
		low_line_ = (static_cast<int>(low_it->low) / static_cast<int>(grid_step_)) * grid_step_;
		high_line_ = (static_cast<int>(high_it->high) / static_cast<int>(grid_step_) + 1) * grid_step_;

		zoom_ = height_/(high_line_ - low_line_);
		block_width_ = (width_ - off_start_ - off_end_)/deq.size();
	}

};

class ProjectorCombine : public ProjectorBars {
public:
	ProjectorCombine(const std::deque<Bar>& deq, double width, double height, double off_start, double off_end, double off_inner, double grid_step,
		double top_off, double bottom_off, int left_count, int right_count, double price)
			: ProjectorBars(deq, width, height, off_start, off_end, off_inner, grid_step),
			  top_off_(top_off), bottom_off_(bottom_off), left_count_(left_count), right_count_(right_count), price_(price) {

		high_line_ = price_ + top_off_;
		low_line_ = price_ - bottom_off_;

		zoom_ = height_/(high_line_ - low_line_);
		block_width_ = (width_ - off_start_ - off_end_)/static_cast<double>(left_count + right_count + 1);
	}


private:
	double top_off_;
	double bottom_off_;
	int left_count_;
	int right_count_;
	double price_;
};

class ProjectorGraph : public Projector {
public:
	ProjectorGraph(const std::vector<Agg>& cont, double width, double height, double off_start, double off_end, double off_inner, double grid_step)
			: Projector(width, height, off_start, off_end, off_inner, grid_step), cont_(cont) {

		const int bot_off = 12;
		auto [low_it, high_it] = std::minmax_element(cont_.begin(), cont_.end(), [](const auto& lhs, const auto& rhs) {
			return lhs.sum < rhs.sum;
		});

		low_line_ = (static_cast<int>(low_it->sum) / static_cast<int>(grid_step_) - 1) * grid_step_;
		high_line_ = (static_cast<int>(high_it->sum) / static_cast<int>(grid_step_) + 1) * grid_step_;

		zoom_ = (height_ - bot_off)/(high_line_ - low_line_);
//		zoom_ = (height_)/(high_line_ - low_line_);
		block_width_ = (width_ - off_start_ - off_end_)/(cont_.size() + 1); // один блок добавляем, так как график начинается с 0
	}

private:
	const std::vector<Agg>& cont_;
};

class ProjectorIndicator : public Projector {
public:
	ProjectorIndicator(const std::deque<double>& cont, double width, double height, double off_start, double off_end, double off_inner, double grid_step,
		double top_off, double bottom_off)
			: Projector(width, height, off_start, off_end, off_inner, grid_step), cont_(cont), top_off_(top_off), bottom_off_(bottom_off) {

		high_line_ = top_off_;
		low_line_ = bottom_off_;

		zoom_ = (height_)/(high_line_ - low_line_);
		block_width_ = (width_ - off_start_ - off_end_)/(cont_.size());
	}

private:
	const std::deque<double>& cont_;
	double top_off_;
	double bottom_off_;
};



} // render
#endif
