#pragma once

#include "Renderer/Renderer.h"
#include "CharTexture.h"

#include <glm.hpp>
#define NOMINMAX // Get rid of standard min and max macros in Windows.h
#include <Windows.h>
#include <vector>
#include <string>

//#define RENDERMODE_RAYCAST
#define RENDERMODE_RASTERIZE
#define MULTITHREAD 1

class Tri;

class ConsoleRenderer : public Renderer
{
public:
	ConsoleRenderer(const unsigned int& _width, const unsigned int& _height);
	virtual ~ConsoleRenderer() override;

	virtual bool Initialise() override;
	virtual const char* RegisterMesh(const char* _meshPath) override;
	virtual void DrawMesh(const char* _modelReference, TransformComponent* const _transform) override;
	virtual void Flush(const float& _deltaTime) override;

	// RAYCAST MODE
	void RenderRaycast();
	wchar_t RaycastPixel(unsigned int _column, unsigned int _row);

	// RASTERIZE MODE
	void RenderRasterize();
	void RasterizeTri(const Tri& _tri, TransformComponent* const _transform);
	std::vector<glm::vec3> PlotLine(const glm::vec3& _p0, const glm::vec3& _p1);
	void OrderPointsByYThenX(glm::vec4& _high, glm::vec4& _low);

	void WriteToScreen(int _row, int _col, wchar_t _char);
	void WriteToScreen(int _row, int _col, const std::wstring& _s);

protected:

	std::vector<std::pair<const char*, TransformComponent* const>> m_ObjectsToRender;
	
	CharTexture m_RenderTex;
	wchar_t* m_ImageData;
	float* m_DepthData;
	std::vector<unsigned int> m_ImageHorizontalIter, m_ImageVerticalIter;

	// Render parameters

	const int m_Width, m_Height;

	wchar_t* m_ScreenBuffer;
	HANDLE m_Console;
};