#include "Scene/ObjectPrimitive.hpp"

namespace Raytracer {

ObjectPrimitive::ObjectPrimitive(const ::std::string& name, const Material& material, std::shared_ptr<Geometry::Shape> shape) :
    Object(Type::PRIMITIVE, name),
    mMaterial(material),
    mShape(std::move(shape)) {}

std::optional<Object::Intersection> ObjectPrimitive::Intersect(const Ray& ray) const {
    if (!mShape) {
        return std::nullopt;
    }

    auto geometryIntersection = mShape->Intersect(ray);
    if (geometryIntersection.has_value()) {
        Intersection intersection;
        intersection.t = geometryIntersection->t;
        intersection.point = geometryIntersection->point;
        intersection.normal = geometryIntersection->normal;
        intersection.object = std::const_pointer_cast<ObjectPrimitive>(shared_from_this());
        return intersection;
    }

    return std::nullopt;
}

void ObjectPrimitive::SetVisible(bool visible) {
    mVisible = visible;
}
bool ObjectPrimitive::IsVisible() const {
    return mVisible;
}

std::vector<std::shared_ptr<ObjectPrimitive>> ObjectPrimitive::GetLightSources() const {
    if (EmitsLight()) {
        return {std::const_pointer_cast<ObjectPrimitive>(shared_from_this())};
    } else {
        return {};
    }
}

Material& ObjectPrimitive::GetMaterial() {
    return mMaterial;
}

bool ObjectPrimitive::EmitsLight() const {
    return mMaterial.EmitsLight();
}

Color ObjectPrimitive::GetColor(const Intersection& intersection) const {
    return mMaterial.GetColor(intersection);
}

std::shared_ptr<Geometry::Shape> ObjectPrimitive::GetShape() const {
    return mShape;
}

Vector3D ObjectPrimitive::GetNormal(const Intersection& intersection) const {
    // TODO: Normal texturing
    return intersection.normal;
}

void ObjectPrimitive::Evolve(double timeDelta) {
    if (!IsDynamic()) {
        return;
    }
    const double sEpsilon = 1e-8;
    if (mVelocity.Norm() > sEpsilon) {
        Translate(mVelocity * timeDelta);
    }
    if (mAcceleration.Norm() > sEpsilon) {
        Vector3D deltaV = mAcceleration * timeDelta;
        Translate((mVelocity + deltaV * 0.5) * timeDelta);  // Verlet integration
        mVelocity += deltaV;
    }
    if (mAngularVelocity.Norm() > sEpsilon) {
        // TODO: Right now we only rotate around lines through the origin
        Geometry::Line rotationAxis(Vector3D({0.0, 0.0, 0.0}), mAngularVelocity.Normalized());
        Rotate(mAngularVelocity.Norm() * timeDelta, rotationAxis);
    }
    if (mSpin.Norm() > sEpsilon) {
        Spin(mSpin.Norm() * timeDelta, mSpin.Normalized());
    }
}

void ObjectPrimitive::PrintInfo() const {
    PrintInfoBase();
}

void ObjectPrimitive::Translate(const Vector3D& translation) {
    mShape->SetPosition(mShape->GetPosition() + translation);
}

void ObjectPrimitive::Rotate(double angle, const Geometry::Line& axis) {
    mShape->Rotate(angle, axis);
}

void ObjectPrimitive::Spin(double angle, const Vector3D& axis) {
    mShape->Spin(angle, axis);
}

}  // namespace Raytracer
