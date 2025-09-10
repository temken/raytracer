#include "Utilities/Configuration.hpp"

#include "version.hpp"

#include <filesystem>
#include <iostream>

namespace Raytracer {

Configuration& Configuration::GetInstance() {
    static Configuration instance;
    return instance;
}

void Configuration::ParseYamlFile(const std::string& path) {
    try {
        mRoot = YAML::LoadFile(path);
    } catch (const YAML::Exception& e) {
        throw std::runtime_error(std::string("Failed to load YAML: ") + e.what());
    }

    if (!mRoot || !mRoot.IsMap()) {
        throw std::runtime_error("Top-level YAML must be a map");
    }

    if (auto id = mRoot["id"]) {
        mID = id.as<std::string>();
    } else {
        throw std::runtime_error("Missing required key: id");
    }

    CreateOutputDirectory();
}

std::string Configuration::GetID() const {
    return mID;
}

Camera Configuration::ConstructCamera() const {
    if (!mRoot) {
        throw std::runtime_error("No YAML loaded");
    }
    auto node = mRoot["camera"];
    if (!node) {
        throw std::runtime_error("Missing camera section");
    }

    // Parse camera parameters
    double fieldOfView = node["fov_deg"].as<double>();
    Vector3D position = {
        node["position"][0].as<double>(),
        node["position"][1].as<double>(),
        node["position"][2].as<double>()};
    Vector3D direction = {
        node["direction"][0].as<double>(),
        node["direction"][1].as<double>(),
        node["direction"][2].as<double>()};
    auto res = node["resolution"];
    size_t width = res["width"].as<int>();
    size_t height = res["height"].as<int>();
    bool useAntiAliasing = node["antialiasing"].as<bool>();
    size_t samplesPerPixel = node["samples_per_pixel"].as<int>();

    // Configure the camera
    Camera camera(position, direction);
    camera.SetFieldOfView(fieldOfView);
    camera.SetResolution(width, height);
    camera.SetSamplesPerPixel(samplesPerPixel);
    camera.SetUseAntiAliasing(useAntiAliasing);

    return camera;
}

Scene Configuration::ConstructScene() const {
    return Scene();
}

std::string Configuration::GetImagesDirectory() const {
    return PROJECT_DIR "output/" + mID + "/images/";
}

std::string Configuration::GetVideosDirectory() const {
    return PROJECT_DIR "output/" + mID + "/videos/";
}

void Configuration::PrintInfo() const {
    std::cout << "Configuration Information:" << std::endl;
    std::cout << "ID: " << mID << std::endl;
}

void Configuration::CreateOutputDirectory() const {
    std::filesystem::create_directories(GetImagesDirectory());
    std::filesystem::create_directories(GetVideosDirectory());
}

}  // namespace Raytracer
