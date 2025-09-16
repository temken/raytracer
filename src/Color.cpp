#include "Utilities/Color.hpp"

#include <algorithm>
#include <cmath>

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

}  // namespace Raytracer
