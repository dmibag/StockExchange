#ifndef SvgH
#define SvgH

#include <cstdint>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

namespace svg {

struct Point {
	Point() = default;
	Point(double x, double y) :
			x(x), y(y) {
	}
	double x = 0.;
	double y = 0.;
};

struct Rgb {
	uint8_t red = 0;
	uint8_t green = 0;
	uint8_t blue = 0;
	Rgb(uint8_t r, uint8_t g, uint8_t b) :
			red(r), green(g), blue(b) {
	}
	Rgb() = default;
};

struct Rgba {
	uint8_t red = 0;
	uint8_t green = 0;
	uint8_t blue = 0;
	double opacity = 1.;
	Rgba(uint8_t r, uint8_t g, uint8_t b, double o) :
			red(r), green(g), blue(b), opacity(o) {
	}
	Rgba() = default;
};

using Color = std::variant<std::monostate, std::string, svg::Rgb, svg::Rgba>;

inline const Color NoneColor { std::string("none") };

enum class StrokeLineCap {
    BUTT, ROUND, SQUARE,
};
enum class StrokeLineJoin {
    ARCS, BEVEL, MITER, MITER_CLIP, ROUND,
};
std::ostream& operator<<(std::ostream &out, const StrokeLineCap &line_cap);
std::ostream& operator<<(std::ostream &out, const StrokeLineJoin &line_join);

std::string SolColor(Color color);

template<typename Owner>
class PathProps {
public:
	Owner& SetFillColor(Color color) {
		fill_color_ = std::move(color);
		return AsOwner();
	}

	Owner& SetFillOpacity(double fill_opacity) {
		fill_opacity_ = fill_opacity;
		return AsOwner();
	}

	Owner& SetStrokeOpacity(double stroke_opacity) {
		stroke_opacity_ = stroke_opacity;
		return AsOwner();
	}

	Owner& SetStrokeColor(Color color) {
		stroke_color_ = std::move(color);
		return AsOwner();
	}

	Owner& SetStrokeWidth(double width) {
		width_ = std::move(width);
		return AsOwner();
	}

	Owner& SetStrokeLineCap(StrokeLineCap line_cap) {
        line_cap_ = std::move(line_cap);
        return AsOwner();
	}
	Owner& SetStrokeLineJoin(StrokeLineJoin line_join) {
		line_join_ = std::move(line_join);
		return AsOwner();
	}

protected:
	~PathProps() = default;

	void RenderAttrs(std::ostream &out) const {
		using namespace std::literals;
		if (fill_color_) {
			out << " fill=\""sv << SolColor(*fill_color_) << "\""sv;
		}
		if (stroke_color_) {
			auto s = *stroke_color_;
			out << " stroke=\""sv << SolColor(*stroke_color_) << "\""sv;
		}
		if (width_) {
			out << " stroke-width=\""sv << *width_ << "\""sv;
		}
		if (fill_opacity_) {
			out << " fill-opacity=\""sv << *fill_opacity_ << "\""sv;
		}
		if (stroke_opacity_) {
			out << " stroke-opacity=\""sv << *stroke_opacity_ << "\""sv;
		}
        if (line_cap_) {
            out << " stroke-linecap=\""sv << *line_cap_ << "\""sv;
        }
        if (line_join_) {
            out << " stroke-linejoin=\""sv << *line_join_ << "\""sv;
            ;
		}
	}

private:
	Owner& AsOwner() {
		return static_cast<Owner&>(*this);
	}

	std::optional<Color> fill_color_;
	std::optional<Color> stroke_color_;
	std::optional<double> width_;
	std::optional<double> fill_opacity_;
	std::optional<double> stroke_opacity_;
    std::optional<StrokeLineCap> line_cap_;
	std::optional<StrokeLineJoin> line_join_;
};

class Object {
public:
	void Render(std::ostream &out) const;
	virtual ~Object() = default;

private:
	virtual void RenderObject(std::ostream &out) const = 0;
};

class Circle final : public Object, public PathProps<Circle> {
public:
	Circle& SetCenter(Point center);
	Circle& SetRadius(double radius);

private:
	void RenderObject(std::ostream &out) const override;
	Point center_;
	double radius_ = 1.0;
};

class Rect final : public Object, public PathProps<Rect> {
public:
	Rect& SetBegin(Point point);
	Rect& SetWidth(double w);
	Rect& SetHeight(double h);
	Rect& SetRX(double rx);

private:
	void RenderObject(std::ostream &out) const override;

	Point begin_;
	double w_;
	double h_;
	std::optional<double> rx_;
};

class Line final : public Object, public PathProps<Line> {
public:
	Line& SetBegin(Point point);
	Line& SetEnd(Point point);
	Line& SetStrokeArray(std::string);

private:
	void RenderObject(std::ostream &out) const override;
	Point begin_;
	Point end_;
	std::optional<std::string> stroke_array_;
};

class Block final : public Object {
public:
	Block& SetBlock(std::string block);

private:
	void RenderObject(std::ostream &out) const override;

	std::string block_;
};

class Polyline final : public Object, public PathProps<Polyline> {
public:
	Polyline& AddPoint(Point point);

private:
	void RenderObject(std::ostream &out) const override;
	std::string str_points_;
};

class Text final : public Object, public PathProps<Text> {
public:
	Text& SetPosition(Point pos);
	Text& SetOffset(Point offset);
	Text& SetFontSize(uint32_t size);
	Text& SetFontFamily(std::string font_family);
	Text& SetFontWeight(std::string font_weight);
	Text& SetData(std::string data);

private:
	void RenderObject(std::ostream &out) const override;

	Point pos_;
	Point offset_;
	uint32_t size_ = 1;
	std::string font_family_;
	std::string font_weight_;
	std::string data_;
};

class Title final : public Object {
public:
	Title& SetTitle(std::string text);

private:
	void RenderObject(std::ostream &out) const override;

	std::string text_;
};

class ObjectContainer {
public:
	ObjectContainer(int width, int height) : width_(width), height_(height) {}
	ObjectContainer() = delete;

	void AddPtr(std::unique_ptr<Object> &&obj) {
		objects_.emplace_back(std::move(obj));
	}

	template<typename Obj>
	void Add(Obj obj) {
		AddPtr(std::make_unique<Obj>(std::move(obj)));
	}

	void Render(std::ostream &out) const {
		using namespace std::literals;
		out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl;
		out << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"" << std::to_string(width_) << "\" height=\"" << std::to_string(height_) << "\"  version=\"1.1\">"sv << std::endl;
		for (const auto &ob : objects_) {
			ob->Render(out);
		}
//		out << "<text x=\"0\" y=\"15\" fill=\"red\">DONE</text>" << std::endl;
		out << "</svg>"sv;
	}

private:
	std::vector<std::unique_ptr<Object>> objects_;
	int width_;
	int height_;
};

class Drawable {
public:
	virtual void Draw(ObjectContainer &container) const = 0;
	virtual ~Drawable() = default;
};

}  // namespace svg
#endif
