#include "Geometry/Shapes/Rectangle.hpp"

namespace Raytracer::Geometry {

Rectangle::Rectangle(const Vector3D& center, const Vector3D& normal, const Vector3D& widthDirection, double width, double height) :
    Shape(Type::RECTANGLE, center, normal, widthDirection),
    mWidth(width),
    mHeight(height) {
}

std::optional<Intersection> Rectangle::Intersect(const Line& line) const {
    const Vector3D& normal = GetOrthonormalBasis()[2];
    double denom = normal.Dot(line.GetDirection());
    if (std::fabs(denom) < sEpsilon) {
        return std::nullopt;  // Ray is parallel to the rectangle plane
    }

    double t = (mPosition - line.GetOrigin()).Dot(normal) / denom;
    if (t < sEpsilon) {  // Intersection is behind the line origin
        return std::nullopt;
    }

    Vector3D hitPoint = line(t);

    std::pair<double, double> uv = GetSurfaceParameters(hitPoint);

    if (std::fabs(uv.first) <= 0.5 && std::fabs(uv.second) <= 0.5) {
        return Intersection{t, hitPoint, normal};
    }
    return std::nullopt;
}

double Rectangle::SurfaceArea() const {
    return mWidth * mHeight;
}

std::vector<Vector3D> Rectangle::SampleSurfacePoints(std::size_t numPoints, std::mt19937& prng) const {
    std::vector<Vector3D> points;

    std::uniform_real_distribution<double> distU(-mWidth / 2.0, mWidth / 2.0);
    std::uniform_real_distribution<double> distV(-mHeight / 2.0, mHeight / 2.0);

    for (std::size_t i = 0; i < numPoints; ++i) {
        double u = distU(prng);
        double v = distV(prng);
        Vector3D point = mPosition + u * GetOrthonormalBasis()[0] + v * GetOrthonormalBasis()[1];
        points.push_back(point);
    }

    return points;
}

std::vector<Vector3D> Rectangle::GetKeyPoints() const {
    const Vector3D& u = GetOrthonormalBasis()[0];
    const Vector3D& v = GetOrthonormalBasis()[1];
    return {mPosition,
            mPosition - u * (mWidth / 2.0) - v * (mHeight / 2.0),
            mPosition + u * (mWidth / 2.0) - v * (mHeight / 2.0),
            mPosition + u * (mWidth / 2.0) + v * (mHeight / 2.0),
            mPosition - u * (mWidth / 2.0) + v * (mHeight / 2.0)};
}

std::pair<double, double> Rectangle::GetSurfaceParameters(const Vector3D& point) const {
    // Relative to center
    Vector3D localPoint = point - mPosition;

    double uCoord = localPoint.Dot(GetOrthonormalBasis()[0]);
    double vCoord = localPoint.Dot(GetOrthonormalBasis()[1]);

    // Map to [-0.5, 0.5]
    double u = uCoord / mWidth;
    double v = vCoord / mHeight;

    return {u, v};
}

void Rectangle::PrintInfo() const {
    PrintInfoBase();
    std::cout << "\tWidth: " << mWidth << std::endl
              << "\tHeight: " << mHeight << std::endl;
}

}  // namespace Raytracer::Geometry
