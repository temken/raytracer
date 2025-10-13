#include "Scene/Scene.hpp"

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

void Scene::Evolve(double timeStep) {
    for (auto& object : mObjects) {
        object->Evolve(timeStep);
    }
}

size_t Scene::NumberOfObjects() const {
    return mObjects.size();
}

size_t Scene::NumberOfLightSources() const {
    size_t count = 0;
    for (const auto& object : mObjects) {
        if (object->EmitsLight()) {
            count++;
        }
    }
    return count;
}

void Scene::PrintInfo() const {
    std::cout << "Scene Information:" << std::endl
              << "-------------------" << std::endl
              << "Background Color:\t" << mBackgroundColor << std::endl
              << "Objects in Scene:\t" << NumberOfObjects() << std::endl
              << "Light Sources in Scene:\t" << NumberOfLightSources() << std::endl
              << std::endl;
}

}  // namespace Raytracer
