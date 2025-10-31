#pragma once

#include "Rendering/Ray.hpp"
#include "Scene/Scene.hpp"
#include "Utilities/Color.hpp"
#include "Utilities/Intersection.hpp"

#include <optional>
#include <random>
#include <string>

namespace Raytracer {

class Renderer {
public:
    enum class Type {
        SIMPLE,
        DETERMINISTIC,
        RAY_TRACER,
        PATH_TRACER,
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
    const size_t kMaximumDepth = 10;  // Maximum recursion depth for rays
    double kAmbientFactor = 0.0;

    static constexpr double kEpsilon = 1e-6;
    std::mt19937 mGenerator{std::random_device{}()};

    virtual std::optional<Intersection> Intersect(const Ray& ray, const Scene& scene);

    void CollectDirectLighting(Ray& ray, const Scene& scene, const Intersection& intersection, std::size_t numLightSamples = 0);
};

}  // namespace Raytracer
