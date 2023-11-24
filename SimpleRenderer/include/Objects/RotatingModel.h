#pragma once

#include "Object.h"

class RotatingModel : public Object
{
public:
	RotatingModel();
	~RotatingModel();

	static ObjectType GetObjectType() { return ObjectType::RotatingMonkey; }

	virtual void Update(const float& _deltaTime);
	virtual void Render();

private:
	float Time;
};