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

	// ���� ������
	// ��������� ����� �������� ��� ������� ����� � ������� � ������ [0; 1].
	// 1 ��� �������������� ������� ���� ��������, 0.5 ��� ����������.
	Result PowerGreen(BarTimeFrame type, int inspect_bars_count, int pattern_count = 3) const;

	// ���� ���������� ������� ����� � �������
	// ��������� ����� �������� ��� ������� ����� � ������� � ������ [0; 1].
	// 1 ��� �������������� ������� ���� ��������, 0.5 ��� ����������.
	Result PowerGreenCount(BarTimeFrame type, int inspect_bars_count, int pattern_count = 3) const;

	// �������� ���� ������ ����
	// [0; 1] �� 0.5 ���� ����.
	Result BodyOffset(BarTimeFrame type, int cur) const;

	// ���� ���� ������ ����
	// [0; 1] ��� ������ ��� ���� ������.
	Result BodyWhole(BarTimeFrame type, int cur) const;

	// ���� ����
	// ��������� ����� ���� CUR ���� � ������� ����� ��� ����� ������
	// > 1 ����� ��� ������ �������� �������
	Result PowerBar(BarTimeFrame type, int cur, int count) const;

	// ���� �����
	// ��������� ���� ��� ��� SRC � ����� ��� ��� SRC. [0; 1]
	// 1 ������ ��� ������ ��� �����, 0.5 - ���������
	Result PowerUpper(int count_bars, double src , int pattern_count = 3) const;

	// ����� � ����
	// ���������� ���� PRICE ������ ������������� ����, ���������� ��������� ������������ ������ [0; 1]
	// 0 - ����� ��������, ���� ����� ������, 1 - ����� ��������, ���� ������
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
