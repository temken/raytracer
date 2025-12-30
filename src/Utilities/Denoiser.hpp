#pragma once

#include "Utilities/Image.hpp"

namespace Raytracer {

class Denoiser {
public:
    enum class Method {
        BLUR,
    };

    static Image Denoise(const Image& inputImage, Method method);

    static Image Blur(const Image& inputImage, std::size_t blurCount = 1);

    static Image RemoveHotPixels(const Image& inputImage);

private:
};

}  // namespace Raytracer
