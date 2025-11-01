#include "Geometry/Shapes/Sphere.hpp"

namespace Raytracer::Geometry {

Sphere::Sphere(const Vector3D& position, double radius, const Vector3D& orientation) :
    Shape(Type::SPHERE, position, orientation),
    mRadius(radius) {
}

std::optional<Intersection> Sphere::Intersect(const Line& line) const {
    Vector3D oc = line.GetOrigin() - mPosition;

    double a = line.GetDirection().NormSquared();
    double b = 2.0 * oc.Dot(line.GetDirection());
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
        if (r < t) {
            t = r;
        }
    }
    if (t == std::numeric_limits<double>::infinity()) {
        return std::nullopt;  // no valid root
    }

    Vector3D intersectionPoint = line(t);
    Vector3D normal = (intersectionPoint - mPosition).Normalized();

    return Intersection{t, intersectionPoint, normal};
}

double Sphere::SurfaceArea() const {
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

std::pair<double, double> Sphere::GetSurfaceParameters(const Vector3D& point) const {
    // TODO: Implement preojection
    return {0.0, 0.0};
}

void Sphere::PrintInfo() const {
    PrintInfoBase();
    std::cout << "\tRadius:\t" << mRadius << std::endl
              << std::endl;
}

}  // namespace Raytracer::Geometry
