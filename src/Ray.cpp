#include "Rendering/Ray.hpp"

namespace Raytracer {

Vector3D Ray::GetOrigin() const {
    return origin;
}
Vector3D Ray::GetDirection() const {
    return direction;
}

void Ray::SetOrigin(const Vector3D& newOrigin) {
    origin = newOrigin;
}
void Ray::SetDirection(const Vector3D& newDirection) {
    direction = newDirection.Normalized();
}

Vector3D Ray::PointAtParameter(double t) const {
    return origin + t * direction;
}

}  // namespace Raytracer
