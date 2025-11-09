#pragma once

#include "Geometry/Shape.hpp"

namespace Raytracer::Geometry {

class Sphere : public Shape {
public:
    Sphere(const Vector3D& position, const double radius, const Vector3D& orientation = Vector3D({0.0, 0.0, 1.0}), const Vector3D& reference_direction = Vector3D({1.0, 0.0, 0.0}));

    virtual std::optional<Intersection> Intersect(const Line& line) const override;

    virtual double SurfaceArea() const override;
    virtual std::vector<Vector3D> SampleSurfacePoints(std::size_t numPoints, std::mt19937& prng) const override;

    virtual std::vector<Vector3D> GetKeyPoints() const override;
    virtual std::pair<double, double> GetSurfaceParameters(const Vector3D& point) const override;
    
    // Static version of GetSurfaceParameters
    static std::pair<double, double> GetSurfaceParameters(const Vector3D& point, const Vector3D& sphereCenter, const OrthonormalBasis& basis);

    virtual void PrintInfo() const override;

private:
    double mRadius;
};

}  // namespace Raytracer::Geometry
