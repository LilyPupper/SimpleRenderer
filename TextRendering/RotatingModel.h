#pragma once

#include "Object.h"

class CharTexture;

class RotatingModel : public Object
{
public:
	RotatingModel(const char* _filepath);
	~RotatingModel();

	virtual void Update(const float& _deltaTime);
	virtual void Render(CharTexture* _texture);

private:
	float m_Time;
};