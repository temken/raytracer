#include "Geometry/Shapes/Ring.hpp"

#include <cmath>

namespace Raytracer::Geometry {

Ring::Ring(const Vector3D& position, const Vector3D& normal, double innerRadius, double outerRadius) :
    Shape(Type::RING, position, normal),
    mInnerRadius(innerRadius),
    mOuterRadius(outerRadius) {}

std::optional<Intersection> Ring::Intersect(const Line& line) const {
    const Vector3D& normal = GetBasisVector(OrthonormalBasis::BasisVector::eZ);
    double denom = normal.Dot(line.GetDirection());
    if (std::fabs(denom) < sEpsilon) {
        return std::nullopt;
    }

    double t = (mPosition - line.GetOrigin()).Dot(normal) / denom;
    if (t < line.GetTMin()) {
        return std::nullopt;
    }

    Vector3D hitPoint = line(t);
    Vector3D localPoint = hitPoint - mPosition;

    if (localPoint.Norm() <= mOuterRadius && localPoint.Norm() >= mInnerRadius) {
        return Intersection{t, hitPoint, normal};
    }
    return std::nullopt;
}

double Ring::SurfaceArea() const {
    return M_PI * (mOuterRadius * mOuterRadius - mInnerRadius * mInnerRadius);
}

std::vector<Vector3D> Ring::SampleSurfacePoints(std::size_t numPoints, std::mt19937& prng) const {
    std::vector<Vector3D> points;

    std::uniform_real_distribution<double> uniformDist(0.0, 1.0);
    std::uniform_real_distribution<double> angleDist(0.0, 2.0 * M_PI);

    for (std::size_t i = 0; i < numPoints; ++i) {
        double u = uniformDist(prng);
        double r = std::sqrt(u * (mOuterRadius * mOuterRadius - mInnerRadius * mInnerRadius) + mInnerRadius * mInnerRadius);
        double theta = angleDist(prng);

        Vector3D point = mPosition + r * (std::cos(theta) * mOrthonormalBasis[0] + std::sin(theta) * mOrthonormalBasis[1]);
        points.push_back(point);
    }
    return points;
}

std::vector<Vector3D> Ring::GetKeyPoints() const {
    return {
        mPosition + 0.5 * (mInnerRadius + mOuterRadius) * mOrthonormalBasis[0],
        mPosition - 0.5 * (mInnerRadius + mOuterRadius) * mOrthonormalBasis[0],
        mPosition + 0.5 * (mInnerRadius + mOuterRadius) * mOrthonormalBasis[1],
        mPosition - 0.5 * (mInnerRadius + mOuterRadius) * mOrthonormalBasis[1],
    };
}

std::pair<double, double> Ring::GetSurfaceParameters(const Vector3D& point) const {
    return {0.0, 0.0};  // Not implemented
}

void Ring::PrintInfo() const {
    PrintInfoBase();
    std::cout << "\tInner Radius:\t" << mInnerRadius << std::endl
              << "\tOuter Radius:\t" << mOuterRadius << std::endl
              << std::endl;
}

}  // namespace Raytracer::Geometry
