#include <raylib.h>
#include <raymath.h>

#define RAYGUI_IMPLEMENTATION
#define RAYGUI_VALUEBOX_MAX_CHARS 10

#include <raygui.h>

#include <vector>
#include <numeric>
#include <cmath>
#include <cassert>

#include <iostream>

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

        Interpolator(const Polygon &polygon) : polygon(polygon) {}

        Point Step(float speed) {
            float len = polygon.Length();
            float step_len = fmodf(speed * GetFrameTime() * len, len);
            int npoints = polygon.vertexes.size();
            
            Point a = polygon.GetPoint(edge_idx);
            Point b = polygon.GetPoint((edge_idx + 1) % npoints);

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
                b = polygon.GetPoint((edge_idx + 2) % npoints);

                edge_idx = (edge_idx + 1) % npoints;
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
    float rotation = 0;

    Polygon polygon;

    Ellipse(Point center, float a, float b, int poly_steps=30) : center(center), a(a), b(b) {
        for (float t = 0.5 * M_PI; t <= 2.5 * M_PI; t += 2 * M_PI / poly_steps) {
            polygon.AddPoint({center.x + a * sinf(t), center.y + b * cosf(t)});
        }
    }

    void Draw(Color line_color) {
        Circle(center, std::min(a, b)/15).Draw(GREEN);
        Circle(polygon.GetCenter(), std::min(a, b)/15).Draw(RED);
        polygon.Draw(line_color);
    }

    void Rotate(float angle) {
        angle *= GetFrameTime();

        polygon.Rotate(angle);

        rotation += angle;
        while (rotation >= 2 * M_PI)  rotation -= 2 * M_PI;
        while (rotation <= -2 * M_PI) rotation += 2 * M_PI;
    }

    void ResetRotation() {
        polygon.Rotate(-rotation);
        rotation = 0;
    }

    void SetCenter(Point point) {
        polygon.Shift(point - center);
        center = point;
    }
};

inline Point GetScreenCenter() {
    return Point { GetScreenWidth() / 2.f, GetScreenHeight() / 2.f };
}

struct GuiVar {
    char buffer[RAYGUI_VALUEBOX_MAX_CHARS + 1] = "0";
    float value;
    bool editmode = false;

    GuiVar(float value) : value(value) {}
    operator float() const {
        return value;
    }

    void Reset() {
        memset(buffer, 0, sizeof(buffer));
        buffer[0] = '0';

        value = 0;
    }
};

int main() {
    InitWindow(WIDTH, HEIGHT, "Ellipses");
    SetTargetFPS(60);

    Point center = GetScreenCenter();
    center.x *= 0.75;

    Ellipse e1(center,                 200., 100., 20);
    GuiVar avel1 = 0;

    Ellipse e2(e1.polygon.GetPoint(0), 100., 50.,  20);
    GuiVar vel2  = 0;
    GuiVar avel2 = 0;

    Ellipse e3(e2.polygon.GetPoint(0), 50.,  25,   20);
    GuiVar vel3  = 0;
    GuiVar avel3 = 0;

    Rectangle button_panel { GetScreenWidth() - 380.f, 50, 350, GetScreenHeight() - 100.f };
    Rectangle button_avel1 { button_panel.x + 20 + 45, button_panel.y + 10 + 75, 80, 30 };

    Rectangle button_vel2  { button_panel.x + 20 + 135, button_panel.y + 10 + 35, 80, 30 };
    Rectangle button_avel2 { button_panel.x + 20 + 135, button_panel.y + 10 + 75, 80, 30 };

    Rectangle button_vel3  { button_panel.x + 20 + 225, button_panel.y + 10 + 35, 80, 30 };
    Rectangle button_avel3 { button_panel.x + 20 + 225, button_panel.y + 10 + 75, 80, 30 };

    bool pause = false;

    while (!WindowShouldClose()) {
        
        if (IsKeyPressed(KEY_SPACE)) {
            pause = !pause;
        }
        if (IsKeyDown(KEY_LEFT_CONTROL)) {
            if (IsKeyPressed(KEY_R)) {
                avel1.Reset();

                vel2.Reset();
                avel2.Reset();

                vel3.Reset();
                avel3.Reset();

                e1.ResetRotation();
                e2.ResetRotation();
                e3.ResetRotation();
                
                e2.SetCenter(e1.polygon.GetPoint(0));
                e3.SetCenter(e2.polygon.GetPoint(0));
            }
        } else {
            if (IsKeyPressed(KEY_R)) {
                e1.ResetRotation();
                e2.ResetRotation();
                e3.ResetRotation();
                
                e2.SetCenter(e1.polygon.GetPoint(0));
                e3.SetCenter(e2.polygon.GetPoint(0));
            }
        }

        BeginDrawing();
            ClearBackground(GetColor(0x181818ff));

            DrawRectangleLinesEx(button_panel, 1, GRAY);

            DrawText("1", button_panel.x + 100, button_panel.y + 15, 15, GRAY);
            DrawText("2", button_panel.x + 190, button_panel.y + 15, 15, GRAY);
            DrawText("3", button_panel.x + 280, button_panel.y + 15, 15, GRAY);

            DrawText("Vel: ",  button_panel.x + 20, button_panel.y + 15 + 35, 15, GRAY);
            DrawText("Avel: ", button_panel.x + 20, button_panel.y + 15 + 75, 15, GRAY);

            if (GuiValueBoxFloat(button_avel1, nullptr, avel1.buffer, &avel1.value, avel1.editmode)) {
                avel1.editmode = !avel1.editmode;
            }

            if (GuiValueBoxFloat(button_vel2, nullptr, vel2.buffer, &vel2.value, vel2.editmode)) {
                vel2.editmode = !vel2.editmode;
            }
            if (GuiValueBoxFloat(button_avel2, nullptr, avel2.buffer, &avel2.value, avel2.editmode)) {
                avel2.editmode = !avel2.editmode;
            }

            if (GuiValueBoxFloat(button_vel3, nullptr, vel3.buffer, &vel3.value, vel3.editmode)) {
                vel3.editmode = !vel3.editmode;
            }
            if (GuiValueBoxFloat(button_avel3, nullptr, avel3.buffer, &avel3.value, avel3.editmode)) {
                avel3.editmode = !avel3.editmode;
            }

            if (pause) {
                DrawText("paused", 20, 20, 15, GRAY);
            }

            e1.Draw(YELLOW);
            e2.Draw(YELLOW);
            e3.Draw(YELLOW);

        EndDrawing();

        if (!pause) {
            e1.Rotate(avel1);
            e2.Rotate(avel2);
            e3.Rotate(avel3);
            
            e2.SetCenter(e1.polygon.interpolator.Step(vel2));
            e3.SetCenter(e2.polygon.interpolator.Step(vel3));
        }
    }

    CloseWindow();

    return 0;
}