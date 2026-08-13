#include "Window.h" 
#include <iostream>
#include <vector>

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_image/SDL_image.h>

#include "Shape.h"
#include "Texture.h" 
#include "Render/Text.h"
#include "Engine/Entity.h"
#include "Engine/GameManager.h"

Window::Window(const char* pName, int32 width, int32 height, uint32 windowFlags, uint32 rendererFlags, int32 x, int32 y)
{
	Create(pName, width, height, windowFlags, rendererFlags, x, y);
}
Window::~Window()
{
	End();
};

void Window::Create(const char* pName, int32 width, int32 height, uint32 windowFlags, uint32 rendererFlags, int32 x, int32 y)
{
	m_Width = width;
	m_Height = height;

	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO))
	{
		std::cout << "SDL_Init_Error :  " << SDL_GetError() << std::endl;
		return;
	}
	 
	SDL_WindowFlags sdlWindowFlags = 0;
	if (windowFlags & SDL_WINDOW_FLAGS::WINDOW_RESIZABLE)
		sdlWindowFlags |= SDL_WINDOW_RESIZABLE; 

	mp_Window = SDL_CreateWindow(pName, width, height, sdlWindowFlags);
	if (mp_Window == nullptr) {
		std::cerr << "Window failed to create : " << SDL_GetError() << std::endl;
		return;
	}

	SDL_SetWindowPosition(mp_Window, x, y);

	mp_Renderer = SDL_CreateRenderer(mp_Window, nullptr);
	if (mp_Renderer == nullptr) {
		std::cerr << "Renderer failed to create : " << SDL_GetError() << std::endl;
		return;
	}

	if (rendererFlags & SDL_RENDERER_FLAGS::RENDERER_PRESENTVSYNC)
		SDL_SetRenderVSync(mp_Renderer, 1);

	if (!Audio::Init())
	{
		std::cout << "[Initialisation] : Audio Error : " << SDL_GetError() << std::endl;
		return;
	}

	// Create a Render Target

	mp_RenderTarget = SDL_CreateTexture(mp_Renderer, SDL_PIXELFORMAT_BGRA8888, SDL_TEXTUREACCESS_TARGET, static_cast<int32>(RENDER_TARGET_WIDTH), static_cast<int32>(RENDER_TARGET_HEIGHT));
	SDL_SetTextureScaleMode(mp_RenderTarget, SDL_SCALEMODE_LINEAR);

	if (!mp_RenderTarget)
	{
		std::cout << "CreateTexture failed: " << SDL_GetError() << '\n';
	}

	if (!SDL_SetRenderTarget(mp_Renderer, mp_RenderTarget))
	{
		std::cout << "SetRenderTarget failed: " << SDL_GetError() << '\n';
	}

	SDL_SetRenderDrawColor(mp_Renderer, 255, 0, 0, 255);

	SDL_RenderClear(mp_Renderer);

	SDL_SetRenderTarget(mp_Renderer, NULL);

	mp_Dst = new SDL_FRect();
	mp_Dst->x = 0.f;
	mp_Dst->y = 0.f;
	mp_Dst->w = static_cast<float32>(width);
	mp_Dst->h = static_cast<float32>(height);
}

Vector2f Window::GetMousePositionOnRenderTarget()
{
	Vector2u mousePos = GetMousePosition();

	if (mp_Dst->w <= 0.f || mp_Dst->h <= 0.f)
		return Vector2f{ RENDER_TARGET_WIDTH * 0.5f, RENDER_TARGET_HEIGHT * 0.5f };

	float32 scaleX = RENDER_TARGET_WIDTH / mp_Dst->w;
	float32 scaleY = RENDER_TARGET_HEIGHT / mp_Dst->h;

	float32 x = (static_cast<float32>(mousePos.x) - mp_Dst->x) * scaleX;
	float32 y = (static_cast<float32>(mousePos.y) - mp_Dst->y) * scaleY;

	return Vector2f{ x, y };
}

