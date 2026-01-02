#pragma once

#include "Utilities/Pixel.hpp"

#include <cstddef>
#include <string>
#include <vector>

namespace Raytracer {

class Image {
public:
    Image() = default;
    Image(std::size_t width, std::size_t height, const Color& backgroundColor = Color(0, 0, 0));
    Image(std::string filepath, bool assume_srgb = true);

    std::size_t GetWidth() const;
    std::size_t GetHeight() const;

    void SetPixel(std::size_t x, std::size_t y, const Color& color);
    Pixel GetPixel(std::size_t x, std::size_t y) const;
    std::vector<Pixel> GetNeighbors(std::size_t x, std::size_t y) const;

    Color GetColor(std::size_t x, std::size_t y) const;

    bool Save(bool openFile = false, std::string filepath = "") const;

    void PrintToTerminal(std::size_t width, double terminalCharAspectRatio = 18.0 / 7.0) const;

    void Clear(const Color& color = Color(0, 0, 0));

    void PrintInfo() const;

private:
    std::size_t mWidth = 100;
    std::size_t mHeight = 100;
    std::vector<Pixel> mPixels;

    void CheckBounds(std::size_t x, std::size_t y) const;
    size_t CountBlackPixels() const;
    double CalculateBlackPixelRatio() const;
};

}  // namespace Raytracer
