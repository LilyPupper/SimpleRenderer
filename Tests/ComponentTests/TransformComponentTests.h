#pragma once

#include "gtest/gtest.h"
//#include "gmock/gmock-matchers.h"

#include "Components/TransformComponent.h"
#include "Objects/Object.h"
#include "Components/Component.h"

namespace TransformComponentTests
{
	TEST(TransformComponentTest, SetGetScale)
	{
		TransformComponent transform(nullptr);

		constexpr float scale1 = 1.123456f;
		transform.SetScale(scale1);
		EXPECT_EQ(transform.GetScale().x, scale1);
		EXPECT_EQ(transform.GetScale().y, scale1);
		EXPECT_EQ(transform.GetScale().z, scale1);

		constexpr float scale2x = 2.123456f;
		constexpr float scale2y = 3.123456f;
		constexpr float scale2z = 4.123456f;
		transform.SetScale(scale2x, scale2y, scale2z);
		EXPECT_EQ(transform.GetScale().x, scale2x);
		EXPECT_EQ(transform.GetScale().y, scale2y);
		EXPECT_EQ(transform.GetScale().z, scale2z);
	}

	TEST(TransformComponentTest, Scale)
	{
		TransformComponent transform(nullptr);

		constexpr float a = 1.5f;
		constexpr float b = 150.f;
		transform.SetScale(a);
		transform.ScaleBy(b);
		EXPECT_EQ(transform.GetScale().x, a * b);
		EXPECT_EQ(transform.GetScale().y, a * b);
		EXPECT_EQ(transform.GetScale().z, a * b);

		constexpr float c = 2123.123456f;
		constexpr float d = c * 1.654321f;
		transform.SetScale(a);
		transform.ScaleBy(b, c, d);
		EXPECT_EQ(transform.GetScale().x, a * b);
		EXPECT_EQ(transform.GetScale().y, a * c);
		EXPECT_EQ(transform.GetScale().z, a * d);
	}

	TEST(TransformComponentTest, SetGetRotation)
	{
		TransformComponent transform(nullptr);

		constexpr float rotX = 1.123456f;
		constexpr float rotY = 5.123456f;
		constexpr float rotZ = 25.123456f;
		const glm::vec3 rot(rotX, rotY, rotZ);
		transform.SetRotation(rot);
		
		EXPECT_THAT(transform.GetRotation().x, testing::FloatEq(rotX));
		EXPECT_THAT(transform.GetRotation().y, testing::FloatEq(rotY));
		EXPECT_THAT(transform.GetRotation().z, testing::FloatEq(rotZ));
	}

	TEST(TransformComponentTest, Rotate)
	{
		TransformComponent transform(nullptr);

		transform.SetRotation(glm::vec3(0.f, 0.f, 0.f));
		EXPECT_THAT(transform.GetRotation().x, testing::FloatEq(0.f));
		EXPECT_THAT(transform.GetRotation().y, testing::FloatEq(0.f));
		EXPECT_THAT(transform.GetRotation().z, testing::FloatEq(0.f));

		constexpr float rot = 90.123456f;
		constexpr float halfRot = rot * 0.5f;
		constexpr float maxError = 0.000001f;

		transform.Rotate(rot, glm::vec3(1.f, 0.f, 0.f));
		EXPECT_THAT(transform.GetRotation().x, testing::FloatEq(rot));
		EXPECT_THAT(transform.GetRotation().y, testing::FloatEq(0.f));
		EXPECT_THAT(transform.GetRotation().z, testing::FloatEq(0.f));
		transform.SetRotation(glm::vec3(0.f, 0.f, 0.f));

		transform.Rotate(rot, glm::vec3(0.f, 1.f, 0.f));
		EXPECT_THAT(transform.GetRotation().x, testing::FloatEq(0.f));
		EXPECT_THAT(transform.GetRotation().y, testing::FloatEq(rot));
		EXPECT_THAT(transform.GetRotation().z, testing::FloatEq(0.f));
		transform.SetRotation(glm::vec3(0.f, 0.f, 0.f));
		
		transform.Rotate(rot, glm::vec3(0.f, 0.f, 1.f));
		EXPECT_THAT(transform.GetRotation().x, testing::FloatEq(0.f));
		EXPECT_THAT(transform.GetRotation().y, testing::FloatEq(0.f));
		EXPECT_THAT(transform.GetRotation().z, testing::FloatEq(rot));
		transform.SetRotation(glm::vec3(0.f, 0.f, 0.f));
		
		transform.Rotate(halfRot, glm::vec3(1.f, 1.f, 1.f));
		EXPECT_THAT(transform.GetRotation().x, testing::FloatEq(halfRot));
		EXPECT_THAT(transform.GetRotation().y, testing::FloatEq(halfRot));
		EXPECT_THAT(transform.GetRotation().z, testing::FloatEq(halfRot));
	}

