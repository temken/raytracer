#pragma once

#include "Geometry/Intersection.hpp"
#include "Geometry/Line.hpp"
#include "Geometry/Shape.hpp"
#include "Geometry/Vector.hpp"

#include <array>
#include <optional>
#include <vector>

namespace Raytracer::Geometry {

class Triangle : public Shape {
public:
    Triangle(const Vector3D& vertex1, const Vector3D& vertex2, const Vector3D& vertex3);

    std::optional<Intersection> Intersect(const Line& line) const override;

    double SurfaceArea() const override;

    std::vector<Vector3D> SampleSurfacePoints(std::size_t numPoints, std::mt19937& prng) const override;

    std::vector<Vector3D> GetKeyPoints() const override;

    std::pair<double, double> GetSurfaceParameters(const Vector3D& point) const override;

    void PrintInfo() const override;

private:
    std::array<Vector3D, 3> mVertices;  // Local coordinates of the triangle's vertices relative to mPosition
    std::array<Vector3D, 2> mEdges;     // Edges in local coordinates
};

}  // namespace Raytracer::Geometry
