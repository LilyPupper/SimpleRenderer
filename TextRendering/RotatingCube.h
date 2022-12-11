#pragma once

#include "Object.h"

class RotatingCube : public Object
{
public:
	RotatingCube();
	~RotatingCube();

	virtual void Update(const float& _deltaTime);
	virtual void Render();
};