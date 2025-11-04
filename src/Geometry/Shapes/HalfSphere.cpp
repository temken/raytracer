#include "Geometry/Shapes/HalfSphere.hpp"

namespace Raytracer::Geometry {

HalfSphere::HalfSphere(const Vector3D& position, double radius, const Vector3D& orientation) :
    Shape(Type::HALF_SPHERE, position, orientation),
    mRadius(radius) {
}

std::optional<Intersection> HalfSphere::Intersect(const Line& line) const {
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
        if (r < line.GetTMin()) {
            continue;
        }
        Vector3D intersectionPoint = line(r);
        Vector3D fromCenter = intersectionPoint - mPosition;
        if (fromCenter.Dot(GetBasisVector(OrthonormalBasis::BasisVector::eZ)) < 0.0) {  // Keep the hemisphere in the +orientation direction
            continue;
        }
        if (r < t) {
            t = r;
        }
    }

    if (t == std::numeric_limits<double>::infinity()) {
        return std::nullopt;  // no valid intersection
    }

    Vector3D intersectionPoint = line(t);
    Vector3D normal = (intersectionPoint - mPosition).Normalized();

    return Intersection{t, intersectionPoint, normal};
}

double HalfSphere::SurfaceArea() const {
    return 2.0 * M_PI * mRadius * mRadius;
}

std::vector<Vector3D> HalfSphere::SampleSurfacePoints(std::size_t numPoints, std::mt19937& prng) const {
    std::vector<Vector3D> points;

    std::uniform_real_distribution<double> uniformDistribution(0.0, 1.0);

    for (std::size_t i = 0; i < numPoints; ++i) {
        double u = uniformDistribution(prng);
        double v = uniformDistribution(prng);

        double phi = 2.0 * M_PI * u;  // azimuthal angle [0, 2π]
        double cosTheta = v;          // polar angle [0, π/2] - only hemisphere
        double sinTheta = std::sqrt(1.0 - cosTheta * cosTheta);

        // Point in local hemisphere coordinates (z+ is "up")
        Vector3D localPoint = {
            mRadius * sinTheta * std::cos(phi),
            mRadius * sinTheta * std::sin(phi),
            mRadius * cosTheta,
        };

        // Transform to world coordinates using orthonormal basis
        Vector3D point = mOrthonormalBasis.ToGlobal(localPoint) + mPosition;

        points.push_back(point);
    }
    return points;
}

std::vector<Vector3D> HalfSphere::GetKeyPoints() const {
    return {mPosition + mRadius * GetBasisVector(OrthonormalBasis::BasisVector::eZ)};
}

std::pair<double, double> HalfSphere::GetSurfaceParameters(const Vector3D& point) const {
    // TODO: Implement preojection
    return {0.0, 0.0};
}

void HalfSphere::PrintInfo() const {
    PrintInfoBase();
    std::cout << "\tRadius:\t" << mRadius << std::endl
              << std::endl;
}

}  // namespace Raytracer::Geometry
