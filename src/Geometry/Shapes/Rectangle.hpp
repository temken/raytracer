#pragma once

#include "Geometry/Shape.hpp"

namespace Raytracer::Geometry {

class Rectangle : public Shape {
public:
    Rectangle(const Vector3D& position, const Vector3D& normal, const Vector3D& edgeA, const Vector3D& edgeB);

    std::optional<Intersection> Intersect(const Line& line, double tMin) const;

    virtual double SurfaceArea() const override;
    virtual std::vector<Vector3D> SampleSurfacePoints(std::size_t numPoints, std::mt19937& prng) const override;

    virtual std::vector<Vector3D> GetKeyPoints() const override;
    virtual std::pair<double, double> SurfaceParameters(const Vector3D& point) const override;

    virtual void PrintInfo() const override;

private:
};

}  // namespace Raytracer::Geometry
