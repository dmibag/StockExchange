//---------------------------------------------------------------------------

#ifndef ckpiH
#define ckpiH

#include "cbars.h"
#include "structures.h"

#include <optional>
#include <vector>

class Kpi {
public:
	Kpi(const Bars& bars) : bars_(bars) {}

	bool IsGoingUp(double open_price) const;

	// СИЛА ЗЕЛЕНИ
	// отношение суммм размеров тел зеленых баров к красным в хвосте [0; 1].
	// 1 это превалирование зеленых сумм размеров, 0.5 это одинаковое.
	Result PowerGreen(BarTimeFrame type, int inspect_bars_count, int pattern_count = 3) const;

	// СИЛА количества зеленых баров к красным
	// отношение суммм размеров тел зеленых баров к красным в хвосте [0; 1].
	// 1 это превалирование зеленых сумм размеров, 0.5 это одинаковое.
	Result PowerGreenCount(BarTimeFrame type, int inspect_bars_count, int pattern_count = 3) const;

	// СМЕЩЕНИЕ тела внутри бара
	// [0; 1] до 0.5 тело ниже.
	Result BodyOffset(BarTimeFrame type, int cur) const;

	// СИЛА тела внутри бара
	// [0; 1] чем больше тем тело больше.
	Result BodyWhole(BarTimeFrame type, int cur) const;

	// СИЛА БАРА
	// отношение длины тела CUR бара к средней длине тел баров хвоста
	// > 1 начит бар больше среднего размера
	Result PowerBar(BarTimeFrame type, int cur, int count) const;

	// СИЛА ВЕРХА
	// отношение сумм тел над SRC к сумме тел под SRC. [0; 1]
	// 1 сверху тел больше чем снизу, 0.5 - одинаково
	Result PowerUpper(int count_bars, double src , int pattern_count = 3) const;

	// ТРЕНД В БАРЕ
	// показывает цену PRICE внутри многодневного бара, фактически положение относительно тренда [0; 1]
	// 0 - тренд падающий, цена внизу тренда, 1 - тренд растущий, цена вверху
	Result TrendBar(int days, double price) const;

   	Result InBar(BarTimeFrame type, int count, double price) const;

    Result GetAvgDiff(int num) const;

	Result GetCost(BarTimeFrame type, int num) const;

private:
	const Bars& bars_;
};

class KpiLine {
public:
	KpiLine(const Bars& bars) : bars_(bars) {}

private:
	const Bars& bars_;
};
//---------------------------------------------------------------------------
#endif
