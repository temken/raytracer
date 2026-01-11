#include "Rendering/RendererPathTracerNEE.hpp"

namespace Raytracer {

RendererPathTracerNEE::RendererPathTracerNEE() :
    Renderer(Type::PATH_TRACER_NEE, false) {
}

Color RendererPathTracerNEE::TraceRay(Ray ray, const Scene& scene) {
    bool hadDiffuseInteraction = false;

    while (ray.GetDepth() < kMaximumDepth) {
        auto intersection = Intersect(ray, scene);
        if (!intersection.has_value()) {
            return ray.GetRadiance() + ray.GetThroughput() * scene.GetBackgroundColor(ray);
        }
        auto& material = intersection->object->GetMaterial();
        // Capture throughput before material interaction for correct direct lighting
        Color throughputBefore = ray.GetThroughput();

        // Add light emission - avoid double-counting with NEE
        if (material.EmitsLight()) {
            // Add emission only if we've never had a diffuse bounce (no NEE sampling yet)
            if (!hadDiffuseInteraction) {
                ray.AddRadiance(ray.GetThroughput() * material.GetEmission());
            }
            break;
        }

        auto interactionType = material.Interact(ray, intersection.value());
        if (interactionType == Material::InteractionType::DIFFUSE) {
            CollectDirectLighting(ray, scene, intersection.value(), throughputBefore, kNumLightSamples);
            hadDiffuseInteraction = true;
        }

        // Russian roulette after a few bounces
        if (ray.GetDepth() >= 3) {
            Color throughput = ray.GetThroughput();
            double luminance = throughput.Luminance();
            double p = std::clamp(luminance, 0.1, 0.95);
            if (mDistribution(mGenerator) > p) {
                break;
            }
            ray.UpdateThroughput(1.0 / p);
        }
    }
    return ray.GetRadiance();
}

}  // namespace Raytracer
