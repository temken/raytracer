#include "Scene.hpp"

namespace Raytracer {

Scene::Scene(const Color& backgroundColor) :
    mBackgroundColor(backgroundColor) {
}

void Scene::AddObject(std::unique_ptr<Object> object) {
    mObjects.push_back(std::move(object));
}

Color Scene::TraceRay(const Ray& ray) const {
    auto intersection = Intersect(ray);
    if (intersection) {
        return intersection->object->GetColor();
    }
    return mBackgroundColor;
}

std::optional<Intersection> Scene::Intersect(const Ray& ray, double epsilon) const {
    std::optional<Intersection> closestIntersection;
    for (const auto& object : mObjects) {
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
