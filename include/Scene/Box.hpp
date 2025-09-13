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
    Box(const std::string& name, const Vector3D& center, double length, double width, double height, const std::array<Color, 6>& colors, const std::array<bool, 6>& reflectives);
    Box(const std::string& name, const Vector3D& center, double length, double width, double height, const std::array<Color, 6>& colors, bool reflective = false);
    Box(const std::string& name, const Vector3D& center, double length, double width, double height, const Color& color, bool reflective = false);
    Box(const std::string& name, const Vector3D& center, double size, const Color& color);
    Box(const std::string& name, const Vector3D& center, double size, const std::array<Color, 6>& colors);

    virtual std::optional<Intersection> Intersect(const Ray& ray) const override;

    virtual void PrintInfo() const override;

private:
    Vector3D mCenter;
    std::vector<Rectangle> mFaces = {};
    double mLength, mWidth, mHeight;
};

}  // namespace Raytracer
