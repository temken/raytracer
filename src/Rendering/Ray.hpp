#pragma once

#include "Utilities/Color.hpp"
#include "Utilities/Vector.hpp"

namespace Raytracer {

class Ray {
public:
    Ray() = default;
    Ray(const Vector3D& origin, const Vector3D& direction) :
        mOrigin(origin),
        mDirection(direction.Normalized()) {}

    Vector3D GetOrigin() const;
    Vector3D GetDirection() const;

    void SetOrigin(const Vector3D& newOrigin);
    void SetDirection(const Vector3D& newDirection);

    Color GetRadiance() const;
    void AddRadiance(const Color& contribution);

    Color GetThroughput() const;
    void UpdateThroughput(const Color& throughput);
    void UpdateThroughput(double scalar);

    size_t GetDepth() const;
    void IncrementDepth();

    Vector3D PointAtParameter(double t) const;

    Vector3D operator()(double t) const {
        return PointAtParameter(t);
    }

    bool operator==(const Ray& other) const {
        return mOrigin == other.mOrigin && mDirection == other.mDirection;
    }

    // stream
    friend std::ostream& operator<<(std::ostream& os, const Ray& ray) {
        os << ray.mOrigin << " + t * " << ray.mDirection;
        return os;
    }

private:
    Vector3D mOrigin = Vector3D({0.0, 0.0, 0.0});
    Vector3D mDirection = Vector3D({1.0, 0.0, 0.0});

    Color mRadiance = Color(0.0, 0.0, 0.0);    // Accumulated radiance
    Color mThroughput = Color(1.0, 1.0, 1.0);  // Monte Carlo weighting

    size_t mDepth = 0;  // Number of interactions
};

}  // namespace Raytracer
