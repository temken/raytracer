#define STB_IMAGE_IMPLEMENTATION

#include "Utilities/Texture.hpp"

#include "stb_image.h"

namespace Raytracer {

Texture::Texture(std::string filePath) :
    mFilePath(filePath) {
    mImage = Image(filePath);
}

Color Texture::GetColorAt(double u, double v) const {
    size_t x = static_cast<size_t>(v * mImage.GetWidth()) % mImage.GetWidth();
    size_t y = static_cast<size_t>(u * mImage.GetHeight()) % mImage.GetHeight();
    return mImage.GetPixel(x, y);
}

}  // namespace Raytracer
