#pragma once

#include "Geometry/Shapes/Box.hpp"
#include "Geometry/Vector.hpp"

namespace Raytracer::Geometry {

class BoxAxisAligned : public Box {
public:
    BoxAxisAligned(const Vector3D& center, double length, double width, double height);

private:
    // Rotation method deleted to enforce axis-alignment
    void Spin(double angle, OrthonormalBasis::BasisVector axis) = delete;
};

}  // namespace Raytracer::Geometry
