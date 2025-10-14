#pragma once

#include "Rendering/Ray.hpp"
#include "Rendering/Renderer.hpp"
#include "Scene/Scene.hpp"
#include "Utilities/Color.hpp"
#include "Utilities/Intersection.hpp"

namespace Raytracer {

class RendererDeterministic : public Renderer {
public:
    RendererDeterministic() :
        Renderer(Type::DETERMINISTIC, true) {};
    virtual Color TraceRay(Ray ray, const Scene& scene) override;

private:
    const bool kApplyRoughness = false;
};

}  // namespace Raytracer
