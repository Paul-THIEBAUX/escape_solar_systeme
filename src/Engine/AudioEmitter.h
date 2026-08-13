#pragma once
#include "RessourceManager.h"

class AudioEmitter
{
public:

	void SetMusicVolume(int32 volume);

	void StopMusic();
	void PauseMusic();
	void ResumeMusic();

	void StopAllSound();

	bool IsMusicPlaying();
	bool IsMusicPaused();

	/// <summary>
	/// Mode : 1 = 1 fois,  0 = 1 boucle, -1 = infinite loop
	/// </summary>
	/// <param name="id"></param>
	/// <param name="mode"></param>
	void PlayMusic(const std::string& id, int32 mode);
	/// <summary>
	/// Mode : 1 = 1 fois,  0 = 1 boucle, -1 = infinite loop
	/// </summary>
	/// <param name="id"></param>
	/// <param name="mode"></param>
	void PlaySoundEffect(const std::string& id, int32 mode, int32 volume);

	AudioEmitter() = default;
	~AudioEmitter() = default;
};