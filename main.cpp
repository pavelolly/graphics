#include <raylib.h>
#include <raymath.h>

#define RAYGUI_IMPLEMENTATION
#define RAYGUI_VALUEBOX_MAX_CHARS 10
#include <raygui.h>

#include <vector>
#include <initializer_list>
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

void DrawPoint(Point point, Color color, float radius=0) {
    for (float x = -radius; x <= radius; x++) {
        for (float y = -radius; y <= radius; y++) {
            if (x * x + y * y <= radius * radius) {
                DrawPixel(x + point.x, y + point.y, color);
            }
        }
    }
}

class Polygon {
public:
    Polygon() {}

    Polygon(std::initializer_list<Point> points) {
        for (Point point : points) {
            vertexes.push_back(point);
        }
    }

    void AddPoint(Point point) {
        vertexes.push_back(point);
    }

    Point GetPoint(size_t idx) const {
        return vertexes.at(idx);
    }

    size_t NumPoints() const {
        return vertexes.size();
    }

    void Shift(Point shift) {
        for (Point &point : vertexes) {
            point += shift;
        }
    }

    void Rotate(float angle) {
        Point center = RealCenter();
        for (Point &point : vertexes) {
            point = RotatePoint(point, angle, center);
        }

        rotation = fmodf(rotation + angle, 2 * M_PI);
    }

    void ResetRotation() {
        Rotate(-rotation);
        rotation = 0;
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

    // Shape specific

    void Draw(Color color) const {
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
protected:
    std::vector<Point> vertexes;
    float rotation = 0;
};

struct Ellipse : public Polygon {
    Point center;
    float a, b;

    Ellipse() = delete;
    void AddPoint(Point point) = delete;

    Ellipse(Point center, float a, float b, int poly_steps=30) : Polygon(), center(center), a(a), b(b) {
        for (float t = 0.5 * M_PI; t <= 2.5 * M_PI; t += 2 * M_PI / poly_steps) {
            Polygon::AddPoint({center.x + a * sinf(t), center.y + b * cosf(t)});
        }
    }

    void Draw(Color line_color) const {
        DrawPoint(center, GREEN, std::min(a, b)/15);
        DrawPoint(RealCenter(), RED, std::min(a, b)/15);
        Polygon::Draw(line_color);
    }

    void SetCenter(Point new_center) {
        Shift(new_center - center);
        center = new_center;
    }
};

class Interpolator {
public:
    Interpolator(const Polygon &polygon) : polygon(&polygon) {}

    bool Valid() {
        return polygon;
    }

    Point Step(float speed) {
        float len = polygon->Length();
        int npoints = polygon->NumPoints();

        if (npoints == 0) {
            return Vector2Zeros;
        }
        if (len == 0) {
            return polygon->GetPoint(0);
        }

        float step_len = fmodf(speed * len, len);
            
        Point a = polygon->GetPoint(edge_idx);
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
private:
    const Polygon *polygon;

    // index of starting point
    size_t edge_idx = 0;
    float  t = 0;
};

struct GuiInputBox {
    Rectangle box;
    float*    value;
    bool      editmode = false;
    char      text_buffer[RAYGUI_VALUEBOX_MAX_CHARS + 1] = {0};

    GuiInputBox(Rectangle box, float *value) : box(box), value(value) {}

    void ResetValue() {
        memset(text_buffer, 0, sizeof(text_buffer));
        *value = 0;
    }

    void Draw() {
        if (GuiValueBoxFloat(box, nullptr, text_buffer, value, editmode)) {
            editmode = !editmode;
        }
    }
};

struct GuiInputBoxPanel {
    Rectangle box;
    std::vector<GuiInputBox> input_boxes;

    GuiInputBoxPanel(Rectangle box) : box(box) {}

    void AddInputBox(Rectangle box, float *value) {
        Rectangle actual { this->box.x + box.x, this->box.y + box.y, box.width, box.height };
        input_boxes.emplace_back(actual, value);
    }

    void Draw() {
        DrawRectangleLinesEx(box, 1, GRAY);
        DrawText("1", box.x + 100, box.y + 15, 15, GRAY);
        DrawText("2", box.x + 190, box.y + 15, 15, GRAY);
        DrawText("3", box.x + 280, box.y + 15, 15, GRAY);

        DrawText("Vel: ",  box.x + 20, box.y + 15 + 35, 15, GRAY);
        DrawText("Avel: ", box.x + 20, box.y + 15 + 75, 15, GRAY);

        for (GuiInputBox &ib : input_boxes) {
            ib.Draw();
        }
    }

    void ResetValues() {
        for (GuiInputBox &ib : input_boxes) {
            ib.ResetValue();
        }
    }
};

struct EllipseAnimation {
    Ellipse ellipse;
    Interpolator interpolator;
    
    float moving_speed   = 0;
    float rotation_speed = 0;

    struct {
        Point center;
        float a;
        float b;
    } _init;
    
    EllipseAnimation(Point center, float a, float b, const Polygon &trajectory) :
        ellipse(center, a, b, 20), _init{center, a, b}, interpolator(trajectory) {}


    void Update() {
        float dt = GetFrameTime();
        ellipse.Rotate(rotation_speed * dt);
        if (interpolator.Valid()) {
            ellipse.SetCenter(interpolator.Step(moving_speed * dt));
        }
    }

    void Reset() {
        ellipse.ResetRotation();
        ellipse.SetCenter(_init.center);
    }
};

int main() {
    InitWindow(WIDTH, HEIGHT, "Ellipses");
    SetTargetFPS(60);

    Point center = GetScreenCenter();
    center.x *= 0.75;

    Polygon e1_trajectory { center };

    EllipseAnimation e1(center,                 200.f, 100.f, e1_trajectory);
    EllipseAnimation e2(e1.ellipse.GetPoint(0), 100.f, 50.f,  e1.ellipse);
    EllipseAnimation e3(e2.ellipse.GetPoint(0), 50.f,  25.f,  e2.ellipse);

    GuiInputBoxPanel panel( { GetScreenWidth() - 380.f, 50, 350, GetScreenHeight() - 100.f } );
    panel.AddInputBox(Rectangle{ 65, 85, 80, 30 },  &e1.rotation_speed);

    panel.AddInputBox(Rectangle{ 155, 45, 80, 30 }, &e2.moving_speed);
    panel.AddInputBox(Rectangle{ 155, 85, 80, 30 }, &e2.rotation_speed);

    panel.AddInputBox(Rectangle{ 245, 45, 80, 30 }, &e3.moving_speed);
    panel.AddInputBox(Rectangle{ 245, 85, 80, 30 }, &e3.rotation_speed);

    bool pause = false;

    while (!WindowShouldClose()) {
        
        if (IsKeyPressed(KEY_SPACE)) {
            pause = !pause;
        }
        if (IsKeyDown(KEY_LEFT_CONTROL)) {
            if (IsKeyPressed(KEY_R)) {
                panel.ResetValues();

                e1.Reset();
                e2.Reset();
                e3.Reset();
            }
        } else {
            if (IsKeyPressed(KEY_R)) {
                e1.Reset();
                e2.Reset();
                e3.Reset();
            }
        }

        BeginDrawing();
            ClearBackground(GetColor(0x181818ff));

            panel.Draw();

            if (pause) {
                DrawText("paused", 20, 20, 15, GRAY);
            }

            e1.ellipse.Draw(YELLOW);
            e2.ellipse.Draw(YELLOW);
            e3.ellipse.Draw(YELLOW);

        EndDrawing();

        if (!pause) {
            e1.Update();
            e2.Update();
            e3.Update();
        }
    }

    CloseWindow();

    return 0;
}