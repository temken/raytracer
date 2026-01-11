#include "Rendering/Material.hpp"

#include "Scene/ObjectPrimitive.hpp"
#include "Version.hpp"

namespace Raytracer {

Material::Material() : mBaseColor(1.0, 1.0, 1.0), mSpecularColor(1.0, 1.0, 1.0), mEmission(0.0, 0.0, 0.0), mRoughness(0.0) {
    // Default probabilities
    mInteractionProbabilities[InteractionType::DIFFUSE] = 1.0;
    mInteractionProbabilities[InteractionType::REFLECTIVE] = 0.0;
    mInteractionProbabilities[InteractionType::REFRACTIVE] = 0.0;
}

Material::Material(const Color& baseColor, double roughness, double refractiveIndex, double meanFreePath, double radiance) :
    mBaseColor(baseColor),
    mSpecularColor(baseColor),  // Default to same as baseColor
    mEmission(radiance * baseColor),
    mRoughness(roughness),
    mRefractiveIndex(refractiveIndex),
    mMeanFreePath(meanFreePath),
    mUseFresnel(true) {
    // Default probabilities
    mInteractionProbabilities[InteractionType::DIFFUSE] = 1.0;
    mInteractionProbabilities[InteractionType::REFLECTIVE] = 0.0;
    mInteractionProbabilities[InteractionType::REFRACTIVE] = 0.0;
    NormalizeProbabilities();
}

Material::InteractionType Material::Interact(Ray& ray, const Object::Intersection& intersection, bool applyRoughness) {
    // Draw a random number in [0,1)
    const double r = mDistribution(mGenerator);

    double cumulative = 0.0;

    auto probabilities = mInteractionProbabilities;
    if (mUseFresnel) {
        double cosThetaI = ray.IncidentAngleCosine(intersection.normal);
        if (cosThetaI < 0.0) {
            cosThetaI = -cosThetaI;
        }
        auto fresnelProbs = GetFresnelCorrectedProbabilities(cosThetaI);
        probabilities = fresnelProbs;
    }

    // Ensure probabilities sum to 1.0 elsewhere (constructor or setup).
    // Iterate deterministically (use std::map or a fixed array).
    for (const auto& [type, prob] : probabilities) {
        cumulative += prob;
        if (r <= cumulative) {
            switch (type) {
                case InteractionType::DIFFUSE:
                    Diffuse(ray, intersection, prob);
                    return InteractionType::DIFFUSE;

                case InteractionType::REFLECTIVE:
                    Reflect(ray, intersection, applyRoughness, prob);
                    return InteractionType::REFLECTIVE;

                case InteractionType::REFRACTIVE:
                    Refract(ray, intersection, applyRoughness, prob);
                    return InteractionType::REFRACTIVE;
            }
        }
    }

    throw std::runtime_error("Material::Interact: No interaction type selected; check probabilities.");
}

void Material::Diffuse(Ray& ray, const Object::Intersection& intersection, double probability) {
    // Diffuse surface: random new direction in hemisphere
    // Build ONB around normal
    Vector3D eZ = ray.IsEntering(intersection.normal) ? intersection.normal : -1.0 * intersection.normal;
    Vector3D a = (std::fabs(eZ[0]) > 0.707) ? Vector3D({0.0, 1.0, 0.0}) : Vector3D({1.0, 0.0, 0.0});
    Vector3D eX = a.Cross(eZ).Normalized();
    Vector3D eY = eZ.Cross(eX);

    // Cosine-weighted hemisphere sample in local coords
    double u1 = mDistribution(mGenerator);
    double u2 = mDistribution(mGenerator);
    double cosTheta = std::sqrt(u1);
    double sinTheta = std::sqrt(1.0 - u1);
    double phi = 2.0 * M_PI * u2;
    double x = sinTheta * std::cos(phi);
    double y = sinTheta * std::sin(phi);

    // Transform to world and normalize
    Vector3D newDir = (x * eX + y * eY + cosTheta * eZ).Normalized();

    ray.SetOrigin(intersection.point + kEpsilon * newDir);
    ray.SetDirection(newDir);
    ray.IncrementDepth();

    // ---- Physically based energy update ----
    // f_r = albedo / π, PDF = cosθ / π  ->  throughput *= albedo
    ray.UpdateThroughput(GetColor(intersection) / probability);
}

void Material::Reflect(Ray& ray, const Object::Intersection& intersection, bool applyRoughness, double probability) {
    Vector3D incomingDir = ray.GetDirection();
    Vector3D newDir = incomingDir - 2 * incomingDir.Dot(intersection.normal) * intersection.normal;
    if (applyRoughness && mRoughness > 0.0) {
        double cosThetaMax = std::cos(mRoughness * (M_PI / 2.0));  // roughness=1 => 90° cone
        newDir = SampleCone(newDir, cosThetaMax);
        // For rough reflection, divide by probability since it's continuous sampling
        ray.UpdateThroughput(mSpecularColor / probability);
    } else {
        // Perfect mirror - no probability compensation for delta function
        ray.UpdateThroughput(mSpecularColor);
    }
    ray.SetOrigin(intersection.point + kEpsilon * newDir);
    ray.SetDirection(newDir);
    ray.IncrementDepth();
}

void Material::Refract(Ray& ray, const Object::Intersection& intersection, bool applyRoughness, double probability) {
    Vector3D d = ray.GetDirection().Normalized();
    Vector3D n = intersection.normal.Normalized();

    // Determine if the ray is entering or exiting
    double cosThetaI = ray.IncidentAngleCosine(n);
    bool entering = cosThetaI > 0.0;
    double etaI = 1.0;               // refractive index of incident medium (air)
    double etaT = mRefractiveIndex;  // refractive index of material

    if (!entering) {
        n *= -1.0;
        std::swap(etaI, etaT);  // swap indices
        cosThetaI = -cosThetaI;
    }

    double eta = etaI / etaT;
    double sin2ThetaT = eta * eta * (1.0 - cosThetaI * cosThetaI);

    // Handle total internal reflection
    if (sin2ThetaT > 1.0) {
        Object::Intersection tmpIntersection = intersection;
        tmpIntersection.normal = n;  // Use the correct normal for reflection
        Reflect(ray, tmpIntersection, applyRoughness, probability);
        return;
    }

    double cosThetaT = std::sqrt(1.0 - sin2ThetaT);

    // Refracted direction
    Vector3D refractDir = eta * d + (eta * cosThetaI - cosThetaT) * n;
    refractDir = refractDir.Normalized();

    // Roughness / glossy refraction
    if (applyRoughness && mRoughness > 0.0) {
        double cosThetaMax = std::cos(mRoughness * (M_PI / 4.0));  // half the reflection roughness
        refractDir = SampleCone(refractDir, cosThetaMax);
        // For rough refraction, divide by probability since it's continuous sampling
        ray.UpdateThroughput(GetColor(intersection) / probability);
    } else {
        // Perfect refraction - no probability compensation for delta function
        ray.UpdateThroughput(GetColor(intersection));
    }
    ray.SetOrigin(intersection.point + kEpsilon * refractDir);
    ray.SetDirection(refractDir);
    ray.IncrementDepth();
}

Color Material::GetColor(const Object::Intersection& intersection) const {
    if (mColorTexture) {
        auto uv = intersection.object->GetShape()->GetSurfaceParameters(intersection.point);
        // TODO: Do not mix the ranges. Either [-0.5,0.5] everywhere or [0,1] everywhere
        return mColorTexture->GetColorAt(uv.first + 0.5, uv.second + 0.5) * mBaseColor;
    }
    return GetBaseColor();
}

Color Material::GetBaseColor() const {
    return mBaseColor;
}

void Material::SetBaseColor(const Color& color) {
    mBaseColor = color;
}

Color Material::GetSpecularColor() const {
    return mSpecularColor;
}
void Material::SetSpecularColor(const Color& color) {
    mSpecularColor = color;
}

double Material::GetRoughness() const {
    return mRoughness;
}
void Material::SetRoughness(double roughness) {
    mRoughness = roughness;
}

double Material::GetRefractiveIndex() const {
    return mRefractiveIndex;
}
void Material::SetRefractiveIndex(double refractiveIndex) {
    mRefractiveIndex = refractiveIndex;
}

double Material::GetMeanFreePath() const {
    return mMeanFreePath;
}
void Material::SetMeanFreePath(double meanFreePath) {
    mMeanFreePath = meanFreePath;
}

bool Material::EmitsLight() const {
    return mEmission != BLACK;
}

Color Material::GetEmission() const {
    return mEmission;
}

void Material::SetEmission(const Color& emission) {
    mEmission = emission;
}

bool Material::UsesFresnel() const {
    return mUseFresnel;
}

void Material::SetUseFresnel(bool useFresnel) {
    mUseFresnel = useFresnel;
}

std::map<Material::InteractionType, double> Material::GetInteractionProbabilities() const {
    return mInteractionProbabilities;
}

void Material::SetInteractionProbabilities(const std::map<InteractionType, double>& probs) {
    mInteractionProbabilities = probs;
    NormalizeProbabilities();
}

Material::InteractionType Material::MostLikelyInteraction() const {
    double maxProb = 0.0;
    InteractionType mostLikely = InteractionType::DIFFUSE;

    for (const auto& [type, prob] : mInteractionProbabilities) {
        if (prob > maxProb) {
            maxProb = prob;
            mostLikely = type;
        }
    }

    return mostLikely;
}

void Material::SetColorTexture(std::string filename) {
    std::string filepath = TOP_LEVEL_DIR "textures/" + filename;
    mColorTexture = Texture(filepath);
}

void Material::PrintInfo() const {
    std::cout << "Material Info:" << std::endl
              << "\tBase Color:\t" << mBaseColor << std::endl
              << "\tSpecular Color:\t" << mSpecularColor << std::endl
              << "\tEmission:\t" << mEmission << std::endl
              << "\tColor Texture:\t" << (mColorTexture.has_value() ? "[x]" : "[ ]") << std::endl
              << "\tRoughness:\t" << mRoughness << std::endl
              << "\tRefractive Index:\t" << mRefractiveIndex << std::endl
              << "\tMean Free Path:\t" << mMeanFreePath << std::endl
              << "\tUse Fresnel:\t" << (mUseFresnel ? "[x]" : "[ ]") << std::endl
              << "\tInteraction Probabilities:" << std::endl;
    for (const auto& [type, prob] : mInteractionProbabilities) {
        std::string typeStr;
        switch (type) {
            case InteractionType::DIFFUSE:
                typeStr = "Diffuse";
                break;
            case InteractionType::REFLECTIVE:
                typeStr = "Reflective";
                break;
            case InteractionType::REFRACTIVE:
                typeStr = "Refractive";
                break;
        }
        std::cout << "\t\t" << typeStr << ": " << prob << std::endl;
    }
    std::cout << std::endl;
}

void Material::NormalizeProbabilities() {
    double total = 0.0;
    for (const auto& pair : mInteractionProbabilities) {
        total += pair.second;
    }
    for (auto& pair : mInteractionProbabilities) {
        pair.second /= total;
    }
}

std::map<Material::InteractionType, double> Material::GetFresnelCorrectedProbabilities(double cosThetaI) const {
    double RO = mInteractionProbabilities.at(InteractionType::REFLECTIVE);

    if (RO < kEpsilon) {
        // No reflective component, nothing to adjust
        return mInteractionProbabilities;
    } else if (RO > 1.0 - kEpsilon) {
        // Perfect mirror, all probability to reflection
        return {
            {Material::InteractionType::DIFFUSE, 0.0},
            {Material::InteractionType::REFLECTIVE, 1.0},
            {Material::InteractionType::REFRACTIVE, 0.0}};
    }

    // Schlick's approximation
    double R = RO + (1.0 - RO) * std::pow(1.0 - cosThetaI, 5);

    // Rescale the other two probabilities
    double rescaledDiffuseProbability = mInteractionProbabilities.at(InteractionType::DIFFUSE) * (1.0 - R) / (1.0 - RO);
    double rescaledRefractiveProbability = mInteractionProbabilities.at(InteractionType::REFRACTIVE) * (1.0 - R) / (1.0 - RO);

    // TODO Check the normalization
    // TODO Check validity when R0 = 0 or 1
    // TODO Incident angel as function

    return {
        {Material::InteractionType::DIFFUSE, rescaledDiffuseProbability},
        {Material::InteractionType::REFLECTIVE, R},
        {Material::InteractionType::REFRACTIVE, rescaledRefractiveProbability}};
}

Vector3D Material::SampleCone(const Vector3D& axis, double cosThetaMax) {
    double u1 = mDistribution(mGenerator);  // ∈ [0,1)
    double u2 = mDistribution(mGenerator);

    // Uniform sampling of cos(theta) in [cosThetaMax, 1]
    double cosTheta = (1.0 - u1) + u1 * cosThetaMax;  // linear interpolation
    double sinTheta = std::sqrt(1.0 - cosTheta * cosTheta);
    double phi = 2.0 * M_PI * u2;

    double x = sinTheta * std::cos(phi);
    double y = sinTheta * std::sin(phi);
    double z = cosTheta;

    // Build orthonormal basis
    Vector3D eZ = axis.Normalized();
    Vector3D a = (std::fabs(eZ[0]) > 0.707) ? Vector3D{0.0, 1.0, 0.0} : Vector3D{1.0, 0.0, 0.0};
    Vector3D eX = a.Cross(eZ).Normalized();
    Vector3D eY = eZ.Cross(eX);

    return (x * eX + y * eY + z * eZ).Normalized();
}

}  // namespace Raytracer
