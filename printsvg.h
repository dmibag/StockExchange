//---------------------------------------------------------------------------

#ifndef printsvgH
#define printsvgH
//---------------------------------------------------------------------------

#pragma once

#include "svg.h"
#include "structures.h"
#include "cbars.h"
#include "crun.h"
#include "projectors.h"

#include <algorithm>
#include <deque>
#include <cmath>
#include <set>
#include <string>
#include <unordered_map>

namespace render {

using namespace svg;
using namespace std::literals;

namespace shapes {

const double PI_ = std::acos(-1);


class MapText final : public svg::Drawable {
public:
    MapText(std::string_view data, svg::Point position, svg::Point offset, int label_font_size,
            svg::Color underlayer_color, double underlayer_width, svg::Color fill, bool font_weight = true) :
            data_(data), position_(position), offset_(offset), label_font_size_(label_font_size), underlayer_color_(
                    underlayer_color), underlayer_width_(underlayer_width), fill_(fill), font_weight_(font_weight) {
	}

	void Draw(svg::ObjectContainer &container) const override;

private:
    std::string_view data_;
    svg::Point position_;
    svg::Point offset_;
    int label_font_size_;
    svg::Color underlayer_color_;
    double underlayer_width_;
    svg::Color fill_;
	bool font_weight_;
};

class Bar final : public svg::Drawable {
public:

	Bar(BarShowProps bar_props, double offset, double opacity, bool lines, bool black, bool hint)
			: bar_props_(std::move(bar_props)), offset_(offset), opacity_(opacity), lines_(lines), black_(black), hint_(hint) {
	}

	void Draw(svg::ObjectContainer &container) const override;

private:
	BarShowProps bar_props_;
	double offset_;
	double hint_;
	double opacity_;
	bool lines_;
    bool black_;
};

class Grid final : public svg::Drawable {
public:

	Grid(Projector& projector) : projector_(projector) {
	}

	void Draw(svg::ObjectContainer &container) const override;

private:
	Projector& projector_;
};

class Deals final : public svg::Drawable {
public:

	Deals(const Projector& projector, const std::deque<::Bar>& deq, const std::vector<EnterData>& sample)
			: projector_(projector), deq_(deq), samples_(sample) {
	}

	void Draw(svg::ObjectContainer &container) const override;

private:
	const std::vector<EnterData>& samples_;
	const std::deque<::Bar>& deq_;
	const Projector& projector_;
};

class Deal final : public svg::Drawable {
public:
	using IT = std::deque<::Bar>::const_iterator;

	Deal(const Projector& projector, const std::deque<::Bar>& deq, const EnterData& sample, IT it_first, IT it_open, IT it_close, double opacity, bool base, bool hint, bool follow)
			: projector_(projector), deq_(deq), sample_(sample), it_first_(it_first), it_open_(it_open), it_close_(it_close), opacity_(opacity), base_(base), hint_(hint), follow_(follow) {
	}

	void Draw(svg::ObjectContainer &container) const override;

private:
	const Projector& projector_;
	const std::deque<::Bar>& deq_;
	const EnterData& sample_;
	IT it_first_;
	IT it_open_;
	IT it_close_;
	double opacity_;
	bool base_;
	bool hint_;
    bool follow_;
};

class Triangle final : public svg::Drawable {
public:

	Triangle(svg::Point point, double width, bool buy, bool good)
			: point_(point), width_(width), buy_(buy), good_(good) {
	}

	void Draw(svg::ObjectContainer &container) const override;

private:
	svg::Point point_;
	double width_;
	bool buy_;
	bool good_;

};

class Graph final : public svg::Drawable {
public:

	Graph(const Projector& projector, const Experiment& exp)
			: projector_(projector), exp_(exp) {
	}

	void Draw(svg::ObjectContainer &container) const override;

private:
	const Projector& projector_;
	const Experiment& exp_;
};

} // shapes


svg::ObjectContainer RenderMap(const std::deque<Bar> &deq,
		const std::deque<double> &avg,
		const std::deque<double> &avg_speed,
		std::vector<EnterData> &samples,
		int width,
		int height);

svg::ObjectContainer RenderCombine(Run& run, std::vector<EnterData>& samples, int display_width, int display_height,
	   double top_off, double bottom_off, int left_count, int right_count, bool lines, bool black, double opacity, bool grid, bool buy, bool good, double grid_step);

svg::ObjectContainer RenderGraph(const Experiment& experiment, int display_width, int display_height);


} // render

#endif

