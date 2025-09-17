#include "Scene/Sphere.hpp"

#include <cmath>
#include <limits>

namespace Raytracer {

Sphere::Sphere(const std::string& name, const Vector3D& center, double radius, const Color& color) :
    Object(name, color),
    mCenter(center),
    mRadius(radius) {}

// Ray-sphere intersection
std::optional<Intersection> Sphere::Intersect(const Ray& ray) {
    Vector3D oc = ray.GetOrigin() - mCenter;

    double a = ray.GetDirection().NormSquared();
    double b = 2.0 * oc.Dot(ray.GetDirection());
    double c = oc.NormSquared() - mRadius * mRadius;

    double discriminant = b * b - 4 * a * c;
    if (discriminant < 0.0) {
        return std::nullopt;
    }

    double sqrtD = std::sqrt(discriminant);
    std::array<double, 2> roots = {
        (-b - sqrtD) / (2.0 * a),
        (-b + sqrtD) / (2.0 * a)};

    double t = std::numeric_limits<double>::infinity();
    for (double r : roots) {
        if (r > sEpsilon && r < t) {
            t = r;
        }
    }
    if (t == std::numeric_limits<double>::infinity()) {
        return std::nullopt;  // no valid root
    }

    Vector3D hitPoint = ray.GetOrigin() + t * ray.GetDirection();
    Vector3D normal = (hitPoint - mCenter).Normalized();

    return Intersection{t, hitPoint, normal, this};
}

void Sphere::PrintInfo() const {
    std::cout << "Sphere:" << std::endl
              << "\tCenter: " << mCenter << std::endl
              << "\tRadius: " << mRadius << std::endl
              << "\tColor: " << mColor << std::endl;
}

}  // namespace Raytracer
