#include "Utilities/Configuration.hpp"

#include "Geometry/Shapes.hpp"
#include "Scene/ObjectComposite.hpp"
#include "Scene/ObjectPrimitive.hpp"
#include "Scene/Objects.hpp"
#include "Version.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>

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
        mSceneID = id.as<std::string>();
    } else {
        throw std::runtime_error("Missing required key: id");
    }

    mRunID = CreateRunID();

    CreateOutputDirectory();

    CopyYamlFile(path);
}

std::string Configuration::GetSceneID() const {
    return mSceneID;
}

std::string Configuration::GetRunID() const {
    return mRunID;
}

size_t Configuration::GetNumThreads() const {
    // Parse the number of threads from the YAML, default to 1 if not specified
    if (auto threads = mRoot["threads"]) {
        return threads.as<size_t>();
    } else {
        throw std::runtime_error("Missing required key: threads");
    }
}

RenderConfig Configuration::GetRenderConfig() const {
    RenderConfig config;

    if (!mRoot) {
        throw std::runtime_error("No YAML loaded");
    }
    auto node = mRoot["render"];
    if (!node) {
        throw std::runtime_error("Missing render section");
    }

    config.renderImage = node["image"] ? node["image"].as<bool>() : true;
    config.renderVideo = node["video"] ? node["video"].as<bool>() : false;
    config.videoDuration = node["video_duration"] ? node["video_duration"].as<double>() : 5.0;
    config.openOutputFiles = node["open_output_files"] ? node["open_output_files"].as<bool>() : true;

    return config;
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
    } else if (renderingEngineStr == "RAY_TRACER") {
        rendererType = Renderer::Type::RAY_TRACER;
    } else if (renderingEngineStr == "PATH_TRACER") {
        rendererType = Renderer::Type::PATH_TRACER;
    } else if (renderingEngineStr == "DETERMINISTIC") {
        rendererType = Renderer::Type::DETERMINISTIC;
    } else if (renderingEngineStr == "PATH_TRACER_NEE") {
        rendererType = Renderer::Type::PATH_TRACER_NEE;
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

    // Background
    auto bg = node["background_color"];
    Color backgroundColor = ParseColor(bg);
    Scene scene(backgroundColor);

    // Optional background texture
    if (node["background_texture"]) {
        std::string textureFile = node["background_texture"].as<std::string>();
        scene.SetColorTexture(textureFile);
    }

    if (auto objs = node["objects"]) {
        for (auto obj : objs) {
            std::string type = obj["type"].as<std::string>();

            if (type == "Disk") {
                scene.AddObject(std::make_shared<ObjectPrimitive>(ParseDisk(obj)));
            } else if (type == "Sphere") {
                scene.AddObject(std::make_shared<ObjectPrimitive>(ParseSphere(obj)));
            } else if (type == "Rectangle") {
                scene.AddObject(std::make_shared<ObjectPrimitive>(ParseRectangle(obj)));
            } else if (type == "Box") {
                scene.AddObject(std::make_shared<ObjectPrimitive>(ParseBox(obj)));
            } else if (type == "Cone") {
                scene.AddObject(std::make_shared<ObjectPrimitive>(ParseCone(obj)));
            } else if (type == "Cylinder") {
                scene.AddObject(std::make_shared<ObjectPrimitive>(ParseCylinder(obj)));
            } else if (type == "CylindricalShell") {
                scene.AddObject(std::make_shared<ObjectPrimitive>(ParseCylindricalShell(obj)));
            } else if (type == "Tetrahedron") {
                scene.AddObject(std::make_shared<ObjectPrimitive>(ParseTetrahedron(obj)));
            } else if (type == "Octahedron") {
                scene.AddObject(std::make_shared<ObjectPrimitive>(ParseOctahedron(obj)));
            } else if (type == "Ring") {
                scene.AddObject(std::make_shared<ObjectPrimitive>(ParseRing(obj)));
            } else if (type == "Torus") {
                scene.AddObject(std::make_shared<ObjectPrimitive>(ParseTorus(obj)));
            } else if (type == "Triangle") {
                scene.AddObject(std::make_shared<ObjectPrimitive>(ParseTriangle(obj)));
            } else if (type == "Tube") {
                scene.AddObject(std::make_shared<ObjectPrimitive>(ParseTube(obj)));
            } else if (type == "SphericalCap") {
                scene.AddObject(std::make_shared<ObjectPrimitive>(ParseSphericalCap(obj)));
            } else if (type == "HalfTorus") {
                scene.AddObject(std::make_shared<ObjectPrimitive>(ParseHalfTorus(obj)));
            } else if (type == "HalfTorusWithSphericalCaps") {
                scene.AddObject(std::make_shared<ObjectPrimitive>(ParseHalfTorusWithSphericalCaps(obj)));
            } else if (type == "BoxAxisAligned") {
                scene.AddObject(std::make_shared<ObjectPrimitive>(ParseBoxAxisAligned(obj)));
            } else {
                // Check if it's a composite object type
                static const std::vector<std::string> compositeTypes = {"Globus"};
                auto it = std::find(compositeTypes.begin(), compositeTypes.end(), type);
                if (it != compositeTypes.end()) {
                    scene.AddObject(std::make_shared<ObjectComposite>(ParseCompositeObject(obj, type)));
                } else {
                    throw std::runtime_error("Unknown object type: " + type);
                }
            }
        }
    }

    double time = node["time"] ? node["time"].as<double>() : 0.0;
    scene.SetTime(time);

    return scene;
}

