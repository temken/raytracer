#include "Utilities/Configuration.hpp"

#include "version.hpp"

#include <filesystem>
#include <iostream>
#include <map>

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

    if(auto threads = mRoot["threads"]) {
        mNumThreads = threads.as<size_t>();
    } else {
        throw std::runtime_error("Missing required key: threads");
    }

    CreateOutputDirectory();
}

std::string Configuration::GetID() const {
    return mID;
}

size_t Configuration::GetNumThreads() const {
    return mNumThreads;
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

    std::string renderingEngineStr = node["renderer_type"] ? node["renderer_type"].as<std::string>() : "SIMPLE";
    Renderer::Type rendererType;
    if (renderingEngineStr == "SIMPLE") {
        rendererType = Renderer::Type::SIMPLE;
    } else if (renderingEngineStr == "MONTE_CARLO") {
        rendererType = Renderer::Type::MONTE_CARLO;
    } else {
        throw std::invalid_argument("Unknown rendering type: " + renderingEngineStr);
    }

    double fieldOfView = node["fov_deg"].as<double>();
    Vector3D position = ParseVector3D(node["position"]);
    Vector3D direction = ParseVector3D(node["direction"]);

    // Dynamics
    Vector3D velocity = node["velocity"] ? ParseVector3D(node["velocity"]) : Vector3D({0.0, 0.0, 0.0});
    Vector3D angularVelocity = node["angular_velocity"] ? ParseVector3D(node["angular_velocity"]) : Vector3D({0.0, 0.0, 0.0});
    Vector3D spin = node["spin"] ? ParseVector3D(node["spin"]) : Vector3D({0.0, 0.0, 0.0});

    auto res = node["resolution"];
    size_t width = res["width"].as<int>();
    size_t height = res["height"].as<int>();
    bool useAntiAliasing = node["antialiasing"].as<bool>();
    bool blurImage = node["blur_image"] ? node["blur_image"].as<bool>() : false;
    size_t samplesPerPixel = node["samples_per_pixel"].as<int>();
    double framesPerSecond = node["framesPerSecond"] ? node["framesPerSecond"].as<double>() : 30.0;

    // Configure the camera
    Camera camera(position, direction, rendererType);

    camera.SetVelocity(velocity);
    camera.SetAngularVelocity(angularVelocity);
    camera.SetSpin(spin);

    camera.SetFieldOfView(fieldOfView);
    camera.SetResolution(width, height);
    camera.SetSamplesPerPixel(samplesPerPixel);
    camera.SetUseAntiAliasing(useAntiAliasing);
    camera.SetBlurImage(blurImage);
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
            } else if (type == "Cylinder") {
                scene.AddObject(std::make_unique<Cylinder>(ParseCylinder(obj)));
            } else if (type == "Ring") {
                scene.AddObject(std::make_unique<Torus>(ParseTorus(obj)));
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
    std::cout << "\nConfiguration Information:" << std::endl;
    std::cout << "ID:\t\t" << mID << std::endl;
    std::cout << "Threads:\t" << mNumThreads << std::endl;
    std::cout << "Output:\t\t" << GetOutputDirectory() << std::endl
              << std::endl;
}

Vector3D Configuration::ParseVector3D(const YAML::Node& n) {
    return {
        n[0].as<double>(),
        n[1].as<double>(),
        n[2].as<double>()};
}

Color Configuration::ParseColor(const YAML::Node& n) {

    static const std::map<std::string, Color> namedColors = {
        {"black",  BLACK},
        {"white",  WHITE},
        {"gray",   GRAY},
        {"red",    RED},
        {"orange", ORANGE},
        {"yellow", YELLOW},
        {"cyan",   CYAN},
        {"blue",   BLUE},
        {"green",  GREEN},
    };

    // --- Named colors (scalar) ---
    if (n.IsScalar()) {
        std::string s = n.as<std::string>();
        std::transform(s.begin(), s.end(), s.begin(),
                       [](unsigned char c) { return std::tolower(c); });

        auto it = namedColors.find(s);
        if (it != namedColors.end())
            return it->second;

        throw std::runtime_error("Unknown color name: " + s);
    }

    // --- RGB sequence: [r, g, b] ---
    if (n.IsSequence() && n.size() == 3) {
        double r = n[0].as<double>();
        double g = n[1].as<double>();
        double b = n[2].as<double>();

        double maxv = std::max({r, g, b});
        if (maxv > 1.0)
            return Color(r / 255.0, g / 255.0, b / 255.0);
        else
            return Color(r, g, b);
    }

    // --- RGB map: {r: ..., g: ..., b: ...} ---
    if (n.IsMap()) {
        if (!n["r"] || !n["g"] || !n["b"])
            throw std::runtime_error("Color map must contain keys r, g, and b");

        double r = n["r"].as<double>();
        double g = n["g"].as<double>();
        double b = n["b"].as<double>();

        double maxv = std::max({r, g, b});
        if (maxv > 1.0)
            return Color(r / 255.0, g / 255.0, b / 255.0);
        else
            return Color(r, g, b);
    }
    throw std::runtime_error("Color must be a scalar name, a 3-element sequence, or a map \{r,g,b\}");
}

