#include "Texture.h"
#include "Utils.h"
#include "Window.h"
#include "Surface.h"
#include "Engine/AssetEngine.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <filesystem>

SDL_Texture* Texture::CreateTexture(Window* window)
{
	if (mp_texture != nullptr && !m_textureChanged)
		return mp_texture;

	if (mp_surface == nullptr || !mp_surface->IsSurfaceInit())
	{
		GCLE_WARN << "Surface is null" << ENDL;
		return mp_texture;
	}

	SDL_DestroyTexture(mp_texture);

	SDL_Texture* texture = SDL_CreateTextureFromSurface(window->GetRenderer(), mp_surface->GetSDL_Surface());
	if (texture == NULL)
	{
		GCLE_WARN << "Error during creation of the texture" << ENDL;
		return nullptr;
	}

	m_textureChanged = false;
	SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
	mp_texture = texture;

	return mp_texture;
}

bool Texture::ToDestroy()
{
	return m_toDestroy;
}

void Texture::Destroy()
{
	m_toDestroy = true;
}

SDL_Texture* Texture::GetSDLTexture()
{
	return mp_texture;
};

bool Texture::IsTextureInit()
{
	return mp_texture == nullptr ? false : true;
}
;

Texture::Texture(Window* pWindow, const std::string& path)
{
	InitTexture(pWindow, path);
}

Texture::Texture(Window* pWindow, Asset* pAsset)
{
	InitTextureWithBuffer(pWindow, pAsset);
}

Texture::Texture(Window* pWindow, Surface* pSurface)
{
	InitTextureWithSurface(pWindow, pSurface);
}

Texture::~Texture() 
{
	SDL_DestroyTexture(mp_texture); 
}

void Texture::InitTextureWithSurface(Window* window, Surface* surface)
{
	if (surface == nullptr || !surface->IsSurfaceInit())
	{
		GCLE_WARN << "Surface is null" << ENDL;
		return;
	}

	mp_surface = surface;

	SDL_Texture* texture = SDL_CreateTextureFromSurface(window->GetRenderer(), surface->GetSDL_Surface());
	if (texture == NULL)
		GCLE_WARN << "Error during creation of the texture" << ENDL;

	SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
	mp_texture = texture; 

	m_Id = mp_surface->GetID();
} 

void Texture::InitTextureWithBuffer(Window* pWindow, Asset* pAsset)
{
	SDL_IOStream* io = SDL_IOFromConstMem(pAsset->data.data(), pAsset->data.size());
	if (io == NULL)
	{
		GCLE_WARN << "Can't read data to create texture" << ENDL;
		return;
	}

	SDL_Surface* surface = IMG_Load_IO(io, true); 
	if (surface == NULL)
	{
		GCLE_WARN << "Can't Create Surface" << ENDL;
		return;
	}

	pAsset->width = surface->w;
	pAsset->height = surface->h;

	SDL_Texture* texture = SDL_CreateTextureFromSurface(pWindow->GetRenderer(), surface);
	if (texture == NULL)
		GCLE_WARN << "Error during creation of the texture" << ENDL;

	//SDL_FreeSurface(surface);
	SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
	mp_texture = texture;

	m_Id = pAsset->name;
}

void Texture::InitTexture(Window* pWindow, const std::string& path)
{
	SDL_Surface* surface = IMG_Load(path.c_str());
	if (surface == NULL)
		GCLE_WARN << "Couldn't initialize surface for texture with path : " + path << ENDL;

	SDL_Texture* texture = SDL_CreateTextureFromSurface(pWindow->GetRenderer(), surface);
	if (texture == NULL)
		GCLE_WARN << "Error during creation of the texture with path : " + path << ENDL;


	//SDL_FreeSurface(surface);
	SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
	mp_texture = texture;


	std::filesystem::path path2(path);
	m_Id = path2.stem().string();
}

std::string& Texture::GetId()
{
	return m_Id;
}
