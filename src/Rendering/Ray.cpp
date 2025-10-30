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

Color Ray::GetRadiance() const {
    return mRadiance;
}

void Ray::AddRadiance(const Color& contribution) {
    mRadiance += contribution;
}

Color Ray::GetThroughput() const {
    return mThroughput;
}

void Ray::UpdateThroughput(const Color& throughput) {
    mThroughput *= throughput;
}

void Ray::UpdateThroughput(double scalar) {
    mThroughput = mThroughput * scalar;
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

double Ray::IncidentAngleCosine(const Vector3D& normal) const {
    return -1.0 * mDirection.Dot(normal.Normalized());
}

bool Ray::IsEntering(const Vector3D& normal) const {
    return IncidentAngleCosine(normal) > 0.0;
}

}  // namespace Raytracer
