#include "Scene/Object.hpp"

#include "Geometry/OrthonormalBasis.hpp"

#include <stdexcept>

namespace Raytracer {

Object::Object(const std::string& name, const Material& material, std::shared_ptr<Geometry::Shape> shape) :
    mName(name),
    mMaterial(material),
    mShape(std::move(shape)) {
}

std::string Object::GetName() const {
    return mName;
}

void Object::SetVisible(bool visible) {
    mVisible = visible;
}

bool Object::IsVisible() const {
    return mVisible;
}

Material& Object::GetMaterial() {
    return mMaterial;
}

void Object::SetMaterial(const Material& material) {
    mMaterial = material;
}

std::shared_ptr<Geometry::Shape> Object::GetShape() const {
    return mShape;
}

void Object::SetShape(const std::shared_ptr<Geometry::Shape>& shape) {
    mShape = shape;
}

Color Object::GetColor(const HitRecord& hitRecord) const {
    return mMaterial.GetColor(hitRecord);
}

Vector3D Object::GetNormal(const HitRecord& hitRecord) const {
    // TODO: Normal texturing
    return hitRecord.normal;
}

bool Object::EmitsLight() const {
    return mMaterial.EmitsLight();
}

Vector3D Object::GetVelocity() const {
    return mVelocity;
}

void Object::SetVelocity(const Vector3D& velocity) {
    mVelocity = velocity;
}

Vector3D Object::GetAcceleration() const {
    return mAcceleration;
}

void Object::SetAcceleration(const Vector3D& acceleration) {
    mAcceleration = acceleration;
}

Vector3D Object::GetAngularVelocity() const {
    return mAngularVelocity;
}

void Object::SetAngularVelocity(const Vector3D& angularVelocity) {
    mAngularVelocity = angularVelocity;
}

Vector3D Object::GetSpin() const {
    return mSpin;
}

void Object::SetSpin(const Vector3D& spin) {
    mSpin = spin;
}

std::optional<HitRecord> Object::Intersect(const Ray& ray) {
    if (!mShape) {
        return std::nullopt;
    }

    auto intersection = mShape->Intersect(ray);
    if (intersection) {
        HitRecord hitRecord;
        hitRecord.t = intersection->t;
        hitRecord.point = intersection->point;
        hitRecord.normal = intersection->normal;
        hitRecord.object = this;
        return hitRecord;
    }
    return std::nullopt;
}

bool Object::IsDynamic() const {
    return (mVelocity.Norm() > sEpsilon || mAcceleration.Norm() > sEpsilon || mAngularVelocity.Norm() > sEpsilon || mSpin.Norm() > sEpsilon);
}

void Object::Evolve(double timeStep) {
    if (!IsDynamic()) {
        return;
    }
    if (mVelocity.Norm() > sEpsilon) {
        Translate(mVelocity * timeStep);
    }
    if (mAcceleration.Norm() > sEpsilon) {
        Vector3D deltaV = mAcceleration * timeStep;
        Translate((mVelocity + deltaV * 0.5) * timeStep);  // Verlet integration
        mVelocity += deltaV;
    }
    if (mAngularVelocity.Norm() > sEpsilon) {
        // TODO: Right now we only rotate around the world z-axis
        Rotate(mAngularVelocity.Norm() * timeStep);
    }
    if (mSpin.Norm() > sEpsilon) {
        Spin(mSpin.Norm() * timeStep, mSpin.Normalized());
    }
}

void Object::PrintInfo() const {
    PrintInfoBase();
}

void Object::Translate(const Vector3D& translation) {
    mShape->SetPosition(mShape->GetPosition() + translation);
}

void Object::Rotate(double angle, const Geometry::Line& axis) {
    // TODO: This should actually be a rotation around a line
    mShape->Rotate(angle, axis);
}

void Object::Spin(double angle, const Vector3D& axis) {
    mShape->Spin(angle, axis);
}

void Object::PrintInfoBase() const {
    std::cout << "Object:" << mName << std::endl
              << "\tVisible:\t" << (mVisible ? "[x]" : "[ ]") << std::endl;
    mShape->PrintInfo();
    mMaterial.PrintInfo();

    if (IsDynamic()) {
        std::cout << "\tDynamic properties:" << std::endl
                  << "\t\tVelocity:\t" << mVelocity << std::endl
                  << "\t\tAcceleration:\t" << mAcceleration << std::endl
                  << "\t\tAngular Velocity:\t" << mAngularVelocity << std::endl
                  << "\t\tSpin:\t" << mSpin << std::endl;
    }
    std::cout << std::endl;
}

}  // namespace Raytracer
