#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

#include "Rendering/Image.hpp"
#include "Utilities/Configuration.hpp"
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

inline float srgb_to_linear(float x) {
    if (x <= 0.04045f) {
        return x / 12.92f;
    }
    return std::pow((x + 0.055f) / 1.055f, 2.4f);
}

Image::Image(std::string filepath, bool assume_srgb) {
    int w, h, n;
    unsigned char* data = stbi_load(filepath.c_str(), &w, &h, &n, 3);  // force 3 channels (RGB)
    if (!data) {
        throw std::runtime_error("Failed to load image: " + filepath);
    }

    mWidth = static_cast<std::size_t>(w);
    mHeight = static_cast<std::size_t>(h);
    mPixels.resize(mWidth * mHeight);

    for (std::size_t y = 0; y < mHeight; ++y) {
        for (std::size_t x = 0; x < mWidth; ++x) {
            size_t idx = (y * mWidth + x) * 3;
            float r = data[idx + 0] / 255.0f;
            float g = data[idx + 1] / 255.0f;
            float b = data[idx + 2] / 255.0f;
            if (assume_srgb) {
                r = srgb_to_linear(r);
                g = srgb_to_linear(g);
                b = srgb_to_linear(b);
            }
            mPixels[y * mWidth + x] = Color(r, g, b);
        }
    }

    stbi_image_free(data);
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

double Image::CalculateBlackPixelRatio() const {
    if (mPixels.empty()) {
        return 0.0;
    }
    size_t blackPixelCount = CountBlackPixels();
    return static_cast<double>(blackPixelCount) / static_cast<double>(mPixels.size());
}

size_t Image::CountBlackPixels() const {
    return std::count_if(mPixels.begin(), mPixels.end(), [](const Color& c) {
        return c == BLACK;
    });
}

bool Image::Save(bool openFile, std::string filepath) const {
    if (mWidth == 0 || mHeight == 0 || mPixels.empty()) {
        return false;
    }

    if (filepath.empty()) {
        std::string directory = Configuration::GetInstance().GetOutputDirectory();
        // Create /images/ folder if it does not exist
        std::filesystem::create_directories(directory + "/images/");
        filepath = directory + "/images/image_" + std::to_string(std::time(nullptr)) + ".png";
    }

    std::vector<std::uint8_t> rgba;
    rgba.resize(mWidth * mHeight * 4);

    std::size_t idx = 0;
    for (const auto& c : mPixels) {
        auto rgb255 = c.GetRGB255();
        rgba[idx + 0] = static_cast<std::uint8_t>(rgb255[0]);
        rgba[idx + 1] = static_cast<std::uint8_t>(rgb255[1]);
        rgba[idx + 2] = static_cast<std::uint8_t>(rgb255[2]);
        rgba[idx + 3] = 255;  // Set alpha to 255 (opaque)
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

    std::filesystem::path filename = filepath;
    std::filesystem::path dirPath = filename.parent_path();
    if (!dirPath.empty()) {
        std::filesystem::create_directories(dirPath);
    }
    int ok = stbi_write_png(filename.c_str(), w, h, comp, rgba.data(), stride);

    if (openFile) {
        std::string command;
#ifdef __APPLE__
        command = "open " + filepath;
#elif __linux__
        command = "xdg-open " + filepath;
#elif _WIN32
        command = "start " + filepath;
#endif
        std::system(command.c_str());
    }

    return ok != 0;
}

void Image::CheckBounds(std::size_t x, std::size_t y) const {
    if (x >= mWidth || y >= mHeight) {
        throw std::out_of_range("Pixel coordinate out of bounds");
    }
}

}  // namespace Raytracer
