#include "Geometry/Shapes/Tube.hpp"

namespace Raytracer::Geometry {

Tube::Tube(const Vector3D& position, const Vector3D& orientation, double radius, double length) :
    Shape(Type::TUBE, position, orientation),
    mRadius(radius),
    mLength(length) {
}

std::optional<Intersection> Tube::Intersect(const Line& line) const {
    Vector3D orientation = GetOrientation();
    Vector3D d = line.GetDirection();
    Vector3D oc = line.GetOrigin() - mPosition;

    double dDotN = d.Dot(orientation);
    Vector3D dPerp = d - dDotN * orientation;

    double ocDotN = oc.Dot(orientation);
    Vector3D ocPerp = oc - ocDotN * orientation;

    double a = dPerp.NormSquared();
    double b = 2.0 * ocPerp.Dot(dPerp);
    double c = ocPerp.NormSquared() - mRadius * mRadius;

    double discriminant = b * b - 4 * a * c;
    if (discriminant < 0.0) {
        return std::nullopt;
    }

    double sqrtD = std::sqrt(discriminant);
    std::array<double, 2> roots = {
        (-b - sqrtD) / (2.0 * a),
        (-b + sqrtD) / (2.0 * a)};

    double tCylinder = std::numeric_limits<double>::infinity();
    for (double r : roots) {
        if (r > sEpsilon && r < tCylinder) {
            tCylinder = r;
        }
    }
    if (tCylinder == std::numeric_limits<double>::infinity()) {
        return std::nullopt;  // No valid root
    }

    // Check if the intersection point is within the height bounds
    Vector3D intersectionPoint = line(tCylinder);
    double heightAtIntersection = (intersectionPoint - mPosition).Dot(orientation);
    if (std::abs(heightAtIntersection) <= mLength / 2.0) {
        Vector3D normalAtHit = (intersectionPoint - mPosition - heightAtIntersection * orientation).Normalized();
        return Intersection{tCylinder, intersectionPoint, normalAtHit};
    }

    return std::nullopt;
}

double Tube::SurfaceArea() const {
    return 2 * M_PI * mRadius * mLength;
}

std::vector<Vector3D> Tube::SampleSurfacePoints(std::size_t numPoints, std::mt19937& prng) const {
    std::vector<Vector3D> points;

    // Distributions for uniform sampling
    std::uniform_real_distribution<double> angleDist(0.0, 2.0 * M_PI);
    std::uniform_real_distribution<double> lengthDist(-0.5 * mLength, 0.5 * mLength);

    for (std::size_t i = 0; i < numPoints; ++i) {
        double theta = angleDist(prng);
        double h = lengthDist(prng);

        // Point on circle of given height
        Vector3D circlePoint = mPosition + mRadius * (std::cos(theta) * GetBasisVector(OrthonormalBasis::BasisVector::eX) + std::sin(theta) * GetBasisVector(OrthonormalBasis::BasisVector::eY)) + GetBasisVector(OrthonormalBasis::BasisVector::eZ) * h;
        points.push_back(circlePoint);
    }

    return points;
}

std::vector<Vector3D> Tube::GetKeyPoints() const {
    auto eZ = GetBasisVector(OrthonormalBasis::BasisVector::eZ);
    std::vector<Vector3D> keyPoints = {
        mPosition,
        mPosition + mLength / 4.0 * eZ,
        mPosition - mLength / 4.0 * eZ,
        mPosition + (mLength / 2.0) * eZ,
        mPosition - (mLength / 2.0) * eZ,
    };
    return keyPoints;
}

std::pair<double, double> Tube::GetSurfaceParameters(const Vector3D& point) const {
    // TODO: Implement projection
    return {0.0, 0.0};
}

void Tube::PrintInfo() const {
    PrintInfoBase();
    std::cout << "\tRadius:\t" << mRadius << std::endl
              << "\tLength:\t" << mLength << std::endl
              << std::endl;
}

}  // namespace Raytracer::Geometry
