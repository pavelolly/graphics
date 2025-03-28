#include "geometry.hpp"

#include <numeric>
#include <numbers>
#include <cmath>

Point RotatePoint(Point point, float angle, Point center) {
    point -= center;

    float x = point.x;
    float y = point.y;

    point.x = x * std::cos(angle) - y * std::sin(angle);
    point.y = x * std::sin(angle) + y * std::cos(angle);

    return point + center;
}

float Distance(Point a, Point b) {
    float x = a.x - b.x;
    float y = a.y - b.y;
    return std::sqrt(x * x + y * y);
}

float Length(Point a) {
    return std::sqrt(a.x * a.x + a.y * a.y);
}

Point Lerp(Point a, Point b, float t) {
    return a + (b - a) * t;
}

void DrawLineDotted(Point start, Point end, float segment_len, float thick, Color color) {
    const auto nsegments = static_cast<int>(std::ceil(Distance(start, end) / (2 * segment_len)));
    
    Point delta = end - start;
    Point step = Vector2Normalize(delta) * segment_len;

    bool draw_end_cap = true;
    
    for (int i = 0; i < nsegments; ++i) {
        Point segment_start = start + step * static_cast<float>(2 * i);
        Point segment_end   = segment_start + step;

        if (i == nsegments - 1 && Distance(segment_start, segment_end) > Distance(segment_start, end)) {
            segment_end = end;
            draw_end_cap = false;
        }

        DrawLineEx(segment_start, segment_end, thick, color);
    }

    if (draw_end_cap) {
        DrawLineEx(end + step*0.05f, end - step*0.05f, thick, color);
    }    
};

bool IsInsideTriangle(Point p, Point a, Point b, Point c) {
    bool side1 = signbit((p.x - a.x) * (b.y - a.y) - (p.y - a.y) * (b.x - a.x));
    bool side2 = signbit((p.x - b.x) * (c.y - b.y) - (p.y - b.y) * (c.x - b.x));
    bool side3 = signbit((p.x - c.x) * (a.y - c.y) - (p.y - c.y) * (a.x - c.x));

    return side1 == side2 && side2 == side3;
}

bool IsInsideTriangle2(Point p, Point a, Point b, Point c) {
    float d  = (b.x - a.x) * (c.y - a.y) - (c.x - a.x) * (b.y - a.y);
    float k1 = ((b.x - p.x) * (c.y - p.y) - (c.x - p.x) * (b.y - p.y)) / d;
    float k2 = ((p.x - a.x) * (c.y - a.y) - (c.x - a.x) * (p.y - a.y)) / d;
    float k3 = 1 - k1 - k2;

    return k1 > 0 && k2 > 0 && k3 > 0;
}

std::function<Point(float)> BezierFunc(const std::vector<Point> &control_points) {
    size_t size = control_points.size();
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
            for (uint32_t i = 1; i <= order; ++i) {
                coefs[i] = coefs[i - 1] * static_cast<float>(order - i + 1) / static_cast<float>(i);
            }

            // multiply coefs by control points
            for (uint32_t i = 0; i < coefs.size(); ++i) {
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

Polygon::Polygon(std::initializer_list<Point> points) {
    vertexes.reserve(points.size());
    std::copy(points.begin(), points.end(), vertexes.begin());
}

void Polygon::Rotate(float angle) {
    Point center = GetCenter();
    for (Point &point : vertexes) {
        point = RotatePoint(point, angle, center);
    }
}

float Polygon::Perimeter() const {
    if (vertexes.size() <= 1) {
        return 0;
    }

    Point a = vertexes.front();
    float len = 0;
    for (size_t i = 1; i < vertexes.size(); i++) {
        len += Distance(a, vertexes[i]);
        a = vertexes[i];
    }
    len += Distance(a, vertexes.front());
    return len;
}

Point Polygon::RealCenter() const {
    Point center = std::accumulate(vertexes.begin(), vertexes.end(), Vector2Zeros);
    center.x /= vertexes.size();
    center.y /= vertexes.size();
    return center;
}

void Polygon::Draw(Color color_line, Color color_point) const {
    if (vertexes.size() == 0) {
        return;
    }

    Point a = vertexes.front();
    for (size_t i = 1; i < vertexes.size(); i++) {
        DrawLineV(a, vertexes[i], color_line);
        DrawCircleV(vertexes[i], 5, color_point);
        a = vertexes[i];
    }
    DrawLineV(a, vertexes.front(), color_line);
    DrawCircleV(vertexes.front(), 5, color_point);

    DrawCircleV(RealCenter(), 7, color_point);
}

Ellipse::Ellipse(Point center, float a, float b, int poly_steps) : center(center), a(a), b(b) {
    static constexpr float pi = std::numbers::pi_v<float>;

    for (int i = 0; i <= poly_steps; ++i) {
        float t = pi / 2 + 2 * pi * (static_cast<float>(i) / poly_steps);
        Polygon::AddPoint({ center.x + a * sinf(t), center.y + b * cosf(t) });
    }
}

void Ellipse::Draw(Color color_line, Color color_point) const {
    DrawCircleV(center, std::min(a, b) / 15, GREEN);
    DrawCircleV(GetCenter(), std::min(a, b) / 15, color_point);
    Polygon::Draw(color_line);
}