#pragma once

#include "Geometry/Intersection.hpp"
#include "Geometry/Line.hpp"
#include "Geometry/Shape.hpp"

#include <optional>
#include <random>
#include <vector>

namespace Raytracer::Geometry {

class Rectangle : public Shape {
public:
    Rectangle(const Vector3D& center, const Vector3D& normal, const Vector3D& widthDirection, double width, double height);

    virtual std::optional<Intersection> Intersect(const Line& line) const override;

    virtual double SurfaceArea() const override;
    virtual std::vector<Vector3D> SampleSurfacePoints(std::size_t numPoints, std::mt19937& prng) const override;

    virtual std::vector<Vector3D> GetKeyPoints() const override;
    virtual std::pair<double, double> SurfaceParameters(const Vector3D& point) const override;

    virtual void PrintInfo() const override;

private:
    double mWidth;
    double mHeight;
};

}  // namespace Raytracer::Geometry
