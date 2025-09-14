#pragma once

#include "Rendering/Camera.hpp"
#include "Scene/Box.hpp"
#include "Scene/Cylinder.hpp"
#include "Scene/Disk.hpp"
#include "Scene/Rectangle.hpp"
#include "Scene/Scene.hpp"
#include "Scene/Sphere.hpp"

#include <yaml-cpp/yaml.h>

namespace Raytracer {

class Configuration {
public:
    static Configuration& GetInstance();

    void ParseYamlFile(const std::string& path);

    std::string GetID() const;
    Camera ConstructCamera() const;
    Scene ConstructScene() const;

    std::string GetOutputDirectory() const;

    // non-copyable
    Configuration(const Configuration&) = delete;
    Configuration& operator=(const Configuration&) = delete;

    void PrintInfo() const;

private:
    Configuration() = default;
    ~Configuration() = default;

    std::string mID;
    YAML::Node mRoot;

    static Vector3D ParseVector3D(const YAML::Node& n);
    static Color ParseColor(const YAML::Node& n);

    Sphere ParseSphere(const YAML::Node& obj) const;
    Disk ParseDisk(const YAML::Node& obj) const;
    Rectangle ParseRectangle(const YAML::Node& obj) const;
    Box ParseBox(const YAML::Node& obj) const;
    Cylinder ParseCylinder(const YAML::Node& obj) const;

    void CreateOutputDirectory() const;
};

}  // namespace Raytracer
