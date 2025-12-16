#pragma once

#include "Geometry/Shape.hpp"

namespace Raytracer::Geometry {

class SphericalCap : public Shape {
public:
    SphericalCap(const Vector3D& position, const double radius, const Vector3D& orientation = Vector3D({0.0, 0.0, 1.0}), double maxAngle = M_PI / 2.0);

    virtual std::optional<Intersection> Intersect(const Line& line) const override;

    virtual double SurfaceArea() const override;
    virtual std::vector<Vector3D> SampleSurfacePoints(std::size_t numPoints, std::mt19937& prng) const override;

    virtual std::vector<Vector3D> GetKeyPoints() const override;
    virtual std::pair<double, double> GetSurfaceParameters(const Vector3D& point) const override;

    virtual void PrintInfo() const override;

private:
    double mRadius;
    double mCosMaxAngle;
};

}  // namespace Raytracer::Geometry
