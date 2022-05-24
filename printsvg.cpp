//---------------------------------------------------------------------------

#pragma hdrstop

#include "printsvg.h"

#include <memory>

//---------------------------------------------------------------------------
#pragma package(smart_init)


namespace render {

using namespace std;

namespace shapes {

void MapText::Draw(svg::ObjectContainer &container) const {
    using namespace std::literals;
    auto basetext = svg::Text().SetPosition(position_).SetOffset(offset_).SetFontSize(label_font_size_).SetFontFamily(
            "Verdana"s).SetData(std::string(data_));
    if (font_weight_) basetext.SetFontWeight("bold"s);
    auto textunder = Text { basetext }.SetFillColor(underlayer_color_).SetStrokeColor(underlayer_color_).SetStrokeWidth(
			underlayer_width_).SetStrokeLineCap(StrokeLineCap::ROUND).SetStrokeLineJoin(StrokeLineJoin::ROUND);
    auto text = Text { basetext }.SetFillColor(fill_);
    container.Add(std::move(textunder));
    container.Add(std::move(text));
}

void Bar::Draw(svg::ObjectContainer &container) const {
	svg::Color bar_color = (black_)
		?  "grey"s
		: ((bar_props_.type == BarShowProps::GREEN) ? "green"s : "red"s);
	auto rect = svg::Rect().SetFillColor(bar_color)
						   .SetFillOpacity(opacity_)
						   .SetBegin({offset_, bar_props_.y2})
						   .SetWidth(bar_props_.w)
						   .SetHeight(bar_props_.y3 - bar_props_.y2)
						   .SetRX(2);
	if (lines_) {
		rect = svg::Rect{rect}.SetStrokeColor(bar_color).SetStrokeWidth(1);
	}
	else {
		rect = svg::Rect{rect}.SetStrokeColor(svg::NoneColor);
	}
	auto line_up = svg::Line().SetBegin({offset_ + bar_props_.w/2, bar_props_.y1})
						   .SetEnd({offset_ + bar_props_.w/2, bar_props_.y2})
						   .SetStrokeColor("grey"s)
						   .SetStrokeWidth(1.);
	auto line_down = svg::Line().SetBegin({offset_ + bar_props_.w/2, bar_props_.y3})
						   .SetEnd({offset_ + bar_props_.w/2, bar_props_.y4})
						   .SetStrokeColor("grey"s)
						   .SetStrokeWidth(1.);
	if (hint_) {
		container.Add(svg::Block().SetBlock("<g>"));
		container.Add(svg::Title().SetTitle(bar_props_.title));
	}
	container.Add(std::move(rect));
	if (lines_) container.Add(std::move(line_up));
	if (lines_) container.Add(std::move(line_down));
	if (hint_) {
		container.Add(svg::Block().SetBlock("</g>"));
	}
}

void Grid::Draw(svg::ObjectContainer& container) const {
	auto base_line = svg::Line().SetStrokeColor("grey"s).SetStrokeWidth(1).SetStrokeArray("5").SetStrokeOpacity(0.3);

	for (double cur = projector_.GetHighLine() - projector_.GetGridStep(); cur > projector_.GetLowLine(); cur = cur - projector_.GetGridStep()) {
		shapes::MapText(std::to_string(static_cast<int>(cur)), {0, projector_.GetY(cur)}, {10, 4}, 14, svg::Rgba(255,255,255,0.85), 3, "black"s, false).Draw(container);

		container.Add(
			svg::Line{base_line}.SetBegin(
				{projector_.GetOffStart(), projector_.GetY(cur)}
			).SetEnd(
				{projector_.GetWidth() - projector_.GetOffEnd(), projector_.GetY(cur)}
			)
		);
	}
}

void Triangle::Draw(svg::ObjectContainer& container) const {
	auto polyline = svg::Polyline().SetFillColor((good_) ? "green"s : "red"s);

	auto point_right {point_};
	point_right.x += width_;

	auto point_mid {point_};
	point_mid.x += width_/2;
	double h = sin((60 * PI_)/180)*width_;
	point_mid.y += (buy_) ? -h : h;

	polyline.AddPoint(point_).AddPoint(point_right).AddPoint(point_mid).AddPoint(point_);
	container.Add(std::move(polyline));
}

void Deals::Draw(svg::ObjectContainer& container) const {


	for (const auto& enter : samples_) {
		auto it_finded = std::next(std::upper_bound( deq_.begin(), deq_.end(), enter.open_bar.datetime, [](TDateTime date, const ::Bar& bar){
			return date < bar.datetime;
		}), - 1);
		if (it_finded == deq_.end()) {
			throw std::exception("open bar not founded in RenderCombine");
		}

		auto it_closed = std::next(std::upper_bound( deq_.begin(), deq_.end(), enter.close_bar.datetime, [](TDateTime date, const ::Bar& bar){
			return date < bar.datetime;
		}), -1 );

		if (it_closed == deq_.end()) {
			throw std::exception("closed bar not founded in RenderCombine");
		}

		shapes::Deal(projector_, deq_, enter, deq_.begin(), it_finded, it_closed, 0.4, true, true, true).Draw(container);

	}
}

void Deal::Draw(svg::ObjectContainer& container) const {
	bool buy = sample_.sample.buy;
	bool good = sample_.good();

	svg::Rect up_rect;
	svg::Rect down_rect;

	auto top_height = projector_.GetY(sample_.opened_price) - projector_.GetY(sample_.sample.close_top);
	auto bot_height = projector_.GetY(sample_.sample.close_bottom) - projector_.GetY(sample_.opened_price);

	svg::Rect base_rect;
	if (base_) {
//		base_rect = svg::Rect().SetStrokeColor(svg::NoneColor)
		base_rect = svg::Rect().SetStrokeColor("black"s)
					   .SetBegin({projector_.GetOffset(it_first_, it_open_) - 5, projector_.GetY(sample_.sample.close_top) - 5})
					   .SetWidth(projector_.GetOffset(it_first_, it_close_) - projector_.GetOffset(it_first_, it_open_) + projector_.GetObjWidth() + 10)
					   .SetHeight(top_height + bot_height + 10).SetFillColor(svg::Rgba(255, 255, 255, 0.7));//.SetFillOpacity(0.5);
	}

	auto base_top_rect = svg::Rect().SetStrokeColor("grey"s).SetStrokeWidth(1)
					   .SetBegin({projector_.GetOffset(it_first_, it_open_), projector_.GetY(sample_.sample.close_top)})
					   .SetWidth(projector_.GetOffset(it_first_, it_close_) - projector_.GetOffset(it_first_, it_open_) + projector_.GetObjWidth())
					   .SetHeight(top_height);

	auto base_bottom_rect = svg::Rect().SetStrokeColor("grey"s).SetStrokeWidth(1)
					   .SetBegin({projector_.GetOffset(it_first_, it_open_), projector_.GetY(sample_.opened_price)})
					   .SetWidth(projector_.GetOffset(it_first_, it_close_) - projector_.GetOffset(it_first_, it_open_) + projector_.GetObjWidth())
					   .SetHeight(bot_height);
	if (buy) {
		if (good) {
//			up_rect = svg::Rect{base_top_rect}.SetFillColor(svg::NoneColor);
			up_rect = svg::Rect{base_top_rect}.SetFillColor("green"s).SetFillOpacity(opacity_);
			down_rect = svg::Rect{base_bottom_rect}.SetFillColor(svg::NoneColor);
		}
		else {
			up_rect = svg::Rect{base_top_rect}.SetFillColor(svg::NoneColor);
			down_rect = svg::Rect{base_bottom_rect}.SetFillColor("red"s).SetFillOpacity(opacity_);
//			down_rect = svg::Rect{base_bottom_rect}.SetFillColor(svg::NoneColor);
		}
	}
	else {
		if (good) {
			up_rect = svg::Rect{base_top_rect}.SetFillColor(svg::NoneColor);
			down_rect = svg::Rect{base_bottom_rect}.SetFillColor("green"s).SetFillOpacity(opacity_);
//			down_rect = svg::Rect{base_bottom_rect}.SetFillColor(svg::NoneColor);
		}
		else {
//			up_rect = svg::Rect{base_top_rect}.SetFillColor(svg::NoneColor);
			up_rect = svg::Rect{base_top_rect}.SetFillColor("red"s).SetFillOpacity(opacity_);
			down_rect = svg::Rect{base_bottom_rect}.SetFillColor(svg::NoneColor);
		}
	}

	if (hint_) {
		container.Add(svg::Block().SetBlock("<g>"));
		container.Add(svg::Title().SetTitle(sample_.title()));
	}

	if (base_) container.Add(std::move(base_rect));
	container.Add(std::move(up_rect));
	container.Add(std::move(down_rect));

	if (hint_) {
		container.Add(svg::Block().SetBlock("</g>"));
	}
	if (follow_) {
		for (int i = 0; i < sample_.ups.size(); ++i) {
			container.Add(
				svg::Circle().SetCenter(
							{
								projector_.GetOffset(it_first_, it_open_ + i + 1) + projector_.GetObjWidth()/2,
								projector_.GetY(sample_.ups[i])
							}).SetRadius(1).SetFillColor("yellow"s).SetFillOpacity(1).SetStrokeColor("black"s).SetStrokeWidth(1)
			);
			container.Add(
				svg::Circle().SetCenter(
							{
								projector_.GetOffset(it_first_, it_open_ + i + 1) + projector_.GetObjWidth()/2,
								projector_.GetY(sample_.downs[i])
							}).SetRadius(1).SetFillColor("yellow"s).SetFillOpacity(1).SetStrokeColor("black"s).SetStrokeWidth(1)
			);
		}
	}
}

void Graph::Draw(svg::ObjectContainer& container) const {

	auto sum_line = svg::Polyline().SetStrokeColor("blue"s).SetStrokeWidth(2).SetFillColor(svg::NoneColor);
	auto slump_line = svg::Polyline().SetStrokeColor("red"s).SetStrokeWidth(1).SetFillColor(svg::NoneColor);

	auto year_line = svg::Line().SetStrokeColor("grey"s).SetStrokeWidth(1).SetStrokeArray("5");

	sum_line.AddPoint({ projector_.GetOffset(exp_.agg.begin(), exp_.agg.begin(), 0), projector_.GetY(0) });
	slump_line.AddPoint({ projector_.GetOffset(exp_.agg.begin(), exp_.agg.begin(), 0), projector_.GetY(0) });

	unsigned short old_year = exp_.data.at(0).open_bar.datetimevals.year;

	for (auto it = exp_.agg.begin(); it != exp_.agg.end(); ++it) {
		sum_line.AddPoint({ projector_.GetOffset(exp_.agg.begin(), it, 1), projector_.GetY(it->sum) });
		slump_line.AddPoint({ projector_.GetOffset(exp_.agg.begin(), it, 1), projector_.GetY(it->slump) });

		unsigned short year = exp_.data.at(std::distance(exp_.agg.begin(), it)).open_bar.datetimevals.year;

		if (year != old_year) {
			old_year = year;
			auto left = projector_.GetOffset(exp_.agg.begin(), it, 1) - projector_.GetObjWidth()/2;

			shapes::MapText(std::to_string(year),
				{left, projector_.GetY(0)},
				{2, 12}, 10, svg::Rgba(255,255,255,0.85), 1, "black"s, false).Draw(container);

			container.Add(
				svg::Line{year_line}.SetBegin(
					{left, projector_.GetHighLine()}
				).SetEnd(
					{left, projector_.GetLowLine()}
				)
			);

		}
	}
	container.Add(std::move(sum_line));
	container.Add(std::move(slump_line));
}

} // shapes

svg::ObjectContainer RenderMap(const deque<Bar> &deq, const deque<double> &avg, const deque<double> &ind, std::vector<EnterData> &samples, int width, int height) {
	svg::ObjectContainer doc(width, height);
	ProjectorAllInOne pr(deq, width, height, 70, 10, 0, 2000);

	auto base_rect = svg::Rect().SetFillColor(svg::NoneColor).SetStrokeColor("grey"s).SetStrokeWidth(1);

	doc.Add(svg::Rect{base_rect}.SetBegin({0, 0}).SetWidth(pr.GetWidth()).SetHeight(pr.GetHeight()));

	shapes::Grid(pr).Draw(doc);

	for (auto it = deq.begin(); it != deq.end(); ++it) {
		shapes::Bar(pr.GetBarProps(it), pr.GetOffset(deq.begin(), it), 1, true, false, true).Draw(doc);

		doc.Add(
			svg::Circle().SetCenter({pr.GetOffset(deq.begin(), it) + pr.GetObjWidth()/2, pr.GetY(avg.at(std::distance(deq.begin(), it)))})
						 .SetRadius(2).SetFillColor("blue"s).SetFillOpacity(0.4).SetStrokeColor("black"s).SetStrokeWidth(1)
		);

		if (ind.at(std::distance(deq.begin(), it)) == 1.0) {
			doc.Add(
				svg::Circle().SetCenter({pr.GetOffset(deq.begin(), it-1) + pr.GetObjWidth()/2, pr.GetY((it-1)->high)})
							 .SetRadius(6).SetFillColor("green"s).SetFillOpacity(0.4).SetStrokeColor("black"s).SetStrokeWidth(1)
			);

		}
		else if (ind.at(std::distance(deq.begin(), it)) == -1.0) {
			doc.Add(
				svg::Circle().SetCenter({pr.GetOffset(deq.begin(), it-1) + pr.GetObjWidth()/2, pr.GetY((it-1)->low)})
							 .SetRadius(6).SetFillColor("red"s).SetFillOpacity(0.4).SetStrokeColor("black"s).SetStrokeWidth(1)
			);

		}
	}
// Draw avg speed indicator
	{
		ProjectorIndicator pr_avg_speed(ind, width, height, 70, 10, 0, 2000, 100, 0);
		for (auto it = deq.begin(); it != deq.end(); ++it) {
			doc.Add(
				svg::Circle().SetCenter({pr_avg_speed.GetOffset(deq.begin(), it) + pr_avg_speed.GetObjWidth()/2,
										 pr_avg_speed.GetY(it->vol / (it->high - it->low))})
							 .SetRadius(1).SetFillColor("green"s).SetFillOpacity(0.4).SetStrokeColor("black"s).SetStrokeWidth(1)
			);
		}
	}
//


	int period = deq.at(0).iperiod;

	vector<EnterData> sam;
	std::copy_if(samples.begin(), samples.end(), std::back_inserter(sam), [&](EnterData& src){
		return src.open_bar.iperiod == period
			&& src.correct()
			&& src.open_bar.datetime >= deq.front().datetime
			&& src.close_bar.datetime <= deq.back().datetime;
	});

	shapes::Deals(pr, deq, sam).Draw(doc);

	shapes::MapText(std::to_string(static_cast<int>(period)), {static_cast<double>(width), 0}, {-60, 20}, 20,
								   svg::Rgba(255,255,255,0.85), 3, "black"s, true).Draw(doc);

	return doc;
}

svg::ObjectContainer RenderCombine(Run& run, std::vector<EnterData>& samples, int display_width, int display_height,
		double top_off, double bottom_off, int left_count, int right_count, bool lines, bool black, double opacity, bool grid, bool buy, bool good, double grid_step) {

	svg::ObjectContainer doc(display_width, display_height);

	// draw grid

	int period{0};
	std::deque<Bar> deq;
	Bars bars(10);
	for (const auto& enter : samples) {
		// get (ubdate) bars
		if (enter.open_bar.iperiod != period) {
			period = enter.open_bar.iperiod;
			bars = run.BuildBars(enter.open_bar.iperiod);
			switch (enter.sample.base_params.type) {
				case BarTimeFrame::M: deq = std::move(bars.M().Deq()); break;
				case BarTimeFrame::H: deq = std::move(bars.H().Deq()); break;
				case BarTimeFrame::D: deq = std::move(bars.D().Deq()); break;
			}
		}
		auto it_finded = std::next(std::upper_bound( deq.begin(), deq.end(), enter.open_bar.datetime, [](TDate date, const Bar& bar){
			return date < bar.datetime;
		}), - 1);
		if (it_finded == deq.end()) {
			throw std::exception("open bar not founded in RenderCombine");
		}
		auto it_first = std::next(it_finded, -left_count);
		auto it_last = std::next(it_finded, right_count + 1);

		int top_off_ready = (grid) ? top_off : enter.sample.GetStoplossOffset()*top_off;
		int bottom_off_ready = (grid) ? bottom_off : enter.sample.GetStoplossOffset()*bottom_off;

		ProjectorCombine pr(deq, display_width, display_height, 3, 3, 3, 2000,
			top_off_ready, bottom_off_ready, left_count, right_count, enter.opened_price);

		for (auto it = it_first; it != deq.end() && it != it_last; ++it) {
			shapes::Bar(pr.GetBarProps(it), pr.GetOffset(it_first, it), opacity, lines, black, false).Draw(doc);
		}
		auto it_closed = std::next(std::upper_bound( deq.begin(), deq.end(), enter.close_bar.datetime, [](TDate date, const Bar& bar){
			return date < bar.datetime;
		}), - 1);

		if (it_closed == deq.end()) {
			throw std::exception("closed bar not founded in RenderCombine");
		}

		shapes::Deal(pr, deq, enter, it_first, it_finded, it_closed, opacity, false, false, false).Draw(doc);
	}

	string_view buy_text = buy ? "BUY"sv : "SELL"sv;
	shapes::MapText(buy_text, {0, 0}, {10, 20}, 20, svg::Rgba(255,255,255,0.85), 3, "black"s, true).Draw(doc);

	svg::Color good_color = good ? "green"s : "red"s;
	string_view good_text = good ? "SUCCES"sv : "LOST"sv;
	shapes::MapText(good_text, {0, 0}, {10, 40}, 20, svg::Rgba(255,255,255,0.85), 3, good_color, true).Draw(doc);

	return doc;
}

svg::ObjectContainer RenderGraph(const Experiment& experiment, int display_width, int display_height) {

	svg::ObjectContainer doc(display_width, display_height);
	ProjectorGraph pr(experiment.agg, display_width, display_height, 70., 10., 0., 10000.);

	shapes::Grid(pr).Draw(doc);
	doc.Add(
		svg::Line().SetStrokeColor("black"s).SetStrokeWidth(1)
		.SetBegin( {pr.GetOffStart(), pr.GetY(0)} ).SetEnd(	{pr.GetWidth() - pr.GetOffEnd(), pr.GetY(0)}
		)
	);
	shapes::Graph(pr, experiment).Draw(doc);

	return doc;
}

} // render
