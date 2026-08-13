#pragma once
#include "Font.h"
#include <string>
#include "PrimitiveTypes.h"
#include "include.h"

class Window;
struct SDL_Texture;
struct SDL_Color;
struct SDL_FRect; 


class Text
{
public:

	SDL_Texture* GetTexture(Window* pWindow);
	SDL_FRect* GetSDLRect();

	Text(Font* pFont, const std::string& text, Vector2f pos, int32 fontSize, byte r = 255, byte g = 255, byte b = 255, byte a = 255);

	~Text();

	void SetColor(byte r, byte g, byte b, byte a = 255);
	void SetFont(const std::string& id);
	void SetText(const std::string& text);

	void SetPosition(int32 x, int32 y);


	Font* GetFont();
	void SetFontSize(int32 size);
	int32 GetFontSize();

	Vector2f GetSizes();

	bool IsActive() const;
	void SetActive(bool active);

private:

	Font* mp_Font = nullptr;
	SDL_Texture* mp_Texture = nullptr;

	std::string m_Text;

	SDL_Color* mp_Color = nullptr;
	SDL_FRect* mp_Rect = nullptr;

	bool m_NeedToChange = false;
	int32 m_FontSize = 24;

	bool m_IsActive = true;

};