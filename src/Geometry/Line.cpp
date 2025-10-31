#include "Geometry/Line.hpp"

namespace Raytracer::Geometry {

Line::Line(const Vector3D& origin, const Vector3D& direction, double tMin) :
    mOrigin(origin),
    mDirection(direction.Normalized()),
    tMin(tMin) {
}

Vector3D Line::GetOrigin() const {
    return mOrigin;
}

Vector3D Line::GetDirection() const {
    return mDirection;
}

void Line::SetOrigin(const Vector3D& newOrigin) {
    mOrigin = newOrigin;
}

void Line::SetDirection(const Vector3D& newDirection) {
    mDirection = newDirection.Normalized();
}

Vector3D Line::PointAtParameter(double t) const {
    if (t < tMin) {
        throw std::out_of_range("Parameter t is less than tMin");
    }
    return mOrigin + t * mDirection;
}

// Operators
Vector3D Line::operator()(double t) const {
    return PointAtParameter(t);
}

bool Line::operator==(const Line& other) const {
    return mOrigin == other.mOrigin && mDirection == other.mDirection;
}

// stream
std::ostream& operator<<(std::ostream& os, const Line& line) {
    os << line.GetOrigin() << " + t * " << line.GetDirection();
    return os;
}

}  // namespace Raytracer::Geometry
