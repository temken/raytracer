#pragma once

#include "Geometry/CompositeShape.hpp"

namespace Raytracer::Geometry {

class Octahedron : public CompositeShape {
public:
    // Constructor (position is center of Octahedron base triangle)
    Octahedron(const Vector3D& position, const Vector3D& orientation, const Vector3D& edgeDirection, double edgeLength);

    virtual void PrintInfo() const override;

private:
    double mEdgeLength;
};

}  // namespace Raytracer::Geometry
