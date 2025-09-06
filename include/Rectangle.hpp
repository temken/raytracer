#pragma once

#include "Color.hpp"
#include "Object.hpp"
#include "Ray.hpp"
#include "Vector.hpp"

#include <optional>

namespace Raytracer {

class Rectangle : public Object {
public:
    Rectangle(const Vector3D& center, const Vector3D& normal, double width, double height, const Color& color);

    virtual std::optional<Intersection> Intersect(const Ray& ray) const override;

    virtual void PrintInfo() const override;

private:
    Vector3D mCenter;
    Vector3D mNormal;
    Vector3D mU;  // Local x-axis in the rectangle plane
    Vector3D mV;  // Local y-axis in the rectangle plane
    double mWidth;
    double mHeight;
};

}  // namespace Raytracer
