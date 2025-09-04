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

void Scene::PrintSceneInfo() const {
    std::cout << "Scene Information:" << std::endl;
    std::cout << "Background Color: " << mBackgroundColor << std::endl;
    std::cout << "Objects in Scene: " << mObjects.size() << std::endl;
    for (const auto& object : mObjects) {
        object->PrintInfo();
    }
    std::cout << std::endl;
}

}  // namespace Raytracer
