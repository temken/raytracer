#pragma once

#include "Geometry/Intersection.hpp"
#include "Geometry/Vector.hpp"

#include <limits>

namespace Raytracer {

class Object;

struct HitRecord : public Geometry::Intersection {
    Object* object;  // pointer to the object at the hit point
};

}  // namespace Raytracer
