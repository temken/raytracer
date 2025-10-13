#pragma once

#include "Rendering/Material.hpp"
#include "Utilities/Vector.hpp"

namespace Raytracer {

class Object;

struct Intersection {
    double t = std::numeric_limits<double>::infinity();  // distance along ray
    Vector3D point;                                      // world-space hit point
    Vector3D normal;                                     // surface normal at hit
    Material* material;                                  // pointer to the material at the hit point

    bool operator<(const Intersection& other) const {
        return t < other.t;
    }

    bool operator>(const Intersection& other) const {
        return t > other.t;
    }
};

}  // namespace Raytracer
