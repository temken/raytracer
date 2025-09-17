#pragma once

#include "Scene/Disk.hpp"
#include "Scene/Object.hpp"

namespace Raytracer {

class Cylinder : public Object {
public:
    Cylinder(const std::string& name, const Vector3D& center, const Vector3D& normal, double radius, double height, const Color& mantleColor, const Color& capColor);

    virtual std::optional<Intersection> Intersect(const Ray& ray) override;

    virtual void PrintInfo() const override;

private:
    Vector3D mCenter;
    Vector3D mNormal;
    double mRadius;
    double mHeight;
    Disk mTopDisk;
    Disk mBottomDisk;

    std::optional<Intersection> IntersectCaps(const Ray& ray);
    std::optional<Intersection> IntersectMantle(const Ray& ray);
};

}  // namespace Raytracer
