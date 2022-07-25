#include <gtest/gtest.h>

#include "collision.h"
#include "helpers.h"

TEST(CollisionTests, RectangleCollision) {
	Position positions[2]{};
	Size sizes[2]{};

	positions[0] = { 0.0f, 0.0f};
	positions[1] = { 5.0f, 5.0f};

	sizes[0] = { 10.0f, 10.0f };
	sizes[1] = { 10.0f, 10.0f };

	EXPECT_TRUE(rectangle_collide(positions, sizes, &positions[1], &sizes[1]));

	positions[1] = { 11.0f, 11.0f };
	EXPECT_FALSE(rectangle_collide(positions, sizes, &positions[1], &sizes[1]));
}

TEST(CollisionTests, PointIsLeftOfLine) {
	Position point = { 0.0f, 0.0f };

	Position a = { 1.0f, -5.0f };
	Position b = { 1.0f, 5.0f };

	EXPECT_TRUE(point_left(&point, &a, &b));
	EXPECT_FALSE(point_left(&point, &b, &a));
}

TEST(CollisionTests, SideOfPoint) {

	Position rect = { -5.0f, -5.0f };
	Size size = { 10.0f, 10.0f };

	Position point = { -5.0f, 0.0f };
	EXPECT_EQ(side_of_point(&point, &rect, &size), RECTANGLE_LEFT);

	point = { 5.0f, 0.0f };
	EXPECT_EQ(side_of_point(&point, &rect, &size), RECTANGLE_RIGHT);

	point = { 0.0f, 4.0f };
	EXPECT_EQ(side_of_point(&point, &rect, &size), RECTANGLE_TOP);

	point = { 0.0f, -5.0f };
	EXPECT_EQ(side_of_point(&point, &rect, &size), RECTANGLE_BOTTOM);
}

TEST(MathTests, PointToSegment) {
	Position p = { 0.f, 0.f };
	Position s1 = { -5.0f, 5.0f };
	Position s2 = { 5.0f, 5.0f };

	EXPECT_EQ(distance_point_to_segment(&p, &s1, &s2), 5.0f);
}


