#pragma once

#include "Geometry/Vector.hpp"
#include "Rendering/Ray.hpp"
#include "Scene/Object.hpp"
#include "Utilities/Color.hpp"

#include <optional>

namespace Raytracer {

class Disk : public Object {
public:
    Disk(const std::string& name, const Material& material, const Vector3D& center, const Vector3D& normal, double radius);

    virtual double GetSurfaceArea() const override;
    virtual std::vector<Vector3D> SampleSurfacePoints(std::size_t numPoints, std::mt19937& prng) const override;
    virtual std::vector<Vector3D> GetKeyPoints() const override;

    virtual std::optional<Intersection> Intersect(const Ray& ray) override;

    virtual void PrintInfo() const override;

private:
    double mRadius;
};

}  // namespace Raytracer
