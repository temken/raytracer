#pragma once

#include "Geometry/Vector.hpp"
#include "Utilities/Color.hpp"
#include "Utilities/Image.hpp"

#include <vector>

namespace Raytracer {

struct GBufferData {
    bool hit = false;
    float depth = 0.0;
    Vector3D normal = Vector3D({0.0, 0.0, 0.0});
    Color albedo = Color(0.0, 0.0, 0.0);
};

class GBuffer {
public:
    GBuffer(std::size_t width, std::size_t height);
    GBuffer(const Image& image);

    void SetData(std::size_t x, std::size_t y, const GBufferData& data);
    GBufferData& GetData(std::size_t x, std::size_t y);

    std::size_t GetWidth() const;
    std::size_t GetHeight() const;

    void Resize(std::size_t width, std::size_t height);
    void Reset();

    Image CreateDepthImage() const;
    Image CreateNormalImage() const;
    Image CreateAlbedoImage() const;

private:
    std::size_t mWidth;
    std::size_t mHeight;
    std::vector<GBufferData> mData;

    void CheckBounds(std::size_t x, std::size_t y) const;
};
}  // namespace Raytracer
