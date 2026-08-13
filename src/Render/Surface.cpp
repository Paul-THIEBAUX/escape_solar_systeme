#include "Surface.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <include.h>
#include "Window.h"
#include "Engine/AssetEngine.h"

std::string Surface::GetID()
{
	return m_id;
}

SDL_Surface* Surface::GetSDL_Surface()
{
	return mp_surface;
}

bool Surface::IsSurfaceInit()
{
	return mp_surface == nullptr ? false : true;
}

Surface::Surface(Window* window, Asset* asset)
{
	InitSurfaceWithBuffer(window, asset);
}

Surface::Surface(Window* window, const std::string& path)
{
	InitSurface(window, path);
}

Surface::~Surface()
{
	SDL_DestroySurface(mp_surface);
}

void Surface::InitSurfaceWithBuffer(Window* window, Asset* asset)
{
	SDL_IOStream* rw = SDL_IOFromMem(asset->data.data(), static_cast<int>(asset->data.size()));
	if (rw == NULL)
	{
		GCLE_WARN << "Can't read data to create surface" << ENDL;
		return;
	}

	SDL_Surface* surface = IMG_Load_IO(rw, true); //ca free rw tout seul yay
	if (surface == NULL)
	{
		GCLE_WARN << "Can't Create Surface" << ENDL;
		return;
	}

	asset->width = surface->w;
	asset->height = surface->h;

	mp_surface = surface;

	m_id = asset->name;
}

void Surface::InitSurface(Window* window, const std::string& path)
{
	SDL_Surface* surface = IMG_Load(path.c_str());
	if (surface == NULL)
		GCLE_WARN << "Couldn't initialize surface for texture with path : " + path << ENDL;

	mp_surface = surface;

	std::filesystem::path path2(path);
	m_id = path2.stem().string();
}
