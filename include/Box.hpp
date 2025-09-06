#pragma once

#include "Color.hpp"
#include "Object.hpp"
#include "Ray.hpp"
#include "Rectangle.hpp"
#include "Vector.hpp"

#include <optional>
#include <vector>

namespace Raytracer {

class Box : public Object {
public:
    // Constructors for Box with different parameters
    Box(const Vector3D& center, double length, double width, double height, const std::array<Color, 6>& colors);
    Box(const Vector3D& center, double length, double width, double height, const Color& color);
    Box(const Vector3D& center, double size, const Color& color);
    Box(const Vector3D& center, double size, const std::array<Color, 6>& colors);

    virtual std::optional<Intersection> Intersect(const Ray& ray) const override;

    virtual void PrintInfo() const override;

private:
    Vector3D mCenter;
    std::vector<Rectangle> mFaces = {};
    double mLength, mWidth, mHeight;
};

}  // namespace Raytracer
