#pragma once
#include "Render/Shape.h"
#include "Core/include.h"

struct SDL_Renderer;
struct SDL_Texture;
struct SDL_Window;
struct SDL_FRect; 

  
enum SDL_WINDOW_FLAGS {
	WINDOW_SHOWN = 0x00000004, 
	WINDOW_RESIZABLE = 0x00000020
};
enum SDL_RENDERER_FLAGS : int32 {
	RENDERER_SOFTWARE = 0x00000001,      
	RENDERER_ACCELERATED = 0x00000002,   
	RENDERER_PRESENTVSYNC = 0x00000004,	 
	RENDERER_TARGETTEXTURE = 0x00000008  
};

enum SDL_WINDOW_POSITION
{
	WINDOWPOS_CENTERED = (0x2FFF0000u | (0)),
	WINDOWPOS_UNDEFINED = (0x1FFF0000u | (0))
};


class Text;

class Window
{
public:

	Window() = default;
	Window(const char* pName, int32 width, int32 height, uint32 windowFlags, uint32 rendererFlags, int32 x, int32 y);


	~Window();



	void Create(const char* pName,
		int32 width,
		int32 height,
		uint32 windowFlags = SDL_WINDOW_FLAGS::WINDOW_SHOWN,
		uint32 rendererFlags = SDL_RENDERER_FLAGS::RENDERER_ACCELERATED,
		int32 x = 0,
		int32 y = 0
	);

	void ClearWindowWithColor(uint8 r, uint8 g, uint8 b, uint8 a);

	void Present();
	void Clear();


	void End();

	SDL_Window* GetWindow();
	SDL_Renderer* GetRenderer();

	void DrawTextOnRenderer(Text* pText);
	void DrawOnRenderer(SDL_Texture* pTexture, SDL_FRect* pSrcrect, SDL_FRect* pDstrect);
	void Draw(gcle::Shape* pShape);

	void DrawDebug(gcle::Shape* pShape, Vector2f offset = { 0.f, 0.f }, float32 zoom = 1.0f);

	void SetWindowSize(int32 width, int32 height);
	Vector2u GetSize();

public:
	Vector2u GetMousePosition();
	Vector2f GetWindowSize();
	Vector2f GetMousePositionOnRenderTarget();

public:
	bool IsInsideWindow(GameObject* gm);

public:
	void SetFullscreen(bool fullscreen);
	void ToggleFullscreen();
	bool IsFullscreen() const;

private:

private:
	SDL_Window* mp_Window = nullptr;
	SDL_Renderer* mp_Renderer = nullptr;
	SDL_Texture* mp_RenderTarget = nullptr;
	SDL_FRect* mp_Dst;
	uint32 m_Width, m_Height = 0;

	bool m_IsFullscreen = false;

private:
	friend class Event;

};