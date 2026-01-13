#pragma once

#include "Scene/ObjectComposite.hpp"

namespace Raytracer::Items {

ObjectComposite CreateGlass(double referenceLength, const Vector3D& position, const Vector3D& orientation, const Vector3D& referenceDirection, Color glassColor = WHITE, double fillingFraction = 0.0, Color fillingColor = WHITE);

}  // namespace Raytracer::Items
