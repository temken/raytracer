#include "Rendering/RendererDeterministic.hpp"

namespace Raytracer {

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
        switch (mostLikelyInteraction) {
            case Material::InteractionType::DIFFUSE:
                material.Diffuse(ray, intersection.value());
                CollectDirectLighting(ray, scene, intersection.value());
                return ray.GetRadiance();
            case Material::InteractionType::REFLECTIVE:
                material.Reflect(ray, intersection.value(), kApplyRoughness);
                break;
            case Material::InteractionType::REFRACTIVE:
                material.Refract(ray, intersection.value(), kApplyRoughness);
                break;
        }
    }
    return ray.GetRadiance();
}

void RendererDeterministic::CollectDirectLighting(Ray& ray, const Scene& scene, const Intersection& intersection) {
    const auto& material = intersection.object->GetMaterial();
    const Vector3D& x = intersection.point;
    Vector3D n = intersection.normal.Normalized();

    if (ray.IsEntering(n)) {
        n = -1.0 * n;
    }

    bool anyLightHit = false;
    Color directRadiance(0.0, 0.0, 0.0);
    for (const auto& lightSource : scene.GetLightSources()) {
        const double lightArea = lightSource->GetSurfaceArea();

        const Color Le = lightSource->GetMaterial().GetEmission();  // emitted radiance (RGB)

        const std::vector<Vector3D> lightPoints = lightSource->GetKeyPoints();

        Color colorSum(0.0, 0.0, 0.0);

        std::size_t lightHits = 0;
        for (const Vector3D& y : lightPoints) {
            Vector3D toLight = y - x;
            const double dist2 = toLight.NormSquared();
            toLight.Normalize();

            Ray shadowRay(x + toLight * kEpsilon, toLight);
            auto shadowHit = Intersect(shadowRay, scene);
            if (!shadowHit.has_value() || shadowHit->object != lightSource.get()) {
                continue;  // occluded or no intersection
            }
            anyLightHit = true;
            const Vector3D nL = shadowHit->normal.Normalized();

            const double cosSurface = std::max(0.0, n.Dot(toLight));
            const double cosLight = std::max(0.0, nL.Dot((-1.0) * toLight));

            if (cosSurface <= 0.0 || cosLight <= 0.0) {
                continue;
            }

            // Lambertian BRDF
            const Color f_r = material.GetColor(intersection) * (1.0 / M_PI);

            // Geometry factor
            const double G = cosSurface * cosLight * lightArea / dist2;

            // Direct contribution
            colorSum += f_r * Le * G;
            lightHits++;
        }

        if (lightHits > 0) {
            directRadiance += colorSum / static_cast<double>(lightHits);
        }
    }

    if (!anyLightHit) {
        ray.AddRadiance(kAmbientFactor * ray.GetThroughput());
    } else {
        // Add this direct lighting ` scaled by the ray's current throughput
        ray.AddRadiance(ray.GetThroughput() * directRadiance);
    }
}

}  // namespace Raytracer
