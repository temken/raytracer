#pragma once

#include "Color.hpp"

#include <cstddef>
#include <string>
#include <vector>

namespace Raytracer {

class Image {
public:
    Image() = default;
    Image(std::size_t width, std::size_t height, const Color& backgroundColor = Color(0, 0, 0));

    std::size_t GetWidth() const;
    std::size_t GetHeight() const;

    void SetPixel(std::size_t x, std::size_t y, const Color& color);
    Color GetPixel(std::size_t x, std::size_t y) const;

    bool Save(const std::string& filepath) const;

    void Clear(const Color& color = Color(0, 0, 0));

private:
    std::size_t mWidth = 100;
    std::size_t mHeight = 100;
    std::vector<Color> mPixels;

    void CheckBounds(std::size_t x, std::size_t y) const;
};

}  // namespace Raytracer
