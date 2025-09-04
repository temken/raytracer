#pragma once

#include "Color.hpp"
#include "Object.hpp"

#include <optional>

namespace Raytracer {

class Sphere : public Object {
public:
    Sphere(const Vector3D& center, double radius, const Color& color);

    std::optional<Intersection> Intersect(const Ray& ray) const override;

    void PrintInfo() const override;

private:
    Vector3D mCenter;
    double mRadius;
};

}  // namespace Raytracer
