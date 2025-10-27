#pragma once

#include "Rendering/Renderer.hpp"

namespace Raytracer {

class RendererRayTracer : public Renderer {
public:
    RendererRayTracer();

    virtual Color TraceRay(Ray ray, const Scene& scene) override;

private:
    std::mt19937 mGenerator{std::random_device{}()};
    std::uniform_real_distribution<double> mDistribution{0.0, 1.0};

    static constexpr double kEpsilon = 1e-6;
    static constexpr double kAmbientFactor = 0.1;
    static constexpr size_t kNumLightSamples = 5;

    void CollectDirectLighting(Ray& ray, const Scene& scene, const Intersection& intersection);
};

}  // namespace Raytracer
