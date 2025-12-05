#pragma once

#include "Geometry/Intersection.hpp"
#include "Geometry/Line.hpp"
#include "Geometry/Shape.hpp"
#include "Geometry/Shapes/Torus.hpp"
#include "Geometry/Vector.hpp"

namespace Raytracer::Geometry {

class HalfTorus : public Torus {
public:
    HalfTorus(const Vector3D& position, const Vector3D& orientation, const Vector3D& referenceDirection, double majorRadius, double minorRadius);

    virtual std::optional<Intersection> Intersect(const Line& line) const override;

    virtual double SurfaceArea() const override;
    virtual std::vector<Vector3D> SampleSurfacePoints(std::size_t numPoints, std::mt19937& prng) const override;

    virtual std::vector<Vector3D> GetKeyPoints() const override;
    virtual std::pair<double, double> GetSurfaceParameters(const Vector3D& point) const override;

    virtual void PrintInfo() const override;

private:
};

}  // namespace Raytracer::Geometry
