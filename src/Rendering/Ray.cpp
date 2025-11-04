#include "Rendering/Ray.hpp"

namespace Raytracer {

Ray::Ray(const Vector3D& origin, const Vector3D& direction) :
    Line(origin, direction.Normalized(), 0.0) {}

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

double Ray::IncidentAngleCosine(const Vector3D& normal) const {
    return -1.0 * mDirection.Dot(normal.Normalized());
}

bool Ray::IsEntering(const Vector3D& normal) const {
    return IncidentAngleCosine(normal) > 0.0;
}

}  // namespace Raytracer
