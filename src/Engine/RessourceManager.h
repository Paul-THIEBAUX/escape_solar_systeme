#pragma once
#include <unordered_map>
#include <filesystem>
#include <fstream>
#include <string>
#include "Render/Sprite.h"
#include "Render/Sound.h"
#include "Render/Music.h"
#include "Render/Font.h"
#include "Render/GCLE_Audio.h"
#include "Render/Surface.h"
#include "include.h"

class Window; 

struct SurfaceStruct {
public:
	Surface* mp_surface = nullptr;
	
private:
	uint32 m_Flag = 0b0;

public:
	void SetFlag(uint32 flag) {
		m_Flag = flag;
	}

	void AddFlag(uint32 flag) {
		m_Flag |= flag;
	}

	uint32 GetFlag() const {
		return m_Flag;
	}

	void UnloadTexture() {
		if (mp_surface != nullptr)
			delete mp_surface;
		mp_surface = nullptr;
	}

	bool IsLoaded() {
		return mp_surface != nullptr;
	}

	~SurfaceStruct() {
		delete mp_surface;
	}

};


class RessourceManager
{
public:

	static RessourceManager& GetInstance();
	static void DestroyInstance();

	Font* GetFont(const std::string& id);
	SurfaceStruct* GetSurface(const std::string& id);
	Surface* LoadSurface(Window* pWindow, const std::string& path, const std::string& id);

	void AddTexture(const std::string& id, Texture* tex);

	void SetMusicVolume(int32 volume) ;

	void StopMusic()	 ;
	void PauseMusic()	 ;
	void ResumeMusic()	 ;
	void StopAllSound()  ;

	bool isMusicPlaying();
	bool IsMusicPaused() ;

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

	bool LoadMusic(const std::string& path, const std::string& id);
	bool LoadSound(const std::string& path, const std::string& id);

	bool LoadFont(const std::string& path, const std::string& id);

	void Init(Window* pWindow);


	void InitTextureFolder(Window* pWindow);
	void InitMusicFolder();
	void InitSoundFolder();
	void InitFont();

	void EraseTexture(const std::string& id);

	void DeleteAll();
	void DeleteFont(const std::string& id);
	void DeleteAllFont();
	void DeleteMusic(const std::string& id);
	void DeleteAllMusic();
	void DeleteSound(const std::string& id);
	void DeleteAllSound();
	void DeleteSurface(const std::string& id);
	void DeleteAllSurface();

	~RessourceManager();

private:
	void ForcePutSurface(Surface* pText, std::string id);

private:

	std::unordered_map<std::string, Font*> m_FontMap; 
	std::unordered_map<std::string, Sound*> m_SoundMap;
	std::unordered_map<std::string, Music*> m_MusicMap; 
	std::unordered_map<std::string, SurfaceStruct> m_surfaceMap; 
	std::unordered_map<std::string, std::vector<Texture*>> m_textures;

private:
	static RessourceManager* s_Instance;


private:

	friend class AssetEngine;
	friend class SceneManager;
};