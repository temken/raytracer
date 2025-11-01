#include "Geometry/Shapes/Disk.hpp"

namespace Raytracer::Geometry {

Disk::Disk(const Vector3D& position, const Vector3D& normal, double radius) :
    Shape(Type::DISK, position, normal),
    mRadius(radius) {
}

std::optional<Intersection> Disk::Intersect(const Line& line) const {
    const Vector3D& normal = GetBasisVector(OrthonormalBasis::BasisVector::eZ);
    double denom = normal.Dot(line.GetDirection());
    if (std::fabs(denom) < sEpsilon) {
        return std::nullopt;
    }

    double t = (mPosition - line.GetOrigin()).Dot(normal) / denom;
    if (t < sEpsilon) {
        return std::nullopt;
    }

    Vector3D hitPoint = line(t);
    Vector3D localPoint = hitPoint - mPosition;

    if (localPoint.Norm() <= mRadius) {
        return Intersection{t, hitPoint, normal};
    }
    return std::nullopt;
}

double Disk::SurfaceArea() const {
    return M_PI * mRadius * mRadius;
}

std::vector<Vector3D> Disk::SampleSurfacePoints(std::size_t numPoints, std::mt19937& prng) const {
    std::vector<Vector3D> points;

    std::uniform_real_distribution<double> radiusDist(0.0, mRadius);
    std::uniform_real_distribution<double> angleDist(0.0, 2.0 * M_PI);

    for (std::size_t i = 0; i < numPoints; ++i) {
        double r = std::sqrt(radiusDist(prng));  // sqrt for uniform area distribution
        double theta = angleDist(prng);

        Vector3D point = mPosition + r * (std::cos(theta) * mOrthonormalBasis[0] + std::sin(theta) * mOrthonormalBasis[1]);
        points.push_back(point);
    }
    return points;
}

std::vector<Vector3D> Disk::GetKeyPoints() const {
    return {mPosition};
}

std::pair<double, double> Disk::GetSurfaceParameters(const Vector3D& point) const {
}

void Disk::PrintInfo() const {
    PrintInfoBase();
    std::cout << "\tRadius:\t" << mRadius << std::endl
              << std::endl;
}

}  // namespace Raytracer::Geometry
