#include "Geometry/Shapes/BoxAxisAligned.hpp"

#include "Geometry/Shapes/Rectangle.hpp"

namespace Raytracer::Geometry {

BoxAxisAligned::BoxAxisAligned(const Vector3D& center, double length, double width, double height) :
    Box(center, Vector3D({0.0, 0.0, 1.0}), Vector3D({1.0, 0.0, 0.0}), length, width, height) {
    mType = Type::BOX_AXIS_ALIGNED;
}

}  // namespace Raytracer::Geometry
