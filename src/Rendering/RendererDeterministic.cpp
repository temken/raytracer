#include "Rendering/RendererDeterministic.hpp"

namespace Raytracer {

RendererDeterministic::RendererDeterministic() :
    Renderer(Type::DETERMINISTIC, true) {
    kAmbientFactor = 3e-3;
}

Color RendererDeterministic::TraceRay(Ray ray, const Scene& scene) {
    while (ray.GetDepth() < kMaximumDepth) {
        auto intersection = Intersect(ray, scene);
        if (!intersection.has_value()) {
            return scene.GetBackgroundColor();
        }

        auto& material = intersection->object->GetMaterial();
        if (material.EmitsLight()) {
            return ray.GetThroughput() * material.GetEmission();
        }

        Material::InteractionType mostLikelyInteraction = material.MostLikelyInteraction();
        const bool applyRoughness = !mIsDeterministic;
        switch (mostLikelyInteraction) {
            case Material::InteractionType::DIFFUSE:
                material.Diffuse(ray, intersection.value());
                CollectDirectLighting(ray, scene, intersection.value());
                return ray.GetRadiance();
            case Material::InteractionType::REFLECTIVE:
                material.Reflect(ray, intersection.value(), applyRoughness);
                break;
            case Material::InteractionType::REFRACTIVE:
                material.Refract(ray, intersection.value(), applyRoughness);
                break;
        }
    }
    return ray.GetRadiance();
}

}  // namespace Raytracer