std::string Configuration::GetOutputDirectory() const {
    return PROJECT_DIR "output/" + mSceneID;
}

void Configuration::PrintInfo() const {
    std::cout << "\nConfiguration Information:" << std::endl
              << "Scene ID:\t\t" << mSceneID << std::endl
              << "Run ID:\t\t" << mRunID << std::endl
              << "Threads:\t" << GetNumThreads() << std::endl
              << "Output:\t\t" << GetOutputDirectory() << std::endl
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
        {"black", BLACK},
        {"white", WHITE},
        {"gray", GRAY},
        {"red", RED},
        {"orange", ORANGE},
        {"yellow", YELLOW},
        {"cyan", CYAN},
        {"blue", BLUE},
        {"green", GREEN},
    };

    // --- Named colors (scalar) ---
    if (n.IsScalar()) {
        std::string s = n.as<std::string>();
        std::transform(s.begin(), s.end(), s.begin(),
                       [](unsigned char c) { return std::tolower(c); });

        auto it = namedColors.find(s);
        if (it != namedColors.end()) {
            return it->second;
        }

        throw std::runtime_error("Unknown color name: " + s);
    }

    // --- RGB sequence: [r, g, b] ---
    if (n.IsSequence() && n.size() == 3) {
        double r = n[0].as<double>();
        double g = n[1].as<double>();
        double b = n[2].as<double>();

        double maxv = std::max({r, g, b});
        if (maxv > 1.0) {
            return Color(r / 255.0, g / 255.0, b / 255.0);
        } else {
            return Color(r, g, b);
        }
    }

    // --- RGB map: {r: ..., g: ..., b: ...} ---
    if (n.IsMap()) {
        if (!n["r"] || !n["g"] || !n["b"]) {
            throw std::runtime_error("Color map must contain keys r, g, and b");
        }

        double r = n["r"].as<double>();
        double g = n["g"].as<double>();
        double b = n["b"].as<double>();

        double maxv = std::max({r, g, b});
        if (maxv > 1.0) {
            return Color(r / 255.0, g / 255.0, b / 255.0);
        } else {
            return Color(r, g, b);
        }
    }
    throw std::runtime_error("Color must be a scalar name, a 3-element sequence, or a map {r,g,b}");
}

