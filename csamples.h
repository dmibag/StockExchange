//---------------------------------------------------------------------------

#ifndef csamplesH
#define csamplesH

#include "ckpi.h"
//---------------------------------------------------------------------------

class BaseSample {
public:
	BaseSample(const Bars& bars, const Kpi& kpi) : bars_(bars), K(kpi) {}

	std::optional<std::vector<SampleData>> Find(BasePatParams pat_params) const;


private:
	const Bars& bars_;
	const Kpi& K;

	std::optional<std::vector<SampleData>> P3DownLine(BasePatParams pat_params) const;
	std::optional<std::vector<SampleData>> P3DownLineSide(BasePatParams pat_params) const;
	std::optional<std::vector<SampleData>> P3UpLine(BasePatParams pat_params) const;
	std::optional<std::vector<SampleData>> P3UpLineSide(BasePatParams pat_params) const;
	std::optional<std::vector<SampleData>> P3NoLineNar(BasePatParams pat_params) const;
	std::optional<std::vector<SampleData>> P3FollowUp(BasePatParams pat_params) const;
	std::optional<std::vector<SampleData>> P3FollowDown(BasePatParams pat_params) const;
	std::optional<std::vector<SampleData>> P2Up(BasePatParams pat_params) const;
	std::optional<std::vector<SampleData>> P2Down(BasePatParams pat_params) const;
	std::optional<std::vector<SampleData>> CostUp(BasePatParams pat_params) const;
	std::optional<std::vector<SampleData>> CostDown(BasePatParams pat_params) const;
};



#endif
