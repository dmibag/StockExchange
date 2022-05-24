//---------------------------------------------------------------------------

#ifndef cbarsH
#define cbarsH

#include <algorithm>
#include <deque>
#include <map>
#include <memory>

#include "structures.h"


class FrameBars;

class BaseIndicator {
public:
	virtual ~BaseIndicator() = default;

	std::deque<double>& Deq() {
		return deq_;
	}
	const std::deque<double>& Deq() const {
		return deq_;
	}
	int Tail() const {
		return tail_;
	}
	int& Tail() {
		return tail_;
	}
	void EraseBefore(size_t count) {
		deq_.erase(deq_.begin(), std::next(deq_.begin(), count));
	}
	void EraseAfter(size_t count) {
		deq_.erase(std::next(deq_.begin(), count), deq_.end());
	}
	double Indicator(int num) const {
		return *(deq_.rbegin() + num);
	}
	void Clear() {
		Deq().clear();
	}

	virtual void Update(const FrameBars&) {};
	BaseIndicator() = default;
protected:
	BaseIndicator(int tail) : tail_(tail) {}

private:

	int tail_{1};
	std::deque<double> deq_;
};

// сериализуется через базовый класс, своих членов нет!!! они не сериализуются, ысе данные хранить в базе
class IndicatorAvg : public BaseIndicator {
public:
	IndicatorAvg(size_t avg) : BaseIndicator(avg) {}
	void Update(const FrameBars& frame_bars) override;
};

// сериализуется через базовый класс, своих членов нет!!! они не сериализуются, ысе данные хранить в базе
class IndicatorAvgSpeed : public BaseIndicator {
public:
	IndicatorAvgSpeed(size_t avg = 0) : BaseIndicator(avg) {}
	void Update(const FrameBars& frame_bars) override;
};

class IndicatorTopCost : public BaseIndicator {
public:
	IndicatorTopCost(size_t avg = 0) : BaseIndicator(avg) {}
	void Update(const FrameBars& frame_bars) override;
};

class FrameBars {
public:
	using Deque = std::deque<Bar>;
	using IndocatorsMap = std::map<std::string, std::unique_ptr<BaseIndicator>>;

	FrameBars() = default;
	FrameBars(BarTimeFrame type, double step) : type_(type), step_(step) {}
	bool Update(const Bar& bar);

	const Deque& Deq() const {
		return deq_;
	}

	Deque& Deq() {
		return deq_;
	}

	Bar FromLast(int num) const { return *(deq_.end() - 2 - num); }
	bool HasClosedBars(int count) const { return (deq_.size() - 1) >= count; }
	double Low(int num) const { return FromLast(num).low; }
	double High(int num) const { return FromLast(num).high; }
	double Open(int num) const { return FromLast(num).open; }
	double Close(int num) const { return FromLast(num).close; }
	double Vol(int num) const { return FromLast(num).vol; }
	double Step() const { return step_; }
	double& Step() { return step_; }

	double WholeSize(int num) const {
		Bar bar = FromLast(num);
		return (bar.high - bar.low == 0) ? step_ : bar.high - bar.low ;
	}

	double BodySize(int num) const {
		Bar bar = FromLast(num);
		return (std::abs(bar.open - bar.close) == 0) ? step_: std::abs(bar.open - bar.close);
	}

	double BodyUp(int num) const {
		Bar bar = FromLast(num);
		return (bar.open > bar.close) ? bar.open : bar.close ;
	}

	double BodyDown(int num) const {
		Bar bar = FromLast(num);
		return (bar.open > bar.close) ? bar.close : bar.open ;
	}

	const DateTimeValues& DT(int num) const { return FromLast(num).datetimevals; }

	double Green(int num) const {
		Bar bar = FromLast(num);
		if (Open(num) > Close(num)) {
			return -1;
		}
		return 1;
	}

	BarTimeFrame Frame() const {
		return type_;
	}

	BarTimeFrame& Frame() {
		return type_;
	}

