#include "AudioEmitter.h"

void AudioEmitter::SetMusicVolume(int32 volume)
{
	RessourceManager::GetInstance().SetMusicVolume(volume);
}

void AudioEmitter::StopMusic()
{
	RessourceManager::GetInstance().StopMusic();
}

void AudioEmitter::PauseMusic()
{
	RessourceManager::GetInstance().PauseMusic();
}

void AudioEmitter::ResumeMusic()
{
	RessourceManager::GetInstance().ResumeMusic();
}

void AudioEmitter::StopAllSound()
{
	RessourceManager::GetInstance().StopAllSound();
}

bool AudioEmitter::IsMusicPlaying()
{
	return RessourceManager::GetInstance().isMusicPlaying();
}

bool AudioEmitter::IsMusicPaused()
{
	return RessourceManager::GetInstance().IsMusicPaused();
}

void AudioEmitter::PlayMusic(const std::string& id, int32 mode)
{
	RessourceManager::GetInstance().PlayMusic(id, mode);
}

void AudioEmitter::PlaySoundEffect(const std::string& id, int32 mode, int32 volume)
{
	RessourceManager::GetInstance().PlaySoundEffect(id, mode, volume);
}