	TEST(TransformComponentTest, RotateX)
	{
		TransformComponent transform(nullptr);

		transform.SetRotation(glm::vec3(0.f, 0.f, 0.f));
		constexpr float rot = 90.123456f;
		transform.RotateX(rot);
		EXPECT_THAT(transform.GetRotation().x, testing::FloatEq(rot));
	}

	TEST(TransformComponentTest, RotateY)
	{
		TransformComponent transform(nullptr);

		transform.SetRotation(glm::vec3(0.f, 0.f, 0.f));
		constexpr float rot = 90.123456f;
		transform.RotateY(rot);
		EXPECT_THAT(transform.GetRotation().y, testing::FloatEq(rot));
	}

	TEST(TransformComponentTest, RotateZ)
	{
		TransformComponent transform(nullptr);

		transform.SetRotation(glm::vec3(0.f, 0.f, 0.f));
		constexpr float rot = 90.123456f;
		transform.RotateZ(rot);
		EXPECT_THAT(transform.GetRotation().z, testing::FloatEq(rot));
	}

	TEST(TransformComponentTest, SetGetPosition)
	{
		TransformComponent transform(nullptr);

		const float x = 1234.f;
		const float y = 0.1234f;
		const float z = -9001.f;
		transform.SetPosition(x, y, z);
		EXPECT_EQ(transform.GetPosition().x, x);
		EXPECT_EQ(transform.GetPosition().y, y);
		EXPECT_EQ(transform.GetPosition().z, z);

		const float arbitrary = 1.123456f;
		transform.SetPosition(x * arbitrary, y * arbitrary, z * arbitrary);

		glm::vec3 newPos(x, y, z);
		transform.SetPosition(newPos);
		EXPECT_EQ(transform.GetPosition().x, x);
		EXPECT_EQ(transform.GetPosition().y, y);
		EXPECT_EQ(transform.GetPosition().z, z);
	}

	TEST(TransformComponentTest, Translate)
	{
		TransformComponent transform(nullptr);

		constexpr float x = 50.123456f;
		constexpr float y = 150.123456f;
		constexpr float z = 250.123456f;
		transform.Translate(x, y, z);
		EXPECT_EQ(transform.GetPosition().x, x);
		EXPECT_EQ(transform.GetPosition().y, y);
		EXPECT_EQ(transform.GetPosition().z, z);

		transform.Translate(glm::vec3(x, y, z));
		EXPECT_EQ(transform.GetPosition().x, x + x);
		EXPECT_EQ(transform.GetPosition().y, y + y);
		EXPECT_EQ(transform.GetPosition().z, z + z);
	}

	TEST(TransformComponentTest, GetForward)
	{
		TransformComponent transform(nullptr);

		constexpr float x = 50.123456f;
		constexpr float y = 150.123456f;
		constexpr float z = 250.123456f;
		constexpr float theta = 75.123456f;
		transform.Rotate(theta, glm::vec3(x, y, z));

		const glm::vec3 rot = transform.GetQuaternion() * glm::vec3(0.f, 0.f, 1.f);
		EXPECT_THAT(transform.GetForward().x, testing::FloatEq(rot.x));
		EXPECT_THAT(transform.GetForward().y, testing::FloatEq(rot.y));
		EXPECT_THAT(transform.GetForward().z, testing::FloatEq(rot.z));
	}

	TEST(TransformComponentTest, GetRight)
	{
		TransformComponent transform(nullptr);

		constexpr float x = 50.123456f;
		constexpr float y = 150.123456f;
		constexpr float z = 250.123456f;
		constexpr float theta = 75.123456f;
		transform.Rotate(theta, glm::vec3(x, y, z));

		const glm::vec3 rot = transform.GetQuaternion() * glm::vec3(1.f, 0.f, 0.f);
		EXPECT_THAT(transform.GetRight().x, testing::FloatEq(rot.x));
		EXPECT_THAT(transform.GetRight().y, testing::FloatEq(rot.y));
		EXPECT_THAT(transform.GetRight().z, testing::FloatEq(rot.z));
	}

