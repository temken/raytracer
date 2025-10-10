#pragma once

#include "Rendering/Ray.hpp"
#include "Scene/Object.hpp"
#include "Scene/Rectangle.hpp"
#include "Utilities/Color.hpp"
#include "Utilities/Vector.hpp"

#include <optional>
#include <vector>

namespace Raytracer {

class Box : public Object {
public:
    // Constructors for Box with different parameters
    Box(const std::string& name, const Material& material, const Vector3D& center, double length, double width, double height);

    virtual std::optional<Intersection> Intersect(const Ray& ray) override;

    virtual void PrintInfo() const override;

private:
    std::vector<Rectangle> mFaces = {};
    double mLength, mWidth, mHeight;
};

}  // namespace Raytracer
