#include "Utilities/Color.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>

namespace Raytracer {

Color::Color(double red, double green, double blue) :
    r(red),
    g(green),
    b(blue) {
}

double Color::R() const {
    return r;
}

double Color::G() const {
    return g;
}

double Color::B() const {
    return b;
}

double Color::Luminance() const {
    return 0.299 * r + 0.587 * g + 0.114 * b;
}

std::array<double, 3> Color::GetRGB() const {
    return {r, g, b};
}

std::array<int, 3> Color::GetRGB255() const {
    return {
        static_cast<int>(std::lround(r * 255.0)),
        static_cast<int>(std::lround(g * 255.0)),
        static_cast<int>(std::lround(b * 255.0))};
}

std::string Color::GetHexColor() const {
    auto rgb = GetRGB255();
    std::stringstream ss;
    ss << std::hex << std::uppercase << std::setfill('0');
    ss << "#";
    for (int i = 0; i < rgb.size(); ++i) {
        ss << std::setw(2) << rgb[i];
    }
    return ss.str();
}

void Color::PrintTerminalPixel() const {
    auto rgb = GetRGB255();
    std::cout << "\033[38;2;"
              << rgb[0] << ";" << rgb[1] << ";" << rgb[2] << "m"
              << "█"
              << "\033[0m";
}

}  // namespace Raytracer
