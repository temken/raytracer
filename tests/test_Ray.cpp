#include "gtest/gtest.h"

#include "Rendering/Ray.hpp"
#include "Utilities/Vector.hpp"

using namespace Raytracer;

TEST(TestRay, TestPointAtParameter) {
    // ARRANGE
    Ray ray(Vector3D({0.0, 0.0, 0.0}), Vector3D({1.0, 0.0, 0.0}));
    double t = 2.0;

    // ACT
    Vector3D point = ray.PointAtParameter(t);

    // ASSERT
    EXPECT_EQ(point, Vector3D({2.0, 0.0, 0.0}));
}
