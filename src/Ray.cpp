#include "Rendering/Ray.hpp"

namespace Raytracer {

Vector3D Ray::GetOrigin() const {
    return mOrigin;
}
Vector3D Ray::GetDirection() const {
    return mDirection;
}

void Ray::SetOrigin(const Vector3D& newOrigin) {
    mOrigin = newOrigin;
}
void Ray::SetDirection(const Vector3D& newDirection) {
    mDirection = newDirection.Normalized();
}

Vector3D Ray::PointAtParameter(double t) const {
    return mOrigin + t * mDirection;
}

}  // namespace Raytracer
