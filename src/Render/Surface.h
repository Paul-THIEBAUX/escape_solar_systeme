#pragma once
#include <string>

class Window;
struct Asset;
struct SDL_Texture;
struct SDL_Surface;


class Surface
{
private:

	SDL_Surface* mp_surface = nullptr;

	std::string m_id;

public:

	std::string GetID();

	SDL_Surface* GetSDL_Surface();
	bool IsSurfaceInit();

	Surface(Window* window, Asset* asset);
	Surface(Window* window, const std::string& path);
	~Surface();

	void InitSurfaceWithBuffer(Window* window, Asset* asset);
	void InitSurface(Window* window, const std::string& path);
};