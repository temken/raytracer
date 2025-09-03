#include "Renderer.hpp"

namespace Raytracer {

Color Renderer::TraceRay(const Ray& ray, const Scene& scene) {
    auto intersection = Intersect(ray, scene);
    if (intersection) {
        return intersection->object->GetColor();
    }
    return scene.GetBackgroundColor();
}

std::optional<Intersection> Renderer::Intersect(const Ray& ray, const Scene& scene, double epsilon) {
    std::optional<Intersection> closestIntersection;
    for (const auto& object : scene.GetObjects()) {
        if (!object || !object->IsVisible()) {
            continue;
        }

        if (auto intersection = object->Intersect(ray)) {
            if (!closestIntersection && intersection->t < closestIntersection->t && intersection->t > epsilon) {
                closestIntersection.emplace(*intersection);
            }
        }
    }
    return closestIntersection;
}

}  // namespace Raytracer
