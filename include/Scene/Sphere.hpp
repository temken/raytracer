#pragma once

#include "Scene/Object.hpp"
#include "Utilities/Color.hpp"

#include <optional>

namespace Raytracer {

class Sphere : public Object {
public:
    Sphere(const std::string& name, const Vector3D& center, double radius, const Color& color);

    std::optional<Intersection> Intersect(const Ray& ray) override;

    void PrintInfo() const override;

private:
    double mRadius;
};

}  // namespace Raytracer
