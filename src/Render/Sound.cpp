#include "Sound.h"
#include "Utils.h"
#include "GCLE_Audio.h"

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>


bool Sound::IsSoundInit()
{
	return mp_Audio == nullptr ? false : true;
}

Sound::Sound(const std::string& path)
{
	InitSound(path);
}

Sound::~Sound()
{
	if (mp_Track != nullptr)
		MIX_DestroyTrack(mp_Track);
	if (mp_Audio != nullptr)
		MIX_DestroyAudio(mp_Audio);
}

void Sound::InitSound(const std::string& path)
{
	MIX_Mixer* mixer = Audio::GetMixer();
	if (mixer == nullptr)
	{
		GCLE_WARN << "Mixer not initialized, can't load sound with path : " + path << ENDL;
		return;
	}
	 
	MIX_Audio* audio = MIX_LoadAudio(mixer, path.c_str(), true);
	if (audio == NULL)
	{
		GCLE_WARN << "Couldn't initialize sound with path : " + path << ENDL;
		return;
	}

	mp_Audio = audio;
	mp_Track = MIX_CreateTrack(mixer);
	if (mp_Track != nullptr)
		MIX_TagTrack(mp_Track, "sfx");
}

void Sound::PlaySound(int mode, int volume)
{
	if (mp_Audio == nullptr || mp_Track == nullptr)
	{
		GCLE_WARN << "Sound doesn't have a value" << ENDL;
		return;
	}

	MIX_SetTrackGain(mp_Track, static_cast<float32>(volume) / 128.0f);
	MIX_SetTrackAudio(mp_Track, mp_Audio);

	SDL_PropertiesID options = SDL_CreateProperties();
	SDL_SetNumberProperty(options, MIX_PROP_PLAY_LOOPS_NUMBER, mode);
	MIX_PlayTrack(mp_Track, options);
	SDL_DestroyProperties(options);
}