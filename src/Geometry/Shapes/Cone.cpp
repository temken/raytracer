#include "Geometry/Shapes/Cone.hpp"

namespace Raytracer::Geometry {

Cone::Cone(const Vector3D& position, const Vector3D& orientation, double radius, double height) :
    Shape(Shape::Type::CONE, position, orientation),
    mRadius(radius),
    mHeight(height) {
    mSlantHeight = std::sqrt(mHeight * mHeight + mRadius * mRadius);
}

std::optional<Intersection> Cone::Intersect(const Line& line) const {
    // Line
    Vector3D origin = line.GetOrigin();
    Vector3D direction = line.GetDirection();

    // Cone
    Vector3D coneNormal = GetBasisVector(OrthonormalBasis::BasisVector::eZ);
    Vector3D coneApex = mPosition + mHeight * coneNormal;
    double cosTheta = mHeight / mSlantHeight;
    double cosTheta2 = cosTheta * cosTheta;

    // Parameters of the quadratic equation atˆ2 + bt + c = 0
    Vector3D co = origin - coneApex;
    double a = std::pow(direction.Dot(coneNormal), 2) - cosTheta2;
    double b = 2.0 * (direction.Dot(coneNormal) * co.Dot(coneNormal) - direction.Dot(co) * cosTheta2);
    double c = std::pow(co.Dot(coneNormal), 2) - co.NormSquared() * cosTheta2;

    double discriminant = b * b - 4.0 * a * c;

    std::vector<double> roots;
    if (discriminant < 0.0) {
        return std::nullopt;  // No intersection
    } else if (std::abs(discriminant) < sEpsilon) {
        double root = -b / (2.0 * a);
        roots.push_back(root);
    } else {
        double sqrtDiscriminant = std::sqrt(discriminant);
        double root1 = (-b - sqrtDiscriminant) / (2.0 * a);
        double root2 = (-b + sqrtDiscriminant) / (2.0 * a);
        roots.push_back(root1);
        roots.push_back(root2);
    }

    std::vector<Intersection> intersections;
    for (double root : roots) {
        if (root < line.GetTMin()) {
            continue;  // Ignore intersections behind the ray origin
        }
        Vector3D intersectionPoint = line.PointAtParameter(root);
        Vector3D v = intersectionPoint - coneApex;
        double heightAlongAxis = v.Dot(coneNormal);
        if (heightAlongAxis <= 0.0 && heightAlongAxis >= -mHeight) {
            Vector3D axialComponent = coneNormal * heightAlongAxis;
            Vector3D radialComponent = v - axialComponent;
            double tanTheta = mRadius / mHeight;
            Vector3D normal = (radialComponent - tanTheta * tanTheta * axialComponent).Normalized();
            intersections.push_back(Intersection{root, intersectionPoint, normal});
        }
    }
    if (intersections.empty()) {
        return std::nullopt;
    }
    return *std::min_element(intersections.begin(), intersections.end());
}

double Cone::SurfaceArea() const {
    return M_PI * mRadius * std::sqrt(mHeight * mHeight + mRadius * mRadius);
}

std::vector<Vector3D> Cone::SampleSurfacePoints(std::size_t numPoints, std::mt19937& prng) const {
    std::vector<Vector3D> points;

    std::uniform_real_distribution<double> dist(0.0, 1.0);

    for (std::size_t i = 0; i < numPoints; ++i) {
        double u = dist(prng);
        double v = dist(prng);

        double rho = std::sqrt(u) * mSlantHeight;
        double theta = 2.0 * M_PI * v;

        double cosTheta = mHeight / mSlantHeight;
        double sinTheta = mRadius / mSlantHeight;

        double radialDistance = (rho * sinTheta);
        double x = radialDistance * std::cos(theta);
        double y = radialDistance * std::sin(theta);
        double z = mHeight - (rho * cosTheta);
        Vector3D localPoint{x, y, z};
        Vector3D globalPoint = mOrthonormalBasis.ToGlobal(localPoint) + mPosition;
        points.push_back(globalPoint);
    }
    return points;
}

std::vector<Vector3D> Cone::GetKeyPoints() const {
    return {mPosition};
}

std::pair<double, double> Cone::GetSurfaceParameters(const Vector3D& point) const {
    return {0.0, 0.0};
}

void Cone::PrintInfo() const {
    PrintInfoBase();
    std::cout << "\tRadius: " << mRadius << std::endl
              << "\tHeight: " << mHeight << std::endl
              << std::endl;
}

}  // namespace Raytracer::Geometry
