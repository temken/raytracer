#pragma once

#include "Rendering/GBuffer.hpp"
#include "Utilities/Image.hpp"

namespace Raytracer {

class Denoiser {
public:
    enum class Method {
        BLUR,
        GAUSSIAN_BLUR,
        BILATERAL_FILTER,
        JOINT_BILATERAL_FILTER,
    };

    static Image Denoise(const Image& inputImage, Method method, GBuffer* gbuffer = nullptr);

    static Image Denoise(const Image& inputImage, Method method, std::size_t iterations = 1, GBuffer* gbuffer = nullptr);

    static Image Blur(const Image& inputImage, std::size_t radius = 1);

    static Image GaussianBlur(const Image& inputImage, double sigma);

    static Image BilateralFilter(const Image& inputImage, double sigmaSpatial, double sigmaColor);

    static Image JointBilateralFilter(const Image& inputImage, GBuffer& gbuffer, double sigmaSpatial, double sigmaNormal, double sigmaDepth, double sigmaAlbedo);

    static Image RemoveHotPixels(const Image& inputImage);

private:
    // Helper functions
    static double Gaussian(double x, double sigma);
    static std::vector<std::vector<double>> CreateGaussianKernel(double sigma, int kernelRadius);
};

}  // namespace Raytracer
