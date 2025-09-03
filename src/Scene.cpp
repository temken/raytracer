#include "Scene.hpp"

namespace Raytracer {

Scene::Scene(const Color& backgroundColor) :
    mBackgroundColor(backgroundColor) {
}

void Scene::AddObject(std::unique_ptr<Object> object) {
    mObjects.push_back(std::move(object));
}

const std::vector<std::unique_ptr<Object>>& Scene::GetObjects() const {
    return mObjects;
}

Color Scene::GetBackgroundColor() const {
    return mBackgroundColor;
}

}  // namespace Raytracer
