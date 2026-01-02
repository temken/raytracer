#include "Utilities/Denoiser.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>

namespace Raytracer {

Image Denoiser::Denoise(const Image& inputImage, Denoiser::Method method) {
    switch (method) {
        case Method::BLUR:
            return Blur(inputImage, 1);
        case Method::GAUSSIAN_BLUR:
            return GaussianBlur(inputImage, 1.0);
        case Method::BILATERAL_FILTER:
            return BilateralFilter(inputImage, 2.0, 0.1);
        default:
            throw std::invalid_argument("Unsupported denoising method.");
    }
}

Image Denoiser::Blur(const Image& inputImage, std::size_t blurCount) {
    if (inputImage.GetWidth() < 3 || inputImage.GetHeight() < 3) {
        return inputImage;  // Image too small to blur
    }

    Image outputImage = inputImage;
    // Simple box blur implementation
    for (std::size_t i = 0; i < blurCount; i++) {
        for (std::size_t y = 1; y < outputImage.GetHeight() - 1; y++) {
            for (std::size_t x = 1; x < outputImage.GetWidth() - 1; x++) {
                Color sum(0.0, 0.0, 0.0);
                for (int dy = -1; dy <= 1; ++dy) {
                    for (int dx = -1; dx <= 1; ++dx) {
                        sum += outputImage.GetPixel(x + dx, y + dy);
                    }
                }
                outputImage.SetPixel(x, y, sum * (1.0 / 9.0));
            }
        }
    }
    return outputImage;
}

double Denoiser::Gaussian(double x, double sigma) {
    return (1.0 / (sigma * std::sqrt(2.0 * M_PI))) * std::exp(-(x * x) / (2.0 * sigma * sigma));
}

std::vector<std::vector<double>> Denoiser::CreateGaussianKernel(double sigma) {
    std::size_t kernelRadius = static_cast<std::size_t>(std::ceil(3 * sigma));
    std::vector<std::vector<double>> kernel(2 * kernelRadius + 1, std::vector<double>(2 * kernelRadius + 1));
    double sum = 0.0;
    for (int y = -static_cast<int>(kernelRadius); y <= static_cast<int>(kernelRadius); y++) {
        for (int x = -static_cast<int>(kernelRadius); x <= static_cast<int>(kernelRadius); x++) {
            double r = std::sqrt(x * x + y * y);
            double value = Gaussian(r, sigma);
            kernel[y + static_cast<int>(kernelRadius)][x + static_cast<int>(kernelRadius)] = value;
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
    int kernelRadius = static_cast<int>(std::ceil(3 * sigma));
    if (kernelRadius < 1) {
        return inputImage;
    }
    // Create Gaussian kernel
    auto kernel = CreateGaussianKernel(sigma);

    Image outputImage = inputImage;
    for (std::size_t y = 0; y < inputImage.GetHeight(); ++y) {
        for (std::size_t x = 0; x < inputImage.GetWidth(); ++x) {
            Color sum(0.0, 0.0, 0.0);
            double weightSum = 0.0;
            for (int ky = -kernelRadius; ky <= kernelRadius; ++ky) {
                for (int kx = -kernelRadius; kx <= kernelRadius; ++kx) {
                    if (x + kx < 0 || x + kx >= inputImage.GetWidth() ||
                        y + ky < 0 || y + ky >= inputImage.GetHeight()) {
                        continue;
                    }
                    Color pixel = inputImage.GetPixel(x + kx, y + ky);
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
    int spatialKernelRadius = static_cast<int>(std::ceil(3 * sigmaSpatial));
    if (spatialKernelRadius < 1) {
        return inputImage;
    }
    auto spatialKernel = CreateGaussianKernel(sigmaSpatial);

    Image outputImage = inputImage;
    for (std::size_t y = 0; y < inputImage.GetHeight(); ++y) {
        for (std::size_t x = 0; x < inputImage.GetWidth(); ++x) {
            Color sum(0.0, 0.0, 0.0);
            double weightSum = 0.0;
            Color centerPixel = inputImage.GetPixel(x, y);

            for (int ky = -spatialKernelRadius; ky <= spatialKernelRadius; ++ky) {
                for (int kx = -spatialKernelRadius; kx <= spatialKernelRadius; ++kx) {
                    if (x + kx < 0 || x + kx >= inputImage.GetWidth() ||
                        y + ky < 0 || y + ky >= inputImage.GetHeight()) {
                        continue;
                    }
                    Color pixel = inputImage.GetPixel(x + kx, y + ky);
                    double spatialWeight = spatialKernel[ky + static_cast<int>(spatialKernel.size() / 2)][kx + static_cast<int>(spatialKernel.size() / 2)];
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

Image Denoiser::RemoveHotPixels(const Image& input) {
    if (input.GetWidth() < 3 || input.GetHeight() < 3) {
        return input;  // Image too small to process
    }
    Image output = input;

    const int radius = 2;
    const double outlierFactor = 3.0;
    const double minimumLuminance = 1e-4;

    std::size_t hotPixelCount = 0;

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
            const double sigmaEst = madToSigma * mad;
            const double threshold = medianLuminance + kMad * sigmaEst;

            if (centerLuminance > threshold) {
                const double newLuminance = threshold;
                const double scale = newLuminance / centerLuminance;
                hotPixelCount++;
                output.SetPixel(x, y, center * scale);
            }
        }
    }
    std::cout << "Removed " << hotPixelCount << " hot pixels." << std::endl;
    return output;
}

}  // namespace Raytracer
