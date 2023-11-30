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
	void DrawTriangleToScreen(const Tri& _tri, const Tri& _screenSpaceTri, TransformComponent* const _transform);

	wchar_t LightIntensityToAsciiCharacter(const float _lightIntensity) const;

	inline void WriteToScreen(int _row, int _col, wchar_t _char)
	{
		GetCurrentScreenBuffer()[_row * Width + _col] = _char;
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
	std::vector<unsigned int> ImageHorizontalIter, ImageVerticalIter;

	wchar_t* ConsoleBuffer;
	HANDLE Console;

	std::vector<std::future<void>> Futures;
	rdx::thread_pool ThreadPool;

	std::atomic_bool bIsScreenBufferReady;
	std::atomic_bool bRenderThreadContinue;

	//const int CharacterMapLength = 10;
	//const wchar_t* CharacterMap = L" .:-=+*8#%@";

	const int CharacterMapLength = 70;
	const wchar_t* CharacterMap = L" .'`^\",:;Il!i><~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";
};