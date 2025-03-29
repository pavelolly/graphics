#pragma once

#include <raylib.h>
#include <raymath.h>

#include <deque>
#include <functional>
#include <memory>
#include <ranges>
#include <type_traits>
#include <cassert>

using Point = Vector2;

template <typename Range>
concept RangeOfPoints = std::ranges::random_access_range<Range> &&
                        std::convertible_to<typename std::ranges::iterator_t<Range>::value_type,
                                            Point>;

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

// check if point is on the same side to each triangle edge
bool IsInsideTriangle(Point p, Point a, Point b, Point c);
// calaculate barycentric coordinates
bool IsInsideTriangle2(Point p, Point a, Point b, Point c);

// get function of bezier curve of order control_points.size() - 1
std::function<Point(float)> BezierFunc(const RangeOfPoints auto &control_points) {
    size_t size = std::ranges::size(control_points);
    if (size <= 1) {
        return {};
    }
    size_t order = size - 1;

    switch (order) {
        case 0:
            assert(false && "unreachable (order == 0)");
        case 1: {
            Point p1 = control_points[0];
            Point p2 = control_points[1];

            return [p1, p2](float t) -> Point {
                return p1 * (1 - t) + p2 * t;
            };
        }
        case 2: {
            Point p1 = control_points[0];
            Point p2 = control_points[1];
            Point p3 = control_points[2];

            return [p1, p2, p3] (float t) -> Point {
                return p1 * (1 - t) * (1 - t) +
                       p2 * 2 * t * (1 - t)   +
                       p3 * t * t;
            };
        }
        case 3: {
            Point p1 = control_points[0];
            Point p2 = control_points[1];
            Point p3 = control_points[2];
            Point p4 = control_points[3];

            return [p1, p2, p3, p4] (float t) -> Point {
                return p1 * (1 - t) * (1 - t) * (1 - t) +
                       p2 * 3 * (1 - t) * (1 - t) * t   +
                       p3 * 3 * (1 - t) * t * t         +
                       p4 * t * t * t;
            };
        }
        default: {
            /*
                General formula: ```f(t) = sum i=[0..order] { control_point[i] * (order choose i) * (1 - t)^(order - i) * t^i }```
            */

            // calculate binomial coefs ```(order choose i) = order! / (i! * (order - i)!)```
            // (! - factorial)
            std::vector<Point> coefs(order + 1);
            coefs[0] = { 1.f, 1.f };
            for (int i = 1; i <= order; ++i) {
                coefs[i] = coefs[i - 1] * static_cast<float>(order - i + 1) / static_cast<float>(i);
            }

            // multiply coefs by control points
            for (int i = 0; i < coefs.size(); ++i) {
                coefs[i] *= control_points[i];
            }

            return [order, coefs = std::move(coefs)] (float t) -> Point {
                if (FloatEquals(t, 0)) {
                    return coefs.front();
                } else if (FloatEquals(t, 1)) {
                    return coefs.back();
                }

                Point res = Vector2Zeros;
                float t_mul = powf(1 - t, static_cast<float>(order));
                for (auto coef : coefs) {
                    res += coef * t_mul;
                    t_mul *= t / (1 - t);
                }
                return res;
            };
        }
    }
}

void DrawLineDotted(Point start, Point end, float segment_len, float thick, Color color);

struct Polygon {
    std::deque<Point> vertexes;

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