#pragma once

#include "Rendering/Ray.hpp"
#include "Scene/Object.hpp"
#include "Utilities/Intersection.hpp"

#include <memory>
#include <vector>

namespace Raytracer {

class Scene {
public:
    Scene(const Color& backgroundColor = BLACK);

    void AddObject(std::shared_ptr<Object> object);

    const std::vector<std::shared_ptr<Object>>& GetObjects() const;
    const std::vector<std::shared_ptr<Object>>& GetLightSources() const;

    Color GetBackgroundColor() const;

    void Evolve(double timeStep);

    size_t NumberOfObjects() const;
    size_t NumberOfLightSources() const;

    void PrintInfo() const;

private:
    std::vector<std::shared_ptr<Object>> mObjects;
    std::vector<std::shared_ptr<Object>> mLightSources;
    Color mBackgroundColor;
};

}  // namespace Raytracer
