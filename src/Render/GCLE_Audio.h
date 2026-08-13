#pragma once

struct MIX_Mixer;
struct MIX_Track;

class Audio
{
public: 
	static bool Init(); 
	static void Shutdown();

	static MIX_Mixer* GetMixer();
	static void SetCurrentMusicTrack(MIX_Track* track);

	static void SetMusicVolume(int volume);
	static void StopMusic();
	static void PauseMusic();
	static void ResumeMusic();
	static void StopAllSound();
	static bool IsAMusicPlaying();
	static bool IsAMusicPaused();

public:
	static MIX_Mixer* Mixer;
	static MIX_Track* MusicTrack;
};