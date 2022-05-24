#pragma hdrstop

#include "svg.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

namespace svg {

using namespace std::literals;

std::string SolColor(Color color) {
	if (auto *str = std::get_if<std::string>(&color)) {
		return *str;
	}
	if (auto *rgb = std::get_if<svg::Rgb>(&color)) {
		using namespace std::string_literals;
		std::ostringstream out;
		out << "rgb("s << (int) rgb->red << ',' << (int) rgb->green << ',' << (int) rgb->blue << ')';
		return out.str();
	}
	if (auto *rgba = std::get_if<svg::Rgba>(&color)) {
		using namespace std::string_literals;
		std::ostringstream out;
		out << "rgba("s << (int) rgba->red << ',' << (int) rgba->green << ','
			<< (int) rgba->blue << ',' << rgba->opacity << ')';
		return out.str();
	}
	return std::string("none");
};

void Object::Render(std::ostream &out) const {
	RenderObject(out);
	out << std::endl;
}

std::ostream& operator<<(std::ostream &out, const StrokeLineCap &line_cap) {
    using namespace std::literals;
    switch (line_cap) {
    case StrokeLineCap::BUTT:
        out << "butt"sv;
        break;
    case StrokeLineCap::ROUND:
        out << "round"sv;
        break;
    case StrokeLineCap::SQUARE:
        out << "square"sv;
        break;
    }
    return out;
}
std::ostream& operator<<(std::ostream &out, const StrokeLineJoin &line_join) {
    using namespace std::literals;
    switch (line_join) {
    case StrokeLineJoin::ARCS:
        out << "arcs"sv;
        break;
    case StrokeLineJoin::BEVEL:
        out << "bevel"sv;
        break;
    case StrokeLineJoin::MITER:
        out << "miter"sv;
        break;
    case StrokeLineJoin::MITER_CLIP:
        out << "miter-clip"sv;
        break;
    case StrokeLineJoin::ROUND:
        out << "round"sv;
        break;
    }
    return out;
}

// ---------- Circle ------------------
Circle& Circle::SetCenter(Point center) {
	center_ = center;
	return *this;
}

Circle& Circle::SetRadius(double radius) {
	radius_ = radius;
	return *this;
}

void Circle::RenderObject(std::ostream &out) const {
	out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
	out << "r=\""sv << radius_ << "\" "sv;
	RenderAttrs(out);
	out << "/>"sv;
}

// ----------  Line    ------------------
Line& Line::SetBegin(Point point) {
	begin_ = point;
	return *this;
}

Line& Line::SetEnd(Point point) {
	end_ = point;
	return *this;
}

Line& Line::SetStrokeArray(std::string arr) {
	stroke_array_ = arr;
	return *this;
}

void Line::RenderObject(std::ostream &out) const {
	out
		<< "<line x1=\""sv << begin_.x
		<< "\" y1=\""sv << begin_.y
		<< "\" x2=\""sv << end_.x
		<< "\" y2=\""sv << end_.y << "\""sv;
	if (stroke_array_) {
		out << " stroke-dasharray=\""sv << *stroke_array_ << "\""sv;
	}
	RenderAttrs(out);
	out << "/>"sv;
}

// ---------- Block ---------------------
Block& Block::SetBlock(std::string block) {
		block_ = std::move(block);
		return *this;
}

void Block::RenderObject(std::ostream &out) const {
	out << block_;
}
// ---------- Title ---------------------
Title& Title::SetTitle(std::string text) {
		text_ = std::move(text);
		return *this;
}

void Title::RenderObject(std::ostream &out) const {
	out << "<title>"sv << text_ << "</title>"sv;
}
// ---------- Polyline ------------------
Polyline& Polyline::AddPoint(Point point) {
	std::ostringstream out;
	if (str_points_.length()) out << ' ';
	out << point.x << ',' << point.y;
	str_points_ += out.str();
	return *this;
}

void Polyline::RenderObject(std::ostream &out) const {
	out << "<polyline points=\""sv << str_points_ << "\" "sv;
	RenderAttrs(out);
	out << "/>"sv;
}

// ---------- Rect ------------------

Rect& Rect::SetBegin(Point point) {
	begin_ = point;
	return *this;
}

Rect& Rect::SetWidth(double w) {
	w_ = w;
	return *this;
}

Rect& Rect::SetHeight(double h) {
	h_ = h;
	return *this;
}

Rect& Rect::SetRX(double rx) {
	rx_ = rx;
	return *this;
}
//   <rect x="100" y="460.8" width="10" height="153.6" rx="2" fill="green" fill-opacity=".3"/>

void Rect::RenderObject(std::ostream &out) const {
	out
		<< "<rect x=\""sv << begin_.x
		<< "\" y=\""sv << begin_.y
		<< "\" width=\""sv << w_
		<< "\" height=\""sv << h_ << "\""sv;
		if (rx_) {
			out << " rx=\""sv <<  *rx_ << "\""sv;
		}
	RenderAttrs(out);
	out << "/>"sv;
}

// ---------- Text ------------------
Text& Text::SetPosition(Point pos) {
	pos_ = pos;
	return *this;
}

Text& Text::SetOffset(Point offset) {
	offset_ = offset;
	return *this;
}

Text& Text::SetFontSize(uint32_t size) {
	size_ = size;
	return *this;
}

Text& Text::SetFontFamily(std::string font_family) {
	font_family_ = std::move(font_family);
	return *this;
}

Text& Text::SetFontWeight(std::string font_weight) {
	font_weight_ = std::move(font_weight);
	return *this;
}

Text& Text::SetData(std::string data) {
    for (auto c : data) {
        std::string rep;
        switch ((int) c) {
        case 34:
            rep = "&quot;"s;
            break; // "
        case 39:
            rep = "&apos;"s;
            break; // '
        case 60:
            rep = "&lt;"s;
            break; // <
        case 62:
            rep = "&gt;"s;
            break; // >
        case 38:
            rep = "&amp;"s;
            break; // &
        default:
            rep = c;
        }
        data_ += std::move(rep);
    }
    return *this;
}

void Text::RenderObject(std::ostream &out) const {
	out << "<text"sv;
    RenderAttrs(out);
    out << " x=\""sv << pos_.x << "\""sv << " y=\""sv << pos_.y << "\""sv << " dx=\""sv << offset_.x << "\""sv
            << " dy=\""sv << offset_.y << "\""sv << " font-size=\""sv << size_ << "\""sv;
    if (font_family_.length()) out << " font-family=\"" << font_family_ << "\""sv;
    if (font_weight_.length()) out << " font-weight=\"" << font_weight_ << "\""sv;
    out << ">"sv;
    out << data_;
    out << "</text>"sv;
    return;
}

}  // namespace svg
