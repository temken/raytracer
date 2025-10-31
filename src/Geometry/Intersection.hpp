#pragma once

#include "Geometry/Vector.hpp"

#include <limits>

namespace Raytracer::Geometry {

struct Intersection {
    double t = std::numeric_limits<double>::infinity();  // parameter/distance along line
    Vector3D point;                                      // world-space intersection point
    Vector3D normal;                                     // surface normal at intersection

    bool operator<(const Intersection& other) const {
        return t < other.t;
    }

    bool operator>(const Intersection& other) const {
        return t > other.t;
    }
};

}  // namespace Raytracer::Geometry
