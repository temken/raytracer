#pragma once

#include <array>
#include <iomanip>
#include <sstream>

namespace Raytracer {

class Color {
public:
    Color() = default;
    Color(double r, double g, double b, double a = 1.0);

    double R();
    double G();
    double B();
    double A();

    std::array<double, 4> GetRGBA() const;
    std::array<double, 3> GetRGB() const;
    std::array<int, 4> GetRGBA255() const;
    std::array<int, 3> GetRGB255() const;

    std::string GetHexColor() const;

    // Operators
    bool operator==(const Color& other) const {
        return (r == other.r) && (g == other.g) && (b == other.b) && (a == other.a);
    }

    Color operator+(const Color& other) const {
        return Color(r + other.r, g + other.g, b + other.b, a + other.a);
    }

    Color& operator+=(const Color& other) {
        r += other.r;
        g += other.g;
        b += other.b;
        a += other.a;
        return *this;
    }

    Color operator/(double scalar) const {
        return Color(r / scalar, g / scalar, b / scalar, a / scalar);
    }

    Color& operator/=(double scalar) {
        r /= scalar;
        g /= scalar;
        b /= scalar;
        a /= scalar;
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const Color& color) {
        os << "RGBA(" << color.r << ", " << color.g << ", " << color.b << ", " << color.a << ")";
        return os;
    }

private:
    double r = 0.0;
    double g = 0.0;
    double b = 0.0;
    double a = 1.0;
};

}  // namespace Raytracer
