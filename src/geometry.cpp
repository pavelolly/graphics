#include "geometry.hpp"

#include <numeric>
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

void DrawPoint(Point point, Color color, float radius) {
    for (float x = -radius; x <= radius; x++) {
        for (float y = -radius; y <= radius; y++) {
            if (x * x + y * y <= radius * radius) {
                DrawPixel(x + point.x, y + point.y, color);
            }
        }
    }
}

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
        DrawPoint(vertexes[i], color_point, 5);
        a = vertexes[i];
    }
    DrawLineV(a, vertexes.front(), color_line);
    DrawPoint(vertexes.front(), color_point, 5);

    DrawPoint(RealCenter(), color_point, 7);
}

Ellipse::Ellipse(Point center, float a, float b, int poly_steps) : center(center), a(a), b(b) {
    using std::numbers::pi;

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
    DrawPoint(center, GREEN, std::min(a, b) / 15);
    DrawPoint(GetCenter(), color_point, std::min(a, b) / 15);
    Polygon::Draw(color_line);
}