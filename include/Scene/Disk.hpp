#pragma once

#include "Rendering/Ray.hpp"
#include "Scene/Object.hpp"
#include "Utilities/Color.hpp"
#include "Utilities/Vector.hpp"

#include <optional>

namespace Raytracer {

class Disk : public Object {
public:
    Disk(const std::string& name, const Vector3D& center, const Vector3D& normal, double radius, const Color& color);

    virtual std::optional<Intersection> Intersect(const Ray& ray) override;

    virtual void PrintInfo() const override;

private:
    double mRadius;
};

}  // namespace Raytracer