Material Configuration::ParseMaterial(const YAML::Node& mat) {
    if (!mat.IsMap()) {
        throw std::runtime_error("Material node must be a map");
    }

    // 1️⃣ Parse colors
    Color baseColor = Color(1.0, 1.0, 1.0);  // default white
    if (mat["baseColor"]) {
        baseColor = ParseColor(mat["baseColor"]);
    } else {
        throw std::runtime_error("Material must have a base color");
    }

    // Optional specular color
    Color specularColor = baseColor;  // default to base color
    if (mat["specularColor"]) {
        specularColor = ParseColor(mat["specularColor"]);
    }

    // Optional color texture
    std::string textureFilename;
    if (mat["colorTexture"]) {
        textureFilename = mat["colorTexture"].as<std::string>();
    }

    // 2️⃣ Optional scalar properties
    double roughness = mat["roughness"] ? mat["roughness"].as<double>() : 0.0;
    double refractiveIndex = mat["refractiveIndex"] ? mat["refractiveIndex"].as<double>() : 1.0;
    double meanFreePath = mat["meanFreePath"] ? mat["meanFreePath"].as<double>() : 0.0;
    double radiance = mat["radiance"] ? mat["radiance"].as<double>() : 0.0;
    bool useFresnel = mat["useFresnel"] ? mat["useFresnel"].as<bool>() : true;

    // 3️⃣ Construct the material
    Material material(baseColor, roughness, refractiveIndex, meanFreePath, radiance);
    material.SetSpecularColor(specularColor);
    if (!textureFilename.empty()) {
        material.SetColorTexture(textureFilename);
    }
    material.SetUseFresnel(useFresnel);

    // 4️⃣ Optional interaction probabilities
    if (mat["probabilities"] && mat["probabilities"].IsMap()) {
        std::map<Material::InteractionType, double> probs;
        const YAML::Node& p = mat["probabilities"];

        if (p["diffuse"]) {
            probs[Material::InteractionType::DIFFUSE] = p["diffuse"].as<double>();
        }
        if (p["reflective"]) {
            probs[Material::InteractionType::REFLECTIVE] = p["reflective"].as<double>();
        }
        if (p["refractive"]) {
            probs[Material::InteractionType::REFRACTIVE] = p["refractive"] ? p["refractive"].as<double>() : 0.0;
        }

        material.SetInteractionProbabilities(probs);
    }

    return material;
}

Configuration::ObjectProperties Configuration::ParseObjectProperties(const YAML::Node& obj) const {
    ObjectProperties props;

    // Static properties
    props.id = obj["id"].as<std::string>();
    props.visible = obj["visible"] ? obj["visible"].as<bool>() : true;
    props.material = obj["material"] ? ParseMaterial(obj["material"]) : Material();
    props.position = obj["position"] ? ParseVector3D(obj["position"]) : Vector3D({0.0, 0.0, 0.0});
    props.normal = obj["normal"] ? ParseVector3D(obj["normal"]) : Vector3D({0.0, 0.0, 1.0});
    props.referenceDirection = obj["reference_direction"] ? ParseVector3D(obj["reference_direction"]) : Vector3D({0.0, 0.0, 0.0});

    // Dynamics
    props.velocity = obj["velocity"] ? ParseVector3D(obj["velocity"]) : Vector3D({0.0, 0.0, 0.0});
    props.acceleration = obj["acceleration"] ? ParseVector3D(obj["acceleration"]) : Vector3D({0.0, 0.0, 0.0});
    props.angularVelocity = obj["angular_velocity"] ? ParseVector3D(obj["angular_velocity"]) : Vector3D({0.0, 0.0, 0.0});
    props.spin = obj["spin"] ? ParseVector3D(obj["spin"]) : Vector3D({0.0, 0.0, 0.0});

    return props;
}

ObjectPrimitive Configuration::ParseSphere(const YAML::Node& obj) const {
    ObjectProperties props = ParseObjectProperties(obj);
    double radius = obj["radius"].as<double>();

    // Construct the sphere
    ObjectPrimitive sphere = MakePrimitiveObject<Geometry::Sphere>(props.id, props.material, props.position, radius, props.normal, props.referenceDirection);

    sphere.SetVelocity(props.velocity);
    sphere.SetAcceleration(props.acceleration);
    sphere.SetAngularVelocity(props.angularVelocity);
    sphere.SetSpin(props.spin);

    sphere.SetVisible(props.visible);

    return sphere;
}

ObjectPrimitive Configuration::ParseDisk(const YAML::Node& obj) const {
    ObjectProperties props = ParseObjectProperties(obj);
    double radius = obj["radius"].as<double>();

    // Construct the disk
    ObjectPrimitive disk = MakePrimitiveObject<Geometry::Disk>(props.id, props.material, props.position, props.normal, radius);

    disk.SetVelocity(props.velocity);
    disk.SetAcceleration(props.acceleration);
    disk.SetAngularVelocity(props.angularVelocity);
    disk.SetSpin(props.spin);

    disk.SetVisible(props.visible);
    return disk;
}

