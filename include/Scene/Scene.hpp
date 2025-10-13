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

    void AddObject(std::unique_ptr<Object> object);
    const std::vector<std::unique_ptr<Object>>& GetObjects() const;
    Color GetBackgroundColor() const;

    void Evolve(double timeStep);

    size_t NumberOfObjects() const;
    size_t NumberOfLightSources() const;

    void PrintInfo() const;

private:
    std::vector<std::unique_ptr<Object>> mObjects;
    Color mBackgroundColor;
};

}  // namespace Raytracer