	void CutToRange(std::pair<TDate, TDate> range) {
		auto it = std::lower_bound( deq_.begin(), deq_.end(), range.first, [](const Bar& bar, TDate date){
			return bar.datetime < date;
		});

		for (auto & ind : indicators_) {
			ind.second->EraseBefore(std::distance(deq_.begin(), it));
		}

		deq_.erase(deq_.begin(), it);
		it = std::upper_bound( deq_.begin(), deq_.end(), range.second, [](TDate date, const Bar& bar){
			return static_cast<int>(date.Val) < static_cast<int>(bar.datetime.Val);
		});
		for (auto & ind : indicators_) {
			ind.second->EraseAfter(std::distance(deq_.begin(), it));
		}
		deq_.erase(it, deq_.end());
	}

	double IndicatorAt(const std::string& name, int num) const {
		return Indicator(name).Indicator(num);
	}

	const std::deque<double>& IndicatorDeq(const std::string& name) const {
		return Indicator(name).Deq();
	}

	std::deque<double>& IndicatorDeq(const std::string& name) {
		return Indicator(name).Deq();
	}

	void Clear() {
		Deq().clear();

		for (auto & ind : indicators_) {
			ind.second->Clear();
		}
	}

	const BaseIndicator& Indicator(const std::string& name) const {
		return *indicators_.at(name).get();
	}

	BaseIndicator& Indicator(const std::string& name) {
		return *indicators_.at(name).get();
	}

	IndocatorsMap& Indicators() {
		return indicators_;
	}

	const IndocatorsMap& Indicators() const {
		return indicators_;
	}

private:
	Deque deq_;
	BarTimeFrame type_;
	double step_{1.0};

	IndocatorsMap indicators_;
};

void Serialize(const FrameBars& val, std::ostream& out);

std::istream& Deserialize(std::istream& in, FrameBars& val);

//---------------------------------------------------------------------------

class Bars {
public:
	Bars(double step) : step_(step), mins_(BarTimeFrame::M, step), hours_(BarTimeFrame::H, step), days_(BarTimeFrame::D, step) {
		mins_.Indicators()["avg"] = std::make_unique<IndicatorAvg>(80);
		hours_.Indicators()["avg"] = std::make_unique<IndicatorAvg>(20);
		days_.Indicators()["avg"] = std::make_unique<IndicatorAvg>(20);
		mins_.Indicators()["top_cost"] = std::make_unique<IndicatorTopCost>(80);
		hours_.Indicators()["top_cost"] = std::make_unique<IndicatorTopCost>(20);
		days_.Indicators()["top_cost"] = std::make_unique<IndicatorTopCost>(20);
	}

	void Update(const Bar& bar) {
		mins_.Update(bar);
		hours_.Update(bar);
		days_.Update(bar);
	}

	const FrameBars::Deque& Mins() const {
		return mins_.Deq();
	}

	const FrameBars::Deque& Hours() const {
		return hours_.Deq();
	}

	const FrameBars::Deque& Days() const {
		return days_.Deq();
	}

	const FrameBars& M() const {
		return mins_;
	}

	const FrameBars& H() const {
		return hours_;
	}

	const FrameBars& D() const {
		return days_;
	}

	FrameBars& M()  {
		return mins_;
	}

	FrameBars& H()  {
		return hours_;
	}

	FrameBars& D()  {
		return days_;
	}
	void Clear() {
		mins_.Clear();
		hours_.Clear();
		days_.Clear();
	}
	double Step() const { return step_; }
	double& Step() { return step_; }

void CutToRange(std::pair<TDate, TDate> range) {
	mins_.CutToRange(range);
	hours_.CutToRange(range);
	days_.CutToRange(range);
}


private:
	double step_{1.0};
	FrameBars mins_;
	FrameBars hours_;
	FrameBars days_;

};

void Serialize(const Bars& val, std::ostream& out);

std::istream& Deserialize(std::istream& in, Bars& val);

#endif
