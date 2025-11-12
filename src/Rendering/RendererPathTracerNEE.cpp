#include "Rendering/RendererPathTracerNEE.hpp"

namespace Raytracer {

RendererPathTracerNEE::RendererPathTracerNEE() :
    Renderer(Type::PATH_TRACER_NEE, false) {
}

Color RendererPathTracerNEE::TraceRay(Ray ray, const Scene& scene) {
    while (ray.GetDepth() < kMaximumDepth) {
        auto intersection = Intersect(ray, scene);
        if (!intersection.has_value()) {
            return ray.GetRadiance() + ray.GetThroughput() * scene.GetBackgroundColor(ray);
        }
        auto& material = intersection->object->GetMaterial();

        if (material.EmitsLight()) {
            // Terminate path to prevent double-counting
            ray.AddRadiance(ray.GetThroughput() * material.GetEmission());
            break;
        }

        auto interactionType = material.Interact(ray, intersection.value());

        // Only use NEE for diffuse interactions. Actually not sure if this is correct.
        if (interactionType == Material::InteractionType::DIFFUSE) {
            CollectDirectLighting(ray, scene, intersection.value(), kNumLightSamples);
        }

        // Russian roulette after a few bounces
        if (ray.GetDepth() >= 3) {
            Color throughput = ray.GetThroughput();
            double luminance = 0.299 * throughput.R() + 0.587 * throughput.G() + 0.114 * throughput.B();
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