ObjectPrimitive Configuration::ParseRectangle(const YAML::Node& obj) const {
    ObjectProperties props = ParseObjectProperties(obj);
    double width = obj["dimensions"]["width"].as<double>();
    double height = obj["dimensions"]["height"].as<double>();

    // Construct the rectangle
    ObjectPrimitive rectangle = MakePrimitiveObject<Geometry::Rectangle>(props.id, props.material, props.position, props.normal, props.referenceDirection, width, height);

    rectangle.SetVelocity(props.velocity);
    rectangle.SetAcceleration(props.acceleration);
    rectangle.SetAngularVelocity(props.angularVelocity);
    rectangle.SetSpin(props.spin);
    rectangle.SetVisible(props.visible);
    return rectangle;
}

ObjectPrimitive Configuration::ParseBox(const YAML::Node& obj) const {
    ObjectProperties props = ParseObjectProperties(obj);

    auto dim = obj["dimensions"];
    double length = dim["length"].as<double>();
    double width = dim["width"].as<double>();
    double height = dim["height"].as<double>();

    // Construct the box
    ObjectPrimitive box = MakePrimitiveObject<Geometry::Box>(props.id, props.material, props.position, props.normal, props.referenceDirection, length, width, height);

    box.SetVisible(props.visible);

    box.SetVelocity(props.velocity);
    box.SetAcceleration(props.acceleration);
    box.SetAngularVelocity(props.angularVelocity);
    box.SetSpin(props.spin);

    return box;
}

ObjectPrimitive Configuration::ParseCylinder(const YAML::Node& obj) const {
    ObjectProperties props = ParseObjectProperties(obj);
    double radius = obj["radius"].as<double>();
    double height = obj["height"].as<double>();

    // Construct the cylinder
    ObjectPrimitive cylinder = MakePrimitiveObject<Geometry::Cylinder>(props.id, props.material, props.position, props.normal, radius, height);

    cylinder.SetVelocity(props.velocity);
    cylinder.SetAcceleration(props.acceleration);
    cylinder.SetAngularVelocity(props.angularVelocity);
    cylinder.SetSpin(props.spin);

    cylinder.SetVisible(props.visible);
    return cylinder;
}

ObjectPrimitive Configuration::ParseCylindricalShell(const YAML::Node& obj) const {
    ObjectProperties props = ParseObjectProperties(obj);
    double innerRadius = obj["inner_radius"].as<double>();
    double outerRadius = obj["outer_radius"].as<double>();
    double height = obj["height"].as<double>();

    // Construct the cylindrical shell
    ObjectPrimitive shell = MakePrimitiveObject<Geometry::CylindricalShell>(props.id, props.material, props.position, props.normal, innerRadius, outerRadius, height);

    shell.SetVelocity(props.velocity);
    shell.SetAcceleration(props.acceleration);
    shell.SetAngularVelocity(props.angularVelocity);
    shell.SetSpin(props.spin);

    shell.SetVisible(props.visible);
    return shell;
}

ObjectPrimitive Configuration::ParseCone(const YAML::Node& obj) const {
    ObjectProperties props = ParseObjectProperties(obj);
    double baseRadius = obj["base_radius"].as<double>();
    double height = obj["height"].as<double>();

    // Construct the cone
    ObjectPrimitive cone = MakePrimitiveObject<Geometry::Cone>(props.id, props.material, props.position, props.normal, baseRadius, height);

    cone.SetVelocity(props.velocity);
    cone.SetAcceleration(props.acceleration);
    cone.SetAngularVelocity(props.angularVelocity);
    cone.SetSpin(props.spin);

    cone.SetVisible(props.visible);
    return cone;
}

ObjectPrimitive Configuration::ParseTetrahedron(const YAML::Node& obj) const {
    ObjectProperties props = ParseObjectProperties(obj);
    double edgeLength = obj["edge_length"].as<double>();

    // Construct the tetrahedron
    ObjectPrimitive tetrahedron = MakePrimitiveObject<Geometry::Tetrahedron>(props.id, props.material, props.position, props.normal, props.referenceDirection, edgeLength);

    tetrahedron.SetVelocity(props.velocity);
    tetrahedron.SetAcceleration(props.acceleration);
    tetrahedron.SetAngularVelocity(props.angularVelocity);
    tetrahedron.SetSpin(props.spin);

    tetrahedron.SetVisible(props.visible);
    return tetrahedron;
}

