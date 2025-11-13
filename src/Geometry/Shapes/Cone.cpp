#include "Geometry/Shapes/Cone.hpp"

namespace Raytracer::Geometry {

Cone::Cone(const Vector3D& position, const Vector3D& orientation, double radius, double height) :
    Shape(Shape::Type::CONE, position, orientation),
    mRadius(radius),
    mHeight(height) {}

std::optional<Intersection> Cone::Intersect(const Line& line) const {
}

double Cone::SurfaceArea() const {
    return M_PI * mRadius * std::sqrt(mHeight * mHeight + mRadius * mRadius);
}

std::vector<Vector3D> Cone::SampleSurfacePoints(std::size_t numPoints, std::mt19937& prng) const {
}

std::vector<Vector3D> Cone::GetKeyPoints() const {
    return {mPosition};
}

std::pair<double, double> Cone::GetSurfaceParameters(const Vector3D& point) const {
    return {0.0, 0.0};
}

void Cone::PrintInfo() const {
    PrintInfoBase();
    std::cout << "\tRadius: " << mRadius << std::endl
              << "\tHeight: " << mHeight << std::endl
              << std::endl;
}

}  // namespace Raytracer::Geometry
