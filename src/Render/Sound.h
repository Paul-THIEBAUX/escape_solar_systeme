#pragma once

#include <string>

struct MIX_Audio;
struct MIX_Track;

class Sound
{
private:

	MIX_Audio* mp_Audio = nullptr;
	MIX_Track* mp_Track = nullptr;

public:

	bool IsSoundInit();

	Sound(const std::string& path);
	Sound() = default;
	~Sound();

	void InitSound(const std::string& path);

	void PlaySound(int mode, int volume);

};