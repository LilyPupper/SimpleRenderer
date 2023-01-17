#pragma once

#include "Object.h"

class Renderer;
class CharTexture;

class RotatingModel : public Object
{
public:
	RotatingModel(Renderer* _renderer, const char* _filepath);
	~RotatingModel();

	virtual void Update(const float& _deltaTime);
	virtual void Render();

private:
	float m_Time;
	Renderer* m_Renderer;
};