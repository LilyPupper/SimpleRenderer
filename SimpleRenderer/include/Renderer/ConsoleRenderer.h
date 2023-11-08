#pragma once

#include "Renderer/Renderer.h"
#include "CharTexture.h"

#include <glm.hpp>
#define NOMINMAX // Get rid of standard min and max macros in Windows.h
#include <atomic>
#include <future>
#include <Windows.h>
#include <vector>
#include <string>

#include "Threading/ThreadPool.h"

#define SCREEN_BUFFER_COUNT 2

class Tri;

class ConsoleRenderer : public Renderer
{
public:
	ConsoleRenderer(const unsigned int& _width, const unsigned int& _height);
	virtual ~ConsoleRenderer() override;

	virtual bool Initialise() override;
	virtual const char* RegisterMesh(const char* _meshPath) override;
	virtual void DrawMesh(const char* _modelReference, TransformComponent* const _transform) override;
	virtual void Render(const float& _deltaTime) override;
	virtual void PushToScreen(const float& _deltaTime) override;

	// RAYCAST MODE
	void RenderRaycast();
	wchar_t RaycastPixel(unsigned int _column, unsigned int _row);

	// RASTERIZE MODE
	void RenderRasterize();
	void RasterizeTri(const Tri& _tri, TransformComponent* const _transform);
	std::vector<glm::vec3> PlotLine(const glm::vec3& _p0, const glm::vec3& _p1);
	void OrderPointsByYThenX(glm::vec4& _high, glm::vec4& _low);

	inline void WriteToScreen(int _row, int _col, wchar_t _char)
	{
		ConsoleBuffer[_row * Width + _col] = _char;
	}
	inline void WriteToScreen(int _row, int _col, const std::wstring& _s)
	{
		swprintf(&ConsoleBuffer[_row * Width + _col], _s.size() + 1, L"%s", _s.c_str());
	}

	void NextScreenBuffer()
	{
		//std::lock_guard<std::mutex> lg(DoubleBufferMtx);
		PreviousScreenBufferIndex = CurrentScreenBufferIndex;
		++CurrentScreenBufferIndex;
		if (CurrentScreenBufferIndex >= SCREEN_BUFFER_COUNT)
		{
			CurrentScreenBufferIndex = 0;
		}
	}
	wchar_t* GetCurrentScreenBuffer() const
	{
		//std::lock_guard<std::mutex> lg(DoubleBufferMtx);
		return ScreenBuffers[CurrentScreenBufferIndex];
	}
	wchar_t* GetPreviousScreenBuffer() const
	{
		//std::lock_guard<std::mutex> lg(DoubleBufferMtx);
		return ScreenBuffers[PreviousScreenBufferIndex];
	}

protected:

	std::vector<std::pair<const char*, TransformComponent* const>> m_ObjectsToRender;

	int CurrentScreenBufferIndex;
	int PreviousScreenBufferIndex;
	std::mutex DoubleBufferMtx;

	wchar_t* ScreenBuffers[2];

	float* DepthData;
	std::vector<unsigned int> ImageHorizontalIter, m_ImageVerticalIter;

	// Render parameters

	const int Width, Height;

	wchar_t* ConsoleBuffer;
	HANDLE Console;

	std::vector<std::future<void>> Futures;
	rdx::thread_pool ThreadPool;

	std::atomic_bool bIsScreenBufferReady;
	std::atomic_bool bRenderThreadContinue; 

};