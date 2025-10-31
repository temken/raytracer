#include "Geometry/Shapes/Rectangle.hpp"

namespace Raytracer::Geometry {

Rectangle::Rectangle(const Vector3D& center, const Vector3D& normal, const Vector3D& widthDirection, double width, double height) :
    Shape(Type::RECTANGLE, center, normal, widthDirection),
    mWidth(width),
    mHeight(height) {
}

std::optional<Intersection> Rectangle::Intersect(const Line& line) const {
}

double Rectangle::SurfaceArea() const {
}
std::vector<Vector3D> Rectangle::SampleSurfacePoints(std::size_t numPoints, std::mt19937& prng) const {
    std::vector<Vector3D> points;

    std::uniform_real_distribution<double> distU(-mWidth / 2.0, mWidth / 2.0);
    std::uniform_real_distribution<double> distV(-mHeight / 2.0, mHeight / 2.0);

    for (std::size_t i = 0; i < numPoints; ++i) {
        double u = distU(prng);
        double v = distV(prng);
        Vector3D point = mPosition + u * mOrthonormalBasis[0] + v * mOrthonormalBasis[1];
        points.push_back(point);
    }

    return points;
}

std::vector<Vector3D> Rectangle::GetKeyPoints() const {
}
std::pair<double, double> Rectangle::SurfaceParameters(const Vector3D& point) const {
}

void Rectangle::PrintInfo() const {
}

}  // namespace Raytracer::Geometry
