//---------------------------------------------------------------------------

#ifndef crunH
#define crunH

#include "cbars.h"
#include "settings.h"

#include <map>
#include <vector>


class Run {
public:
	struct DealsStat {
		int count{0};
		int good{0};
		double sum{0};
	};

	using VecKeys = std::vector<std::pair<std::string, DealsStat>>;

	Run(Settings& settings, double step);

	std::vector<EnterData> Execute(std::pair<int, int> range) const ;
	Bars BuildBars(int period, std::pair<TDate, TDate> range = {0,TDate(2099,1,1)}) const ;
	std::vector<BasePatParams> GetBasePatterns(std::pair<int, int> range) const ;

	Experiment AggregateSamples(const std::vector<EnterData>& cont, double fee) const ;

	VecKeys CalcKeys(std::pair<int, int> range, double fee) const;

private:
	Settings& settings_;
	double step_;
};
//---------------------------------------------------------------------------
#endif
