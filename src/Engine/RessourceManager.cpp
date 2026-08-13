#include "RessourceManager.h"
#include "Texture.h"
#include "Utils.h"
#include "SceneManager.h"
#include "AssetEngine.h"


#undef PlaySound

RessourceManager* RessourceManager::s_Instance = nullptr;

RessourceManager& RessourceManager::GetInstance() {
	if (s_Instance == nullptr)
		s_Instance = GCLE_NEW RessourceManager();
	return *s_Instance;
}

void RessourceManager::DestroyInstance()
{
	delete s_Instance;
	s_Instance = nullptr;
}

void RessourceManager::PlayMusic(const std::string& id, int32 mode)
{
	if (m_MusicMap.count(id))
		m_MusicMap[id]->PlayMusic(mode);
}

void RessourceManager::PlaySoundEffect(const std::string& id, int32 mode, int32 volume)
{
	if (!m_SoundMap.count(id))
		return;

	m_SoundMap[id]->PlaySound(mode, volume);
}

void RessourceManager::ForcePutSurface(Surface* pText, std::string id)
{
	m_surfaceMap[id].mp_surface = pText;
}

Font* RessourceManager::GetFont(const std::string& id)
{
	auto it = m_FontMap.find(id);
	if (it == m_FontMap.end())
	{
		GCLE_WARN << "Font '" << id << "' doesn't exist" << ENDL;
		return nullptr;
	}
	return it->second;
}

SurfaceStruct* RessourceManager::GetSurface(const std::string& id)
{
	if (!m_surfaceMap.contains(id))
		return nullptr;

	return &m_surfaceMap[id];
}

Surface* RessourceManager::LoadSurface(Window* pWindow, const std::string& path, const std::string& id)
{
	if (m_surfaceMap[id].mp_surface != nullptr)
		return m_surfaceMap[id].mp_surface;

	Surface* texture = new Surface(pWindow, path);
	if (texture == nullptr || !texture->IsSurfaceInit())
	{
		GCLE_WARN << "Got a nullptr Surface for path : " + path << ENDL;
		delete texture;
		return nullptr;
	}

	GCLE_INFO << "Surface created" << ENDL;
	m_surfaceMap[id].mp_surface = texture;
	AssetEngine::GetInstance().AddAsset(id);
	return texture;
}

void RessourceManager::AddTexture(const std::string& id, Texture* tex)
{
	m_textures[id].push_back(tex);
}

bool RessourceManager::LoadMusic(const std::string& path, const std::string& id)
{
	Music* music = new Music(path);
	if (music == nullptr || !music->IsMusicInit())
	{
		GCLE_WARN << "Got nullptr music for path " + path << ENDL;
		delete music;
		return false;
	}

	m_MusicMap[id] = music;
	return true;
}

bool RessourceManager::LoadSound(const std::string& path, const std::string& id)
{
	Sound* sound = new Sound(path);
	if (sound == nullptr || !sound->IsSoundInit())
	{
		GCLE_WARN << "Got a nullptr sound for path : " + path << ENDL;
		delete sound;
		return false;
	}

	m_SoundMap[id] = sound;
	return true;
}

bool RessourceManager::LoadFont(const std::string& path, const std::string& id)
{
	Font* font = new Font(path);
	if (font == NULL)
	{
		GCLE_WARN << "Got a nullptr font for path " + path << ENDL;
		delete font;
		return false;
	}

	m_FontMap[id] = font;
	return true;
}

void RessourceManager::Init(Window* pWindow)
{
	InitTextureFolder(pWindow);
	InitMusicFolder();
	InitSoundFolder();
	InitFont();
}

void RessourceManager::InitTextureFolder(Window* pWindow)
{
	std::filesystem::path filename = "../../assets/textures";

	GCLE_INFO << "CWD: " << std::filesystem::current_path().string() << ENDL; 

	if (!std::filesystem::exists(filename) || !std::filesystem::is_directory(filename))
	{
		GCLE_WARN << "Directory " + filename.string() + "  does not exist" << ENDL;
		return;
	}

	for (auto& entry : std::filesystem::directory_iterator(filename))
	{
		if (!entry.is_regular_file())
		{
			GCLE_WARN << "File is not valid : " + entry.path().filename().string() << ENDL;
			continue;
		}

		if (entry.path().extension() != ".png")
		{
			GCLE_WARN << "Extension is not correct, expected '.png' receive : " + entry.path().extension().string() << ENDL;
			continue;
		}

		if (m_surfaceMap.contains(entry.path().stem().string()))
		{
			GCLE_INFO << "Texture '" << entry.path().stem().string() << "' already loaded" << ENDL;
			continue;
		}

		//LoadTexture(window,entry.path().string(), entry.path().stem().string());
		m_surfaceMap[entry.path().stem().string()].mp_surface = nullptr;
	}

	//for (auto& tex : m_textureMap) {
	//    if (tex.second.GetFlag() & SceneManager::GetInstance().GetCurrentSceneFlag() == 0b0) {
	//        tex.second.UnloadTexture();
	//    }
	//}
}

