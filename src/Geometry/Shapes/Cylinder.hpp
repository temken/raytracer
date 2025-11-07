#pragma once

#include "Geometry/CompositeShape.hpp"

namespace Raytracer::Geometry {

class Cylinder : public CompositeShape {
public:
    Cylinder(const Vector3D& position, const Vector3D& orientation, double radius, double height);

    virtual void PrintInfo() const override;

private:
    double mRadius;
    double mHeight;

    virtual void ComposeShape() override;
};

}  // namespace Raytracer::Geometry
