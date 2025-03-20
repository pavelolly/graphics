#include "geometry.hpp"

#include <numeric>
#include <numbers>
#include <cmath>

Point RotatePoint(Point point, float angle, Point center) {
    point -= center;

    float x = point.x;
    float y = point.y;

    point.x = x * cosf(angle) - y * sinf(angle);
    point.y = x * sinf(angle) + y * cosf(angle);

    return point + center;
}

float Distance(Point a, Point b) {
    float x = a.x - b.x;
    float y = a.y - b.y;
    return powf(x * x + y * y, 0.5f);
}

Point Lerp(Point a, Point b, float t) {
    return a + (b - a) * t;
}

void DrawLineDotted(Point start, Point end, float thick, Color color) {
    static const int SEGMENT_LEN = 20;

    Point sstart = start;
    Point direction = Vector2Normalize(end - start);
    bool draw = true;

    for (;;) {
        Point send = sstart + direction * SEGMENT_LEN;

        if (draw) {

            // we're drawing last segment
            if (Distance(sstart, end) < SEGMENT_LEN) {
                // but we don't wanna draw it if sstart is further then end (so we don't draw extra segment after end point)
                if (Distance(start, end) > Distance(start, sstart)) {
                    DrawLineEx(sstart, end, thick, color);
                }
                break;
            }
        }

        if (draw) {
            DrawLineEx(sstart, send, thick, color);
        }

        sstart = send;
        draw = !draw;
    }

    DrawCircleV(end, thick, color);
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
    size_t order = control_points.size() - 1;
    if (order == 0) {
        return {};
    }

    if (order == 1) {
        Point p1 = control_points[0];
        Point p2 = control_points[1];

        return [p1, p2](float t) -> Point {
            return p1 * (1 - t) + p2 * t;
        };
    } else if (order == 2) {
        Point p1 = control_points[0];
        Point p2 = control_points[1];
        Point p3 = control_points[2];

        return [p1, p2, p3] (float t) -> Point {
            return p1 * (1 - t) * (1 - t) +
                   p2 * 2 * t * (1 - t)   +
                   p3 * t * t;
        };
    } else if (order == 3) {
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
    } else {
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

Polygon::Polygon(std::initializer_list<Point> points) {
    vertexes.reserve(points.size());
    std::copy(points.begin(), points.end(), vertexes.begin());
}

Polygon &Polygon::operator=(const Polygon &other) {
    if (this == &other) {
        return *this;
    }

    vertexes = other.vertexes;

    return *this;
}

Polygon &Polygon::operator=(Polygon &&other) {
    if (this == &other) {
        return *this;
    }

    vertexes = std::move(other.vertexes);

    return *this;
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
    constexpr const float pi = static_cast<float>(std::numbers::pi);

    for (float t = 0.5 * pi; t <= 2.5 * pi; t += 2 * pi / poly_steps) {
        Polygon::AddPoint({ center.x + a * sinf(t), center.y + b * cosf(t) });
    }
}

Ellipse &Ellipse::operator=(const Ellipse &other) {
    if (this == &other) {
        return *this;
    }

    Polygon::operator=(other);

    center = other.center;
    a = other.a;
    b = other.b;

    return *this;
}

Ellipse &Ellipse::operator=(Ellipse &&other) {
    if (this == &other) {
        return *this;
    }

    Polygon::operator=(std::move(other));

    center = other.center;
    a = other.a;
    b = other.b;

    return *this;
}

void Ellipse::Draw(Color color_line, Color color_point) const {
    DrawCircleV(center, std::min(a, b) / 15, GREEN);
    DrawCircleV(GetCenter(), std::min(a, b) / 15, color_point);
    Polygon::Draw(color_line);
}