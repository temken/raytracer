#include "Scene/Disk.hpp"

#include <iostream>

namespace Raytracer {

Disk::Disk(const std::string& name, const Vector3D& center, const Vector3D& normal, double radius, const Color& color) :
    Object(name, color, center, normal),
    mRadius(radius) {}

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
        return Intersection{t, hitPoint, (denom < 0) ? -1.0 * mNormal : mNormal, this};
    }
    return std::nullopt;
}

void Disk::PrintInfo() const {
    std::cout << "Disk: " << mName << std::endl
              << "\tCenter = " << mPosition << std::endl
              << "\tNormal = " << mNormal << std::endl
              << "\tRadius = " << mRadius << std::endl
              << "\tColor = " << mColor << std::endl;
}

}  // namespace Raytracer
