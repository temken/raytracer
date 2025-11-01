#pragma once

#include "Geometry/Shape.hpp"

namespace Raytracer::Geometry {

class Tube : public Shape {
public:
    Tube(const Vector3D& position, const Vector3D& orientation, double radius, double length);

    std::optional<Intersection> Intersect(const Line& line) const;

    virtual double SurfaceArea() const override;
    virtual std::vector<Vector3D> SampleSurfacePoints(std::size_t numPoints, std::mt19937& prng) const override;

    virtual std::vector<Vector3D> GetKeyPoints() const override;
    virtual std::pair<double, double> GetSurfaceParameters(const Vector3D& point) const override;

    virtual void PrintInfo() const override;

private:
    double mRadius;
    double mLength;
};

}  // namespace Raytracer::Geometry
