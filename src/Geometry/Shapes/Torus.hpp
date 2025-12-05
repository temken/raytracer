#pragma once

#include "Geometry/Intersection.hpp"
#include "Geometry/Line.hpp"
#include "Geometry/Shape.hpp"
#include "Geometry/Vector.hpp"

#include <array>
#include <optional>
#include <random>
#include <vector>

namespace Raytracer::Geometry {

class Torus : public Shape {
public:
    Torus(const Vector3D& position, const Vector3D& orientation, double majorRadius, double minorRadius);

    virtual std::optional<Intersection> Intersect(const Line& line) const override;

    virtual double SurfaceArea() const override;
    virtual std::vector<Vector3D> SampleSurfacePoints(std::size_t numPoints, std::mt19937& prng) const override;

    virtual std::vector<Vector3D> GetKeyPoints() const override;
    virtual std::pair<double, double> GetSurfaceParameters(const Vector3D& point) const override;

    virtual void PrintInfo() const override;

private:
    double mMajorRadius;
    double mMinorRadius;

protected:
    std::array<double, 5> ComputeQuarticCoefficients(const Vector3D& localOrigin, const Vector3D& localDirection) const;
    Vector3D ComputeNormalAtPoint(const Vector3D& localPoint) const;
};

}  // namespace Raytracer::Geometry
