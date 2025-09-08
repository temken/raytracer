#include "Rendering/Renderer.hpp"

namespace Raytracer {

Color Renderer::TraceRay(const Ray& ray, const Scene& scene) {
    auto intersection = Intersect(ray, scene);
    if (intersection) {
        if (intersection->object && intersection->object->IsReflective()) {
            Vector3D reflectDir = ray.GetDirection() - 2 * ray.GetDirection().Dot(intersection->normal) * intersection->normal;
            Ray reflectRay(intersection->point, reflectDir);
            return TraceRay(reflectRay, scene);
        } else {
            return intersection->object->GetColor();
        }
    }
    return scene.GetBackgroundColor();
}

bool Renderer::IsDeterministic() const {
    return mIsDeterministic;
}

std::optional<Intersection> Renderer::Intersect(const Ray& ray, const Scene& scene, double epsilon) {
    std::optional<Intersection> closestIntersection;
    for (const auto& object : scene.GetObjects()) {
        if (!object || !object->IsVisible()) {
            continue;
        }

        if (auto intersection = object->Intersect(ray)) {
            if (intersection->t > epsilon && (!closestIntersection || intersection->t < closestIntersection->t)) {
                closestIntersection.emplace(*intersection);
            }
        }
    }
    return closestIntersection;
}

}  // namespace Raytracer
