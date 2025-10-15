#pragma once

#include "Utilities/Color.hpp"
#include "Utilities/Image.hpp"

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
