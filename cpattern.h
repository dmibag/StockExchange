//---------------------------------------------------------------------------

#ifndef cpatternH
#define cpatternH

#include "cbars.h"
#include "structures.h"

#include <memory>
#include <optional>
#include <vector>

class BasePattern {
public:
	virtual ~BasePattern() = default;
	virtual std::optional<BasePatParams> FindPattern() const = 0;
	virtual std::string_view Name() const = 0;
	virtual int ID() const = 0;
};

class Pattern3DownLine : public BasePattern {
public:
	Pattern3DownLine(const Bars& bars) : bars_(bars) {}
	std::optional<BasePatParams> FindPattern() const override;
	std::string_view Name() const override {return name_; }
	int ID() const override {return id_; }

private:
	const Bars& bars_;
	const char *name_ = "3DownLine";
	const int id_ = 1;
};

class Pattern3DownLineSide : public BasePattern {
public:
	Pattern3DownLineSide(const Bars& bars) : bars_(bars) {}
	std::optional<BasePatParams> FindPattern() const override;
	std::string_view Name() const override {return name_; }
	int ID() const override {return id_; }

private:
	const Bars& bars_;
	const char *name_ = "3DownLineSide";
	const int id_ = 3;
};

class Pattern3UpLine : public BasePattern {
public:
	Pattern3UpLine(const Bars& bars) : bars_(bars) {}
	std::optional<BasePatParams> FindPattern() const override;
	std::string_view Name() const override {return name_; }
	int ID() const override {return id_; }

private:
	const Bars& bars_;
	const char *name_ = "3UpLine";
	const int id_ = 2;
};

class Pattern3UpLineSide : public BasePattern {
public:
	Pattern3UpLineSide(const Bars& bars) : bars_(bars) {}
	std::optional<BasePatParams> FindPattern() const override;
	std::string_view Name() const override {return name_; }
	int ID() const override {return id_; }

private:
	const Bars& bars_;
	const char *name_ = "3UpLineSide";
	const int id_ = 4;
};

class Pattern3NoLineNar : public BasePattern {
public:
	Pattern3NoLineNar(const Bars& bars) : bars_(bars) {}
	std::optional<BasePatParams> FindPattern() const override;
	std::string_view Name() const override {return name_; }
	int ID() const override {return id_; }

private:
	const Bars& bars_;
	const char *name_ = "3NoLineNar";
	const int id_ = 7;
};

class Pattern3FollowUp : public BasePattern {
public:
	Pattern3FollowUp(const Bars& bars) : bars_(bars) {}
	std::optional<BasePatParams> FindPattern() const override;
	std::string_view Name() const override {return name_; }
	int ID() const override {return id_; }

private:
	const Bars& bars_;
	const char *name_ = "3FollowUp";
	const int id_ = 8;
};

class Pattern3FollowDown : public BasePattern {
public:
	Pattern3FollowDown(const Bars& bars) : bars_(bars) {}
	std::optional<BasePatParams> FindPattern() const override;
	std::string_view Name() const override {return name_; }
	int ID() const override {return id_; }

private:
	const Bars& bars_;
	const char *name_ = "3FollowDown";
	const int id_ = 9;
};

class Pattern2Up : public BasePattern {
public:
	Pattern2Up(const Bars& bars) : bars_(bars) {}
	std::optional<BasePatParams> FindPattern() const override;
	std::string_view Name() const override {return name_; }
	int ID() const override {return id_; }

private:
	const Bars& bars_;
	const char *name_ = "2Up";
	const int id_ = 10;
};

class Pattern2Down : public BasePattern {
public:
	Pattern2Down(const Bars& bars) : bars_(bars) {}
	std::optional<BasePatParams> FindPattern() const override;
	std::string_view Name() const override {return name_; }
	int ID() const override {return id_; }

private:
	const Bars& bars_;
	const char *name_ = "2Down";
	const int id_ = 11;
};

class PatternCostUp : public BasePattern {
public:
	PatternCostUp(const Bars& bars) : bars_(bars) {}
	std::optional<BasePatParams> FindPattern() const override;
	std::string_view Name() const override {return name_; }
	int ID() const override {return id_; }

private:
	const Bars& bars_;
	const char *name_ = "CostUp";
	const int id_ = 12;
};

class PatternCostDown : public BasePattern {
public:
	PatternCostDown(const Bars& bars) : bars_(bars) {}
	std::optional<BasePatParams> FindPattern() const override;
	std::string_view Name() const override {return name_; }
	int ID() const override {return id_; }

private:
	const Bars& bars_;
	const char *name_ = "CostDown";
	const int id_ = 13;
};

class Patterns {
public:
	Patterns(const Bars& bars);

	std::optional<BasePatParams> FindPattern() {
		for (auto &uptr : patterns_) {
			if (auto pat = uptr->FindPattern()) {
				return pat;
			}
		}
        return std::nullopt;
	}


private:
	const Bars& bars_;
	std::vector<std::unique_ptr<BasePattern>> patterns_;
};
//---------------------------------------------------------------------------
#endif
