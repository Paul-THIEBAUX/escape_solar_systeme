#include "Music.h"
#include "Utils.h"
#include "GCLE_Audio.h"

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

bool Music::IsMusicInit()
{
	return mp_Audio == nullptr ? false : true;
}

Music::Music(const std::string& path)
{
	InitMusic(path);
}

Music::~Music()
{
	if (mp_Track != nullptr)
		MIX_DestroyTrack(mp_Track);
	if (mp_Audio != nullptr)
		MIX_DestroyAudio(mp_Audio);
}

void Music::InitMusic(const std::string& path)
{
	MIX_Mixer* mixer = Audio::GetMixer();
	if (mixer == nullptr)
	{
		GCLE_WARN << "Mixer not initialized, can't load music with path : " + path << ENDL;
		return;
	}
	 
	MIX_Audio* audio = MIX_LoadAudio(mixer, path.c_str(), false);
	if (audio == NULL)
	{
		GCLE_WARN << "Couldn't inititialize music with path : " + path << ENDL;
		return;
	}

	mp_Audio = audio;
	mp_Track = MIX_CreateTrack(mixer);
	if (mp_Track != nullptr)
		MIX_TagTrack(mp_Track, "music");
}

void Music::PlayMusic(int32 mode)
{
	if (mp_Audio == nullptr || mp_Track == nullptr)
	{
		GCLE_WARN << "Music is not inititialize can't play it" << ENDL;
		return;
	}

	MIX_SetTrackAudio(mp_Track, mp_Audio);

	SDL_PropertiesID options = SDL_CreateProperties();
	SDL_SetNumberProperty(options, MIX_PROP_PLAY_LOOPS_NUMBER, mode); 
	MIX_PlayTrack(mp_Track, options);
	SDL_DestroyProperties(options);

	Audio::SetCurrentMusicTrack(mp_Track);
}