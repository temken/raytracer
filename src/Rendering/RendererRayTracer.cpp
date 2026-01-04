#include "Rendering/RendererRayTracer.hpp"

namespace Raytracer {

RendererRayTracer::RendererRayTracer() :
    Renderer(Type::RAY_TRACER, false) {
}

Color RendererRayTracer::TraceRay(Ray ray, const Scene& scene) {
    while (ray.GetDepth() < kMaximumDepth) {
        auto intersection = Intersect(ray, scene);
        if (!intersection.has_value()) {
            return scene.GetBackgroundColor(ray);
        }

        auto& material = intersection->object->GetMaterial();

        if (material.EmitsLight()) {
            ray.UpdateThroughput(material.GetColor(intersection.value()));
            ray.AddRadiance(ray.GetThroughput() * material.GetEmission());
            break;
        }

        Color throughputBefore = ray.GetThroughput();
        auto interactionType = material.Interact(ray, intersection.value());

        if (interactionType == Material::InteractionType::DIFFUSE) {
            CollectDirectLighting(ray, scene, intersection.value(), throughputBefore, kNumLightSamples);
            break;
        }
    }

    return ray.GetRadiance();
}

}  // namespace Raytracer
