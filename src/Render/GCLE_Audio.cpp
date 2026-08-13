#include "GCLE_Audio.h"
#include "Utils.h"

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

MIX_Mixer* Audio::Mixer = nullptr;
MIX_Track* Audio::MusicTrack = nullptr;

bool Audio::Init()
{
	if (!MIX_Init())
	{
		GCLE_WARN << "MIX_Init failed : " << SDL_GetError() << ENDL;
		return false;
	}

	Mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
	if (Mixer == nullptr)
	{
		GCLE_WARN << "MIX_CreateMixerDevice failed : " << SDL_GetError() << ENDL;
		return false;
	}

	return true;
}

void Audio::Shutdown()
{
	MIX_Quit(); 
	Mixer = nullptr;
	MusicTrack = nullptr;
}

MIX_Mixer* Audio::GetMixer()
{
	return Mixer;
}

void Audio::SetCurrentMusicTrack(MIX_Track* track){
	MusicTrack = track; 
}



void Audio::SetMusicVolume(int volume)
{
	if (MusicTrack != nullptr)
		MIX_SetTrackGain(MusicTrack, static_cast<float>(volume) / 128.0f);
}

void Audio::StopMusic()
{
	if (MusicTrack != nullptr)
		MIX_StopTrack(MusicTrack, 0);
}

void Audio::PauseMusic()
{
	if (MusicTrack != nullptr)
		MIX_PauseTrack(MusicTrack);
}

void Audio::ResumeMusic()
{
	if (MusicTrack != nullptr)
		MIX_ResumeTrack(MusicTrack);
}

void Audio::StopAllSound()
{
	if (Mixer != nullptr)
		MIX_StopTag(Mixer, "sfx", 0);
}

bool Audio::IsAMusicPlaying()
{
	return MusicTrack != nullptr && MIX_TrackPlaying(MusicTrack);
}

bool Audio::IsAMusicPaused()
{
	return MusicTrack != nullptr && MIX_TrackPaused(MusicTrack);
}