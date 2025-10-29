#include "Rendering/RendererPathTracer.hpp"

namespace Raytracer {

RendererPathTracer::RendererPathTracer() :
    Renderer(Type::PATH_TRACER, false) {
}

Color RendererPathTracer::TraceRay(Ray ray, const Scene& scene) {
    while (ray.GetDepth() < kMaximumDepth) {
        auto intersection = Intersect(ray, scene);
        if (!intersection.has_value()) {
            // Hit background - multiply background color by current throughput
            return ray.GetRadiance() + ray.GetThroughput() * scene.GetBackgroundColor();
        }
        auto& material = intersection->object->GetMaterial();

        if (material.EmitsLight()) {
            // Add emitted light weighted by current throughput
            ray.AddRadiance(ray.GetThroughput() * material.GetEmission());
            break;
        }

        material.Interact(ray, intersection.value());

        // Russian roulette after a few bounces
        if (ray.GetDepth() >= 3) {
            Color throughput = ray.GetThroughput();
            double p = std::max({throughput.R(), throughput.G(), throughput.B()});
            p = std::clamp(p, 0.05, 0.95);
            if (mDistribution(mGenerator) > p) {
                break;
            }
            ray.UpdateThroughput(1.0 / p);
        }
    }
    return ray.GetRadiance();
}

}  // namespace Raytracer
