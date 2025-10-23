#pragma once

#include "Scene/Disk.hpp"
#include "Scene/Object.hpp"

namespace Raytracer {

class Cylinder : public Object {
public:
    Cylinder(const std::string& name, const Material& material, const Vector3D& center, const Vector3D& normal, double radius, double height);

    virtual double GetSurfaceArea() const override;

    virtual std::optional<Intersection> Intersect(const Ray& ray) override;

    virtual void PrintInfo() const override;

private:
    double mRadius;
    double mHeight;
    Disk mTopDisk;
    Disk mBottomDisk;

    std::optional<Intersection> IntersectMantle(const Ray& ray);
};

}  // namespace Raytracer
