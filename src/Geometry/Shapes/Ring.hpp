#pragma once

#include "Geometry/Intersection.hpp"
#include "Geometry/Line.hpp"
#include "Geometry/Shape.hpp"
#include "Geometry/Vector.hpp"

#include <optional>
#include <random>

namespace Raytracer::Geometry {

class Ring : public Shape {
public:
    Ring(const Vector3D& position, const Vector3D& normal, double innerRadius, double outerRadius);

    virtual std::optional<Intersection> Intersect(const Line& line) const override;

    virtual double SurfaceArea() const override;
    virtual std::vector<Vector3D> SampleSurfacePoints(std::size_t numPoints, std::mt19937& prng) const override;

    virtual std::vector<Vector3D> GetKeyPoints() const override;
    virtual std::pair<double, double> GetSurfaceParameters(const Vector3D& point) const override;

    virtual void PrintInfo() const override;

protected:
    double mInnerRadius;
    double mOuterRadius;
};

}  // namespace Raytracer::Geometry
