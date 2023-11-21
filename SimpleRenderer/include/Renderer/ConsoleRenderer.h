#pragma once

#include "Renderer/RendererBase.h"
#include "CharTexture.h"

#include "glm/glm.hpp"
#define NOMINMAX // Get rid of standard min and max macros in Windows.h
#include <atomic>
#include <future>
#include <Windows.h>
#include <vector>
#include <string>

#include "Threading/ThreadPool.h"

#define SCREEN_BUFFER_COUNT 2

class Tri;

class ConsoleRenderer : public RendererBase
{
public:
	ConsoleRenderer(const unsigned int _width, const unsigned int _height);
	virtual ~ConsoleRenderer() override;

	bool InitialiseConsoleWindow();

	virtual void Render(const float& _deltaTime) override;

	void PushToScreen(const float& _deltaTime);

	void RasterizeTri(const Tri& _tri, TransformComponent* const _transform);
	void DrawTriangleToScreen(const Tri& _worldTri, const Tri& _screenSpaceTri, TransformComponent* const _transform);

	void DrawTriangleTopHalf(const std::vector<glm::vec3>& _longSide, const std::vector<glm::vec3>& _shortSide);
	void DrawTriangleBottomHalf(const std::vector<glm::vec3>& _longSide, const std::vector<glm::vec3>& _shortSide, const int _longSideMiddleIndex);

	std::vector<glm::vec3> PlotLine(const glm::vec3& _p0, const glm::vec3& _p1);
	void OrderPointsByYThenX(glm::vec4& _high, glm::vec4& _low);

	inline void WriteToScreen(int _row, int _col, wchar_t _char)
	{
		GetCurrentScreenBuffer()[_row * Width + _col] = _char;
	}
	inline void WriteToScreen(int _row, int _col, const std::wstring& _s)
	{
		swprintf(&GetCurrentScreenBuffer()[_row * Width + _col], _s.size() + 1, L"%s", _s.c_str());
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

	int CurrentScreenBufferIndex;
	int PreviousScreenBufferIndex;
	std::mutex DoubleBufferMtx;

	wchar_t* ScreenBuffers[SCREEN_BUFFER_COUNT];

	float* DepthData;
	std::vector<unsigned int> ImageHorizontalIter, m_ImageVerticalIter;

	wchar_t* ConsoleBuffer;
	HANDLE Console;

	std::vector<std::future<void>> Futures;
	rdx::thread_pool ThreadPool;

	std::atomic_bool bIsScreenBufferReady;
	std::atomic_bool bRenderThreadContinue;

private:
	glm::mat4 GetMVP(TransformComponent* _transform) const;
	glm::mat4 GetMV(TransformComponent* _transform) const;
	Tri TriangleToScreenSpace(const Tri& _tri, TransformComponent* _transform) const;
	Tri TriangleToWorldSpace(const Tri& _tri, TransformComponent* _transform) const;
};