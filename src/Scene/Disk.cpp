#include "Scene/Disk.hpp"

#include <iostream>

namespace Raytracer {

Disk::Disk(const std::string& name, const Material& material, const Vector3D& center, const Vector3D& normal, double radius) :
    Object(name, material, center, normal),
    mRadius(radius) {}

double Disk::GetSurfaceArea() const {
    return M_PI * mRadius * mRadius;
}

std::vector<Vector3D> Disk::SampleSurfacePoints(std::size_t numPoints, std::mt19937& prng) const {
    std::vector<Vector3D> points;

    std::uniform_real_distribution<double> radiusDist(0.0, mRadius);
    std::uniform_real_distribution<double> angleDist(0.0, 2.0 * M_PI);

    // Build an orthonormal basis around the disk axis
    Vector3D w = mNormal.Normalized();  // axis direction
    // Pick a reference vector that is *not* parallel to w
    Vector3D reference = (std::fabs(w[0]) > 0.9) ? Vector3D({0, 1, 0}) : Vector3D({1, 0, 0});
    // First perpendicular vector
    Vector3D u = w.Cross(reference).Normalized();
    // Second perpendicular vector (completes the right-handed system)
    Vector3D v = w.Cross(u);

    for (std::size_t i = 0; i < numPoints; ++i) {
        double r = std::sqrt(radiusDist(prng));  // sqrt for uniform area distribution
        double theta = angleDist(prng);

        Vector3D point = mPosition + r * (std::cos(theta) * u + std::sin(theta) * v);
        points.push_back(point);
    }
    return points;
}

std::vector<Vector3D> Disk::GetKeyPoints() const {
    return {mPosition};
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
