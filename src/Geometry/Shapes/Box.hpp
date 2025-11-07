#pragma once

#include "Geometry/CompositeShape.hpp"
#include "Geometry/Vector.hpp"

namespace Raytracer::Geometry {

class Box : public CompositeShape {
public:
    Box(const Vector3D& center, const Vector3D& heightDirection, const Vector3D& widthDirection, double length, double width, double height);

    virtual void PrintInfo() const override;

private:
    double mLength, mWidth, mHeight;

    virtual void ComposeShape() override;
};

}  // namespace Raytracer::Geometry
