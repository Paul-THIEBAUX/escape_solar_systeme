#pragma once

#include <string>
#include "include.h"

struct MIX_Audio;
struct MIX_Track;

class Music
{
private:

	MIX_Audio* mp_Audio = nullptr;
	MIX_Track* mp_Track = nullptr;

public:

	bool IsMusicInit() ;

	Music(const std::string& path);
	Music() = default;

	~Music();

	void InitMusic(const std::string& path);

	void PlayMusic(int32 mode);
};