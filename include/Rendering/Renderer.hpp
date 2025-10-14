#pragma once

#include "Rendering/Ray.hpp"
#include "Scene/Scene.hpp"
#include "Utilities/Color.hpp"
#include "Utilities/Intersection.hpp"

namespace Raytracer {

class Renderer {
public:
    enum class Type {
        SIMPLE,
        DETERMINISTIC,
        MONTE_CARLO,
        // Future renderers can be added here
    };

    explicit Renderer(Type type, bool deterministic);

    virtual Color TraceRay(Ray ray, const Scene& scene) = 0;

    bool IsDeterministic() const;

    Type GetType() const;
    std::string GetTypeString() const;

protected:
    Type mType;
    bool mIsDeterministic = false;
    virtual std::optional<Intersection> Intersect(const Ray& ray, const Scene& scene, double epsilon = 1e-6);
};

}  // namespace Raytracer
