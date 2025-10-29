#pragma once

#include "Scene/Object.hpp"

namespace Raytracer {

class Tube : public Object {
public:
    Tube(const std::string& name, const Material& material, const Vector3D& center, const Vector3D& normal, double radius, double length);

    virtual double GetSurfaceArea() const override;
    virtual std::vector<Vector3D> SampleSurfacePoints(std::size_t numPoints, std::mt19937& prng) const override;
    virtual std::vector<Vector3D> GetKeyPoints() const override;

    virtual std::optional<Intersection> Intersect(const Ray& ray) override;

    virtual void PrintInfo() const override;

private:
    double mRadius;
    double mLength;
};

}  // namespace Raytracer
