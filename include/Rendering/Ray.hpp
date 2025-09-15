#pragma once

#include "Utilities/Color.hpp"
#include "Utilities/Vector.hpp"

namespace Raytracer {

class Ray {
public:
    Ray() = default;
    Ray(const Vector3D& origin, const Vector3D& direction, const Color& color = Color(1.0, 1.0, 1.0, 1.0)) :
        mOrigin(origin),
        mDirection(direction.Normalized()),
        mColor(color) {}

    Vector3D GetOrigin() const;
    Vector3D GetDirection() const;
    Color GetColor() const;

    void SetOrigin(const Vector3D& newOrigin);
    void SetDirection(const Vector3D& newDirection);

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
    Color mColor = Color(1.0, 1.0, 1.0, 1.0);
};

}  // namespace Raytracer
