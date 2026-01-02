#pragma once

#include "Geometry/Vector.hpp"
#include "Utilities/Color.hpp"

namespace Raytracer {

class Pixel {
public:
    Pixel();
    Pixel(const Color& color);

    Color GetColor() const;
    void SetColor(const Color& color);

    Vector3D GetFirstHitNormal() const;
    void SetFirstHitNormal(const Vector3D& normal);

    double GetFirstHitDepth() const;
    void SetFirstHitDepth(double depth);

    Color GetFirstHitAlbedo() const;
    void SetFirstHitAlbedo(const Color& albedo);

private:
    Color mColor;

    // First hit information from renderer needed for denoising
    Vector3D mFirstHitNormal = {0, 0, 0};
    double mFirstHitDepth = 0.0;
    Color mFirstHitAlbedo = BLACK;
};

}  // namespace Raytracer
