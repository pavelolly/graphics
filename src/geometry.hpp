#pragma once

#include <raylib.h>
#include <raymath.h>

#include <vector>
#include <functional>
#include <memory>

#include <cassert>

using Point = Vector2;

inline Point GetScreenCenter() {
    return Point { GetScreenWidth() / 2.f, GetScreenHeight() / 2.f };
}

inline Point GetRandomPoint(int min_x, int max_x, int min_y, int max_y) {
    return Point { static_cast<float>(GetRandomValue(min_x, max_x)),
                   static_cast<float>(GetRandomValue(min_y, max_y)) };
};

Point RotatePoint(Point point, float angle, Point center=Vector2Zeros);
float Distance(Point a, Point b);
Point Lerp(Point a, Point b, float t);

// check if point is on the same side to each triangle edge
bool IsInsideTriangle(Point p, Point a, Point b, Point c);
// calaculate barycentric coordinates
bool IsInsideTriangle2(Point p, Point a, Point b, Point c);

std::function<Point(float)> BezierFunc(const std::vector<Point> &control_points);

void DrawLineDotted(Point start, Point end, float thick, Color color);

struct Polygon {
    std::vector<Point> vertexes;

    Polygon() = default;
    virtual ~Polygon() = default;

    Polygon(std::initializer_list<Point> points);
    Polygon(const Polygon &other) : vertexes(other.vertexes) {}
    Polygon(Polygon &&other) : vertexes(std::move(other.vertexes)) {}

    Polygon &operator =(const Polygon& other);
    Polygon &operator =(Polygon&& other);

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
    Point RealCenter() const;

    // Shape specific

    virtual std::unique_ptr<Polygon> Clone() const {
        return std::make_unique<Polygon>(*this);
    }

    virtual Polygon *CloneInto(Polygon *dest) const {
        assert(typeid(*this) == typeid(*dest) && "trying to CloneInto different type");

        dest->~Polygon();
        return new(dest) Polygon(*this);
    }

    virtual Point GetCenter() const {
        return RealCenter();
    }

    virtual void SetCenter(Point new_center) {
        Point center = GetCenter();
        Shift(new_center - center);
    }

    virtual void Shift(Point shift) {
        for (Point &point : vertexes) {
            point += shift;
        }
    }

    virtual void Draw(Color color_line, Color color_point=BLANK) const;
};

struct Ellipse : Polygon {
    Point center;
    float a, b;

    void AddPoint(Point point) = delete;

    Ellipse(Point center, float a, float b, int poly_steps=40);
    Ellipse(const Ellipse &other) = default;
    Ellipse(Ellipse &&other) = default;

    Ellipse &operator =(const Ellipse &other);
    Ellipse &operator =(Ellipse &&other);

    std::unique_ptr<Polygon> Clone() const override {
        return std::make_unique<Ellipse>(*this);
    }

    Polygon *CloneInto(Polygon *dest) const override {
        assert(typeid(*this) == typeid(*dest) && "trying to CloneInto different type");

        dest->~Polygon();
        return new(dest) Ellipse(*this);
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

    void Draw(Color color_line, Color color_point=RED) const override;
};