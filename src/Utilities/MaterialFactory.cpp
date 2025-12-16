#include "Utilities/MaterialFactory.hpp"

#include "Rendering/Material.hpp"

namespace Raytracer {

Material MaterialFactory::CreateBrass(double roughness) {
    Color brassSpecularColor(0.60, 0.54, 0.24);
    Material brass(BLACK, roughness);
    brass.SetSpecularColor(brassSpecularColor);
    std::map<Material::InteractionType, double> probs = {
        {Material::InteractionType::DIFFUSE, 0.0},
        {Material::InteractionType::REFLECTIVE, 1.0},
        {Material::InteractionType::REFRACTIVE, 0.0}};
    brass.SetInteractionProbabilities(probs);
    return brass;
}

Material MaterialFactory::CreateCopper(double roughness) {
    Color brassSpecularColor(0.62, 0.39, 0.17);
    Material brass(BLACK, roughness);
    brass.SetSpecularColor(brassSpecularColor);
    std::map<Material::InteractionType, double> probs = {
        {Material::InteractionType::DIFFUSE, 0.0},
        {Material::InteractionType::REFLECTIVE, 1.0},
        {Material::InteractionType::REFRACTIVE, 0.0}};
    brass.SetInteractionProbabilities(probs);
    return brass;
}

Material MaterialFactory::CreateGold(double roughness) {
    Color goldSpecularColor(1.0, 0.766, 0.336);
    Material gold(BLACK, roughness);
    gold.SetSpecularColor(goldSpecularColor);
    std::map<Material::InteractionType, double> probs = {
        {Material::InteractionType::DIFFUSE, 0.0},
        {Material::InteractionType::REFLECTIVE, 1.0},
        {Material::InteractionType::REFRACTIVE, 0.0}};
    gold.SetInteractionProbabilities(probs);
    return gold;
}

Material MaterialFactory::CreateSilver(double roughness) {
    Color silverSpecularColor(0.91, 0.92, 0.92);
    Material silver(BLACK, roughness);
    silver.SetSpecularColor(silverSpecularColor);
    std::map<Material::InteractionType, double> probs = {
        {Material::InteractionType::DIFFUSE, 0.0},
        {Material::InteractionType::REFLECTIVE, 1.0},
        {Material::InteractionType::REFRACTIVE, 0.0}};
    silver.SetInteractionProbabilities(probs);
    return silver;
}

// Dielectrics
Material MaterialFactory::CreateGlass(double roughness, Color teint) {
    const double refractiveIndex = 1.5;
    Material glass(teint, roughness, refractiveIndex);
    std::map<Material::InteractionType, double> probs = {
        {Material::InteractionType::DIFFUSE, 0.0},
        {Material::InteractionType::REFLECTIVE, 0.05},
        {Material::InteractionType::REFRACTIVE, 0.95}};
    glass.SetInteractionProbabilities(probs);
    glass.SetUseFresnel(true);
    glass.SetSpecularColor(WHITE);
    return glass;
}

}  // namespace Raytracer
