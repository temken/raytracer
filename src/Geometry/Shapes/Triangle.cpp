#include "Geometry/Shapes/Triangle.hpp"

namespace Raytracer::Geometry {

Triangle::Triangle(const Vector3D& vertex1, const Vector3D& vertex2, const Vector3D& vertex3) :
    Shape(Type::TRIANGLE, (vertex1 + vertex2 + vertex3) / 3.0, (vertex2 - vertex1).Cross(vertex3 - vertex1).Normalized(), (vertex2 - vertex1).Normalized()) {
    // Store vertices in local coordinate system (transformed through orthonormal basis)
    Vector3D relVertex1 = vertex1 - mPosition;
    Vector3D relVertex2 = vertex2 - mPosition;
    Vector3D relVertex3 = vertex3 - mPosition;

    mVertices[0] = mOrthonormalBasis.ToLocal(relVertex1);
    mVertices[1] = mOrthonormalBasis.ToLocal(relVertex2);
    mVertices[2] = mOrthonormalBasis.ToLocal(relVertex3);
    mEdges[0] = mVertices[1] - mVertices[0];
    mEdges[1] = mVertices[2] - mVertices[0];
}

std::optional<Intersection> Triangle::Intersect(const Line& line) const {
    // Algorithm: Möller–Trumbore intersection algorithm
    // Perform calculations in local coordinate system with mPosition at origin

    Vector3D origin = mOrthonormalBasis.ToLocal(line.GetOrigin() - mPosition);
    Vector3D direction = mOrthonormalBasis.ToLocal(line.GetDirection());

    // Möller–Trumbore intersection algorithm in local coordinates
    Vector3D h = direction.Cross(mEdges[1]);
    double det = mEdges[0].Dot(h);

    if (std::fabs(det) < sEpsilon) {
        return std::nullopt;  // Parallel to triangle
    }

    double invDet = 1.0 / det;
    Vector3D s = origin - mVertices[0];
    double u = invDet * s.Dot(h);

    if ((u < 0.0 && std::abs(u) > sEpsilon) || (u > 1.0 && std::abs(u - 1.0) > sEpsilon)) {
        return std::nullopt;
    }

    Vector3D q = s.Cross(mEdges[0]);
    double v = invDet * direction.Dot(q);

    if ((v < 0.0 && std::abs(v) > sEpsilon) || (u + v > 1.0 && std::abs(u + v - 1.0) > sEpsilon)) {
        return std::nullopt;
    }

    double t = invDet * mEdges[1].Dot(q);

    if (t > line.GetTMin()) {
        // Transform intersection point back to world coordinates
        Vector3D localIntersection = origin + t * direction;
        Vector3D worldIntersection = mPosition + mOrthonormalBasis.ToGlobal(localIntersection);

        // Normal is always the Z-axis of the orthonormal basis
        Vector3D normal = mOrthonormalBasis.GetBasisVector(OrthonormalBasis::BasisVector::eZ);

        return Intersection{t, worldIntersection, normal};
    } else {
        return std::nullopt;
    }
}

double Triangle::SurfaceArea() const {
    // Area = 0.5 * |e1 x e2|
    return 0.5 * mEdges[0].Cross(mEdges[1]).Norm();
}

std::vector<Vector3D> Triangle::SampleSurfacePoints(std::size_t numPoints, std::mt19937& prng) const {
    std::vector<Vector3D> points;

    std::uniform_real_distribution<double> dist(0.0, 1.0);

    for (std::size_t i = 0; i < numPoints; ++i) {
        double u = dist(prng);
        double v = dist(prng);

        // Ensure the point is inside the triangle
        if (u + v > 1.0) {
            u = 1.0 - u;
            v = 1.0 - v;
        }

        // Sample point in local coordinates
        Vector3D localPoint = mVertices[0] + u * mEdges[0] + v * mEdges[1];

        // Transform to world coordinates
        Vector3D worldPoint = mPosition + mOrthonormalBasis.ToGlobal(localPoint);
        points.push_back(worldPoint);
    }

    return points;
}

std::vector<Vector3D> Triangle::GetKeyPoints() const {
    std::vector<Vector3D> keyPoints;

    // Transform vertices from local to world coordinates
    keyPoints.push_back(mPosition);  // Center point
    keyPoints.push_back(mPosition + mOrthonormalBasis.ToGlobal(mVertices[0]));
    keyPoints.push_back(mPosition + mOrthonormalBasis.ToGlobal(mVertices[1]));
    keyPoints.push_back(mPosition + mOrthonormalBasis.ToGlobal(mVertices[2]));

    return keyPoints;
}

std::pair<double, double> Triangle::GetSurfaceParameters(const Vector3D& point) const {
}

void Triangle::PrintInfo() const {
    PrintInfoBase();
    std::cout << "Vertices (world coordinates):" << std::endl
              << "\tV1: " << mPosition + mOrthonormalBasis.ToGlobal(mVertices[0]) << std::endl
              << "\tV2: " << mPosition + mOrthonormalBasis.ToGlobal(mVertices[1]) << std::endl
              << "\tV3: " << mPosition + mOrthonormalBasis.ToGlobal(mVertices[2]) << std::endl
              << std::endl;
}

}  // namespace Raytracer::Geometry
