#pragma once

#include <raylib.h>
#include <raymath.h>

#include <vector>
#include <numeric>
#include <cmath>

#include <cassert>

#include "log_macros.h"

using Point = Vector2;

inline Point RotatePoint(Point point, float angle, Point center=Vector2Zeros) {
    point -= center;

    float x = point.x;
    float y = point.y;

    point.x = x * cosf(angle) - y * sinf(angle);
    point.y = x * sinf(angle) + y * cosf(angle);

    return point + center;
}

inline float Distance(Point a, Point b) {
    float x = a.x - b.x;
    float y = a.y - b.y;
    return powf(x * x + y * y, 0.5f);
}

inline Point Lerp(Point a, Point b, float t) {
    return a + (b - a) * t;
}

inline Point GetScreenCenter() {
    return Point { GetScreenWidth() / 2.f, GetScreenHeight() / 2.f };
}

inline void DrawPoint(Point point, Color color, float radius=0) {
    for (float x = -radius; x <= radius; x++) {
        for (float y = -radius; y <= radius; y++) {
            if (x * x + y * y <= radius * radius) {
                DrawPixel(x + point.x, y + point.y, color);
            }
        }
    }
}

struct Polygon {
    std::vector<Point> vertexes;

    Polygon() {}

    Polygon(std::initializer_list<Point> points) {
        for (Point point : points) {
            vertexes.push_back(point);
        }
    }

    Polygon(const Polygon &other) = default;
    Polygon(Polygon &&other) : vertexes(std::move(other.vertexes)) {}

    Polygon &operator =(const Polygon& other) {
        if (this == &other) {
            return *this;
        }

        vertexes = other.vertexes;

        return *this;
    }

    Polygon &operator =(Polygon&& other) {
        if (this == &other) {
            return *this;
        }

        vertexes = std::move(other.vertexes);

        return *this;
    }

    virtual ~Polygon() = default;

    void AddPoint(Point point) {
        vertexes.push_back(point);
    }

    Point GetPoint(size_t idx) const {
        return vertexes.at(idx);
    }

    size_t NumPoints() const {
        return vertexes.size();
    }

    void Rotate(float angle) {
        Point center = GetCenter();
        for (Point &point : vertexes) {
            point = RotatePoint(point, angle, center);
        }
    }

    float Length() const {
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

    Point RealCenter() const {
        Point center = std::accumulate(vertexes.begin(), vertexes.end(), Vector2Zeros);
        center.x /= vertexes.size();
        center.y /= vertexes.size();
        return center;
    }

    void ShiftPoint(size_t idx, Point shift) {
        vertexes[idx] += shift;
    }

    // Shape specific

    virtual Point GetCenter() const {
        return RealCenter();
    }

    virtual void SetCenter(Point new_center) {
        Point center = GetCenter();
        Shift(new_center - center);
    }

    virtual void Draw(Color color_line, Color color_point=BLANK) const {
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

    virtual void Shift(Point shift) {
        for (Point &point : vertexes) {
            point += shift;
        }
    }
};

struct Ellipse : Polygon {
    Point center;
    float a, b;

    Ellipse()                                = delete;
    void AddPoint(Point point)               = delete;
    void ShiftPoint(size_t idx, Point shift) = delete;

    Ellipse(Point center, float a, float b, int poly_steps=40) : Polygon(), center(center), a(a), b(b) {
        for (float t = 0.5 * std::numbers::pi; t <= 2.5 * std::numbers::pi; t += 2 * std::numbers::pi / poly_steps) {
            Polygon::AddPoint({center.x + a * sinf(t), center.y + b * cosf(t)});
        }
    }

    void Draw(Color color_line, Color color_point=RED) const override {
        DrawPoint(center, GREEN, std::min(a, b)/15);
        DrawPoint(GetCenter(), color_point, std::min(a, b)/15);
        Polygon::Draw(color_line);
    }

    void SetCenter(Point new_center) override {
        Polygon::Shift(new_center - center);
        center = new_center;
    }

    Point GetCenter() const override {
        return center;
    }

    void Shift(Point shift) override {
        Polygon::Shift(shift);
        center += shift;
    }
};

struct Interpolator {
    const Polygon *polygon = nullptr;
    Point default_point    = Vector2Zeros;
    size_t edge_idx        = 0;
    float t                = 0;

    Interpolator(Point point=Vector2Zeros) : default_point(point) {};
    Interpolator(const Polygon &polygon) : polygon(&polygon), default_point(polygon.GetPoint(0)) {}

    void Reset() {
        edge_idx = 0;
        t = 0;
    }

    Point Step(float speed) {
        if (!polygon) {
            return default_point;
        }

        float len   = polygon->Length();
        int npoints = polygon->NumPoints();

        if (npoints == 0) {
            return Vector2Zeros;
        }
        if (len == 0) {
            return polygon->GetPoint(0);
        }

        float step_len = fmodf(speed, len);
            
        Point a = polygon->GetPoint(edge_idx % npoints);
        Point b = polygon->GetPoint((edge_idx + 1) % npoints);

        Point current_pos = Lerp(a, b, t);

        while (step_len > 0) {
            float d = Distance(current_pos, b);

            if (d > step_len) {
                t += step_len / Distance(a, b);
                break;
            }

            // switch to next edge
            step_len -= d;
            t = 0;

            current_pos = b;
            a = b;
            b = polygon->GetPoint((edge_idx + 2) % npoints);

            edge_idx = (edge_idx + 1) % npoints;
        }

        return Lerp(a, b, t);
    }
};