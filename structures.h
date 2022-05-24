//---------------------------------------------------------------------------

#ifndef structuresH
#define structuresH

#include <iomanip>
#include <locale>
#include <systdate.h>
#include <string>
#include <vector>

//<TICKER>,<PER>,<DATE>,<TIME>,<OPEN>,<HIGH>,<LOW>,<CLOSE>,<VOL>
//SPFB.RTS,1,20090111,103000,60480.0000000,64945.0000000,60480.0000000,62485.0000000,

static const char* SEPAR  = ";";

struct comma_separator : std::numpunct<char> {
	virtual char do_decimal_point() const override { return ','; }
};

enum class BarTimeFrame {
	M,
	H,
	D
};
inline std::ostream& operator<<(std::ostream& output, const BarTimeFrame& par) {
	switch (par) {
		case BarTimeFrame::M: output << "MIN"; break;
		case BarTimeFrame::H: output << "HOUR"; break;
		case BarTimeFrame::D: output << "DAY"; break;
	default:
		output << "ERR";
	}
	return output;
}

struct Result {
	std::string name;
	double value{};
    bool key_element{false}; // not printing

	Result(std::string_view sname = "", double svalue = 0.0)
		: name(sname), value(svalue) {}

	bool operator<(double rhs) const { return value < rhs; }
	bool operator>(double rhs) const { return value > rhs; }

	std::ostream& PrintName(std::ostream& output) const;
};

void Serialize(const Result& val, std::ostream& out);

std::istream& Deserialize(std::istream& in, Result& val);

std::ostream& operator<<(std::ostream& output, const Result& res);

	struct DateTimeValues {
	unsigned short year{};
	unsigned short month{};
	unsigned short day{};
	unsigned short hour{};
	unsigned short min{};

	bool operator==(DateTimeValues rhs) const;
};

struct Bar {
	int iperiod;
	TDateTime datetime;
	DateTimeValues datetimevals;
	double open{};
	double high{};
	double low{};
	double close{};
	int vol{};

	void CutToMin();
	void CutToHour();
	void CutToDay();

	double MidBodyPrice() const {
		double mid = std::fabs((open - close)/2.);
		double ret = (open > close) ? open - mid : close - mid;
        return ret;
	}

	bool Green() const {
        return open < close;
    }

	static std::ostream& PrintHeader(std::ostream& output);
};

std::ostream& operator<<(std::ostream& output, const Bar& bar);

inline bool operator==(const Bar & lhs, double rhs) {
	return lhs.high >= rhs && lhs.low <= rhs;
}

inline bool operator!=(const Bar & lhs, double rhs) {
	return !(lhs == rhs);
}

inline bool operator>(const Bar & lhs, double rhs) {
	return lhs.low > rhs;
}

inline bool operator<(const Bar & lhs, double rhs) {
	return lhs.high < rhs;
}


struct BasePatParams {
	double plus{};
	double minus{};
	double cur{};
	std::string name;
	int id{};
	Bar bar;
	BarTimeFrame type{BarTimeFrame::H};

	static std::ostream& PrintHeader(std::ostream& output);
};

void Serialize(const BasePatParams& val, std::ostream& out);

std::istream& Deserialize(std::istream& in, BasePatParams& val);

std::ostream& operator<<(std::ostream& output, const BasePatParams& par);

enum class FOLLOW {
	NONE,
	PROFIT,
	LOSS
};

struct SampleData {
	bool buy{true};
	double open_price{};
	double close_top{};
	double close_bottom{};
	bool up;
	std::string key;
	FOLLOW follow{FOLLOW::NONE};

	std::vector<Result> params;
	BasePatParams base_params;

	double GetStoplossOffset() const {
		return base_params.cur;
	}
	std::ostream& PrintHeader(std::ostream& output) const;
};

void Serialize(const SampleData& val, std::ostream& out);

std::istream& Deserialize(std::istream& in, SampleData& val);

std::ostream& operator<<(std::ostream& output, const SampleData& par);

struct ApproveParams {
	bool new_day{};
	bool end_day{};
	bool new_hour{};
	bool end_hour{};
	bool new_min{};
	bool end_min{};
	std::pair<TDate, TDate> date_range;
};


struct EnterData {
	double opened_price{};
	double closed_price{};
	bool correct_open{true};
	bool correct_close{true};
	double close_top{};
	double close_bottom{};
	double offset{};
	int story_double{};
	std::string story_txt;
	std::vector<double> ups;
	std::vector<double> downs;
	Bar open_bar;
	Bar close_bar;
	SampleData sample;
// calc fields
	bool good() const;     // прибыльная ли сделка или убыточная
	bool correct() const;  // были ли ошибки
	double result() const; // сколько заработано/потрачено денег
    std::string title() const;

	std::ostream& PrintHeader(std::ostream& output) const;
};

void Serialize(const EnterData& val, std::ostream& out);

std::istream& Deserialize(std::istream& in, EnterData& val);

std::ostream& operator<<(std::ostream& output, const EnterData& par);

 ////////////////////////////  EXPERIMENTs data ///////////////////////////////
//не экспортируется в CSV!
struct Aggregate {
	int correct_count{0};
	int incorrect_count{0};
	double sum{0};
	double earned_count{0};
	double spend_count{0};

	double max_slump{0};
	int max_spend_together{0};
};
void Serialize(const Aggregate& val, std::ostream& out);

std::istream& Deserialize(std::istream& in, Aggregate& val);

// для вывода в окно, не в CSV
std::ostream& operator<<(std::ostream& output, const Aggregate& agg);

struct Agg {
	int year{2000};
	TDate date;
	double sum{0.};
	double slump{0.};
	double got{0.};

	Agg(int year_, TDate date_, double sum_, double slump_, double got_)
		: year(year_), date(date_), sum(sum_), slump(slump_), got(got_) {}
	Agg() = default;

	std::ostream& PrintHeader(std::ostream& output) const;
};
//---------------------------------------------------------------------------

void Serialize(const Agg& val, std::ostream& out);

std::istream& Deserialize(std::istream& in, Agg& val);

std::ostream& operator<<(std::ostream& output, const Agg& par);

//---------------------------------------------------------------------------

struct Experiment {
	Aggregate result;
	std::vector<Agg> agg;
	std::vector<EnterData> data;
};

void Serialize(const Experiment& val, std::ostream& out);

std::istream& Deserialize(std::istream& in, Experiment& val);

////////////////////////////////////////////////////////////////////////////

void operator >> (std::string_view str, Bar& dbstr);

//////////////////////////////////////////////////////////////////////////

struct BarShowProps {
	double y1;
	double y2;
	double y3;
	double y4;
	double w;
	std::string title;

	enum Type {
		GREEN,
		RED,
		WHITE
	} type;
};

//---------------------------------------------------------------------------
#endif
