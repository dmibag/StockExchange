//---------------------------------------------------------------------------

#pragma hdrstop

#include <tuple>
#include <sstream>
#include "structures.h"
#include "utils.h"

#pragma package(smart_init)

using namespace std;

bool DateTimeValues::operator==(DateTimeValues rhs) const {
	return std::tie(year,month,day,hour,min) == std::tie(rhs.year,rhs.month,rhs.day,rhs.hour,rhs.min);
}

void Bar::CutToMin() {
	datetime = TDateTime(datetimevals.year, datetimevals.month, datetimevals.day, datetimevals.hour, (datetimevals.min / 15)*15, 0, 0);
	datetimevals.min = (datetimevals.min / 15)*15;
}

void Bar::CutToHour() {
	datetime = TDateTime(datetimevals.year, datetimevals.month, datetimevals.day, datetimevals.hour, 0, 0, 0);
	datetimevals.min = 0;
}

void Bar::CutToDay() {
	datetime = TDateTime(datetimevals.year, datetimevals.month, datetimevals.day, 0, 0, 0, 0);
	datetimevals.hour = 0;
	datetimevals.min = 0;
}

void Serialize(const Result& val, std::ostream& out) {
	Serialize(val.name, out);
	Serialize(val.value, out);
	Serialize(val.key_element, out);
}

std::istream& Deserialize(std::istream& in, Result& val) {
	Deserialize(in, val.name);
	Deserialize(in, val.value);
	Deserialize(in, val.key_element);
	return in;
}

std::ostream& operator<<(std::ostream& output, const Result& res) {
	output << res.value;
	return output;
}

std::ostream& Result::PrintName(std::ostream& output) const {
	output << name;
	return output;
}

std::ostream& operator<<(std::ostream& output , const Bar& bar) {
	output
		<< bar.iperiod
		<< SEPAR << bar.datetimevals.year << '.'
		<< std::setfill('0') << std::right << std::setw(2) << bar.datetimevals.month << '.'
		<< std::setfill('0') << std::right << std::setw(2) << bar.datetimevals.day
		<< SEPAR << std::setfill('0') << std::right << std::setw(2) << bar.datetimevals.hour << ':'
		<< std::setfill('0') << std::right << std::setw(2) << bar.datetimevals.min << ':'
		<< "00"
		<< SEPAR << bar.open
		<< SEPAR << bar.high
		<< SEPAR << bar.low
		<< SEPAR << bar.close
		<< SEPAR << bar.vol
	;
	return output;
}

std::ostream& Bar::PrintHeader(std::ostream& output) {
	output
		<< "iperiod"
		<< SEPAR << "date"
		<< SEPAR << "time"
		<< SEPAR << "open"
		<< SEPAR << "high"
		<< SEPAR << "low"
		<< SEPAR << "close"
		<< SEPAR << "vol"
	;
	return output;
}


void Serialize(const BasePatParams& val, std::ostream& out) {
	Serialize(val.plus, out);
	Serialize(val.minus, out);
	Serialize(val.cur, out);
	Serialize(val.name, out);
	Serialize(val.id, out);
	Serialize(val.bar, out);
	Serialize(val.type, out);
}

std::istream& Deserialize(std::istream& in, BasePatParams& val) {
	Deserialize(in, val.plus);
	Deserialize(in, val.minus);
	Deserialize(in, val.cur);
	Deserialize(in, val.name);
	Deserialize(in, val.id);
	Deserialize(in, val.bar);
	Deserialize(in, val.type);
	return in;
}

std::ostream& operator<<(std::ostream& output, const BasePatParams& par) {
	output
		<< par.id
		<< SEPAR << par.name
		<< SEPAR << par.type
		<< SEPAR << par.bar
		<< SEPAR << par.cur
		<< SEPAR << par.plus
		<< SEPAR << par.minus
	;
	return output;
}

std::ostream& BasePatParams::PrintHeader(std::ostream& output) {
	output
		<< "id"
		<< SEPAR << "name"
		<< SEPAR << "type"
		<< SEPAR;
		Bar::PrintHeader(output)
		<< SEPAR << "cur"
		<< SEPAR << "plus"
		<< SEPAR << "minus"
	;
	return output;
}

std::ostream& PrintVector(std::ostream& output, vector<Result> vec) {
	for (const auto& el : vec) {
		output << el;
		if (&el != &vec.back())
			output << SEPAR;
	}
	return output;
}

void Serialize(const SampleData& val, std::ostream& out) {
	Serialize(val.buy, out);
	Serialize(val.open_price, out);
	Serialize(val.close_top, out);
	Serialize(val.close_bottom, out);
	Serialize(val.up, out);
	Serialize(val.key, out);
	Serialize(val.follow, out);
	Serialize(val.params, out);
	Serialize(val.base_params, out);
}

