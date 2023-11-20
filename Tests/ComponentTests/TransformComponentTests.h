#pragma once

#include "gtest/gtest.h"
#include "Components/TransformComponent.h"
//#include "Components/TransformComponent.cpp"
#include "Objects/Object.h"
//#include "Objects/Object.cpp"
#include "Components/Component.h"
//#include "Components/Component.cpp"

namespace TransformComponentTests
{
	TEST(TransformComponentTest, SetScale)
	{
		TransformComponent transform(nullptr);

		const float scale1 = 9.123456f;
		transform.SetScale(scale1);
		EXPECT_EQ(transform.GetScale().x, scale1);
		EXPECT_EQ(transform.GetScale().y, scale1);
		EXPECT_EQ(transform.GetScale().z, scale1);

		const float scale2x = 8.123456f;
		const float scale2y = 7.123456f;
		const float scale2z = 6.123456f;
		transform.SetScale(scale2x, scale2y, scale2z);
		EXPECT_EQ(transform.GetScale().x, scale2x);
		EXPECT_EQ(transform.GetScale().y, scale2y);
		EXPECT_EQ(transform.GetScale().z, scale2z);
	}

	TEST(TransformComponentTest, Scale)
	{
		TransformComponent transform(nullptr);

		const float a = 1.5f;
		const float b = 150.f;
		transform.SetScale(a);
		transform.Scale(b);
		EXPECT_EQ(transform.GetScale().x, a * b);
		EXPECT_EQ(transform.GetScale().y, a * b);
		EXPECT_EQ(transform.GetScale().z, a * b);

		const float c = 2123.123456f;
		const float d = c * 1.321f;
		transform.SetScale(a);
		transform.Scale(b, c, d);
		EXPECT_EQ(transform.GetScale().x, a * b);
		EXPECT_EQ(transform.GetScale().y, a * c);
		EXPECT_EQ(transform.GetScale().z, a * d);
	}

	TEST(TransformComponentTest, SetPosition)
	{
		TransformComponent transform(nullptr);

		const float x = 1234.f;
		const float y = 0.1234f;
		const float z = -9001.f;
		transform.SetPosition(x, y, z);
		EXPECT_EQ(transform.GetPosition().x, x);
		EXPECT_EQ(transform.GetPosition().y, y);
		EXPECT_EQ(transform.GetPosition().z, z);
	}
}