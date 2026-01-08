#include "Rendering/RendererDeterministic.hpp"

namespace Raytracer {

RendererDeterministic::RendererDeterministic() :
    Renderer(Type::DETERMINISTIC, true) {
    kAmbientFactor = 3e-2;
}

Color RendererDeterministic::TraceRay(Ray ray, const Scene& scene) {
    while (ray.GetDepth() < kMaximumDepth) {
        auto intersection = Intersect(ray, scene);
        if (!intersection.has_value()) {
            return scene.GetBackgroundColor(ray);
        }

        auto& material = intersection->object->GetMaterial();
        if (material.EmitsLight()) {
            ray.UpdateThroughput(material.GetColor(intersection.value()));
            return ray.GetThroughput() * material.GetEmission();
        }

        Material::InteractionType mostLikelyInteraction = material.MostLikelyInteraction();
        const bool applyRoughness = !mIsDeterministic;
        switch (mostLikelyInteraction) {
            case Material::InteractionType::DIFFUSE: {
                Color throughputBefore = ray.GetThroughput();
                material.Diffuse(ray, intersection.value());
                CollectDirectLighting(ray, scene, intersection.value(), throughputBefore);
                return ray.GetRadiance();
            }
            case Material::InteractionType::REFLECTIVE: {
                material.Reflect(ray, intersection.value(), applyRoughness);
                break;
            }
            case Material::InteractionType::REFRACTIVE: {
                material.Refract(ray, intersection.value(), applyRoughness);
                break;
            }
        }
    }
    return ray.GetRadiance();
}

}  // namespace Raytracer
