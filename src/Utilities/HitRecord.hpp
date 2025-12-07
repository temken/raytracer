#pragma once

#include "Geometry/Intersection.hpp"
#include "Geometry/Vector.hpp"

#include <limits>
#include <memory>

namespace Raytracer {

class Object;

struct HitRecord : public Geometry::Intersection {
    std::shared_ptr<Object> object;  // shared pointer to the object at the hit point
};

}  // namespace Raytracer
