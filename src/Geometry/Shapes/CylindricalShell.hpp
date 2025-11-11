#pragma once

#include "Geometry/CompositeShape.hpp"

namespace Raytracer::Geometry {

class CylindricalShell : public CompositeShape {
public:
    CylindricalShell(const Vector3D& position, const Vector3D& orientation, double innerRadius, double outerRadius, double height);

    virtual void PrintInfo() const override;

private:
    double mInnerRadius;
    double mOuterRadius;
    double mHeight;

    virtual void ComposeShape() override;
};

}  // namespace Raytracer::Geometry
