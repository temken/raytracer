#pragma once

#include "Rendering/Ray.hpp"
#include "Scene/Object.hpp"
#include "Utilities/Color.hpp"
#include "Utilities/Vector.hpp"

#include <optional>
#include <string>

namespace Raytracer {

class Torus : public Object {
public:
    Torus(const std::string& name, const Material& material, const Vector3D& center, const Vector3D& normal, double innerRadius, double outerRadius);

    virtual std::optional<Intersection> Intersect(const Ray& ray) override;

    virtual void PrintInfo() const override;

private:
    double mInnerRadius;
    double mOuterRadius;

    static int SolveQuadratic(const double coeffs[3], std::array<double, 2>& roots);
    static int SolveCubic(const double coeffs[4], std::array<double, 3>& roots);
    static int SolveQuartic(const double coeffs[5], std::array<double, 4>& roots);
};

}  // namespace Raytracer
