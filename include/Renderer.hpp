#pragma once

#include "Color.hpp"
#include "Intersection.hpp"
#include "Ray.hpp"
#include "Scene.hpp"

namespace Raytracer {

class Renderer {
public:
    static Color TraceRay(const Ray& ray, const Scene& scene);

    bool IsDeterministic() const;

private:
    const bool mIsDeterministic = true;

    static std::optional<Intersection> Intersect(const Ray& ray, const Scene& scene, double epsilon = 1e-6);
};

}  // namespace Raytracer
