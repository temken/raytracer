#pragma once

#include "Rendering/Camera.hpp"
#include "Scene/Scene.hpp"

#include <yaml-cpp/yaml.h>

namespace Raytracer {

class Configuration {
public:
    static Configuration& GetInstance();

    void ParseYamlFile(const std::string& path);

    std::string GetID() const;
    Camera ConstructCamera() const;
    Scene ConstructScene() const;

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
    YAML::Node mRoot;

    void CreateOutputDirectory() const;
};

}  // namespace Raytracer
