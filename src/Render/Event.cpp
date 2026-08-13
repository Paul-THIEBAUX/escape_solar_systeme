#include "Event.h"

#include <SDL3/SDL.h>
#include "Engine/GameManager.h" 

bool Event::WindowEvent()
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_EVENT_QUIT)
		{
			return true;
		}
		if (event.type == SDL_EVENT_WINDOW_RESIZED || event.type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED)
		{
			Window* pWindow = GameManager::GetInstance().GetWindow();
			pWindow->m_Width = static_cast<uint32>(pWindow->GetWindowSize().x);
			pWindow->m_Height = static_cast<uint32>(pWindow->GetWindowSize().y);
		}
		if (event.type == SDL_EVENT_MOUSE_WHEEL)
		{
			InputManager::GetInstance().OnMouseWheel(event.wheel.x, event.wheel.y);
		}
	}
	return false;
}