#pragma once

#include "Scene/CompositeObject.hpp"
#include "Scene/Disk.hpp"

namespace Raytracer {

class Cylinder : public CompositeObject {
public:
    Cylinder(const std::string& name, const Material& material, const Vector3D& center, const Vector3D& normal, double radius, double height);

    virtual void PrintInfo() const override;

private:
    double mRadius;
    double mHeight;
};

}  // namespace Raytracer
