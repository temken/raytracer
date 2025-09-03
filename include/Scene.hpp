#pragma once

#include "Intersection.hpp"
#include "Object.hpp"
#include "Ray.hpp"

#include <memory>
#include <vector>

namespace Raytracer {

class Scene {
public:
    Scene(const Color& backgroundColor = Color(0, 0, 0));

    void AddObject(std::unique_ptr<Object> object);
    const std::vector<std::unique_ptr<Object>>& GetObjects() const;
    Color GetBackgroundColor() const;

private:
    std::vector<std::unique_ptr<Object>> mObjects;
    Color mBackgroundColor;
};

}  // namespace Raytracer
