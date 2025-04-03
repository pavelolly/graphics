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

Point Project(Point p, Point a, Point b) {
    Point direction = Vector2Normalize(b - a);
    
    float pa = Distance(a, p);
    if (FloatEquals(pa, 0)) {
        return a;
    }
    float ab = Distance(a, b);
    if (FloatEquals(ab, 0)) {
        return a;
    }
    float pb = Distance(p, b);
    if (FloatEquals(pb, 0)) {
        return b;
    }

    float len = (pa * pa + ab * ab - pb * pb) / (2 * ab);

    return a + direction * len;
}

std::optional<Point> Intersect(Point a, Point b, Point x, Point y) {
    float div = (y.y - x.y)*(b.x - a.x) - (y.x - x.x)*(b.y - a.y);

    if (FloatEquals(div, 0)) {
        return std::nullopt;
    }

    float xi = ( (x.x - y.x) * (a.x * b.y - a.y * b.x) - (a.x - b.x) * (x.x * y.y - x.y * y.x) ) / div;
    float yi = ( (x.y - y.y) * (a.x * b.y - a.y * b.x) - (a.y - b.y) * (x.x * y.y - x.y * y.x) ) / div;
    
    Point i { xi, yi };

    static auto LiesBetween = [](Point p, Point a, Point b) -> bool {
        Point lu { std::min(a.x, b.x), std::min(a.y, b.y) };
        Point rd { std::max(a.x, b.x), std::max(a.y, b.y) };

        return lu.x <= p.x && p.x <= rd.x && lu.y <= p.y && p.y <= rd.y;
    };

    if (LiesBetween(i, a, b) && LiesBetween(i, x, y)) {
        return i;
    }

    return std::nullopt;
}

void DrawLineDotted(Point start, Point end, float segment_len, float thick, Color color) {
    const int nsegments = (int) std::ceil(Distance(start, end) / (2 * segment_len));
    
    Point delta = end - start;
    Point step = Vector2Normalize(delta) * segment_len;

    bool draw_end_cap = true;
    
    for (int i = 0; i < nsegments; ++i) {
        Point segment_start = start + step * (float) (2 * i);
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
    float d  = (b.x - a.x) * (c.y - a.y) - (c.x - a.x) * (b.y - a.y);
    float k1 = ((b.x - p.x) * (c.y - p.y) - (c.x - p.x) * (b.y - p.y)) / d;
    float k2 = ((p.x - a.x) * (c.y - a.y) - (c.x - a.x) * (p.y - a.y)) / d;
    float k3 = 1 - k1 - k2;

    return k1 > 0 && k2 > 0 && k3 > 0;
}

bool IsInsideTriangle2(Point p, Point a, Point b, Point c) {
    bool side1 = std::signbit((p.x - a.x) * (b.y - a.y) - (p.y - a.y) * (b.x - a.x));
    bool side2 = std::signbit((p.x - b.x) * (c.y - b.y) - (p.y - b.y) * (c.x - b.x));
    bool side3 = std::signbit((p.x - c.x) * (a.y - c.y) - (p.y - c.y) * (a.x - c.x));

    return side1 == side2 && side2 == side3;
}

bool IsInsideTriangle3(Point p, Point a, Point b, Point c) {
    Point center = (a + b + c) / 3;

    return !Intersect(p, center, a, b) && !Intersect(p, center, b, c) && !Intersect(p, center, c, a);
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

Point Polygon::GetCenter() const {
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
}

void Polygon::DrawCenter(Color color) const {
    DrawCircleV(GetCenter(), 7, color);
}

Polygon Polygon::Ellipse(Point center, float a, float b, int poly_steps) {
    static constexpr float pi = std::numbers::pi_v<float>;

    Polygon p;

    for (int i = 0; i <= poly_steps; ++i) {
        float t = pi / 2 + 2 * pi * ((float) i / poly_steps);
        p.AddPoint({ center.x + a * sinf(t), center.y + b * cosf(t) });
    }

    return p;
}