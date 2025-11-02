#pragma once

#include "Geometry/CompositeShape.hpp"
#include "Geometry/Vector.hpp"

namespace Raytracer::Geometry {

class BoxAxisAligned : public CompositeShape {
public:
    BoxAxisAligned(const Vector3D& center, double length, double width, double height);

    virtual void PrintInfo() const override;

private:
    double mLength, mWidth, mHeight;

    // Rotation method deleted to enforce axis-alignment
    void Spin(double angle, OrthonormalBasis::BasisVector axis) = delete;
};

}  // namespace Raytracer::Geometry
