#pragma once

#include "Scene/ObjectComposite.hpp"

namespace Raytracer::Items {

ObjectComposite CreateGlobus(double referenceLength, const Vector3D& position, const Vector3D& orientation, const Vector3D& referenceDirection);

}  // namespace Raytracer::Items
