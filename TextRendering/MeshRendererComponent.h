#pragma once

#include "Component.h"

#include <vector>
#include <future>

class Mesh;

enum RENDER_MODE
{
	SINGLE,
	MULTI
};

class MeshRendererComponent : public Component
{
public:
	MeshRendererComponent(Object* _owner, const RENDER_MODE& _mode = MULTI);
	~MeshRendererComponent();

	virtual void Update(const float& _deltaTime);
	virtual void Render(CharTexture* _texture);

	void Render_Single(CharTexture* _texture, const Mesh* _mesh);
	void Render_Async(CharTexture* _texture, const Mesh* _mesh);
	void RenderRow(CharTexture* _texture, const Mesh* _mesh, unsigned int _row);

	void RenderPixel(CharTexture* _texture, const Mesh* _mesh, unsigned int _column, unsigned int _row);

	inline void SetRenderMode(const RENDER_MODE& _mode) { m_Mode = _mode; }

private:
	// Render mode
	RENDER_MODE m_Mode = RENDER_MODE::SINGLE;

	// Async
	std::vector<std::future<void>> m_Futures;
};