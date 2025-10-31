#pragma once

#include "Geometry/Vector.hpp"

#include <limits>

namespace Raytracer {

class Object;

struct Intersection {
    double t = std::numeric_limits<double>::infinity();  // distance along ray
    Vector3D point;                                      // world-space hit point
    Vector3D normal;                                     // surface normal at hit
    Object* object;                                      // pointer to the object at the hit point

    bool operator<(const Intersection& other) const {
        return t < other.t;
    }

    bool operator>(const Intersection& other) const {
        return t > other.t;
    }
};

}  // namespace Raytracer
