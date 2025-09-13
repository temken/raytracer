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
    double framesPerSecond = node["framesPerSecond"] ? node["framesPerSecond"].as<double>() : 30.0;

    // Configure the camera
    Camera camera(position, direction);
    camera.SetFieldOfView(fieldOfView);
    camera.SetResolution(width, height);
    camera.SetSamplesPerPixel(samplesPerPixel);
    camera.SetUseAntiAliasing(useAntiAliasing);
    camera.SetFramesPerSecond(framesPerSecond);

    return camera;
}

Scene Configuration::ConstructScene() const {
    if (!mRoot) {
        throw std::runtime_error("No YAML loaded");
    }
    auto node = mRoot["scene"];
    if (!node) {
        throw std::runtime_error("Missing scene section");
    }

    auto bg = node["background_color"];
    Color backgroundColor = ParseColor(bg);
    Scene scene(backgroundColor);

    if (auto objs = node["objects"]) {
        for (auto obj : objs) {
            std::string type = obj["type"].as<std::string>();

            if (type == "Disk") {
                scene.AddObject(std::make_unique<Disk>(ParseDisk(obj)));
            } else if (type == "Sphere") {
                scene.AddObject(std::make_unique<Sphere>(ParseSphere(obj)));
            } else if (type == "Rectangle") {
                scene.AddObject(std::make_unique<Rectangle>(ParseRectangle(obj)));
            } else if (type == "Box") {
                scene.AddObject(std::make_unique<Box>(ParseBox(obj)));
            } else {
                throw std::runtime_error("Unknown object type: " + type);
            }
        }
    }

    return scene;
}

std::string Configuration::GetOutputDirectory() const {
    return PROJECT_DIR "output/" + mID;
}

void Configuration::PrintInfo() const {
    std::cout << "Configuration Information:" << std::endl;
    std::cout << "ID: " << mID << std::endl;
    std::cout << "Output Directory: " << GetOutputDirectory() << std::endl
              << std::endl;
}

Vector3D Configuration::ParseVector3D(const YAML::Node& n) {
    return {
        n[0].as<double>(),
        n[1].as<double>(),
        n[2].as<double>()};
}

Color Configuration::ParseColor(const YAML::Node& n) {
    if (n.IsSequence()) {
        // RGB triplet: [r, g, b]
        return Color(
            n[0].as<double>(),
            n[1].as<double>(),
            n[2].as<double>());
    }
    if (n.IsScalar()) {
        std::string s = n.as<std::string>();
        // normalize (case-insensitive)
        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });

        // map names to your predefined constants
        if (s == "black") {
            return BLACK;
        }
        if (s == "white") {
            return WHITE;
        }
        if (s == "gray") {
            return GRAY;
        }
        if (s == "red") {
            return RED;
        }
        if (s == "orange") {
            return ORANGE;
        }
        if (s == "yellow") {
            return YELLOW;
        }
        if (s == "cyan") {
            return CYAN;
        }
        if (s == "blue") {
            return BLUE;
        }
        if (s == "green") {
            return GREEN;
        }

        throw std::runtime_error("Unknown color name: " + s);
    }
    throw std::runtime_error("color must be a 3-element sequence or a predefined color name");
}

Disk Configuration::ParseDisk(const YAML::Node& obj) const {
    std::string id = obj["id"].as<std::string>();
    bool visible = obj["visible"] ? obj["visible"].as<bool>() : true;
    Vector3D position = ParseVector3D(obj["position"]);
    Vector3D normal = ParseVector3D(obj["normal"]);
    double radius = obj["radius"].as<double>();

    auto m = obj["material"];
    Color color = ParseColor(m["color"]);
    bool reflective = m["reflective"].as<bool>();

    Disk disk(id, position, normal, radius, color);
    disk.SetVisible(visible);
    disk.SetReflective(reflective);
    return disk;
}

Sphere Configuration::ParseSphere(const YAML::Node& obj) const {
    std::string id = obj["id"].as<std::string>();
    bool visible = obj["visible"] ? obj["visible"].as<bool>() : true;
    Vector3D center = ParseVector3D(obj["position"]);
    double radius = obj["radius"].as<double>();

    auto m = obj["material"];
    Color color = ParseColor(m["color"]);
    bool reflective = m["reflective"].as<bool>();

    Sphere sphere(id, center, radius, color);
    sphere.SetVisible(visible);
    sphere.SetReflective(reflective);
    return sphere;
}

Rectangle Configuration::ParseRectangle(const YAML::Node& obj) const {
    std::string id = obj["id"].as<std::string>();
    bool visible = obj["visible"] ? obj["visible"].as<bool>() : true;
    Vector3D center = ParseVector3D(obj["position"]);
    Vector3D normal = ParseVector3D(obj["normal"]);

    auto dim = obj["dimensions"];
    double width = dim["width"].as<double>();
    double height = dim["height"].as<double>();

    auto m = obj["material"];
    Color color = ParseColor(m["color"]);
    bool reflective = m["reflective"].as<bool>();

    Rectangle rectangle(id, center, normal, width, height, color);
    rectangle.SetVisible(visible);
    rectangle.SetReflective(reflective);
    return rectangle;
}

Box Configuration::ParseBox(const YAML::Node& obj) const {
    std::string id = obj["id"].as<std::string>();
    bool visible = obj["visible"] ? obj["visible"].as<bool>() : true;

    Vector3D center = ParseVector3D(obj["position"]);

    auto dim = obj["dimensions"];
    double length = dim["length"].as<double>();
    double width = dim["width"].as<double>();
    double height = dim["height"].as<double>();

    auto faces = obj["material"]["face_colors"];
    std::vector<std::string> faceKeys = {"+x", "-x", "+y", "-y", "+z", "-z"};
    std::array<Color, 6> colors;
    std::array<bool, 6> reflectives;

    for (size_t i = 0; i < faceKeys.size(); ++i) {
        auto n = faces[faceKeys[i]];
        colors[i] = ParseColor(n["color"]);
        reflectives[i] = n["reflective"].as<bool>();
    }

    Box box(id, center, length, width, height, colors, reflectives);
    box.SetVisible(visible);
    return box;
}

void Configuration::CreateOutputDirectory() const {
    std::filesystem::create_directories(GetOutputDirectory());
}

}  // namespace Raytracer
