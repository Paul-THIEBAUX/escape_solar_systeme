#pragma once
#include <string>

class Window;
class Surface;
struct Asset;
struct SDL_Texture;
struct SDL_Surface;

class Texture
{
protected:

	SDL_Texture* mp_texture = nullptr;
	Surface* mp_surface;

	bool m_isSprite = false;
	bool m_textureChanged = false;

	bool m_toDestroy = false; 

public:

	std::string m_Id = "";

	SDL_Texture* CreateTexture(Window* window);

	bool IsSprite() { return m_isSprite; }; // a move dans le cpp

	bool IsTextureInit();

	bool ToDestroy();
	void Destroy();

	SDL_Texture* GetSDLTexture();

	Texture(Window* pWindow, Surface* pSurface);
	Texture() = default;

	virtual ~Texture() ;

	void InitTextureWithSurface(Window* pWindow, Surface* pSurface); 

	//Maybe not really usefull now
	Texture(Window* pWindow, const std::string& path);
	//Maybe not really usefull now
	Texture(Window* pWindow, Asset* pAsset);
	void InitTextureWithBuffer(Window* pWindow, Asset* pAsset);
	void InitTexture(Window* pWindow, const std::string& path);
	std::string& GetId();
};