#include "Font.h"
#include "Utils.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_Image.h>
#include <filesystem>
#include <iostream>

SDL_Surface* Font::GetFontSurface()
{
	return mp_Font;
}

bool Font::IsFontInit()
{
	return mp_Font == nullptr ? false : true;
}

Font::Font(const std::string& path)
{
    InitFont(path);
}

Font::~Font()
{
    SDL_DestroySurface(mp_Font);
}

void Font::InitFont(const std::string& path)
{
    SDL_Surface* font = IMG_Load(path.c_str());

    if (font == NULL)
    {
        GCLE_WARN << "Couldn't initialize font with path " + path << "  " << SDL_GetError() << ENDL;
        return;
    }

    mp_Font = font;
	ReadFromAtlasChunk(path.c_str());

    return;
}



uint32 Font::ReadBigEndian(const uint8* data) {
	return(data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
}

bool Font::ReadFromAtlasChunk(const std::string& path) {
	std::filesystem::path pngPath = path;
	std::ifstream in(pngPath, std::ios::binary);
	if (!in)
		return false;

	std::vector<uint8> fileData((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
	in.close();

	size_t pos = 8;
	while (pos + 8 <= fileData.size()) {
		uint32 lenght = ReadBigEndian(&fileData[pos]);
		std::string type(fileData.begin() + pos + 4, fileData.begin() + pos + 8);
		if (type == "gATl") {
			const uint8* d = &fileData[pos + 8];
			m_FontSize = static_cast<int32>(ReadBigEndian(d));
			uint32 glyphCount = ReadBigEndian(d + 4);

			size_t offset = 8;
			for (uint32 i = 0; i < glyphCount; i++) {
				GlyphInfo g;
				g.charactere = static_cast<wchar_t>(ReadBigEndian(d + offset));
				offset += 4;
				g.x = static_cast<wchar_t>(ReadBigEndian(d + offset));
				offset += 4;
				g.y = static_cast<wchar_t>(ReadBigEndian(d + offset));
				offset += 4;
				g.width = static_cast<wchar_t>(ReadBigEndian(d + offset));
				offset += 4;
				g.height = static_cast<wchar_t>(ReadBigEndian(d + offset));
				offset += 4;
				g.advanceX = static_cast<wchar_t>(ReadBigEndian(d + offset));
				offset += 4;
				m_Glyphs[static_cast<char>(g.charactere)] = g;
			}
			return true;
		}
		pos += 12 + lenght;
	}
	return false;
}

GlyphInfo* Font::GetGlypInfo(char c)
{
	auto it = m_Glyphs.find(c);
	if (it == m_Glyphs.end())
		return nullptr;

	return &it->second;
}

void Font::GetTextSize(const std::string& text, int32& width, int32& height)
{
	width = 0;
	height = 0;

	for (char c : text)
	{
		GlyphInfo* info = GetGlypInfo(c);

		if (!info)
			continue;

		width += info->advanceX;
		height = std::max(height, info->height);
	}
}


void GlyphInfo::DrawData() {
	std::wcout << "charactere : " << charactere << std::endl;
	std::cout << "x : " << std::to_string(x) << std::endl;
	std::cout << "y : " << std::to_string(y) << std::endl;
	std::cout << "width : " << std::to_string(width) << std::endl;
	std::cout << "height : " << std::to_string(height) << std::endl;
	std::cout << "advanceX : " << std::to_string(advanceX) << std::endl;
}

void GlyphInfo::DrawCharacter() {
	std::wcout << "charactere : " << charactere << std::endl;
}

int32 Font::GetFontSize() { 
	return m_FontSize; 
}
