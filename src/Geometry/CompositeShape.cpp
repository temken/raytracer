#include "Geometry/CompositeShape.hpp"

#include <algorithm>
#include <iostream>
#include <memory>
#include <optional>
#include <vector>

namespace Raytracer::Geometry {

CompositeShape::CompositeShape(const Vector3D& position, const Vector3D& normal, const Vector3D) :
    Shape(Type::BASE, position, normal) {
}

void CompositeShape::AddComponent(std::shared_ptr<Shape> component) {
    mComponents.push_back(std::move(component));
}

std::optional<Intersection> CompositeShape::Intersect(const Line& line, double tMin) const {
    std::optional<Intersection> closestIntersection;
    for (const auto& component : mComponents) {
        auto intersection = component->Intersect(line, tMin);
        if (intersection) {
            if (!closestIntersection || intersection->t < closestIntersection->t) {
                closestIntersection = intersection;
            }
        }
    }
    return closestIntersection;
}

double CompositeShape::SurfaceArea() const {
    double totalArea = 0.0;
    for (const auto& component : mComponents) {
        totalArea += component->SurfaceArea();
    }
    return totalArea;
}

std::vector<Vector3D> CompositeShape::SampleSurfacePoints(std::size_t numPoints, std::mt19937& prng) const {
    std::vector<Vector3D> points;
    // Compute probabilities based on component areas
    std::vector<double> componentAreas;
    for (const auto& component : mComponents) {
        double area = component->SurfaceArea();
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

std::vector<Vector3D> CompositeShape::GetKeyPoints() const {
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

void CompositeShape::PrintInfo() const {
    PrintInfoCompositeBase();
    // std::cout << "Composite Shape Components Info:" << std::endl;
    // for (const auto& component : mComponents) {
    //     component->PrintInfo();
    // }
}

void CompositeShape::PrintInfoCompositeBase() const {
    PrintInfoBase();
    std::cout << "Shape:\tComposite Shape" << std::endl
              << "Number of Components:\t" << mComponents.size() << std::endl
              << std::endl;
}

}  // namespace Raytracer::Geometry
