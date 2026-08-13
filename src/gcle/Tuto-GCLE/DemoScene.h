#pragma once

#include "Engine/Scene.h"
#include "Tuto-GCLE/SmoothCamera.h"

class Panel;
class Slider;
class Button;
class Toggle;
class Image;

namespace Demo
{
	class GCPlayer;
	class GCEnemy;
}

class DemoScene : public Scene
{
public:
	void OnInitialize() override;
	void OnUpdate(Clock& time) override;

private:
	void InitializeHUD();
	void InitializePauseHUD();
	void InitializeOptionsMenu();
	void UpdateHUD();


	void SetMenuActive(std::vector<UI*>& ui, bool active);
	void OpenMenu(std::vector<UI*>& menu);
	void CloseCurrentMenu();
	void HandleEscape();

private:

	float m_Zoom = 1.0f;

	Camera* pSceneCamera = nullptr;

	Demo::GCPlayer* mp_Player = nullptr;
	Demo::GCEnemy* mp_Enemy = nullptr;

	Demo::SmoothCamera m_Smooth;

	// --- HUD ---
	Panel* mp_HealthBarBackground = nullptr;
	Slider* mp_HealthBar = nullptr; 
	Image* mp_PlayerPortrait = nullptr;

	// --- Pause ---
	Panel* mp_PauseMenu = nullptr;
	Button* mp_ResumeButton = nullptr;
	Button* mp_OptionButton = nullptr;
	Button* mp_QuitButton = nullptr;

	// --- Options ---
	Panel* mp_OptionsMenu = nullptr;
	Button* mp_ResolutionButton = nullptr;
	Slider* mp_VolumeSlider = nullptr; 
	Toggle* mp_OptionsMuteToggle = nullptr;
	Button* mp_BackButton = nullptr;

	std::vector<Vector2u> m_Resolutions;
	int32 m_CurrentResolutionIndex = 0;

	std::vector<UI*> m_UIMainHUD;
	std::vector<UI*> m_UIPauseMenu;
	std::vector<UI*> m_UIOptionMenu;

	std::vector<std::vector<UI*>*> m_MenuStack;
};