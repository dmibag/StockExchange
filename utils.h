//---------------------------------------------------------------------------

#ifndef utilsH
#define utilsH

#include "structures.h"
#include "cbars.h"

#include <cmath>
#include <filesystem>
#include <map>
#include <vector>
#include <deque>

inline std::string UniToStr(UnicodeString str) {
	return AnsiString(str).c_str();
}

void PrepareDB(UnicodeString infile, UnicodeString outfile);

std::map<int, std::pair<TDate, TDate>> MakeStartStopDates(UnicodeString IniFileName);

std::vector<std::filesystem::path> GetFilesInDir(const std::filesystem::path& p);

void CreateDbFromTxt(std::vector<std::filesystem::path> src_files, const std::filesystem::path& dst_filename);

std::map<int, std::pair<TDate, TDate>> GetCachedMarkers(UnicodeString FileName);

void SaveBarsToDB(std::string db_name, std::string save_path, double step);

Aggregate ExportExperiment(const Experiment & experiment, std::ostream& output);
//---------------------------------------------------------------------------

inline const double EPSILON = 1e-6;

inline bool IsZero(double value) {
	return std::abs(value) < EPSILON;
}

double GetMiddle(const Bar& bar, double step);
double ClearToStep(double value, double step);

class Prc {
public:
	Prc(double step) : step_(step){}
	Prc() = default;
	Prc& operator()(double prc) {
		prc_ = prc;
		return *this;
	}

	friend double operator-(double lhs, const Prc& rhs) {
		return lhs - rhs.Calc(lhs);
	}
	friend double operator+(double lhs, const Prc& rhs) {
		return lhs + rhs.Calc(lhs);
	}

private:
	double prc_{0.};
	double step_{1.};

	double Calc(double value) const {
		double pre = value * prc_ / 100;
		double mod = std::fmod(pre, step_);

		return mod > step_/2
			? pre + step_ - mod
			: ( (mod == 0.) ? pre : pre - mod );
	}

};

/*
template<typename... Ts>
std::string KeyOld(std::tuple<Ts...> const& tup)
{
	std::string str{"k"};
	std::apply
	(
		[&str](Ts const&... args)
		{
			((str = str + std::to_string(args)), ...);
		}, tup
	);
	return str;
}
*/

template<typename... Ts>
std::string Key(const Ts&... vars)
{
	std::string str{"k"};
	((str = str + std::to_string(vars)), ...);
	return str;
}

template <typename T>
int SegmentV(const T& val, std::vector<T> segs) {
	for (int i = 0; i < segs.size(); ++i) {
		if (val < segs.at(i)) return i;
	}
	return  segs.size();
}

//---------------------------------------------------------------------------
// 								SERIALIZATION
//---------------------------------------------------------------------------

template <typename T>
void Serialize(const T& val, std::ostream& out) {
	out.write(reinterpret_cast<const char*>(&val), sizeof(T));
}
inline void Serialize(const std::string& str, std::ostream& out) {
	size_t size = str.length();
	out.write(reinterpret_cast<const char*>(&size), sizeof(size_t));
	out.write(str.data(), size);
}
template <typename T>
void Serialize(const std::vector<T>& val, std::ostream& out) {
	Serialize(val.size(), out);
	for (const auto &v : val) Serialize(v, out);
}
template <typename T>
void Serialize(const std::deque<T>& val, std::ostream& out) {
	Serialize(val.size(), out);
	for (const auto &v : val) Serialize(v, out);
}
template <typename T1, typename T2>
void Serialize(const std::pair<T1, T2>& val, std::ostream& out) {
	Serialize(val.first, out);
	Serialize(val.second, out);
}
template <typename T1, typename T2>
void Serialize(const std::map<T1, T2>& val, std::ostream& out) {
	Serialize(val.size(), out);
	for (const auto &v : val) {
		Serialize(v.first, out);
		Serialize(v.second, out);
	}
}
template <typename T>
void Serialize(const std::unique_ptr<T>& val, std::ostream& out) {
	Serialize(*val.get(), out);
}

// Deserialization
template <typename T>
std::istream& Deserialize(std::istream& in, T& val) {
	in.read(reinterpret_cast<char*>(&val), sizeof(T));
	return in;
}
inline std::istream& Deserialize(std::istream& in, std::string& str) {
	size_t size;
	Deserialize(in, size);
	str.resize(size);
	in.read(str.data(), size);
	return in;
}
template <typename T>
std::istream& Deserialize(std::istream& in, std::vector<T>& val) {
	size_t size;
	Deserialize(in, size);
	val.resize(size);
	for (size_t i = 0; i < size; ++i) {
		T v;
		Deserialize(in, v);
		val[i] = std::move(v);
	}
	return in;
}
template <typename T>
std::istream& Deserialize(std::istream& in, std::deque<T>& val) {
	size_t size;
	Deserialize(in, size);
	val.resize(size);
	for (size_t i = 0; i < size; ++i) {
		T v;
		Deserialize(in, v);
		val[i] = std::move(v);
	}
	return in;
}
template <typename T1, typename T2>
std::istream& Deserialize(std::istream& in, std::pair<T1, T2>& val) {
	T1 v1;
	T2 v2;
	Deserialize(in, v1);
	Deserialize(in, v2);
	val.first = std::move(v1);
	val.second = std::move(v2);
	return in;
}
template <typename T1, typename T2>
std::istream& Deserialize(std::istream& in, std::map<T1, T2>& val) {
	size_t size;
	Deserialize(in, size);
	for (size_t i = 0; i < size; ++i) {
		T1 v1;
		T2 v2;
		Deserialize(in, v1);
		Deserialize(in, v2);
		val[std::move(v1)] = std::move(v2);
	}
	return in;
}
template <typename T>
std::istream& Deserialize(std::istream& in, std::unique_ptr<T>& val) {
	T* obj = new T();
	Deserialize(in, *obj);
	val.reset(obj);
	return in;
}


#endif

