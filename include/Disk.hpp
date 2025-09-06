#pragma once

#include "Color.hpp"
#include "Object.hpp"
#include "Ray.hpp"
#include "Vector.hpp"

#include <optional>

namespace Raytracer {

class Disk : public Object {
public:
    Disk(const Vector3D& center, const Vector3D& normal, double radius, const Color& color);

    virtual std::optional<Intersection> Intersect(const Ray& ray) const override;

    virtual void PrintInfo() const override;

private:
    Vector3D mCenter;
    Vector3D mNormal;
    double mRadius;
};

}  // namespace Raytracer
