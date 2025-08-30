#pragma once

#include "Intersection.hpp"
#include "Object.hpp"
#include "Ray.hpp"

#include <memory>
#include <vector>

namespace Raytracer {

class Scene {
public:
    Scene(const Color& backgroundColor = Color(0, 0, 0));

    void AddObject(std::unique_ptr<Object> object);

    Color TraceRay(const Ray& ray) const;

private:
    std::vector<std::unique_ptr<Object>> mObjects;
    Color mBackgroundColor;

    std::optional<Intersection> Intersect(const Ray& ray, double epsilon = 1e-6) const;
};

}  // namespace Raytracer
