#include "Rendering/RendererDeterministic.hpp"

namespace Raytracer {

Color RendererDeterministic::TraceRay(Ray ray, const Scene& scene) {
    while (ray.GetDepth() < kMaximumDepth) {
        auto intersection = Intersect(ray, scene);
        if (!intersection.has_value()) {
            return scene.GetBackgroundColor();
        }
        Material::InteractionType mostLikelyInteraction = intersection->object->GetMaterial().MostLikelyInteraction();
        switch (mostLikelyInteraction) {
            case Material::InteractionType::DIFFUSE:
                CollectDirectLighting(ray, scene, intersection.value());
                return ray.GetRadiance();
                // return intersection->object->GetMaterial().GetColor(intersection.value());
            case Material::InteractionType::REFLECTIVE:
                intersection->object->GetMaterial().Reflect(ray, intersection.value(), kApplyRoughness);
                break;
            case Material::InteractionType::REFRACTIVE:
                intersection->object->GetMaterial().Refract(ray, intersection.value(), kApplyRoughness);
                break;
        }
    }
    return ray.GetRadiance();
}

void RendererDeterministic::CollectDirectLighting(Ray& ray, const Scene& scene, const Intersection& intersection) {
    const auto& material = intersection.object->GetMaterial();
    const Vector3D& x = intersection.point;
    const Vector3D n = intersection.normal.Normalized();

    double totalRadiance = 0.0;  // scalar radiance accumulator (per your design)
    Color totalColor(0.0, 0.0, 0.0);

    for (const auto& lightSource : scene.GetLightSources()) {
        const double lightArea = lightSource->GetSurfaceArea();
        const Color lightColor = lightSource->GetMaterial().GetColor(intersection);  // emitted color
        const double lightRadiance = lightSource->GetMaterial().GetRadiance();       // scalar intensity

        std::vector<Vector3D> lightPoints = lightSource->GetKeyPoints();

        double radianceSum = 0.0;
        Color colorSum(0.0, 0.0, 0.0);

        for (const Vector3D& y : lightPoints) {
            Vector3D toLight = y - x;
            const double dist2 = toLight.NormSquared();
            const double dist = std::sqrt(dist2);
            toLight.Normalize();

            Ray shadowRay(x + toLight * kEpsilon, toLight);
            auto shadowHit = Intersect(shadowRay, scene);  // guaranteed to hit something

            if (shadowHit->object != lightSource.get()) {
                continue;
            }

            Vector3D nL = shadowHit->normal.Normalized();

            double cosSurface = std::max(0.0, n.Dot(toLight));
            double cosLight = std::max(0.0, nL.Dot(-1.0 * toLight));

            if (cosSurface <= 0.0 || cosLight <= 0.0) {
                continue;
            }

            // Lambertian BRDF: albedo / pi
            Color brdf = material.GetColor(intersection) * (1.0 / M_PI);

            // Geometry term for area sampling:
            // (cosSurface * cosLight * A) / r^2
            double geometry = (cosSurface * cosLight * lightArea) / dist2;

            // Scalar radiance contribution (for your scalar accumulator):
            double radianceContribution = lightRadiance * geometry;
            radianceSum += radianceContribution;

            // Color contribution (emitted color * brdf * geometry)
            Color contributionColor = brdf * lightColor * geometry;
            colorSum += contributionColor;
        }

        // Average over samples
        radianceSum /= static_cast<double>(lightPoints.size());
        colorSum = 1.0 / static_cast<double>(lightPoints.size()) * colorSum;

        totalRadiance += radianceSum;
        totalColor += colorSum;
    }

    ray.MultiplyColor(totalColor);   // modulate ray color by direct color contribution
    ray.AddRadiance(totalRadiance);  // add scalar radiance
}

}  // namespace Raytracer