std::istream& Deserialize(std::istream& in, SampleData& val) {
	Deserialize(in, val.buy);
	Deserialize(in, val.open_price);
	Deserialize(in, val.close_top);
	Deserialize(in, val.close_bottom);
	Deserialize(in, val.up);
	Deserialize(in, val.key);
	Deserialize(in, val.follow);
	Deserialize(in, val.params);
	Deserialize(in, val.base_params);
	return in;
}

std::ostream& operator<<(std::ostream& output, const SampleData& par) {
	output
		<< par.buy
		<< SEPAR << par.open_price
		<< SEPAR << par.close_top
		<< SEPAR << par.close_bottom
		<< SEPAR << par.up
		<< SEPAR << par.key
		<< SEPAR;
		PrintVector(output, par.params)
		<< SEPAR << par.base_params;
//		<< SEPAR;
	;
	return output;
}

std::ostream& PrintVectorHeader(std::ostream& output, vector<Result> vec) {
	for (const auto& el : vec) {
		el.PrintName(output);
		if (&el != &vec.back())
			output << SEPAR;
	}
	return output;
}

std::ostream& SampleData::PrintHeader(std::ostream& output) const {
	output
		<< "buy"
		<< SEPAR << "open_price"
		<< SEPAR << "close_top"
		<< SEPAR << "close_bottom"
		<< SEPAR << "going_up"
		<< SEPAR << "key"
		<< SEPAR;
		PrintVectorHeader(output, params)
		<< SEPAR;
		BasePatParams::PrintHeader(output)
	;
	return output;
}

void Serialize(const EnterData& val, std::ostream& out) {
	Serialize(val.opened_price, out);
	Serialize(val.closed_price, out);
	Serialize(val.correct_open, out);
	Serialize(val.correct_close, out);
	Serialize(val.close_top, out);
	Serialize(val.close_bottom, out);
	Serialize(val.offset, out);
	Serialize(val.story_double, out);
	Serialize(val.story_txt, out);
	Serialize(val.ups, out);
	Serialize(val.downs, out);
	Serialize(val.open_bar, out);
	Serialize(val.close_bar, out);
	Serialize(val.sample, out);
}

std::istream& Deserialize(std::istream& in, EnterData& val) {
	Deserialize(in, val.opened_price);
	Deserialize(in, val.closed_price);
	Deserialize(in, val.correct_open);
	Deserialize(in, val.correct_close);
	Deserialize(in, val.close_top);
	Deserialize(in, val.close_bottom);
	Deserialize(in, val.offset);
	Deserialize(in, val.story_double);
	Deserialize(in, val.story_txt);
	Deserialize(in, val.ups);
	Deserialize(in, val.downs);
	Deserialize(in, val.open_bar);
	Deserialize(in, val.close_bar);
	Deserialize(in, val.sample);
	return in;
}

bool EnterData::good() const {
	if (sample.buy) {
		return closed_price > opened_price;
	}
	return closed_price < opened_price;
}

bool EnterData::correct() const {
	return correct_open && correct_close;
}

double EnterData::result() const {
	if (sample.buy) {
		return closed_price - opened_price;
	}
	return opened_price - closed_price;
}

std::string EnterData::title() const {
	std::ostringstream str;

	str
	<< std::setfill('0') << std::right << std::setw(2) << open_bar.datetimevals.day << '.'
	<< std::setfill('0') << std::right << std::setw(2) << open_bar.datetimevals.month << '.'
	<< open_bar.datetimevals.year
	<< ' ' << std::setfill('0') << std::right << std::setw(2) << open_bar.datetimevals.hour << ':'
	<< std::setfill('0') << std::right << std::setw(2) << open_bar.datetimevals.min
	<< " result: "sv << std::setprecision(4) << result() << " key: "sv << sample.key;

	return str.str();
}

std::ostream& operator<<(std::ostream& output, const EnterData& par) {
	output
		<< par.good()
		<< SEPAR << par.opened_price
		<< SEPAR << par.closed_price
		<< SEPAR << par.story_double
		<< SEPAR << par.story_txt
		<< SEPAR << par.sample
		<< SEPAR << par.open_bar
		<< SEPAR << par.close_bar
	;
	return output;
}

std::ostream& EnterData::PrintHeader(std::ostream& output) const {
	output
		<< "gain"
		<< SEPAR << "opened_price"
		<< SEPAR << "closed_price"
		<< SEPAR << "story_double"
		<< SEPAR << "story_txt"
		<< SEPAR;
		sample.PrintHeader(output)
		<< SEPAR;
		Bar::PrintHeader(output)
		<< SEPAR;
		Bar::PrintHeader(output)
	;
	return output;
}
//	std::string str("SPFB.RTS-21.03,1,20210318,182000,15006.0000000,150070.0000000,150060.0000000,150070.0000000,34");

//////////////////////////////////////////////////////////////////////////
void Serialize(const Aggregate& val, std::ostream& out) {
	Serialize(val.correct_count, out);
	Serialize(val.incorrect_count, out);
	Serialize(val.sum, out);
	Serialize(val.earned_count, out);
	Serialize(val.spend_count, out);
	Serialize(val.max_slump, out);
	Serialize(val.max_spend_together, out);
}

