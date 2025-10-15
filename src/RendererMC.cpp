#include "Rendering/RendererMC.hpp"

namespace Raytracer {

RendererMC::RendererMC() :
    Renderer(Type::MONTE_CARLO, false) {
}

Color RendererMC::TraceRay(Ray ray, const Scene& scene) {
    size_t interactions = 0;
    while (interactions < mMaximumInteractions) {
        auto intersection = Intersect(ray, scene);
        if (!intersection.has_value()) {
            return ray.GetColor() * scene.GetBackgroundColor();
        }

        intersection->object->GetMaterial().Interact(ray, intersection.value());

        if (intersection->object->GetMaterial().EmitsLight()) {
            break;
        }

        // Russian roulette after a few bounces
        if (interactions >= 3) {
            Color throughput = ray.GetColor();
            double p = std::max({throughput.R(), throughput.G(), throughput.B()});
            p = std::clamp(p, 0.05, 0.95);
            if (mDistribution(mGenerator) > p) {
                return BLACK;
            }
            throughput = throughput * (1.0 / p);
            ray.SetColor(throughput);
        }
        interactions++;
    }
    return ray.GetRadiance() * ray.GetColor();
}

}  // namespace Raytracer
