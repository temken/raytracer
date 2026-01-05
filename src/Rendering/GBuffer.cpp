#include "Rendering/GBuffer.hpp"

#include <algorithm>
#include <cmath>

namespace Raytracer {

GBuffer::GBuffer(std::size_t width, std::size_t height) :
    mWidth(width),
    mHeight(height),
    mData(width * height) {
    if (mWidth == 0 || mHeight == 0) {
        throw std::invalid_argument("GBuffer dimensions must be positive (non-zero).");
    }
}

GBuffer::GBuffer(const Image& image) :
    GBuffer(image.GetWidth(), image.GetHeight()) {
}

void GBuffer::SetData(std::size_t x, std::size_t y, const GBufferData& data) {
    CheckBounds(x, y);
    mData[y * mWidth + x] = data;
}

GBufferData& GBuffer::GetData(std::size_t x, std::size_t y) {
    CheckBounds(x, y);
    return mData[y * mWidth + x];
}

std::size_t GBuffer::GetWidth() const {
    return mWidth;
}

std::size_t GBuffer::GetHeight() const {
    return mHeight;
}

void GBuffer::Resize(std::size_t width, std::size_t height) {
    mWidth = width;
    mHeight = height;
    mData.resize(mWidth * mHeight);
    Reset();
}

void GBuffer::Reset() {
    std::fill(mData.begin(), mData.end(), GBufferData{});
}

Image GBuffer::CreateDepthImage() const {
    Image depthImage(mWidth, mHeight, BLACK);

    // Collect depths for percentile-based normalization
    std::vector<float> depths;
    for (const auto& data : mData) {
        if (data.hit) {
            depths.push_back(data.depth);
        }
    }

    if (depths.empty()) {
        return depthImage;  // All black if no hits
    }

    // Use 95th percentile to avoid outlier sensitivity
    std::sort(depths.begin(), depths.end());
    float maxDepth = depths[static_cast<std::size_t>(depths.size() * 0.95)];
    if (maxDepth < 1e-6f) {
        maxDepth = 1.0f;  // Avoid division by zero
    }

#pragma omp parallel for
    for (std::size_t y = 0; y < mHeight; y++) {
        for (std::size_t x = 0; x < mWidth; x++) {
            const GBufferData& data = mData[y * mWidth + x];
            if (data.hit) {
                // Logarithmic mapping for better perceptual range
                float depthValue = std::log(data.depth + 1.0f) / std::log(maxDepth + 1.0f);
                depthValue = std::clamp(depthValue, 0.0f, 1.0f);
                depthImage.SetPixel(x, y, Color(depthValue, depthValue, depthValue));
            } else {
                depthImage.SetPixel(x, y, BLACK);
            }
        }
    }
    return depthImage;
}

Image GBuffer::CreateNormalImage() const {
    Image normalImage(mWidth, mHeight, BLACK);
#pragma omp parallel for
    for (std::size_t y = 0; y < mHeight; y++) {
        for (std::size_t x = 0; x < mWidth; x++) {
            const GBufferData& data = mData[y * mWidth + x];
            if (data.hit) {
                // Map normal from [-1,1] to [0,1]
                Color normalColor(
                    0.5 * (data.normal[0] + 1.0),
                    0.5 * (data.normal[1] + 1.0),
                    0.5 * (data.normal[2] + 1.0));
                normalImage.SetPixel(x, y, normalColor);
            } else {
                normalImage.SetPixel(x, y, BLACK);
            }
        }
    }
    return normalImage;
}

Image GBuffer::CreateAlbedoImage() const {
    Image albedoImage(mWidth, mHeight, BLACK);
#pragma omp parallel for
    for (std::size_t y = 0; y < mHeight; y++) {
        for (std::size_t x = 0; x < mWidth; x++) {
            const GBufferData& data = mData[y * mWidth + x];
            if (data.hit) {
                albedoImage.SetPixel(x, y, data.albedo);
            } else {
                albedoImage.SetPixel(x, y, BLACK);
            }
        }
    }
    return albedoImage;
}

void GBuffer::CheckBounds(std::size_t x, std::size_t y) const {
    if (x >= mWidth || y >= mHeight) {
        throw std::out_of_range("GBuffer coordinate out of bounds");
    }
}

}  // namespace Raytracer
