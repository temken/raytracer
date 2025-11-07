#include "Geometry/Shapes/Octahedron.hpp"

#include "Geometry/Shapes/Triangle.hpp"

namespace Raytracer::Geometry {

Octahedron::Octahedron(const Vector3D& center, const Vector3D& orientation, const Vector3D& edgeDirection, double edgeLength) :
    CompositeShape(Shape::Type::OCTAHEDRON, center, orientation, edgeDirection),
    mEdgeLength(edgeLength) {
    ComposeShape();
}

void Octahedron::PrintInfo() const {
    PrintInfoCompositeBase();
    std::cout << "\tEdge Length: " << mEdgeLength << std::endl
              << std::endl;
}

void Octahedron::ComposeShape() {
    mComponents.clear();

    const double a = mEdgeLength / std::sqrt(2.0);

    // 6 vertices of a regular octahedron centered at the origin
    std::array<Vector3D, 6> vertices = {
        Vector3D({a, 0.0, 0.0}),   // +X
        Vector3D({-a, 0.0, 0.0}),  // -X
        Vector3D({0.0, a, 0.0}),   // +Y
        Vector3D({0.0, -a, 0.0}),  // -Y
        Vector3D({0.0, 0.0, a}),   // +Z (top apex)
        Vector3D({0.0, 0.0, -a})   // -Z (bottom apex)
    };

    // Transform into world space using your orthonormal basis and center
    for (auto& v : vertices) {
        v = mOrthonormalBasis.ToGlobal(v) + mPosition;
    }

    // Create triangles (ensure consistent winding for outward normals)
    // Top pyramid (apex index 4)
    AddComponent(std::make_shared<Triangle>(vertices[4], vertices[0], vertices[2]));
    AddComponent(std::make_shared<Triangle>(vertices[4], vertices[2], vertices[1]));
    AddComponent(std::make_shared<Triangle>(vertices[4], vertices[1], vertices[3]));
    AddComponent(std::make_shared<Triangle>(vertices[4], vertices[3], vertices[0]));

    // Bottom pyramid (apex index 5)
    AddComponent(std::make_shared<Triangle>(vertices[5], vertices[2], vertices[0]));
    AddComponent(std::make_shared<Triangle>(vertices[5], vertices[1], vertices[2]));
    AddComponent(std::make_shared<Triangle>(vertices[5], vertices[3], vertices[1]));
    AddComponent(std::make_shared<Triangle>(vertices[5], vertices[0], vertices[3]));
}

}  // namespace Raytracer::Geometry