void RessourceManager::InitMusicFolder()
{
	std::filesystem::path filename = "../../assets/musics";

	if (!std::filesystem::exists(filename) || !std::filesystem::is_directory(filename))
	{
		std::cout << "Directory " + filename.string() + "does not exits" << std::endl;
		return;
	}

	for (auto& entry : std::filesystem::directory_iterator(filename))
	{
		if (!entry.is_regular_file())
		{
			std::cout << "File is not valid : " + entry.path().filename().string();
			continue;
		}

		if (entry.path().extension() != ".mp3")
		{
			std::cout << "Extension is not correct, expected : .mp3, receive" + entry.path().extension().string();
			continue;
		}

		LoadMusic(entry.path().string(), entry.path().stem().string());
	}
}

void RessourceManager::InitSoundFolder()
{
	std::filesystem::path filename = "../../assets/sounds";

	if (!std::filesystem::exists(filename) || !std::filesystem::is_directory(filename))
	{
		std::cout << "Directory " + filename.string() + "does not exits" << std::endl;
		return;
	}

	for (auto& entry : std::filesystem::directory_iterator(filename))
	{
		if (!entry.is_regular_file())
		{
			std::cout << "File is not valid : " + entry.path().filename().string();
			continue;
		}

		if (entry.path().extension() != ".wav")
		{
			std::cout << "Extension is not correct, expected : .wav, receive" + entry.path().extension().string();
			continue;
		}

		LoadSound(entry.path().string(), entry.path().stem().string());
	}
}

void RessourceManager::InitFont()
{
	std::filesystem::path filename = "../../assets/fonts";

	if (!std::filesystem::exists(filename) || !std::filesystem::is_directory(filename))
	{
		std::cout << "Directory " + filename.string() + "does not exits" << std::endl;
		return;
	}

	for (auto& entry : std::filesystem::directory_iterator(filename))
	{
		if (!entry.is_regular_file())
		{
			std::cout << "File is not valid : " + entry.path().filename().string();
			continue;
		}

        if (entry.path().extension() != ".png")
        {
            std::cout << "Extension is not correct, expected : .png, receive" + entry.path().extension().string();
            continue;
        }

        m_FontMap[entry.path().stem().string()] = GCLE_NEW Font(filename.string() +"/" + entry.path().filename().string());
    }
}

void RessourceManager::EraseTexture(const std::string& id)
{
	for (auto& tex : m_textures[id])
		delete tex;

	m_textures[id].clear();
}

void RessourceManager::DeleteAll()
{
	DeleteAllFont();
	DeleteAllMusic();
	DeleteAllSound();
	DeleteAllSurface();
}

void RessourceManager::DeleteFont(const std::string& id)
{
	if (!m_FontMap.contains(id))
		return;

	delete m_FontMap[id];
	m_FontMap.erase(id);
}

void RessourceManager::DeleteAllFont()
{
	for (auto& pair : m_FontMap)
		delete pair.second;

	m_FontMap.clear();
}

void RessourceManager::DeleteMusic(const std::string& id)
{
	if (!m_MusicMap.contains(id))
		return;

	delete m_MusicMap[id];
	m_MusicMap.erase(id);
}

void RessourceManager::DeleteAllMusic()
{
	for (auto& pair : m_MusicMap)
		delete pair.second;

	m_MusicMap.clear();
}

void RessourceManager::DeleteSound(const std::string& id)
{
	if (!m_SoundMap.contains(id))
		return;

	delete m_SoundMap[id];
	m_SoundMap.erase(id);
}

void RessourceManager::DeleteAllSound()
{
	for (auto& pair : m_SoundMap)
		delete pair.second;

	m_SoundMap.clear();
}

void RessourceManager::DeleteSurface(const std::string& id)
{
	if (!m_surfaceMap.count(id))
		return;

	AssetEngine::GetInstance().DeleteAsset(id);
	delete m_surfaceMap[id].mp_surface;
	m_surfaceMap[id].mp_surface = nullptr;
	m_surfaceMap.erase(id);
	EraseTexture(id);
}

void RessourceManager::DeleteAllSurface()
{
	for (auto& pair : m_surfaceMap)
	{
		if (pair.second.mp_surface)
		{
			delete pair.second.mp_surface;
			pair.second.mp_surface = nullptr;
			EraseTexture(pair.first);
		}
	}

	m_surfaceMap.clear();
}

RessourceManager::~RessourceManager()
{
	DeleteAll();
}

void RessourceManager::SetMusicVolume(int32 volume) {
	Audio::SetMusicVolume(volume);
}

void RessourceManager::StopMusic() {
	Audio::StopMusic();
}

void RessourceManager::PauseMusic() {
	Audio::PauseMusic();
}

void RessourceManager::ResumeMusic() {
	Audio::ResumeMusic();
}

void RessourceManager::StopAllSound() {
	Audio::StopAllSound();
}

bool RessourceManager::isMusicPlaying() {
	return Audio::IsAMusicPlaying();
}

bool RessourceManager::IsMusicPaused() {
	return Audio::IsAMusicPaused();
}