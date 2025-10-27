#include "Scene/CompositeObject.hpp"

#include <algorithm>
#include <iostream>
#include <memory>
#include <optional>
#include <vector>

namespace Raytracer {

CompositeObject::CompositeObject(const std::string& name, const Material& material, const Vector3D& position, const Vector3D& normal) :
    Object(name, material, position, normal) {
}

void CompositeObject::AddComponent(std::shared_ptr<Object> component) {
    mComponents.push_back(std::move(component));
}
const std::vector<std::shared_ptr<Object>>& CompositeObject::GetComponents() const {
    return mComponents;
}

double CompositeObject::GetSurfaceArea() const {
    double totalArea = 0.0;
    for (const auto& component : mComponents) {
        totalArea += component->GetSurfaceArea();
    }
    return totalArea;
}

std::vector<Vector3D> CompositeObject::SampleSurfacePoints(std::size_t numPoints, std::mt19937& prng) const {
    std::vector<Vector3D> points;
    // Compute probabilities based on component areas
    std::vector<double> componentAreas;
    for (const auto& component : mComponents) {
        double area = component->GetSurfaceArea();
        componentAreas.push_back(area);
    }
    std::discrete_distribution<std::size_t> componentDist(componentAreas.begin(), componentAreas.end());
    // Sample points
    for (std::size_t i = 0; i < numPoints; i++) {
        std::size_t compIndex = componentDist(prng);
        auto compPoints = mComponents[compIndex]->SampleSurfacePoints(1, prng);
        points.push_back(compPoints[0]);
    }
    return points;
}

std::vector<Vector3D> CompositeObject::GetKeyPoints() const {
    std::vector<Vector3D> keyPoints;
    for (const auto& component : mComponents) {
        auto compKeyPoints = component->GetKeyPoints();
        keyPoints.insert(keyPoints.end(), compKeyPoints.begin(), compKeyPoints.end());
    }
    // Remove duplicates
    std::sort(keyPoints.begin(), keyPoints.end(), [](const Vector3D& a, const Vector3D& b) {
        for (size_t i = 0; i < 3; ++i) {
            if (a[i] != b[i]) {
                return a[i] < b[i];
            }
        }
        return false;
    });
    keyPoints.erase(std::unique(keyPoints.begin(), keyPoints.end()), keyPoints.end());
    return keyPoints;
}

std::optional<Intersection> CompositeObject::Intersect(const Ray& ray) {
    std::optional<Intersection> closestIntersection;
    for (const auto& component : mComponents) {
        auto intersection = component->Intersect(ray);
        if (intersection) {
            if (!closestIntersection || intersection->t < closestIntersection->t) {
                closestIntersection = intersection;
            }
        }
    }
    closestIntersection->object = this;
    return closestIntersection;
}

void CompositeObject::PrintCompositeInfo() const {
    PrintInfoBase();
    std::cout << "Shape:\tComposite Object" << std::endl
              << "\tComponents:\t" << mComponents.size() << std::endl;
    for (const auto& component : mComponents) {
        std::cout << "\t - " << component->GetName() << std::endl;
    }
}

}  // namespace Raytracer