void Window::ClearWindowWithColor(uint8 r, uint8 g, uint8 b, uint8 a)
{
	SDL_SetRenderDrawColor(mp_Renderer, r, g, b, a);
}

void Window::End()
{
	SDL_DestroyTexture(mp_RenderTarget);
	SDL_DestroyRenderer(mp_Renderer);
	SDL_DestroyWindow(mp_Window);

	Audio::Shutdown();
	SDL_Quit();

	delete mp_Dst;
	mp_Dst = nullptr;
}

void Window::Present()
{
	SDL_SetRenderTarget(mp_Renderer, nullptr);

	SDL_SetRenderDrawColor(mp_Renderer, 20, 20, 20, 255);
	SDL_RenderClear(mp_Renderer);

	int32 windowW, windowH;
	SDL_GetWindowSize(mp_Window, &windowW, &windowH);

	constexpr float32 ASPECT = RENDER_TARGET_WIDTH / RENDER_TARGET_HEIGHT;

	if (static_cast<float32>(windowW) / windowH > ASPECT)
	{
		mp_Dst->h = static_cast<float32>(windowH);
		mp_Dst->w = windowH * ASPECT;
		mp_Dst->x = (windowW - mp_Dst->w) * 0.5f;
		mp_Dst->y = 0.f;
	}
	else
	{
		mp_Dst->w = static_cast<float32>(windowW);
		mp_Dst->h = windowW / ASPECT;
		mp_Dst->x = 0.f;
		mp_Dst->y = (windowH - mp_Dst->h) * 0.5f;
	}

	SDL_RenderTexture(mp_Renderer, mp_RenderTarget, nullptr, mp_Dst);

	SDL_RenderPresent(mp_Renderer);
}

void Window::Clear()
{
	SDL_SetRenderTarget(mp_Renderer, mp_RenderTarget);

	SDL_SetRenderDrawColor(mp_Renderer, 30, 30, 30, 255);

	SDL_RenderClear(mp_Renderer);
}

void Window::DrawTextOnRenderer(Text* text)
{
	SDL_Texture* texture = text->GetTexture(this);
	if (texture == nullptr)
		return;

	DrawOnRenderer(texture, nullptr, text->GetSDLRect());
}

void Window::DrawOnRenderer(SDL_Texture* pTexture, SDL_FRect* pSrcrect, SDL_FRect* pDstrect) {
	SDL_RenderTexture(mp_Renderer, pTexture, pSrcrect, pDstrect);
}

void Window::Draw(gcle::Shape* pShape)
{
	const std::vector<SDL_Vertex*>& verticesPtr = pShape->GetVerticies();

	std::vector<SDL_Vertex> vertices;
	vertices.reserve(verticesPtr.size());
	for (SDL_Vertex* v : verticesPtr)
	{
		vertices.push_back(*v);
	}

	if (pShape->GetTexture() == nullptr)
		SDL_RenderGeometry(mp_Renderer, nullptr, vertices.data(), static_cast<int32>(vertices.size()), pShape->GetIndicies().data(), static_cast<int32>(pShape->GetIndicies().size()));
	else {
		SDL_Texture* texture = pShape->GetTexture()->CreateTexture(this);
		SDL_RenderGeometry(mp_Renderer, texture, vertices.data(), static_cast<int32>(vertices.size()), pShape->GetIndicies().data(), static_cast<int32>(pShape->GetIndicies().size()));
	}
}

