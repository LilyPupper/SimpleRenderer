#include "Renderer/SDLRenderer.h"

SDLRenderer::SDLRenderer(const unsigned int& _width, const unsigned int& _height)
	: RendererBase(_width, _height)
{
#ifdef NDEBUG
	HideConsole();
#else
	ShowConsole();
#endif

	SDL_Init(SDL_INIT_VIDEO);
	SDL_CreateWindowAndRenderer(Width, Height, 0, &Window, &Renderer);

	WindowHandle = (HWND)SDL_GetProperty(SDL_GetWindowProperties(Window), "SDL.window.win32.hwnd", nullptr);
	const int a = 10;
}

SDLRenderer::~SDLRenderer()
{
	SDL_DestroyRenderer(Renderer);
	SDL_DestroyWindow(Window);
	SDL_Quit();
}

void SDLRenderer::Render(const float& _deltaTime)
{
	SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 0);
	SDL_RenderClear(Renderer);

	// Draw a red line
	SDL_SetRenderDrawColor(Renderer, 255, 0, 0, 255);
    for (unsigned int i = 0; i < Width; ++i)
        SDL_RenderPoint(Renderer, i, i);

	// We're not taking input just allowing the window to be moved around so it's probably ok to poll on the render thread
	SDL_PollEvent(&Event);

	SDL_RenderPresent(Renderer);
}