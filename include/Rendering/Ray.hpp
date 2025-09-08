#pragma once

#include "Utilities/Vector.hpp"

namespace Raytracer {

class Ray {
public:
    Ray() = default;
    Ray(const Vector3D& origin, const Vector3D& direction) :
        origin(origin),
        direction(direction.Normalized()) {}

    Vector3D GetOrigin() const;
    Vector3D GetDirection() const;

    void SetOrigin(const Vector3D& newOrigin);
    void SetDirection(const Vector3D& newDirection);

    Vector3D PointAtParameter(double t) const;

    Vector3D operator()(double t) const {
        return PointAtParameter(t);
    }

    bool operator==(const Ray& other) const {
        return origin == other.origin && direction == other.direction;
    }

    // stream
    friend std::ostream& operator<<(std::ostream& os, const Ray& ray) {
        os << ray.origin << " + t * " << ray.direction;
        return os;
    }

private:
    Vector3D origin = Vector3D({0.0, 0.0, 0.0});
    Vector3D direction = Vector3D({1.0, 0.0, 0.0});
};

}  // namespace Raytracer