	TEST(TransformComponentTest, GetUp)
	{
		TransformComponent transform(nullptr);

		constexpr float x = 50.123456f;
		constexpr float y = 150.123456f;
		constexpr float z = 250.123456f;
		constexpr float theta = 75.123456f;
		transform.Rotate(theta, glm::vec3(x, y, z));

		const glm::vec3 rot = transform.GetQuaternion() * glm::vec3(0.f, 1.f, 0.f);
		EXPECT_THAT(transform.GetUp().x, testing::FloatEq(rot.x));
		EXPECT_THAT(transform.GetUp().y, testing::FloatEq(rot.y));
		EXPECT_THAT(transform.GetUp().z, testing::FloatEq(rot.z));
	}

	TEST(TransformComponentTest, GetTranslationMatrix)
	{
		TransformComponent transform(nullptr);

		constexpr float x = 50.123456f;
		constexpr float y = 150.123456f;
		constexpr float z = 250.123456f;
		transform.Translate(x, y, z);

		glm::mat4 mat = transform.GetTranslationMatrix();

		glm::mat4 translation = {
			1.f, 0.f, 0.f, 0.f,
			0.f, 1.f, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f,
			x, y, z, 1.f
		};

		for (unsigned int i = 0; i < 4; ++i)
		{
			for (unsigned int j = 0; j < 4; ++j)
			{
				EXPECT_EQ(mat[i][j], translation[i][j]);
			}
		}
	}

	// https://en.wikipedia.org/wiki/Rotation_matrix
	// Lots of help from the "Rotation matrix from axis and angle" section
	TEST(TransformComponentTest, GetRotationMatrix)
	{
		TransformComponent transform(nullptr);

		constexpr float x = 50.123456f;
		constexpr float y = 150.123456f;
		constexpr float z = 250.123456f;
		const float radians = 75.123456f;
		const float theta = glm::radians(radians);
		transform.Rotate(radians, glm::vec3(x, y, z));

		const glm::mat4 rotation = transform.GetRotationMatrix();

		const glm::vec3 axis = glm::normalize(glm::vec3(x, y, z));

		// One of these is row-major, the other is column-major. Who knows which is which..
		const glm::mat4 mat = {
			cosf(theta) + (axis.x * axis.x) * (1.f - cosf(theta)), axis.x * axis.y * (1.f - cosf(theta)) - axis.z * sinf(theta), axis.x * axis.z * (1.f - cosf(theta)) + axis.y * sinf(theta), 0.f,
			axis.y * axis.x * (1.f - cosf(theta)) + axis.z * sinf(theta), cosf(theta) + (axis.y * axis.y) * (1.f - cosf(theta)), axis.y * axis.z * (1.f - cosf(theta)) - axis.x * sinf(theta), 0.f,
			axis.z * axis.x * (1.f - cosf(theta)) - axis.y * sinf(theta), axis.z * axis.y * (1.f - cosf(theta)) + axis.x * sinf(theta), cosf(theta) + (axis.z * axis.z) * (1.f - cosf(theta)), 0.f,
			0.f, 0.f, 0.f, 1.f
		};

		// Flipping from row-major to column-major
		glm::mat4 flippedMat;
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				flippedMat[i][j] = mat[j][i];
			}
		}

		constexpr float maxError = 0.000001f;
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				EXPECT_THAT(rotation[i][j], testing::FloatNear(flippedMat[i][j], maxError));
			}
		}
	}

	TEST(TransformComponentTest, GetScaleMatrix)
	{
		TransformComponent transform(nullptr);

		constexpr float x = 50.123456f;
		constexpr float y = 150.123456f;
		constexpr float z = 250.123456f;
		transform.SetScale(x, y, z);

		glm::mat4 mat = transform.GetScaleMatrix();

		glm::mat4 scale = {
			x, 0.f, 0.f, 0.f,
			0.f, y, 0.f, 0.f,
			0.f, 0.f, z, 0.f,
			0.f, 0.f, 0.f, 1.f
		};

		for (unsigned int i = 0; i < 4; ++i)
		{
			for (unsigned int j = 0; j < 4; ++j)
			{
				EXPECT_EQ(mat[i][j], scale[i][j]);
			}
		}
	}

	TEST(TransformComponentTest, GetTransformationMatrix)
	{
		TransformComponent transform(nullptr);


	}
}