std::istream& Deserialize(std::istream& in, Aggregate& val) {
	Deserialize(in, val.correct_count);
	Deserialize(in, val.incorrect_count);
	Deserialize(in, val.sum);
	Deserialize(in, val.earned_count);
	Deserialize(in, val.spend_count);
	Deserialize(in, val.max_slump);
	Deserialize(in, val.max_spend_together);
	return in;
}

// для вывода в окно, не в CSV
std::ostream& operator<<(std::ostream& output, const Aggregate& agg) {
	using namespace std::literals;
	output
		<< "Корректных сделок "s << agg.correct_count
		<< " ("s
		<< "успешных "s << agg.earned_count
		<< " убыточных "s << agg.spend_count
		<< ")"s
		<< " ошибок "s << agg.incorrect_count << '\n'
		<< "Процент успешных "s << agg.earned_count / (agg.earned_count + agg.spend_count) << '\n'
		<< "Заработано "s << agg.sum << '\n'
		<< "Макс. просадка "s << agg.max_slump << '\n'
		<< "Подряд убыточных "s << agg.max_spend_together << '\n'
	;
	return output;
}

std::ostream& Agg::PrintHeader(std::ostream& output) const {
	output
		<< "year"
		<< SEPAR << "date"
		<< SEPAR << "sum"
		<< SEPAR << "slump"
		<< SEPAR << "got"
	;
	return output;
}

//---------------------------------------------------------------------------

void Serialize(const Agg& val, std::ostream& out) {
	Serialize(val.year, out);
	Serialize(val.date, out);
	Serialize(val.sum, out);
	Serialize(val.slump, out);
	Serialize(val.got, out);
}

std::istream& Deserialize(std::istream& in, Agg& val) {
	Deserialize(in, val.year);
	Deserialize(in, val.date);
	Deserialize(in, val.sum);
	Deserialize(in, val.slump);
	Deserialize(in, val.got);
	return in;
}

std::ostream& operator<<(std::ostream& output, const Agg& par) {
	output
		<< par.year
		<< SEPAR << UniToStr(par.date.DateString())
		<< SEPAR << par.sum
		<< SEPAR << par.slump
		<< SEPAR << par.got
	;
	return output;
}

//---------------------------------------------------------------------------

void Serialize(const Experiment& val, std::ostream& out) {
	Serialize(val.result, out);
	Serialize(val.agg, out);
	Serialize(val.data, out);
}

std::istream& Deserialize(std::istream& in, Experiment& val) {
	Deserialize(in, val.result);
	Deserialize(in, val.agg);
	Deserialize(in, val.data);
	return in;

}

//////////////////////////////////////////////////////////////////////////
void operator >> (std::string_view line, Bar& dbstr) {
	std::string_view str = line;
	Bar arg;
	try {
		auto f = str.find_first_of(',');
		arg.iperiod = std::stoi(std::string(str.substr(f-5,2)))*100 + std::stoi(std::string(str.substr(f-2,2)));
		str.remove_prefix(f+1);
		f = str.find_first_of(',');
		str.remove_prefix(f+1);
		arg.datetimevals.year = std::stoi(std::string(str.substr(0,4)));
		str.remove_prefix(4);
		arg.datetimevals.month = std::stoi(std::string(str.substr(0,2)));
		str.remove_prefix(2);
		arg.datetimevals.day = std::stoi(std::string(str.substr(0,2)));
		str.remove_prefix(3);
		arg.datetimevals.hour = std::stoi(std::string(str.substr(0,2)));
		str.remove_prefix(2);
		arg.datetimevals.min = std::stoi(std::string(str.substr(0,2)));
		arg.datetime = std::move(TDateTime(arg.datetimevals.year, arg.datetimevals.month, arg.datetimevals.day,
								 arg.datetimevals.hour, arg.datetimevals.min, 0, 0));
		str.remove_prefix(5);
		f = str.find_first_of(',');
		arg.open = std::stod(std::string(str.substr(0, f)));
		str.remove_prefix(f+1);
		f = str.find_first_of(',');
		arg.high = std::stod(std::string(str.substr(0, f)));
		str.remove_prefix(f+1);
		f = str.find_first_of(',');
		arg.low = std::stod(std::string(str.substr(0, f)));
		str.remove_prefix(f+1);
		f = str.find_first_of(',');
		arg.close = std::stod(std::string(str.substr(0, f)));
		str.remove_prefix(f+1);
		arg.vol = std::stoi(std::string(str));
		std::swap(arg, dbstr);
	} catch (std::exception &e) {
		ShowMessage("Ошибка в разборе строки из файла DB: " + line + " : " + UnicodeString(e.what()));
		throw e;
	}
}



//---------------------------------------------------------------------------

