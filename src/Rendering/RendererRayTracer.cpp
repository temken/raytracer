#include "Rendering/RendererRayTracer.hpp"

namespace Raytracer {

RendererRayTracer::RendererRayTracer() :
    Renderer(Type::RAY_TRACER, false) {
}

Color RendererRayTracer::TraceRay(Ray ray, const Scene& scene) {
    while (ray.GetDepth() < kMaximumDepth) {
        auto intersection = Intersect(ray, scene);
        if (!intersection.has_value()) {
            return scene.GetBackgroundColor();
        }

        auto& material = intersection->object->GetMaterial();

        if (material.EmitsLight()) {
            ray.AddRadiance(ray.GetThroughput() * material.GetEmission());
            break;
        }

        auto interactionType = material.Interact(ray, intersection.value());

        if (interactionType == Material::InteractionType::DIFFUSE) {
            CollectDirectLighting(ray, scene, intersection.value(), kNumLightSamples);
            break;
        }
    }

    return ray.GetRadiance();
}

}  // namespace Raytracer
