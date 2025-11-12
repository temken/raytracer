#pragma once

#include "Rendering/Renderer.hpp"

#include <random>

namespace Raytracer {

class RendererPathTracerNEE : public Renderer {
public:
    RendererPathTracerNEE();

    virtual Color TraceRay(Ray ray, const Scene& scene) override;

private:
    std::uniform_real_distribution<double> mDistribution{0.0, 1.0};
    static constexpr size_t kNumLightSamples = 5;
};

}  // namespace Raytracer
