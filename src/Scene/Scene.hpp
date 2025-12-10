#pragma once

#include "Rendering/Ray.hpp"
#include "Scene/Object.hpp"
#include "Scene/ObjectPrimitive.hpp"
#include "Utilities/Texture.hpp"

#include <memory>
#include <vector>

namespace Raytracer {

class Scene {
public:
    Scene(const Color& backgroundColor = BLACK);

    void AddObject(std::shared_ptr<Object> object);

    const std::vector<std::shared_ptr<Object>>& GetObjects() const;
    const std::vector<std::shared_ptr<ObjectPrimitive>>& GetLightSources() const;

    Color GetBackgroundColor(const Ray& ray) const;
    void SetColorTexture(std::string filename);

    bool IsDynamic() const;
    void Evolve(double timeStep);

    double GetTime() const;
    void SetTime(double time);

    size_t NumberOfObjects() const;
    size_t NumberOfLightSources() const;

    void PrintInfo() const;

private:
    std::vector<std::shared_ptr<Object>> mObjects;
    std::vector<std::shared_ptr<ObjectPrimitive>> mLightSources;
    std::vector<std::shared_ptr<Object>> mDynamicObjects;

    // Background
    Color mBackgroundColor;
    std::optional<Texture> mBackgroundTexture = std::nullopt;

    double mTime = 0.0;
};

}  // namespace Raytracer
