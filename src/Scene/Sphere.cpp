#include "Scene/Sphere.hpp"

#include <cmath>
#include <limits>

namespace Raytracer {

Sphere::Sphere(const std::string& name, const Material& material, const Vector3D& center, double radius) :
    Object(name, material, center, Vector3D({0, 0, 1})),
    mRadius(radius) {}

double Sphere::GetSurfaceArea() const {
    return 4.0 * M_PI * mRadius * mRadius;
}

std::vector<Vector3D> Sphere::SampleSurfacePoints(std::size_t numPoints, std::mt19937& prng) const {
    std::vector<Vector3D> points;

    std::uniform_real_distribution<double> uniformDistribution(0.0, 1.0);

    for (std::size_t i = 0; i < numPoints; ++i) {
        double u = uniformDistribution(prng);
        double v = uniformDistribution(prng);

        double phi = 2.0 * M_PI * u;      // azimuthal angle
        double cosTheta = 1.0 - 2.0 * v;  // polar angle
        double sinTheta = std::sqrt(1.0 - cosTheta * cosTheta);

        Vector3D point = {
            mRadius * sinTheta * std::cos(phi),
            mRadius * sinTheta * std::sin(phi),
            mRadius * cosTheta,
        };

        points.push_back(mPosition + point);
    }
    return points;
}
std::vector<Vector3D> Sphere::GetKeyPoints() const {
    return {mPosition};
}

std::optional<Intersection> Sphere::Intersect(const Ray& ray) {
    Vector3D oc = ray.GetOrigin() - mPosition;

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
    Vector3D normal = (hitPoint - mPosition).Normalized();

    return Intersection{t, hitPoint, normal, this};
}

void Sphere::PrintInfo() const {
    PrintInfoBase();
    std::cout << "Shape:\t Sphere" << std::endl
              << "\tCenter: " << mPosition << std::endl
              << "\tRadius: " << mRadius << std::endl;
    mMaterial.PrintInfo();
}

}  // namespace Raytracer
