#include "Utilities/Pixel.hpp"

namespace Raytracer {

Pixel::Pixel() : mColor(BLACK) {
}

Pixel::Pixel(const Color& color) : mColor(color) {
}

Color Pixel::GetColor() const {
    return mColor;
}

void Pixel::SetColor(const Color& color) {
    mColor = color;
}

Vector3D Pixel::GetFirstHitNormal() const {
    return mFirstHitNormal;
}

void Pixel::SetFirstHitNormal(const Vector3D& normal) {
    mFirstHitNormal = normal;
}

double Pixel::GetFirstHitDepth() const {
    return mFirstHitDepth;
}

void Pixel::SetFirstHitDepth(double depth) {
    mFirstHitDepth = depth;
}

Color Pixel::GetFirstHitAlbedo() const {
    return mFirstHitAlbedo;
}

void Pixel::SetFirstHitAlbedo(const Color& albedo) {
    mFirstHitAlbedo = albedo;
}

}  // namespace Raytracer
