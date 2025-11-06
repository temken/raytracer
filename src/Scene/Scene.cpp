#include "Scene/Scene.hpp"

namespace Raytracer {

Scene::Scene(const Color& backgroundColor) :
    mBackgroundColor(backgroundColor) {
}

void Scene::AddObject(std::shared_ptr<Object> object) {
    mObjects.push_back(object);
    if (object->EmitsLight()) {
        mLightSources.push_back(object);
    }
    if (object->IsDynamic()) {
        mDynamicObjects.push_back(object);
    }
}

const std::vector<std::shared_ptr<Object>>& Scene::GetObjects() const {
    return mObjects;
}

const std::vector<std::shared_ptr<Object>>& Scene::GetLightSources() const {
    return mLightSources;
}

Color Scene::GetBackgroundColor() const {
    return mBackgroundColor;
}

bool Scene::IsDynamic() const {
    return !mDynamicObjects.empty();
}

void Scene::Evolve(double timeStep) {
    for (auto& object : mDynamicObjects) {
        object->Evolve(timeStep);
    }
}

size_t Scene::NumberOfObjects() const {
    return mObjects.size();
}

size_t Scene::NumberOfLightSources() const {
    return mLightSources.size();
}

void Scene::PrintInfo() const {
    std::cout << "Scene Information:" << std::endl
              << "-------------------" << std::endl
              << "Background Color:\t" << mBackgroundColor << std::endl
              << "Objects in Scene:\t" << NumberOfObjects() << std::endl
              << "Light Sources in Scene:\t" << NumberOfLightSources() << std::endl;
    if (IsDynamic()) {
        std::cout << "Dynamic Objects in Scene:\t" << mDynamicObjects.size() << std::endl;
    }
    std::cout << std::endl;
}

}  // namespace Raytracer