ObjectPrimitive Configuration::ParseRing(const YAML::Node& obj) const {
    ObjectProperties props = ParseObjectProperties(obj);
    double innerRadius = obj["inner_radius"].as<double>();
    double outerRadius = obj["outer_radius"].as<double>();

    // Construct the ring
    ObjectPrimitive ring = MakePrimitiveObject<Geometry::Ring>(props.id, props.material, props.position, props.normal, innerRadius, outerRadius);

    ring.SetVelocity(props.velocity);
    ring.SetAcceleration(props.acceleration);
    ring.SetAngularVelocity(props.angularVelocity);
    ring.SetSpin(props.spin);

    ring.SetVisible(props.visible);
    return ring;
}

ObjectPrimitive Configuration::ParseTorus(const YAML::Node& obj) const {
    ObjectProperties props = ParseObjectProperties(obj);
    double majorRadius = obj["major_radius"].as<double>();
    double minorRadius = obj["minor_radius"].as<double>();

    // Construct the torus
    ObjectPrimitive torus = MakePrimitiveObject<Geometry::Torus>(props.id, props.material, props.position, props.normal, majorRadius, minorRadius);

    torus.SetVelocity(props.velocity);
    torus.SetAcceleration(props.acceleration);
    torus.SetAngularVelocity(props.angularVelocity);
    torus.SetSpin(props.spin);

    torus.SetVisible(props.visible);
    return torus;
}

ObjectPrimitive Configuration::ParseOctahedron(const YAML::Node& obj) const {
    ObjectProperties props = ParseObjectProperties(obj);
    double edgeLength = obj["edge_length"].as<double>();

    // Construct the octahedron
    ObjectPrimitive octahedron = MakePrimitiveObject<Geometry::Octahedron>(props.id, props.material, props.position, props.normal, props.referenceDirection, edgeLength);

    octahedron.SetVelocity(props.velocity);
    octahedron.SetAcceleration(props.acceleration);
    octahedron.SetAngularVelocity(props.angularVelocity);
    octahedron.SetSpin(props.spin);

    octahedron.SetVisible(props.visible);
    return octahedron;
}

ObjectPrimitive Configuration::ParseTriangle(const YAML::Node& obj) const {
    ObjectProperties props = ParseObjectProperties(obj);
    auto vertices = obj["vertices"];
    Vector3D v1 = ParseVector3D(vertices[0]);
    Vector3D v2 = ParseVector3D(vertices[1]);
    Vector3D v3 = ParseVector3D(vertices[2]);

    // Construct the triangle
    ObjectPrimitive triangle = MakePrimitiveObject<Geometry::Triangle>(props.id, props.material, v1, v2, v3);

    triangle.SetVelocity(props.velocity);
    triangle.SetAcceleration(props.acceleration);
    triangle.SetAngularVelocity(props.angularVelocity);
    triangle.SetSpin(props.spin);

    triangle.SetVisible(props.visible);
    return triangle;
}

ObjectPrimitive Configuration::ParseTube(const YAML::Node& obj) const {
    ObjectProperties props = ParseObjectProperties(obj);
    double radius = obj["radius"].as<double>();
    double height = obj["height"].as<double>();

    // Construct the tube
    ObjectPrimitive tube = MakePrimitiveObject<Geometry::Tube>(props.id, props.material, props.position, props.normal, radius, height);

    tube.SetVelocity(props.velocity);
    tube.SetAcceleration(props.acceleration);
    tube.SetAngularVelocity(props.angularVelocity);
    tube.SetSpin(props.spin);

    tube.SetVisible(props.visible);
    return tube;
}

ObjectPrimitive Configuration::ParseSphericalCap(const YAML::Node& obj) const {
    ObjectProperties props = ParseObjectProperties(obj);
    double radius = obj["radius"].as<double>();

    // Construct the half-sphere
    ObjectPrimitive sphericalCap = MakePrimitiveObject<Geometry::SphericalCap>(props.id, props.material, props.position, radius, props.normal);

    // Dynamics
    sphericalCap.SetVelocity(props.velocity);
    sphericalCap.SetAcceleration(props.acceleration);
    sphericalCap.SetAngularVelocity(props.angularVelocity);
    sphericalCap.SetSpin(props.spin);
    sphericalCap.SetVisible(props.visible);

    return sphericalCap;
}

