#pragma once

#include "Object.hpp"
#include "Vector.hpp"

namespace Raytracer {

class Object;

struct Intersection {
    double t;              // distance along ray
    Vector3D point;        // world-space hit point
    Vector3D normal;       // surface normal at hit
    const Object* object;  // pointer to the object hit

    bool operator<(const Intersection& other) const {
        return t < other.t;
    }

    bool operator>(const Intersection& other) const {
        return t > other.t;
    }
};

}  // namespace Raytracer
