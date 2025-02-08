#include <raylib.h>
#include <raymath.h>

#include <vector>
#include <numeric>
#include <cmath>
#include <cassert>

#include <iostream>

#include "gui.h"

#define WIDTH  1600
#define HEIGHT 900

using Point = Vector2;

Point RotatePoint(Point point, float angle, Point center = Vector2Zeros) {
    point -= center;

    float x = point.x;
    float y = point.y;

    point.x = x * cosf(angle) - y * sinf(angle);
    point.y = x * sinf(angle) + y * cosf(angle);

    return point + center;
}

inline float Distance(Point a, Point b) {
    return std::hypotf(a.x - b.x, a.y - b.y);
}

inline Point Lerp(Point a, Point b, float t) {
    return a + (b - a) * t;
}

struct Polygon {
    std::vector<Point> vertexes;

    struct Interpolator {
        const Polygon &polygon;

        // index of starting point
        size_t edge_idx = 0;
        float  t = 0;
        float speed;

        Interpolator(const Polygon &polygon, float speed=0) : polygon(polygon), speed(speed) {}

        Point Step() {
            float step_len = speed * GetFrameTime();
            
            Point a = polygon.GetPoint(edge_idx);
            Point b = polygon.GetPoint((edge_idx + 1) % polygon.vertexes.size());

            Point current_pos = Lerp(a, b, t);

            while (step_len > 0) {
                float d = Distance(current_pos, b);

                if (d > step_len) {
                    t += step_len / Distance(a, b);
                    break;
                }

                step_len -= d;
                t = 0;
                a = b;
                b = polygon.GetPoint((edge_idx + 2) % polygon.vertexes.size());

                edge_idx = (edge_idx + 1) % polygon.vertexes.size();
            }

            return Lerp(a, b, t);
        }
    };

    Interpolator interpolator;

    Polygon() : interpolator(*this) {}

    void AddPoint(Point point) {
        vertexes.push_back(point);
    }

    Point GetPoint(size_t idx) const {
        return idx < vertexes.size() ? vertexes[idx] : Vector2Zeros;
    }

    void Draw(Color color) {
        if (vertexes.size() == 0) {
            return;
        }

        Point a = vertexes.front();
        for (size_t i = 1; i < vertexes.size(); i++) {
            DrawLineV(a, vertexes[i], color);
            a = vertexes[i];
        }
        DrawLineV(a, vertexes.front(), color);
    }

    Point GetCenter() {
        Point center = std::accumulate(vertexes.begin(), vertexes.end(), Vector2Zeros);
        center.x /= vertexes.size();
        center.y /= vertexes.size();
        return center;
    }

    void Shift(Point shift) {
        for (Point &point : vertexes) {
            point += shift;
        }
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
};

struct Circle {
    Point center;
    float r;

    Circle(Point center, float r) : center(center), r(r) {}

    void Draw(Color color) {
        for (float x = -r; x <= r; x++) {
            for (float y = -r; y <= r; y++) {
                if (x * x + y * y <= r * r) {
                    DrawPixel(x + center.x, y + center.y, color);
                }
            }
        }
    }
};

struct Ellipse {
    Point center;
    float a, b;
    float angle = 0;

    Polygon polygon;

    Ellipse(Point center, float a, float b, int poly_steps=30) : center(center), a(a), b(b) {
        for (float t = M_PI / 2; t <= 2.5 * M_PI; t += 2 * M_PI / poly_steps) {
            polygon.AddPoint({center.x + a * sinf(t), center.y + b * cosf(t)});
        }
    }

    void SetMovingSpeed(float speed) {
        polygon.interpolator.speed = speed;
    }

    void Draw(Color line_color, Color center_color) {
        Circle(center, std::min(a, b)/15).Draw(center_color);
        polygon.Draw(line_color);
    }

    void Rotate(float angle) {
        polygon.Rotate(angle);
        this->angle = this->angle + angle;
        while (this->angle >= 2 * M_PI)  this->angle -= 2 * M_PI;
        while (this->angle <= -2 * M_PI) this->angle += 2 * M_PI;
    }

    void SetCenter(Point point) {
        polygon.Shift(point - center);
        center = point;
    }
};

inline Point GetScreenCenter() {
    return Point { GetScreenWidth() / 2.f, GetScreenHeight() / 2.f };
}

int main() {
    InitWindow(WIDTH, HEIGHT, "Ellipses");
    SetTargetFPS(60);

    float vel  = 50;
    float avel = 2 * M_PI / 3;

    Ellipse e1(GetScreenCenter(), 200., 100.);
    e1.SetMovingSpeed(vel);

    Ellipse e2(e1.polygon.GetPoint(0), 100., 50.);
    e2.SetMovingSpeed(vel * 2);

    Ellipse e3(e2.polygon.GetPoint(0), 50., 25);

    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(GetColor(0x181818ff));

            e1.Draw(YELLOW, RED);
            e2.Draw(YELLOW, RED);
            e3.Draw(YELLOW, RED);

        EndDrawing();

        e1.Rotate(avel * GetFrameTime());
        e2.Rotate(avel * 2 * GetFrameTime());
        e3.Rotate(avel * 3 * GetFrameTime());

        e2.SetCenter(e1.polygon.interpolator.Step());
        e3.SetCenter(e2.polygon.interpolator.Step());
    }

    CloseWindow();

    return 0;
}