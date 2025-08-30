#pragma once

#include "Color.hpp"
#include "Intersection.hpp"
#include "Ray.hpp"
#include "Vector.hpp"

#include <optional>

namespace Raytracer {

struct Intersection;

class Object {
public:
    explicit Object(const Color& color) :
        mColor(color) {}
    virtual ~Object() = default;

    Color GetColor() const {
        return mColor;
    }
    bool IsVisible() const {
        return mVisible;
    }
    void SetVisible(bool visible) {
        mVisible = visible;
    }

    virtual std::optional<Intersection> Intersect(const Ray& ray) const = 0;

protected:
    Color mColor;
    bool mVisible = true;
};

}  // namespace Raytracer
