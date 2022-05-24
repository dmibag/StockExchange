//---------------------------------------------------------------------------

#ifndef centerH
#define centerH

#include "structures.h"
#include "cbars.h"

#include <list>
#include <optional>
#include <vector>


class Enter {
public:
	Enter() = delete;
	explicit Enter(BarTimeFrame type, int id, std::string name);

	void AddSample(std::vector<SampleData>&& sample);
	void Update(const Bars& bars, const Bar& prev_typed_bar, const Bar& cur_typed_bar, const Bar& bar, ApproveParams& params);
	std::pair<int, const std::string_view> GetName() const;
	std::vector<EnterData> && ExitWithResults(const Bar& bar);
	void CloseForced(const Bar& bar);
	bool IsEnter() const;
	bool HasSamples() const;

private:
	std::optional<EnterData> enter_;
	std::list<std::pair<int, SampleData>> samples_;
	std::vector<EnterData> results_;
	std::pair<int, std::string> id_name_;
	BarTimeFrame type_;

	void UpdateExpireSamples();
	void CleanSamples();
	bool TryClose(const Bar& bar);
	bool Close(const Bar& bar);
	bool TryEnter(const Bar& bar); // return true if error (cross)
	bool Pass(ApproveParams& params) const;
	int Key(const Bars& bars, BarTimeFrame type, double price) const;
	bool Trend(const Bars& bars, BarTimeFrame type) const;
};

//---------------------------------------------------------------------------
#endif
