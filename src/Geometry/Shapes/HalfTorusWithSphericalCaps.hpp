#pragma once

#include "Geometry/CompositeShape.hpp"

namespace Raytracer::Geometry {

class HalfTorusWithSphericalCaps : public CompositeShape {
public:
    HalfTorusWithSphericalCaps(const Vector3D& position, const Vector3D& orientation, const Vector3D& referenceDirection, double majorRadius, double minorRadius);

    virtual void PrintInfo() const override;

private:
    double mMajorRadius;
    double mMinorRadius;

    virtual void ComposeShape() override;
};

}  // namespace Raytracer::Geometry
