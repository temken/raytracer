#include "gtest/gtest.h"

#include <algorithm>
#include <cmath>

#include "Utilities/Color.hpp"

using namespace Raytracer;

// --- Constructors ---
TEST(TestColor, DefaultConstructor) {
    Color c;
    EXPECT_DOUBLE_EQ(c.R(), 0.0);
    EXPECT_DOUBLE_EQ(c.G(), 0.0);
    EXPECT_DOUBLE_EQ(c.B(), 0.0);
}

TEST(TestColor, ValueConstructor) {
    Color c(0.25, 0.5, 0.75);
    EXPECT_DOUBLE_EQ(c.R(), 0.25);
    EXPECT_DOUBLE_EQ(c.G(), 0.5);
    EXPECT_DOUBLE_EQ(c.B(), 0.75);
}

// --- One test per public function ---

TEST(TestColor, R_Getter) {
    Color c(0.9, 0.0, 0.0);
    EXPECT_DOUBLE_EQ(c.R(), 0.9);
}

TEST(TestColor, G_Getter) {
    Color c(0.0, 0.8, 0.0);
    EXPECT_DOUBLE_EQ(c.G(), 0.8);
}

TEST(TestColor, B_Getter) {
    Color c(0.0, 0.0, 0.7);
    EXPECT_DOUBLE_EQ(c.B(), 0.7);
}

TEST(TestColor, GetRGB) {
    Color c(0.11, 0.22, 0.33);
    auto rgb = c.GetRGB();
    ASSERT_EQ(rgb.size(), 3u);
    EXPECT_DOUBLE_EQ(rgb[0], 0.11);
    EXPECT_DOUBLE_EQ(rgb[1], 0.22);
    EXPECT_DOUBLE_EQ(rgb[2], 0.33);
}

TEST(TestColor, GetHexColor) {
    // Expect uppercase RRGGBB (no alpha) based on 0..255 mapping
    Color c(1.0, 0.5, 0.0);
    const std::string hex = c.GetHexColor();
    // With round-to-nearest: 1.0->FF, 0.5->80, 0.0->00
    // Accept optional leading '#'
    EXPECT_EQ(hex, "#FF8000");
}

TEST(TestColor, StreamOperator) {
    Color c(1.0, 0.5, 0.0);
    std::ostringstream oss;
    oss << c;
    // Matches your operator<< exactly:
    EXPECT_EQ(oss.str(), "RGB(1, 0.5, 0)");
}
