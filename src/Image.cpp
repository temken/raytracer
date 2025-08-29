#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "Image.hpp"
#include "version.hpp"

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <stdexcept>

namespace Raytracer {

Image::Image(std::size_t width, std::size_t height, const Color& backgroundColor) :
    mWidth(width),
    mHeight(height),
    mPixels() {
    if (mWidth == 0 || mHeight == 0) {
        throw std::invalid_argument("Image dimensions must be positive (non-zero).");
    }
    mPixels.assign(mWidth * mHeight, backgroundColor);
}

std::size_t Image::GetWidth() const {
    return mWidth;
}
std::size_t Image::GetHeight() const {
    return mHeight;
}

void Image::SetPixel(std::size_t x, std::size_t y, const Color& color) {
    CheckBounds(x, y);
    mPixels[y * mWidth + x] = color;
}

Color Image::GetPixel(std::size_t x, std::size_t y) const {
    CheckBounds(x, y);
    return mPixels[y * mWidth + x];
}

void Image::Clear(const Color& color) {
    std::fill(mPixels.begin(), mPixels.end(), color);
}

bool Image::Save(const std::string& filename) const {
    if (mWidth == 0 || mHeight == 0 || mPixels.empty()) {
        return false;
    }

    std::vector<std::uint8_t> rgba;
    rgba.resize(mWidth * mHeight * 4);

    std::size_t idx = 0;
    for (const auto& c : mPixels) {
        auto rgba255 = c.GetRGBA255();
        rgba[idx + 0] = static_cast<std::uint8_t>(rgba255[0]);
        rgba[idx + 1] = static_cast<std::uint8_t>(rgba255[1]);
        rgba[idx + 2] = static_cast<std::uint8_t>(rgba255[2]);
        rgba[idx + 3] = static_cast<std::uint8_t>(rgba255[3]);
        idx += 4;
    }

    if (mWidth > static_cast<std::size_t>(std::numeric_limits<int>::max()) ||
        mHeight > static_cast<std::size_t>(std::numeric_limits<int>::max())) {
        throw std::overflow_error("Image dimensions exceed stb_image_write int limits.");
    }

    const int w = static_cast<int>(mWidth);
    const int h = static_cast<int>(mHeight);
    const int comp = 4;
    const int stride = w * comp;

    stbi_flip_vertically_on_write(0);

    std::filesystem::create_directories(std::string(PROJECT_DIR) + "images");
    std::string fullPath = std::string(PROJECT_DIR) + "images/" + filename;
    int ok = stbi_write_png(fullPath.c_str(), w, h, comp, rgba.data(), stride);
    return ok != 0;
}

void Image::CheckBounds(std::size_t x, std::size_t y) const {
    if (x >= mWidth || y >= mHeight) {
        throw std::out_of_range("Pixel coordinate out of bounds");
    }
}

}  // namespace Raytracer
