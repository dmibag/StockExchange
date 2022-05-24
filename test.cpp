//---------------------------------------------------------------------------

#pragma hdrstop

#include "test.h"
#include "structures.h"
#include "utils.h"

#pragma package(smart_init)

namespace {

void TestLoadBarMinute() {
//	std::string str("SPFB.RTS,1,20090111,103000,60480.0000000,64945.0000000,60480.0000000,62485.0000000,640");
	std::string str("SPFB.RTS-21.03,1,20210318,182000,15006.0000000,150070.0000000,150060.0000000,150070.0000000,34");

	Bar dbstr;
	str >> dbstr;
	assert(dbstr.iperiod == 2103);
	assert(dbstr.datetime.DateTimeString() == "18.03.2021 18:20:00");
	DateTimeValues vals{2021, 3, 18, 18, 20};
	assert(dbstr.datetimevals == vals);
	assert(dbstr.open == 15006.);
	assert(dbstr.high == 150070.);
	assert(dbstr.low == 150060.);
	assert(dbstr.close == 150070.);
	assert(dbstr.vol == 34);
}

void TestPrc() {
//	assert(Prc(100.,0.1,10.) == 10.);
//	assert(Prc(105.,0.1,10.) == 10.);
//	assert(Prc(190.,0.1,10.) == 20.);
//	assert(Prc(200.,0.1,10.) == 20.);

	Prc prc(10);
	double d = 100;
	assert(d - prc(10) == 90);
	assert(d - prc(8) == 90);
	assert(d - prc(3) == 100);

}

void TestOther() {
	assert(ClearToStep(100, 10) == 100);
	assert(ClearToStep(96, 10) == 100);
	assert(ClearToStep(94, 10) == 90);
	assert(ClearToStep(90, 10) == 90);
}

void TestTemporary() {
	double b = 100;
	double w = 400;
	double bu = 9900;
	double bd = 9800;
	double h = 10000;
	double l = 9600;

	double btoa = b/w;
	double mid_body = bu - (bu - bd)/2;
	double mbtomw = (h - mid_body)/(h - l); // 0.375

    assert(mbtomw == 0.375);
}
} // namespace

void Test() {
	TestLoadBarMinute();
	TestPrc();
	TestOther();
	TestTemporary();
}

//---------------------------------------------------------------------------

