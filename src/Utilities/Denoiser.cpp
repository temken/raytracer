#include "Utilities/Denoiser.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>

namespace Raytracer {

Image Denoiser::Denoise(const Image& inputImage, Denoiser::Method method, GBuffer* gbuffer) {
    if (inputImage.GetWidth() < 3 || inputImage.GetHeight() < 3) {
        return inputImage;  // Image too small to denoise
    }

    switch (method) {
        case Method::NONE: {
            return inputImage;
        }
        case Method::BLUR: {
            return Blur(inputImage);
        }
        case Method::GAUSSIAN_BLUR: {
            const double sigmaSpatial = 1.0;
            return GaussianBlur(inputImage, sigmaSpatial);
        }
        case Method::BILATERAL_FILTER: {
            const double sigmaSpatial = 2.0;
            const double sigmaColor = 0.1;
            return BilateralFilter(inputImage, sigmaSpatial, sigmaColor);
        }
        case Method::JOINT_BILATERAL_FILTER: {
            const double sigmaSpatial = 2.0;
            const double sigmaNormal = 0.25;
            const double sigmaDepth = 0.1;
            const double sigmaAlbedo = 0.05;
            if (gbuffer == nullptr) {
                throw std::invalid_argument("GBuffer must be provided for joint bilateral filter.");
            }
            return JointBilateralFilter(inputImage, *gbuffer, sigmaSpatial, sigmaNormal, sigmaDepth, sigmaAlbedo);
        }
        default:
            throw std::invalid_argument("Unsupported denoising method.");
    }
}

Image Denoiser::Denoise(const Image& inputImage, Method method, std::size_t iterations, GBuffer* gbuffer) {
    Image outputImage = inputImage;
    for (std::size_t i = 0; i < iterations; ++i) {
        outputImage = Denoise(outputImage, method, gbuffer);
    }
    return outputImage;
}

void Denoiser::ApplyDenoising(Image& image, Method method, std::size_t iterations, GBuffer* gbuffer) {
    image = Denoise(image, method, iterations, gbuffer);
}

Image Denoiser::Blur(const Image& inputImage, std::size_t radius) {
    Image outputImage = inputImage;

#pragma omp parallel for
    for (std::size_t y = radius; y < outputImage.GetHeight() - radius; y++) {
        for (std::size_t x = radius; x < outputImage.GetWidth() - radius; x++) {
            Color sum(0.0, 0.0, 0.0);
            for (int dy = -static_cast<int>(radius); dy <= static_cast<int>(radius); ++dy) {
                for (int dx = -static_cast<int>(radius); dx <= static_cast<int>(radius); ++dx) {
                    sum += outputImage.GetPixel(x + dx, y + dy);
                }
            }
            outputImage.SetPixel(x, y, sum * (1.0 / ((2 * radius + 1) * (2 * radius + 1))));
        }
    }

    return outputImage;
}

double Denoiser::Gaussian(double x, double sigma) {
    return std::exp(-(x * x) / (2.0 * sigma * sigma));  // no need to normalize for filtering
}

std::vector<std::vector<double>> Denoiser::CreateGaussianKernel(double sigma, int kernelRadius) {
    std::vector<std::vector<double>> kernel(2 * kernelRadius + 1, std::vector<double>(2 * kernelRadius + 1));
    double sum = 0.0;
    for (int y = -kernelRadius; y <= kernelRadius; y++) {
        for (int x = -kernelRadius; x <= kernelRadius; x++) {
            double r = std::sqrt(x * x + y * y);
            double value = Gaussian(r, sigma);
            kernel[y + kernelRadius][x + kernelRadius] = value;
            sum += value;
        }
    }

    // Normalize kernel
    for (int y = 0; y < 2 * kernelRadius + 1; ++y) {
        for (int x = 0; x < 2 * kernelRadius + 1; ++x) {
            kernel[y][x] /= sum;
        }
    }
    return kernel;
}

Image Denoiser::GaussianBlur(const Image& inputImage, double sigma) {
    int kernelRadius = static_cast<int>(std::max(1.0, std::ceil(3 * sigma)));
    // Create Gaussian kernel
    auto kernel = CreateGaussianKernel(sigma, kernelRadius);

    Image outputImage = inputImage;
#pragma omp parallel for
    for (std::size_t y = 0; y < inputImage.GetHeight(); ++y) {
        for (std::size_t x = 0; x < inputImage.GetWidth(); ++x) {
            Color sum(0.0, 0.0, 0.0);
            double weightSum = 0.0;
            for (int ky = -kernelRadius; ky <= kernelRadius; ++ky) {
                for (int kx = -kernelRadius; kx <= kernelRadius; ++kx) {
                    int nx = static_cast<int>(x) + kx;
                    int ny = static_cast<int>(y) + ky;
                    if (nx < 0 || nx >= static_cast<int>(inputImage.GetWidth()) ||
                        ny < 0 || ny >= static_cast<int>(inputImage.GetHeight())) {
                        continue;
                    }
                    Color pixel = inputImage.GetPixel(nx, ny);
                    double weight = kernel[ky + kernelRadius][kx + kernelRadius];
                    sum += pixel * weight;
                    weightSum += weight;
                }
            }
            if (weightSum > 0.0) {
                outputImage.SetPixel(x, y, sum / weightSum);
            }
        }
    }

    return outputImage;
}

Image Denoiser::BilateralFilter(const Image& inputImage, double sigmaSpatial, double sigmaColor) {
    Image outputImage = inputImage;

    int spatialKernelRadius = static_cast<int>(std::max(1.0, std::ceil(3 * sigmaSpatial)));
    auto spatialKernel = CreateGaussianKernel(sigmaSpatial, spatialKernelRadius);

#pragma omp parallel for
    for (std::size_t y = 0; y < inputImage.GetHeight(); y++) {
        for (std::size_t x = 0; x < inputImage.GetWidth(); x++) {
            Color sum(0.0, 0.0, 0.0);
            double weightSum = 0.0;
            Color centerPixel = inputImage.GetPixel(x, y);

            for (int ky = -spatialKernelRadius; ky <= spatialKernelRadius; ky++) {
                for (int kx = -spatialKernelRadius; kx <= spatialKernelRadius; kx++) {
                    int nx = static_cast<int>(x) + kx;
                    int ny = static_cast<int>(y) + ky;
                    if (nx < 0 || nx >= static_cast<int>(inputImage.GetWidth()) ||
                        ny < 0 || ny >= static_cast<int>(inputImage.GetHeight())) {
                        continue;
                    }
                    Color pixel = inputImage.GetPixel(nx, ny);
                    double spatialWeight = spatialKernel[ky + spatialKernelRadius][kx + spatialKernelRadius];
                    double colorDistance = (pixel - centerPixel).Length();
                    double colorWeight = Gaussian(colorDistance, sigmaColor);
                    double weight = spatialWeight * colorWeight;
                    sum += pixel * weight;
                    weightSum += weight;
                }
            }
            if (weightSum > 0.0) {
                outputImage.SetPixel(x, y, sum / weightSum);
            }
        }
    }

    return outputImage;
}

Image Denoiser::JointBilateralFilter(const Image& inputImage, GBuffer& gbuffer, double sigmaSpatial, double sigmaNormal, double sigmaDepth, double sigmaAlbedo) {
    // Check image and GBuffer dimensions
    if (inputImage.GetWidth() != gbuffer.GetWidth() || inputImage.GetHeight() != gbuffer.GetHeight()) {
        throw std::invalid_argument("Input image and GBuffer dimensions do not match.");
    }

    int spatialKernelRadius = static_cast<int>(std::max(1.0, std::ceil(3 * sigmaSpatial)));
    auto spatialKernel = CreateGaussianKernel(sigmaSpatial, spatialKernelRadius);

    Image outputImage = inputImage;
#pragma omp parallel for
    for (std::size_t y = 0; y < inputImage.GetHeight(); y++) {
        for (std::size_t x = 0; x < inputImage.GetWidth(); x++) {
            Color sum(0.0, 0.0, 0.0);
            double weightSum = 0.0;
            GBufferData centerPixel = gbuffer.GetData(x, y);
            if (centerPixel.hit == false) {
                outputImage.SetPixel(x, y, inputImage.GetPixel(x, y));
                continue;
            }

            for (int ky = -spatialKernelRadius; ky <= spatialKernelRadius; ky++) {
                for (int kx = -spatialKernelRadius; kx <= spatialKernelRadius; kx++) {
                    int nx = static_cast<int>(x) + kx;
                    int ny = static_cast<int>(y) + ky;
                    if (nx < 0 || nx >= static_cast<int>(inputImage.GetWidth()) ||
                        ny < 0 || ny >= static_cast<int>(inputImage.GetHeight())) {
                        continue;
                    }

                    GBufferData neighborPixel = gbuffer.GetData(nx, ny);
                    if (neighborPixel.hit == false) {
                        continue;
                    }

                    // 1. Spatial weight
                    double spatialWeight = spatialKernel[ky + spatialKernelRadius][kx + spatialKernelRadius];
                    // 2. Depth weight
                    double depthDifference = std::abs(neighborPixel.depth - centerPixel.depth);
                    double depthWeight = Gaussian(depthDifference, sigmaDepth);
                    // 3. Normal weight
                    double normalDot = std::clamp(neighborPixel.normal.Dot(centerPixel.normal), -1.0, 1.0);
                    double normalDifference = std::acos(normalDot);  // in radians
                    double normalWeight = Gaussian(normalDifference, sigmaNormal);
                    // 4. Albedo weight
                    double albedoDifference = (neighborPixel.albedo - centerPixel.albedo).Length();
                    double albedoWeight = Gaussian(albedoDifference, sigmaAlbedo);

                    Color neighborColor = inputImage.GetPixel(nx, ny);
                    double weight = spatialWeight * depthWeight * normalWeight * albedoWeight;

                    sum += neighborColor * weight;
                    weightSum += weight;
                }
            }
            if (weightSum > 0.0) {
                outputImage.SetPixel(x, y, sum / weightSum);
            }
        }
    }

    return outputImage;
}

Image Denoiser::RemoveHotPixels(const Image& input) {
    if (input.GetWidth() < 3 || input.GetHeight() < 3) {
        return input;  // Image too small to process
    }
    Image output = input;

    const int radius = 1;
    const double minimumLuminance = 1e-4;

    // std::size_t hotPixelCount = 0;

    for (int y = 0; y < input.GetHeight(); y++) {
        for (int x = 0; x < input.GetWidth(); x++) {
            const Color& center = input.GetPixel(x, y);
            const double centerLuminance = center.Luminance();

            if (centerLuminance < minimumLuminance) {
                continue;
            }

            // Collect neighborhood luminance
            std::vector<double> neighborLuminances;
            for (int dy = -radius; dy <= radius; ++dy) {
                if (y + dy < 0 || y + dy >= input.GetHeight()) {
                    continue;
                }
                for (int dx = -radius; dx <= radius; ++dx) {
                    if ((dx == 0 && dy == 0) || (x + dx < 0) || (x + dx >= input.GetWidth())) {
                        continue;
                    }
                    neighborLuminances.push_back(input.GetPixel(x + dx, y + dy).Luminance());
                }
            }

            // Find median luminance
            std::sort(neighborLuminances.begin(), neighborLuminances.end());
            const double medianLuminance = neighborLuminances[neighborLuminances.size() / 2];

            // Compute MAD
            std::vector<double> absDeviations;
            for (const double lum : neighborLuminances) {
                absDeviations.push_back(std::abs(lum - medianLuminance));
            }
            std::sort(absDeviations.begin(), absDeviations.end());
            const double mad = absDeviations[absDeviations.size() / 2];

            // Outlier test using MAD (robust sigma estimate)
            // sigma_est = 1.4826 * MAD for normal-like distributions
            const double kMad = 6.0;  // sensitivity multiplier (tune: 3-6)
            const double madToSigma = 1.4826;
            const double sigmaEst = std::max(madToSigma * mad, 1e-6);
            const double threshold = medianLuminance + kMad * sigmaEst;

            if (centerLuminance > threshold) {
                const double newLuminance = threshold;
                const double scale = newLuminance / centerLuminance;
                // hotPixelCount++;
                output.SetPixel(x, y, center * scale);
            }
        }
    }
    return output;
}

}  // namespace Raytracer