bool Window::IsInsideWindow(GameObject* gm) {
	if (gm->GetRenderShape() == nullptr)
		return false;
	Vector2f camPos = SceneManager::GetInstance().GetCurrentScene()->GetCurrentCamera()->GetPosition();
	float32 margin = 50.f;

	AABB entityAABB;
	if (static_cast<int32>(gm->GetRenderShape()->GetRotation()) % 180 != 0) {
		entityAABB = GetRotatedAABB(gm->GetRenderPosition(), { gm->GetRenderShape()->GetWidth(), gm->GetRenderShape()->GetHeight() }, gm->GetRenderShape()->GetRotation() * DEG_TO_RAD);
		entityAABB = { entityAABB.minX - margin, entityAABB.minY - margin, entityAABB.maxX + margin, entityAABB.maxY + margin };
	}
	else
		entityAABB = { gm->GetRenderShape()->GetPosition(0.f, 0.f).x - margin , gm->GetRenderShape()->GetPosition(0.f, 0.f).y - margin , gm->GetRenderShape()->GetPosition(1.f, 1.f).x + margin, gm->GetRenderShape()->GetPosition(1.f, 1.f).y + margin };

	AABB windowAABB = { -margin, -margin, margin + RENDER_TARGET_WIDTH , margin + RENDER_TARGET_HEIGHT };


	return windowAABB.overlaps(entityAABB);
}

void Window::SetFullscreen(bool fullscreen)
{
	if (!SDL_SetWindowFullscreen(mp_Window, fullscreen))
	{
		GCLE_ERROR << "SetWindowFullscreen failed : " << SDL_GetError() << ENDL;
		return;
	}

	m_IsFullscreen = fullscreen;
}

void Window::ToggleFullscreen()
{
	SetFullscreen(!m_IsFullscreen);
}

bool Window::IsFullscreen() const
{
	return m_IsFullscreen;
}

void Window::DrawDebug(gcle::Shape* pShape, Vector2f camPos, float32 zoom)
{
	std::vector<SDL_FPoint*> pointsPtr;

	switch (pShape->GetShape())
	{
	case gcle::Shapes::Rectangle:
	{
		auto pSh = static_cast<gcle::Rectangle*>(pShape);
		pointsPtr = pSh->GetHollow();
		break;
	}
	case gcle::Shapes::Circle:
	{
		pointsPtr = (static_cast<gcle::Circle*>(pShape))->GetHollow();
		break;
	}
	case gcle::Shapes::Triangle:
	{
		pointsPtr = (static_cast<gcle::Triangle*>(pShape))->GetHollow();
		break;
	}
	default:
		break;
	}

	Vector2f screenMiddle{ RENDER_TARGET_WIDTH * 0.5f, RENDER_TARGET_HEIGHT * 0.5f };

	std::vector<SDL_FPoint> points;
	points.reserve(pointsPtr.size());
	for (SDL_FPoint* p : pointsPtr)
	{
		Vector2f world{ p->x, p->y };
		Vector2f screen = (world - camPos) * zoom + screenMiddle;
		points.push_back(SDL_FPoint{ screen.x, screen.y });
	}

	SDL_RenderLines(mp_Renderer, points.data(), static_cast<int32>(points.size()));
}

void Window::SetWindowSize(int32 width, int32 height)
{
	if (mp_Window == nullptr)
		return;
	 
	if (m_IsFullscreen)
		SetFullscreen(false);

	SDL_RestoreWindow(mp_Window);

	if (!SDL_SetWindowSize(mp_Window, width, height))
	{
		GCLE_WARN << "SDL_SetWindowSize failed : " << SDL_GetError() << ENDL;
		return;
	}
	 
	SDL_SyncWindow(mp_Window);
	 
	SDL_SetWindowPosition(mp_Window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

	m_Width = width;
	m_Height = height;
}

Vector2u Window::GetSize()
{
	return { static_cast<int32>(m_Width), static_cast<int32>(m_Height) };
}

Vector2u Window::GetMousePosition()
{
	float32 x = 0.f, y = 0.f;
	SDL_GetMouseState(&x, &y);
	return Vector2u{ static_cast<int32>(x), static_cast<int32>(y) };
}

Vector2f Window::GetWindowSize()
{
	Vector2u size;
	SDL_GetWindowSize(mp_Window, &size.x, &size.y);
	return Vector2f{ static_cast<float32>(size.x), static_cast<float32>(size.y) };
}

SDL_Window* Window::GetWindow(){
	return mp_Window; 
}

SDL_Renderer* Window::GetRenderer() 
{
	return mp_Renderer; 
}
