#pragma once

#include "Rendering/Ray.hpp"
#include "Utilities/Color.hpp"
#include "Utilities/HitRecord.hpp"
#include "Utilities/Texture.hpp"

#include <map>
#include <optional>
#include <random>

namespace Raytracer {

class Material {
public:
    enum class InteractionType {
        DIFFUSE,
        REFLECTIVE,
        REFRACTIVE
    };

    Material();
    Material(const Color& baseColor, double roughness = 1.0, double refractiveIndex = 1.0, double meanFreePath = 0.0, double radiance = 0.0);

    InteractionType Interact(Ray& ray, const HitRecord& hitRecord, bool applyRoughness = true);

    void Diffuse(Ray& incomingRay, const HitRecord& hitRecord, double probability = 1.0);
    void Reflect(Ray& incomingRay, const HitRecord& hitRecord, bool applyRoughness, double probability = 1.0);
    void Refract(Ray& incomingRay, const HitRecord& hitRecord, bool applyRoughness, double probability = 1.0);

    // Get color at intersection point (with texture if available)
    Color GetColor(const HitRecord& hitRecord) const;

    Color GetBaseColor() const;
    void SetBaseColor(const Color& color);

    Color GetSpecularColor() const;
    void SetSpecularColor(const Color& color);

    double GetRoughness() const;
    void SetRoughness(double roughness);

    double GetRefractiveIndex() const;
    void SetRefractiveIndex(double refractiveIndex);

    double GetMeanFreePath() const;
    void SetMeanFreePath(double meanFreePath);

    bool EmitsLight() const;
    Color GetEmission() const;
    void SetEmission(const Color& emission);

    bool UsesFresnel() const;
    void SetUseFresnel(bool useFresnel);

    void SetInteractionProbabilities(const std::map<InteractionType, double>& probs);
    InteractionType MostLikelyInteraction() const;

    void SetColorTexture(std::string filename);

    void PrintInfo() const;

private:
    Color mBaseColor;
    Color mSpecularColor;
    Color mEmission;
    double mRoughness;        // 0 = perfect mirror, 1 = very rough
    double mRefractiveIndex;  // 1 = vacuum, 1.33 = water, 1.5 = glass
    double mMeanFreePath;     // Average distance a photon travels in the material before interacting
    bool mUseFresnel;

    // Probability for each interaction type
    std::map<InteractionType, double> mInteractionProbabilities;

    // Random numbers:
    std::mt19937 mGenerator{std::random_device{}()};
    std::uniform_real_distribution<double> mDistribution{0.0, 1.0};

    // Optional texture
    std::optional<Texture> mColorTexture = std::nullopt;

    static constexpr double kEpsilon = 1e-5;

    void NormalizeProbabilities();
    std::map<InteractionType, double> GetFresnelCorrectedProbabilities(double cosThetaI) const;

    Vector3D SampleCone(const Vector3D& axis, double cosThetaMax);
};

}  // namespace Raytracer
