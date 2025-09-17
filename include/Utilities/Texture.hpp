#pragma once

#include "Rendering/Image.hpp"
#include "Utilities/Color.hpp"

namespace Raytracer {

class Texture {
public:
    Texture(std::string filePath);
    Color GetColorAt(double u, double v) const;

private:
    std::string mFilePath;
    Image mImage;
};

}  // namespace Raytracer
