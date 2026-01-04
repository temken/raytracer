#define STB_IMAGE_IMPLEMENTATION

#include "Utilities/Texture.hpp"

#include "stb_image.h"

namespace Raytracer {

Texture::Texture(std::string filePath) :
    mFilePath(filePath) {
    mImage = Image(filePath);
}

Color Texture::GetColorAt(double u, double v) const {
    // Expects u,v in [0,1]
    size_t x = static_cast<size_t>(u * mImage.GetWidth()) % mImage.GetWidth();
    size_t y = static_cast<size_t>((1.0 - v) * mImage.GetHeight()) % mImage.GetHeight();  // Flip v coordinate to match image coordinate system (higher y -> lower pixel in stb image)
    return mImage.GetPixel(x, y);
}

}  // namespace Raytracer
