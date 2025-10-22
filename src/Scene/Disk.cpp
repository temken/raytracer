#include "Scene/Disk.hpp"

#include <iostream>

namespace Raytracer {

Disk::Disk(const std::string& name, const Material& material, const Vector3D& center, const Vector3D& normal, double radius) :
    Object(name, material, center, normal),
    mRadius(radius) {}

double Disk::GetSurfaceArea() const {
    return M_PI * mRadius * mRadius;
}

std::optional<Intersection> Disk::Intersect(const Ray& ray) {
    double denom = mNormal.Dot(ray.GetDirection());
    if (std::fabs(denom) < sEpsilon) {
        return std::nullopt;
    }

    double t = (mPosition - ray.GetOrigin()).Dot(mNormal) / denom;
    if (t < sEpsilon) {
        return std::nullopt;
    }

    Vector3D hitPoint = ray(t);
    Vector3D localPoint = hitPoint - mPosition;

    if (localPoint.Norm() <= mRadius) {
        return Intersection{t, hitPoint, mNormal, this};
    }
    return std::nullopt;
}

void Disk::PrintInfo() const {
    PrintInfoBase();
    std::cout << "Shape:\tDisk" << std::endl
              << "\tRadius:\t" << mRadius << std::endl;
    mMaterial.PrintInfo();
}

}  // namespace Raytracer
