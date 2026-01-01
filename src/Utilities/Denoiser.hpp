#pragma once

#include "Utilities/Image.hpp"

namespace Raytracer {

class Denoiser {
public:
    enum class Method {
        BLUR,
        GAUSSIAN_BLUR,
    };

    static Image Denoise(const Image& inputImage, Method method);

    static Image Blur(const Image& inputImage, std::size_t blurCount = 1);

    static Image GaussianBlur(const Image& inputImage, double sigma);

    static Image RemoveHotPixels(const Image& inputImage);

private:
    // Helper functions for Gaussian filters
    static double Gaussian(double x, double sigma);
    static std::vector<std::vector<double>> CreateGaussianKernel(double sigma);
};

}  // namespace Raytracer
