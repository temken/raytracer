#pragma once

#include <array>
#include <iomanip>
#include <sstream>

namespace Raytracer {

class Color {
public:
    Color() = default;
    Color(double r, double g, double b);

    double R() const;
    double G() const;
    double B() const;

    std::array<double, 3> GetRGB() const;
    std::array<int, 3> GetRGB255() const;

    std::string GetHexColor() const;

    // Operators
    bool operator==(const Color& other) const {
        return (r == other.r) && (g == other.g) && (b == other.b);
    }

    Color operator*(double scalar) const {
        return Color(r * scalar, g * scalar, b * scalar);
    }

    Color operator*(const Color& other) const {
        return Color(r * other.r, g * other.g, b * other.b);
    }

    Color operator+(const Color& other) const {
        return Color((r + other.r), (g + other.g), (b + other.b));
    }

    Color& operator+=(const Color& other) {
        r = r + other.r;
        g = g + other.g;
        b = b + other.b;
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const Color& color) {
        os << "RGB(" << color.r << ", " << color.g << ", " << color.b << ")";
        return os;
    }

private:
    double r = 0.0;
    double g = 0.0;
    double b = 0.0;
};

// Predefined colors
const Color BLACK = Color(0, 0, 0);
const Color WHITE = Color(1, 1, 1);
const Color GRAY = Color(0.5, 0.5, 0.5);
const Color RED = Color(255.0 / 255, 62.0 / 255, 65.0 / 255);
const Color ORANGE = Color(255.0 / 255, 138.0 / 255, 67.0 / 255);
const Color YELLOW = Color(255.0 / 255, 235.0 / 255, 127.0 / 255);
const Color CYAN = Color(0.0 / 255, 201.0 / 255, 167.0 / 255);
const Color BLUE = Color(0.0 / 255, 126.0 / 255, 167.0 / 255);
const Color GREEN = Color(0.0 / 255, 179.0 / 255, 111.0 / 255);

}  // namespace Raytracer
