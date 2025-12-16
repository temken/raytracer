#pragma once

#include "Rendering/Material.hpp"

namespace Raytracer {

class MaterialFactory {
public:
    // Metals
    static Material CreateBrass(double roughness = 0.2);
    static Material CreateCopper(double roughness = 0.2);
    static Material CreateGold(double roughness = 0.2);
    static Material CreateSilver(double roughness = 0.2);

    // Dielectrics
    static Material CreateGlass(double roughness = 0.0, Color teint = WHITE);

private:
};

}  // namespace Raytracer
