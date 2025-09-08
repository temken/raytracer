#pragma once

#include "Rendering/Camera.hpp"
#include "Scene/Scene.hpp"

namespace Raytracer {

class Configuration {
public:
    static Configuration& GetInstance();

    void ParseYamlFile(const std::string& path);

    std::string GetID() const;
    Camera GetCamera() const;
    Scene GetScene() const;

    std::string GetImagesDirectory() const;
    std::string GetVideosDirectory() const;

    // non-copyable
    Configuration(const Configuration&) = delete;
    Configuration& operator=(const Configuration&) = delete;

    void PrintInfo() const;

private:
    Configuration() = default;
    ~Configuration() = default;

    std::string mID;
    Camera mCamera;
    Scene mScene;

    void CreateOutputDirectory() const;
};

}  // namespace Raytracer
