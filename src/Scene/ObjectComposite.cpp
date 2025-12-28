#include "Scene/ObjectComposite.hpp"

namespace Raytracer {

ObjectComposite::ObjectComposite(const std::string& name, double referenceLength, const Vector3D& position, const Vector3D& orientation, const Vector3D& referenceDirection) :
    Object(Type::COMPOSITE, name),
    mReferenceLength(referenceLength),
    mPosition(position),
    mOrthonormalBasis(orientation, referenceDirection) {
}

std::size_t ObjectComposite::NumberOfComponents() const {
    return mComponents.size();
}

void ObjectComposite::AddComponent(const std::shared_ptr<ObjectPrimitive>& component) {
    mComponents.push_back(component);
    if (component->EmitsLight()) {
        mLightSources.push_back(component);
    }
}

std::optional<Object::Intersection> ObjectComposite::Intersect(const Ray& ray) const {
    std::optional<Object::Intersection> closestIntersection;
    for (const auto& component : mComponents) {
        auto intersection = component->Intersect(ray);
        if (intersection.has_value() && (!closestIntersection.has_value() || intersection->t < closestIntersection->t)) {
            closestIntersection = intersection;
        }
    }
    return closestIntersection;
}

void ObjectComposite::SetVisible(bool visible) {
    for (auto& component : mComponents) {
        component->SetVisible(visible);
    }
}

bool ObjectComposite::IsVisible() const {
    for (const auto& component : mComponents) {
        if (component->IsVisible()) {
            return true;
        }
    }
    return false;
}

std::vector<std::shared_ptr<ObjectPrimitive>> ObjectComposite::GetLightSources() const {
    return mLightSources;
}

bool ObjectComposite::IsDynamic() const {
    bool entireObjectIsDynamic = (mVelocity.Norm() > 0.0 || mAcceleration.Norm() > 0.0 ||
                                  mAngularVelocity.Norm() > 0.0 || mSpin.Norm() > 0.0);

    bool anyComponentIsDynamic = false;
    for (const auto& component : mComponents) {
        if (component->IsDynamic()) {
            anyComponentIsDynamic = true;
            break;
        }
    }
    return entireObjectIsDynamic || anyComponentIsDynamic;
}

void ObjectComposite::Evolve(double timeDelta) {
    if (IsDynamic()) {
        const double sEpsilon = 1e-8;
        if (mVelocity.Norm() > sEpsilon) {
            Translate(mVelocity * timeDelta);
        }
        if (mAngularVelocity.Norm() > sEpsilon) {
            double angle = mAngularVelocity.Norm() * timeDelta;
            Vector3D axis = mAngularVelocity.Normalized();
            Rotate(angle, Geometry::Line(mPosition, axis));
        }
        if (mSpin.Norm() > sEpsilon) {
            double angle = mSpin.Norm() * timeDelta;
            Vector3D axis = mSpin.Normalized();
            Spin(angle, axis);
        }
    }

    for (auto& component : mComponents) {
        component->Evolve(timeDelta);
    }
}

void ObjectComposite::PrintInfo() const {
    PrintInfoBase();
    std::cout << "\tNumber of Components:\t" << mComponents.size() << std::endl;
    std::cout << "\tReference Length:\t" << mReferenceLength << std::endl;
    std::cout << "\tPosition:\t" << mPosition << std::endl;
    std::cout << "\tOrientation (Z axis):\t" << mOrthonormalBasis.GetBasisVector(Geometry::OrthonormalBasis::BasisVector::eZ) << std::endl;
    std::cout << "\tComponents:" << std::endl;
    for (const auto& component : mComponents) {
        std::cout << "\t\t- " << component->GetName() << std::endl;
    }
}

void ObjectComposite::Translate(const Vector3D& translation) {
    mPosition += translation;
    for (auto& component : mComponents) {
        component->Translate(translation);
    }
}

void ObjectComposite::Rotate(double angle, const Geometry::Line& axis) {
    // Update orientation
    mOrthonormalBasis.Rotate(angle, axis.GetDirection());

    for (auto& component : mComponents) {
        component->Rotate(angle, axis);
        component->Spin(angle, axis.GetDirection());
    }
}

void ObjectComposite::Spin(double angle, const Vector3D& axis) {
    for (auto& component : mComponents) {
        component->Spin(angle, axis);
    }
}

}  // namespace Raytracer
