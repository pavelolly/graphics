#pragma once

#include "geometry.hpp"


inline std::function<Point(float)> BezierFuncLinear(Point p1, Point p2) {
    return [p1, p2](float t) -> Point {
        return p1 * (1 - t) + p2 * t;
    };
}

inline std::function<Point(float)> BezierFuncQuadratic(Point p1, Point p2, Point p3) {
    return [p1, p2, p3](float t) -> Point {
        return p1 * (1 - t) * (1 - t) + p2 * 2 * t * (1 - t) + p3 * t * t;
    };
}

inline std::function<Point(float)> BezierFuncCubic(Point p1, Point p2, Point p3, Point p4) {
    return [p1, p2, p3, p4](float t) -> Point {
        return p1 * (1 - t) * (1 - t) * (1 - t) +
               p2 * 3 * (1 - t) * (1 - t) * t + p3 * 3 * (1 - t) * t * t +
               p4 * t * t * t;
    };
}

// get function of bezier curve of order control_points.size() - 1
std::function<Point(float)> BezierFunc(const RangeOf<Point> auto &control_points) {
    switch (auto size = std::ranges::size(control_points)) {
        case 0:
        case 1:
            return {};
        case 2:
            return BezierFuncLinear(control_points[0], control_points[1]);
        case 3:
            return BezierFuncQuadratic(control_points[0], control_points[1], control_points[2]);
        case 4:
            return BezierFuncCubic(control_points[0], control_points[1], control_points[2], control_points[3]);
        default: {
            std::vector<Point> coefs(size);
            auto order = static_cast<int>(size - 1);

            /*
                General formula: ```f(t) = sum i=[0..order] { control_point[i] * (order choose i) * (1 - t)^(order - i) * t^i }```
            */

            // calculate binomial coefs ```(order choose i) = order! / (i! * (order - i)!)```
            // (! - factorial)
            coefs[0] = { 1.f, 1.f };
            for (int i = 1; i <= order; ++i) {
                coefs[i] = coefs[i - 1] * static_cast<float>(order - i + 1) / static_cast<float>(i);
            }

            // multiply coefs by control points
            for (int i = 0; i < static_cast<int>(coefs.size()); ++i) {
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

struct BezierCurve {
    std::deque<Point> control_points;
    std::vector<Point> curve_points;
    int bezier_segments;
    
    BezierCurve(int bezier_segments=100);

    void DrawControlPoints(Color color_points, Color color_lines=BLANK) const;
    void DrawCurve(Color color) const;
    void Update();
};