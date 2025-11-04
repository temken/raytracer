#include "Geometry/Shapes/Tetrahedron.hpp"

#include "Geometry/Shapes/Triangle.hpp"

namespace Raytracer::Geometry {

Tetrahedron::Tetrahedron(const Vector3D& center, const Vector3D& orientation, const Vector3D& edgeDirection, double edgeLength) :
    CompositeShape(Shape::Type::TETRAHEDRON, center, orientation, edgeDirection),
    mEdgeLength(edgeLength) {
    // Define vertices relative to origin with base triangle centered at origin
    std::array<Vector3D, 4> vertices = {
        Vector3D({-edgeLength / 2.0, -edgeLength * std::sqrt(3.0) / 6.0, 0.0}),  // Base triangle vertex 1
        Vector3D({edgeLength / 2.0, -edgeLength * std::sqrt(3.0) / 6.0, 0.0}),   // Base triangle vertex 2
        Vector3D({0.0, edgeLength * std::sqrt(3.0) / 3.0, 0.0}),                 // Base triangle vertex 3
        Vector3D({0.0, 0.0, edgeLength * std::sqrt(6.0) / 3.0})};                // Apex vertex

    // Transform vertices to world coordinates (base triangle center at 'center' position)
    for (auto& vertex : vertices) {
        vertex = mOrthonormalBasis.ToGlobal(vertex) + center;
    }

    // Create triangles with correct winding for outward-pointing normals
    AddComponent(std::make_shared<Triangle>(vertices[2], vertices[1], vertices[0]));
    AddComponent(std::make_shared<Triangle>(vertices[1], vertices[0], vertices[3]));
    AddComponent(std::make_shared<Triangle>(vertices[2], vertices[1], vertices[3]));
    AddComponent(std::make_shared<Triangle>(vertices[0], vertices[2], vertices[3]));
}

void Tetrahedron::PrintInfo() const {
    PrintInfoCompositeBase();
    std::cout << "\tEdge Length: " << mEdgeLength << std::endl
              << std::endl;
}

}  // namespace Raytracer::Geometry
