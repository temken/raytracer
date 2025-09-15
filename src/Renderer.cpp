#include "Rendering/Renderer.hpp"

namespace Raytracer {

Renderer::Renderer(Type type, bool deterministic) :
    mType(type),
    mIsDeterministic(deterministic) {}

bool Renderer::IsDeterministic() const {
    return mIsDeterministic;
}

Renderer::Type Renderer::GetType() const {
    return mType;
}

std::string Renderer::GetTypeString() const {
    switch (mType) {
        case Type::SIMPLE:
            return "Simple";
        // Future renderers can be added here
        default:
            return "Unknown";
    }
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
