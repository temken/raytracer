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

Color Ray::GetColor() const {
    return mColor;
}

void Ray::SetColor(const Color& color) {
    mColor = color;
}

void Ray::MultiplyColor(const Color& color) {
    mColor = mColor * color;
}

double Ray::GetRadiance() const {
    return mRadiance;
}

void Ray::SetRadiance(double radiance) {
    mRadiance = radiance;
}

void Ray::AddRadiance(double radiance) {
    mRadiance += radiance;
}

size_t Ray::GetDepth() const {
    return mDepth;
}

void Ray::IncrementDepth() {
    mDepth++;
}

Vector3D Ray::PointAtParameter(double t) const {
    return mOrigin + t * mDirection;
}

}  // namespace Raytracer
