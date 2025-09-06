#include "Disk.hpp"

#include <iostream>

namespace Raytracer {

Disk::Disk(const Vector3D& center, const Vector3D& normal, double radius, const Color& color) :
    Object(color),
    mCenter(center),
    mNormal(normal.Normalized()),
    mRadius(radius) {}

std::optional<Intersection> Disk::Intersect(const Ray& ray) const {
    double denom = mNormal.Dot(ray.GetDirection());
    if (std::fabs(denom) < sEpsilon) {
        return std::nullopt;
    }

    double t = (mCenter - ray.GetOrigin()).Dot(mNormal) / denom;
    if (t < sEpsilon) {
        return std::nullopt;
    }

    Vector3D hitPoint = ray(t);
    Vector3D localPoint = hitPoint - mCenter;

    if (localPoint.Norm() <= mRadius) {
        return Intersection{t, hitPoint, (denom < 0) ? -1.0 * mNormal : mNormal, this};
    }
    return std::nullopt;
}

void Disk::PrintInfo() const {
    std::cout << "Disk: Center = " << mCenter << ", Normal = " << mNormal << ", Radius = " << mRadius << std::endl;
}

}  // namespace Raytracer
