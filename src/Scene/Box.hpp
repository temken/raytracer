#pragma once

#include "Rendering/Ray.hpp"
#include "Scene/CompositeObject.hpp"
#include "Scene/Rectangle.hpp"
#include "Utilities/Color.hpp"
#include "Utilities/Vector.hpp"

#include <optional>
#include <vector>

namespace Raytracer {

class Box : public CompositeObject {
public:
    // Constructors for Box with different parameters
    Box(const std::string& name, const Material& material, const Vector3D& center, double length, double width, double height);

    virtual void PrintInfo() const override;

private:
    double mLength, mWidth, mHeight;
};

}  // namespace Raytracer
