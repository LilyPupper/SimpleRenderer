#pragma once

#include "Object.h"

class CharTexture;

class RotatingCube : public Object
{
public:
	RotatingCube();
	~RotatingCube();

	virtual void Update(const float& _deltaTime);
	virtual void Render(CharTexture* _texture);
};