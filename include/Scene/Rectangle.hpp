#pragma once

#include "Rendering/Ray.hpp"
#include "Scene/Object.hpp"
#include "Utilities/Color.hpp"
#include "Utilities/Texture.hpp"
#include "Utilities/Vector.hpp"

#include <optional>

namespace Raytracer {

class Rectangle : public Object {
public:
    Rectangle(const std::string& name, const Material& material, const Vector3D& center, const Vector3D& normal, double width, double height);

    virtual std::optional<Intersection> Intersect(const Ray& ray) override;

    virtual std::pair<double, double> GetTextureCoordinates(const Vector3D& hitPoint) const override;

    virtual void PrintInfo() const override;

private:
    Vector3D mU;  // Local x-axis in the rectangle plane
    Vector3D mV;  // Local y-axis in the rectangle plane
    double mWidth;
    double mHeight;
};

}  // namespace Raytracer
