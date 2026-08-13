#include "Image.h"

void Image::SetSprite(const std::string& id)
{
	SetTexture(id);
}

void Image::SetTint(Color color)
{
	SetColor(color);
}
