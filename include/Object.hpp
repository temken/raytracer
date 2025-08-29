#pragma once

#include "Color.hpp"
#include "Ray.hpp"
#include "Vector.hpp"

#include <optional>

namespace Raytracer {

class Object;

struct Intersection {
    double t;         // distance along ray
    Vector3D point;   // world-space hit point
    Vector3D normal;  // surface normal at hit
    Object* object;   // pointer to the object hit

    bool operator<(const Intersection& other) const {
        return t < other.t;
    }

    bool operator>(const Intersection& other) const {
        return t > other.t;
    }
};

class Object {
public:
    explicit Object(const Color& color) :
        mColor(color) {}
    virtual ~Object() = default;

    Color GetColor() const {
        return mColor;
    }
    bool IsVisible() const {
        return mVisible;
    }
    void SetVisible(bool visible) {
        mVisible = visible;
    }

    virtual std::optional<Intersection> Intersect(const Ray& ray) const = 0;

protected:
    Color mColor;
    bool mVisible = true;
};

}  // namespace Raytracer