ObjectPrimitive Configuration::ParseHalfTorus(const YAML::Node& obj) const {
    ObjectProperties props = ParseObjectProperties(obj);
    double majorRadius = obj["major_radius"].as<double>();
    double minorRadius = obj["minor_radius"].as<double>();

    // Construct the half-torus
    ObjectPrimitive halfTorus = MakePrimitiveObject<Geometry::HalfTorus>(props.id, props.material, props.position, props.normal, props.referenceDirection, majorRadius, minorRadius);

    halfTorus.SetVelocity(props.velocity);
    halfTorus.SetAcceleration(props.acceleration);
    halfTorus.SetAngularVelocity(props.angularVelocity);
    halfTorus.SetSpin(props.spin);

    halfTorus.SetVisible(props.visible);
    return halfTorus;
}

ObjectPrimitive Configuration::ParseHalfTorusWithSphericalCaps(const YAML::Node& obj) const {
    ObjectProperties props = ParseObjectProperties(obj);
    double majorRadius = obj["major_radius"].as<double>();
    double minorRadius = obj["minor_radius"].as<double>();

    // Construct the half-torus with spherical caps
    ObjectPrimitive halfTorusWithCaps = MakePrimitiveObject<Geometry::HalfTorusWithSphericalCaps>(props.id, props.material, props.position, props.normal, props.referenceDirection, majorRadius, minorRadius);

    halfTorusWithCaps.SetVelocity(props.velocity);
    halfTorusWithCaps.SetAcceleration(props.acceleration);
    halfTorusWithCaps.SetAngularVelocity(props.angularVelocity);
    halfTorusWithCaps.SetSpin(props.spin);

    halfTorusWithCaps.SetVisible(props.visible);
    return halfTorusWithCaps;
}

ObjectPrimitive Configuration::ParseBoxAxisAligned(const YAML::Node& obj) const {
    ObjectProperties props = ParseObjectProperties(obj);
    double length = obj["dimensions"]["length"].as<double>();
    double width = obj["dimensions"]["width"].as<double>();
    double height = obj["dimensions"]["height"].as<double>();

    // Construct the axis-aligned box
    ObjectPrimitive boxAA = MakePrimitiveObject<Geometry::BoxAxisAligned>(props.id, props.material, props.position, length, width, height);

    boxAA.SetVelocity(props.velocity);
    boxAA.SetAcceleration(props.acceleration);
    boxAA.SetAngularVelocity(props.angularVelocity);
    boxAA.SetSpin(props.spin);
    boxAA.SetVisible(props.visible);

    return boxAA;
}

ObjectComposite Configuration::ParseCompositeObject(const YAML::Node& obj, const std::string& type) const {
    ObjectProperties props = ParseObjectProperties(obj);
    double referenceLength = obj["reference_length"].as<double>();

    // Call the appropriate factory method based on type
    if (type == "Globus") {
        return Items::CreateGlobus(referenceLength, props.position, props.normal, props.referenceDirection);
    } else {
        throw std::runtime_error("Unknown composite object type: " + type);
    }
}

std::string Configuration::CreateRunID() const {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&t);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d_%H-%M-%S");
    return oss.str();
}

void Configuration::CreateOutputDirectory() const {
    std::filesystem::create_directories(GetOutputDirectory());
}

void Configuration::CopyYamlFile(const std::string& path) const {
    // Ensure config subdirectory exists
    auto configDir = std::filesystem::path(GetOutputDirectory()) / "configs";
    std::filesystem::create_directories(configDir);

    // Target file path
    auto targetPath = configDir / ("config_" + mRunID + ".yaml");

    // Open source and target files
    std::ifstream src(path);
    if (!src.is_open()) {
        throw std::runtime_error("Failed to open source YAML file: " + path);
    }

    std::ofstream dst(targetPath);
    if (!dst.is_open()) {
        throw std::runtime_error("Failed to create config file: " + targetPath.string());
    }

    // Write header line
    dst << "# " << PROJECT_NAME << "-" << PROJECT_VERSION << "\tgit:"
        << GIT_BRANCH << "/" << GIT_COMMIT_HASH << "\n\n";

    // Copy YAML content
    dst << src.rdbuf();
}

}  // namespace Raytracer
