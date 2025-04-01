#pragma once

#include <raylib.h>
#include <raymath.h>

#include <deque>
#include <ranges>
#include <optional>

using Point = Vector2;

template <typename Range, typename Value>
concept RangeOf = std::ranges::random_access_range<Range> &&
                  std::convertible_to<typename std::ranges::iterator_t<Range>::value_type,
                                       Value>;

inline Point GetScreenCenter() {
    return Point { GetScreenWidth() / 2.f, GetScreenHeight() / 2.f };
}

inline Point GetRandomPoint(int min_x, int max_x, int min_y, int max_y) {
    return Point { static_cast<float>(GetRandomValue(min_x, max_x)),
                   static_cast<float>(GetRandomValue(min_y, max_y)) };
};

Point RotatePoint(Point point, float angle, Point center=Vector2Zeros);
float Distance(Point a, Point b);
float Length(Point a);
Point Lerp(Point a, Point b, float t);
Point Project(Point p, Point a, Point b);
std::optional<Point> Intersect(Point a, Point b, Point x, Point y);

// calaculate barycentric coordinates
bool IsInsideTriangle(Point p, Point a, Point b, Point c);
// check if point is on the same side to each triangle edge
bool IsInsideTriangle2(Point p, Point a, Point b, Point c);
// cast rays from p to center and checks intersections
bool IsInsideTriangle3(Point p, Point a, Point b, Point c);

void DrawLineDotted(Point start, Point end, float segment_len, float thick, Color color);

struct Polygon {
    std::deque<Point> vertexes;

    Polygon() = default;
    Polygon(std::initializer_list<Point> points) : vertexes(points) {}

    static Polygon Ellipse(Point center, float a, float b, int poly_steps=40);

    void AddPoint(Point point) {
        vertexes.push_back(point);
    }

    Point GetPoint(size_t idx) const {
        return idx < vertexes.size() ? vertexes.at(idx) : Vector2Zeros;
    }

    size_t NumPoints() const {
        return vertexes.size();
    }

    void Rotate(float angle);
    float Perimeter() const;
    // get polygon center based on its vertexes
    Point GetCenter() const;

    void SetCenter(Point new_center) {
        Point center = GetCenter();
        Shift(new_center - center);
    }

    void Shift(Point shift) {
        for (Point &point : vertexes) {
            point += shift;
        }
    }

    void Draw(Color color_line, Color color_point=BLANK) const;
    void DrawCenter(Color color) const;
};