#include "gtest/gtest.h"

#include <algorithm>
#include <cmath>

#include "Color.hpp"

using namespace Raytracer;

// Helper to express the expected 0..255 conversion with clamp+round.
static int to255(double x) {
    x = std::clamp(x, 0.0, 1.0);
    return static_cast<int>(std::lround(x * 255.0));
}

// --- Constructors ---
TEST(TestColor, DefaultConstructor) {
    Color c;
    EXPECT_DOUBLE_EQ(c.R(), 0.0);
    EXPECT_DOUBLE_EQ(c.G(), 0.0);
    EXPECT_DOUBLE_EQ(c.B(), 0.0);
    EXPECT_DOUBLE_EQ(c.A(), 0.0);
}

TEST(TestColor, ValueConstructor) {
    Color c(0.25, 0.5, 0.75, 1.0);
    EXPECT_DOUBLE_EQ(c.R(), 0.25);
    EXPECT_DOUBLE_EQ(c.G(), 0.5);
    EXPECT_DOUBLE_EQ(c.B(), 0.75);
    EXPECT_DOUBLE_EQ(c.A(), 1.0);
}

// --- One test per public function ---

TEST(TestColor, R_Getter) {
    Color c(0.9, 0.0, 0.0, 0.0);
    EXPECT_DOUBLE_EQ(c.R(), 0.9);
}

TEST(TestColor, G_Getter) {
    Color c(0.0, 0.8, 0.0, 0.0);
    EXPECT_DOUBLE_EQ(c.G(), 0.8);
}

TEST(TestColor, B_Getter) {
    Color c(0.0, 0.0, 0.7, 0.0);
    EXPECT_DOUBLE_EQ(c.B(), 0.7);
}

TEST(TestColor, A_Getter) {
    Color c(0.0, 0.0, 0.0, 0.6);
    EXPECT_DOUBLE_EQ(c.A(), 0.6);
}

TEST(TestColor, GetRGBA) {
    Color c(0.2, 0.4, 0.6, 0.8);
    auto rgba = c.GetRGBA();
    ASSERT_EQ(rgba.size(), 4u);
    EXPECT_DOUBLE_EQ(rgba[0], 0.2);
    EXPECT_DOUBLE_EQ(rgba[1], 0.4);
    EXPECT_DOUBLE_EQ(rgba[2], 0.6);
    EXPECT_DOUBLE_EQ(rgba[3], 0.8);
}

TEST(TestColor, GetRGB) {
    Color c(0.11, 0.22, 0.33, 0.44);
    auto rgb = c.GetRGB();
    ASSERT_EQ(rgb.size(), 3u);
    EXPECT_DOUBLE_EQ(rgb[0], 0.11);
    EXPECT_DOUBLE_EQ(rgb[1], 0.22);
    EXPECT_DOUBLE_EQ(rgb[2], 0.33);
}

TEST(TestColor, GetRGBA255) {
    // includes clamping + rounding check
    Color c1(1.0, 0.5, 0.0, 1.0);
    auto p1 = c1.GetRGBA255();
    ASSERT_EQ(p1.size(), 4u);
    EXPECT_EQ(p1[0], to255(1.0));  // 255
    EXPECT_EQ(p1[1], to255(0.5));  // 128 (with round-to-nearest)
    EXPECT_EQ(p1[2], to255(0.0));  // 0
    EXPECT_EQ(p1[3], to255(1.0));  // 255

    Color c2(-0.1, 1.7, 0.49, -3.0);  // tests clamping at both ends + rounding
    auto p2 = c2.GetRGBA255();
    EXPECT_EQ(p2[0], to255(0.0));
    EXPECT_EQ(p2[1], to255(1.0));
    EXPECT_EQ(p2[2], to255(0.49));
    EXPECT_EQ(p2[3], to255(0.0));
}

TEST(TestColor, GetHexColor) {
    // Expect uppercase RRGGBB (no alpha) based on 0..255 mapping
    Color c(1.0, 0.5, 0.0, 0.25);
    const std::string hex = c.GetHexColor();
    // With round-to-nearest: 1.0->FF, 0.5->80, 0.0->00
    // Accept optional leading '#'
    EXPECT_EQ(hex, "#FF8000");
}

TEST(TestColor, StreamOperator) {
    Color c(1.0, 0.5, 0.0, 1.0);
    std::ostringstream oss;
    oss << c;
    // Matches your operator<< exactly:
    EXPECT_EQ(oss.str(), "RGBA(1, 0.5, 0, 1)");
}