Configuration::ObjectProperties Configuration::ParseObjectProperties(const YAML::Node& obj) const {
    ObjectProperties props;

    props.id = obj["id"].as<std::string>();
    props.visible = obj["visible"] ? obj["visible"].as<bool>() : true;
    props.emitsLight = obj["emits_light"] ? obj["emits_light"].as<bool>() : false;
    props.position = obj["position"] ? ParseVector3D(obj["position"]) : Vector3D({0.0, 0.0, 0.0});
    props.normal = obj["normal"] ? ParseVector3D(obj["normal"]) : Vector3D({0.0, 0.0, 1.0});

    // Dynamics
    props.velocity = obj["velocity"] ? ParseVector3D(obj["velocity"]) : Vector3D({0.0, 0.0, 0.0});
    props.angularVelocity = obj["angular_velocity"] ? ParseVector3D(obj["angular_velocity"]) : Vector3D({0.0, 0.0, 0.0});
    props.spin = obj["spin"] ? ParseVector3D(obj["spin"]) : Vector3D({0.0, 0.0, 0.0});

    auto m = obj["material"];
    if (m) {
        props.color = m["color"] ? ParseColor(m["color"]) : WHITE;
        props.reflective = m["reflective"] ? m["reflective"].as<bool>() : false;
    } else {
        props.color = WHITE;
        props.reflective = false;
    }

    return props;
}

Sphere Configuration::ParseSphere(const YAML::Node& obj) const {
    ObjectProperties props = ParseObjectProperties(obj);
    double radius = obj["radius"].as<double>();

    // Construct the sphere
    Sphere sphere(props.id, props.position, radius, props.color);

    sphere.SetVelocity(props.velocity);
    sphere.SetAngularVelocity(props.angularVelocity);
    sphere.SetSpin(props.spin);

    sphere.SetVisible(props.visible);
    sphere.SetEmitsLight(props.emitsLight);
    sphere.SetReflective(props.reflective);

    return sphere;
}

Disk Configuration::ParseDisk(const YAML::Node& obj) const {
    ObjectProperties props = ParseObjectProperties(obj);
    double radius = obj["radius"].as<double>();

    // Construct the disk
    Disk disk(props.id, props.position, props.normal, radius, props.color);

    disk.SetVelocity(props.velocity);
    disk.SetAngularVelocity(props.angularVelocity);
    disk.SetSpin(props.spin);

    disk.SetVisible(props.visible);
    disk.SetEmitsLight(props.emitsLight);
    disk.SetReflective(props.reflective);
    return disk;
}

Rectangle Configuration::ParseRectangle(const YAML::Node& obj) const {
    ObjectProperties props = ParseObjectProperties(obj);
    double width = obj["dimensions"]["width"].as<double>();
    double height = obj["dimensions"]["height"].as<double>();
    std::string textureFile = obj["material"]["texture"] ? obj["material"]["texture"].as<std::string>() : "";

    // Construct the rectangle
    Rectangle rectangle(props.id, props.position, props.normal, width, height, props.color);

    rectangle.SetVelocity(props.velocity);
    rectangle.SetAngularVelocity(props.angularVelocity);
    rectangle.SetSpin(props.spin);

    rectangle.SetVisible(props.visible);
    rectangle.SetEmitsLight(props.emitsLight);
    rectangle.SetReflective(props.reflective);
    if (!textureFile.empty()) {
        rectangle.SetTexture(textureFile);
    }
    return rectangle;
}

Box Configuration::ParseBox(const YAML::Node& obj) const {
    ObjectProperties props = ParseObjectProperties(obj);

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

    // Construct the box
    Box box(props.id, props.position, length, width, height, colors, reflectives);
    box.SetVisible(props.visible);
    box.SetEmitsLight(props.emitsLight);

    box.SetVelocity(props.velocity);
    box.SetAngularVelocity(props.angularVelocity);
    box.SetSpin(props.spin);

    return box;
}

Cylinder Configuration::ParseCylinder(const YAML::Node& obj) const {
    ObjectProperties props = ParseObjectProperties(obj);
    double radius = obj["radius"].as<double>();
    double height = obj["height"].as<double>();

    auto m = obj["material"];
    Color mantleColor = ParseColor(m["mantle_color"]);
    Color capColor = ParseColor(m["cap_color"]);

    // Construct the cylinder
    Cylinder cylinder(props.id, props.position, props.normal, radius, height, mantleColor, capColor);

    cylinder.SetVelocity(props.velocity);
    cylinder.SetAngularVelocity(props.angularVelocity);
    cylinder.SetSpin(props.spin);

    cylinder.SetVisible(props.visible);
    cylinder.SetEmitsLight(props.emitsLight);
    cylinder.SetReflective(props.reflective);
    return cylinder;
}

Torus Configuration::ParseTorus(const YAML::Node& obj) const {
    ObjectProperties props = ParseObjectProperties(obj);
    double innerRadius = obj["inner_radius"].as<double>();
    double outerRadius = obj["outer_radius"].as<double>();

    // Construct the torus
    Torus torus(props.id, props.position, props.normal, innerRadius, outerRadius, props.color);

    torus.SetVelocity(props.velocity);
    torus.SetAngularVelocity(props.angularVelocity);
    torus.SetSpin(props.spin);

    torus.SetVisible(props.visible);
    torus.SetEmitsLight(props.emitsLight);
    torus.SetReflective(props.reflective);
    return torus;
}

void Configuration::CreateOutputDirectory() const {
    std::filesystem::create_directories(GetOutputDirectory());
}

}  // namespace Raytracer
