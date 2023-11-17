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
	TEST(TransformComponentTest, SetPosition)
	{
		TransformComponent transform(nullptr);

		for (unsigned int x = 0; x < 1000000; x+=10000)
		{
			for (unsigned int y = 0; y < 1000000; y+=10000)
			{
				for (unsigned int z = 0; z < 1000000; z+=10000)
				{
					glm::vec3 position(x, y, z);
					transform.SetPosition(position);
					EXPECT_EQ(transform.GetPosition().x, x);
					EXPECT_EQ(transform.GetPosition().y, y);
					EXPECT_EQ(transform.GetPosition().z, z);
				}
			}
		}
	}
}