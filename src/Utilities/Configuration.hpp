#pragma once

#include "Rendering/Camera.hpp"
#include "Rendering/Material.hpp"
#include "Scene/Object.hpp"
#include "Scene/Scene.hpp"

#include <yaml-cpp/yaml.h>

namespace Raytracer {

struct RenderConfig {
    bool renderImage = false;
    bool renderVideo = false;
    double videoDuration = 0.0;  // seconds
    bool openOutputFiles = true;
};

class Configuration {
public:
    static Configuration& GetInstance();

    void ParseYamlFile(const std::string& path);

    std::string GetSceneID() const;
    std::string GetRunID() const;

    size_t GetNumThreads() const;

    RenderConfig GetRenderConfig() const;

    bool RenderImage() const;
    bool RenderImageConvergingVideo() const;
    bool RenderVideo() const;
    double GetVideoDuration() const;

    bool OpenOutputFiles() const;

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

    YAML::Node mRoot;

    std::string mSceneID;
    std::string mRunID;

    static Vector3D ParseVector3D(const YAML::Node& n);
    static Color ParseColor(const YAML::Node& n);
    static Material ParseMaterial(const YAML::Node& mat);

    struct RenderOptions {
        bool renderImage = true;
        bool renderImageConvergingVideo = false;
        bool renderVideo = false;
        double videoDuration = 5.0;  // in seconds
    } mRenderOptions;

    void ParseRenderOptions(const YAML::Node& node);

    struct ObjectProperties {
        std::string id;
        bool visible = true;

        // Dynamics
        Vector3D position = Vector3D({0.0, 0.0, 0.0});
        Vector3D normal = Vector3D({0.0, 0.0, 1.0});
        Vector3D referenceDirection = Vector3D({1.0, 0.0, 0.0});
        Vector3D velocity = Vector3D({0.0, 0.0, 0.0});
        Vector3D acceleration = Vector3D({0.0, 0.0, 0.0});
        Vector3D angularVelocity = Vector3D({0.0, 0.0, 0.0});
        Vector3D spin = Vector3D({0.0, 0.0, 0.0});

        Material material;
    };

    ObjectProperties ParseObjectProperties(const YAML::Node& obj) const;

    Object ParseSphere(const YAML::Node& obj) const;
    Object ParseDisk(const YAML::Node& obj) const;
    Object ParseRectangle(const YAML::Node& obj) const;
    Object ParseBox(const YAML::Node& obj) const;
    Object ParseCylinder(const YAML::Node& obj) const;
    Object ParseTetrahedron(const YAML::Node& obj) const;
    Object ParseOctahedron(const YAML::Node& obj) const;
    Object ParseTriangle(const YAML::Node& obj) const;
    Object ParseTube(const YAML::Node& obj) const;
    Object ParseHalfSphere(const YAML::Node& obj) const;
    Object ParseBoxAxisAligned(const YAML::Node& obj) const;

    std::string CreateRunID() const;
    void CreateOutputDirectory() const;
    void CopyYamlFile(const std::string& path) const;
};

}  // namespace Raytracer
