#pragma once

#include "Rendering/Renderer.hpp"

#include <random>

namespace Raytracer {

class RendererPathTracer : public Renderer {
public:
    RendererPathTracer();

    virtual Color TraceRay(Ray ray, const Scene& scene) override;

private:
    std::mt19937 mGenerator{std::random_device{}()};
    std::uniform_real_distribution<double> mDistribution{0.0, 1.0};
};

}  // namespace Raytracer
