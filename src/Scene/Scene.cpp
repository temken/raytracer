#include "Scene/Scene.hpp"

#include "Geometry/OrthonormalBasis.hpp"
#include "Geometry/Shapes/Sphere.hpp"
#include "Version.hpp"

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

Color Scene::GetBackgroundColor(const Ray& ray) const {
    if (mBackgroundTexture) {
        Vector3D direction = ray.GetDirection().Normalized();
        auto uv = Geometry::Sphere::GetSurfaceParameters(direction, Vector3D({0, 0, 0}), Geometry::OrthonormalBasis(Vector3D({0, 0, 1})));

        // Convert from [-0.5, 0.5] range to [0, 1] range and flip horizontal
        // TODO: Fix different ranges
        double u = 0.5 - uv.first;  // Flip horizontal direction
        double v = uv.second + 0.5;
        return mBackgroundTexture->GetColorAt(u, v);
    }
    return mBackgroundColor;
}
void Scene::SetColorTexture(std::string filename) {
    std::string filepath = TOP_LEVEL_DIR "/textures/" + filename;
    mBackgroundTexture = Texture(filepath);
}

bool Scene::IsDynamic() const {
    return !mDynamicObjects.empty();
}

void Scene::Evolve(double timeStep) {
    for (auto& object : mDynamicObjects) {
        object->Evolve(timeStep);
    }
    mTime += timeStep;
}

double Scene::GetTime() const {
    return mTime;
}

void Scene::SetTime(double time) {
    double deltaTime = time - mTime;
    Evolve(deltaTime);
    mTime = time;
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
        std::cout << "Time:\t\t\t" << mTime << std::endl;
        std::cout << "Dynamic Objects in Scene:\t" << mDynamicObjects.size() << std::endl;
    }
    std::cout << std::endl;
}

}  // namespace Raytracer
