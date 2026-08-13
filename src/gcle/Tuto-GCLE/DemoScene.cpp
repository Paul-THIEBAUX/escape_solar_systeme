#include "DemoScene.h"

#include "Engine/PhysicsManager.h"
#include "Engine/RessourceManager.h"
#include "Engine/GameManager.h"
#include "Engine/Panel.h"
#include "Engine/Slider.h"
#include "Engine/Button.h"
#include "Engine/Toggle.h"
#include "Engine/Image.h"
#include "Render/Text.h"

#include "Tuto-GCLE/DemoPlayer.h"
#include "Tuto-GCLE/GCEnemy.h"
#include "Tuto-GCLE/Table.h"
#include "Tuto-GCLE/Wall.h"
#include "Tag.h"

#include "Tuto-GCLE/TileMapLoader.h"

void DemoScene::OnInitialize()
{
	Scene::OnInitialize();

	pSceneCamera = CreateCamera();
	SwitchCamera(pSceneCamera);


	/*Demo::TileMapLoader loader{};
	loader.CreateMap(this, "../../assets/map/test.gcmap");*/

	mp_Player = CreateEntity<Demo::GCPlayer>(gcle::Shapes::Rectangle);
	//SetSelectedEntity(mp_Player);

	m_Smooth.Initialize(pSceneCamera, mp_Player);

	mp_Enemy = CreateEntity<Demo::GCEnemy>(gcle::Shapes::Rectangle);
	mp_Enemy->SetTarget(mp_Player);
	mp_Enemy->SetDetectionRange(600.0f);
	mp_Enemy->SetAttackRange(400.0f);
	mp_Enemy->SetPosition(100.0f, 100.0f);

	Demo::Wall* pWall = CreateEntity<Demo::Wall>(gcle::Shapes::Rectangle);
	pWall->SetPosition(0, -400);
	pWall->SetScale({ 8.0f, 1.0f });

	Demo::Wall* pWall2 = CreateEntity<Demo::Wall>(gcle::Shapes::Rectangle);
	pWall2->SetPosition(0, 400);
	pWall2->SetScale({ 8.0f, 1.0f });

	Demo::Wall* pWall3 = CreateEntity<Demo::Wall>(gcle::Shapes::Rectangle);
	pWall3->SetPosition(-400, 0);
	pWall3->SetScale({ 1.0f, 8.0f });

	Demo::Wall* pWall4 = CreateEntity<Demo::Wall>(gcle::Shapes::Rectangle);
	pWall4->SetPosition(400, 0);
	pWall4->SetScale({ 1.0f, 8.0f }); 

	InitializeHUD();
}

void DemoScene::InitializeHUD()
{
	constexpr float32 HEALTHBAR_WIDTH = 256.0f;
	constexpr float32 HEALTHBAR_HEIGHT = 32.0f;
	 
	mp_HealthBarBackground = CreatePanel(gcle::Shapes::Rectangle, Color{ 30, 30, 30, 200 });
	mp_HealthBarBackground->SetRenderSize(gcle::Shapes::Rectangle, { HEALTHBAR_WIDTH, HEALTHBAR_HEIGHT });
	mp_HealthBarBackground->SetPosition(215.0f, 40.0f);         
	mp_HealthBarBackground->SetLayer(10);

	mp_HealthBar = CreateSlider(gcle::Shapes::Rectangle); 
	mp_HealthBar->SetVisualMode(SliderVisualMode::Fill);
	mp_HealthBar->SetFillAnchor(SliderFillAnchor::Start); 
	mp_HealthBar->SetRenderSize(gcle::Shapes::Rectangle, { HEALTHBAR_WIDTH, HEALTHBAR_HEIGHT });
	mp_HealthBar->SetPosition(215.0f, 40.0f);
	mp_HealthBar->SetColor(Color::Red);
	mp_HealthBar->SetRange(0.0f, static_cast<float32>(mp_Player->GetMaxLife()));
	mp_HealthBar->SetValue(static_cast<float32>(mp_Player->GetCurrentLife()), false);
	mp_HealthBar->SetOnValueChanged(
		[this](float32 value)
		{
			mp_Player->SetCurrentLife(static_cast<int>(std::floor(value)));
		}
	);
	mp_HealthBar->SetInteractable(true);
	mp_HealthBar->SetLayer(11);
	 
	mp_PlayerPortrait = CreateImage(gcle::Shapes::Rectangle, "player_portrait");
	mp_PlayerPortrait->SetRenderSize(gcle::Shapes::Rectangle, { 64.0f, 64.0f });
	mp_PlayerPortrait->SetPosition(40.0f, 40.0f);
	mp_PlayerPortrait->SetLayer(10); 

	m_UIMainHUD = { mp_HealthBarBackground, mp_HealthBar, mp_PlayerPortrait};

	InitializePauseHUD();
	InitializeOptionsMenu();
}

void DemoScene::InitializePauseHUD()
{
	constexpr float WIDTH_MULTIPLYER = 2.0f;
	constexpr float CENTER_X = 960.0f;
	constexpr float CENTER_Y = 540.0f;

	// --- Background ---
	mp_PauseMenu = CreatePanel(gcle::Shapes::Rectangle, Color{ 30, 30, 30, 200 });
	mp_PauseMenu->SetRenderSize(gcle::Shapes::Rectangle, { (WIDTH_MULTIPLYER + 0.5f) * 100, 864.0f });
	mp_PauseMenu->SetPosition(CENTER_X, CENTER_Y);
	mp_PauseMenu->SetLayer(14);

	// --- Resume ---
	mp_ResumeButton = CreateButton(gcle::Shapes::Rectangle, "Resume");
	mp_ResumeButton->SetPosition(CENTER_X, 288.0f);
	mp_ResumeButton->ScaleBy({ WIDTH_MULTIPLYER , 1.0f});
	mp_ResumeButton->SetLayer(15);
	mp_ResumeButton->AddFunction([this]()
		{
			HandleEscape();
		});

	// --- Option ---
	mp_OptionButton = CreateButton(gcle::Shapes::Rectangle, "Options");
	mp_OptionButton->SetPosition(CENTER_X, 576.0f);
	mp_OptionButton->ScaleBy({ WIDTH_MULTIPLYER , 1.0f});
	mp_OptionButton->SetLayer(15);
	mp_OptionButton->AddFunction([this]()
		{
			OpenMenu(m_UIOptionMenu);
		});

	// --- Quit ---
	mp_QuitButton = CreateButton(gcle::Shapes::Rectangle, "Quit");
	mp_QuitButton->SetPosition(CENTER_X, 864.0f);
	mp_QuitButton->ScaleBy({ WIDTH_MULTIPLYER , 1.0f });
	mp_QuitButton->SetLayer(15);
	mp_QuitButton->AddFunction([]()
		{
			GameManager::GetInstance().Quit();
		});


	m_UIPauseMenu = { mp_PauseMenu, mp_ResumeButton, mp_OptionButton, mp_QuitButton };

	SetMenuActive(m_UIPauseMenu, false);
}

void DemoScene::InitializeOptionsMenu()
{
	constexpr float CENTER_X = RENDER_TARGET_WIDTH / 2.0f;
	constexpr float CENTER_Y = RENDER_TARGET_HEIGHT / 2.0f;

	m_Resolutions = {
		{ 1280, 720  },
		{ 1600, 900  },
		{ 1920, 1080 }
	};
	m_CurrentResolutionIndex = 2;

	mp_OptionsMenu = CreatePanel(gcle::Shapes::Rectangle, Color{ 30, 30, 30, 200 });
	mp_OptionsMenu->SetRenderSize(gcle::Shapes::Rectangle, { 400.0f, 632.0f }); 
	mp_OptionsMenu->SetPosition(CENTER_X, CENTER_Y);
	mp_OptionsMenu->SetLayer(14);

	// --- Resolution ---
	std::string resText =
		std::to_string(m_Resolutions[m_CurrentResolutionIndex].x) + "x" +
		std::to_string(m_Resolutions[m_CurrentResolutionIndex].y);

	mp_ResolutionButton = CreateButton(gcle::Shapes::Rectangle, resText);
	mp_ResolutionButton->SetPosition(CENTER_X, 340.0f);
	mp_ResolutionButton->ScaleBy({ 2.5f, 1.0f });
	mp_ResolutionButton->SetLayer(15);
	mp_ResolutionButton->AddFunction([this]()
		{
			m_CurrentResolutionIndex = (m_CurrentResolutionIndex + 1) % static_cast<int32>(m_Resolutions.size());
			Vector2u newRes = m_Resolutions[m_CurrentResolutionIndex];
			GameManager::GetInstance().GetWindow()->SetWindowSize(newRes.x, newRes.y);
			mp_ResolutionButton->GetText()->SetText(
				std::to_string(newRes.x) + "x" + std::to_string(newRes.y));
		});

	// --- Volume ---
	mp_VolumeSlider = CreateSlider(gcle::Shapes::Rectangle);
	mp_VolumeSlider->SetVisualMode(SliderVisualMode::Handle);
	mp_VolumeSlider->SetHandleSizeRatio(0.15f);
	mp_VolumeSlider->SetRenderSize(gcle::Shapes::Rectangle, { 330.0f, 16.0f });
	mp_VolumeSlider->SetPosition(CENTER_X, 460.0f);
	mp_VolumeSlider->SetColor(Color{ 80, 80, 80, 255 });
	mp_VolumeSlider->SetRange(0.0f, 128.0f);
	mp_VolumeSlider->SetValue(128.0f, false);
	mp_VolumeSlider->SetInteractable(true);
	mp_VolumeSlider->SetLayer(15);
	mp_VolumeSlider->SetOnValueChanged([this](float32 value)
		{
			RessourceManager::GetInstance().SetMusicVolume(static_cast<int32>(value));
			 
			if (mp_OptionsMuteToggle != nullptr)
				mp_OptionsMuteToggle->SetOn(value <= 0.0f, false);
		});

	// --- Mute ---
	mp_OptionsMuteToggle = CreateToggle(gcle::Shapes::Rectangle, "icon_mute", "icon_sound");
	mp_OptionsMuteToggle->SetRenderSize(gcle::Shapes::Rectangle, { 48.0f, 48.0f });
	mp_OptionsMuteToggle->SetPosition(CENTER_X, 540.0f);
	mp_OptionsMuteToggle->SetLayer(15);
	mp_OptionsMuteToggle->SetOnValueChanged([this](bool isMuted)
		{
			RessourceManager::GetInstance().SetMusicVolume(
				isMuted ? 0 : static_cast<int32>(mp_VolumeSlider->GetValue()));
		});

	// --- Back ---
	mp_BackButton = CreateButton(gcle::Shapes::Rectangle, "Back");
	mp_BackButton->SetPosition(CENTER_X, 640.0f);
	mp_BackButton->ScaleBy({ 2.5f, 1.0f });
	mp_BackButton->SetLayer(15);
	mp_BackButton->AddFunction([this]()
		{
			CloseCurrentMenu();
		});

	m_UIOptionMenu = { mp_OptionsMenu, mp_ResolutionButton, mp_VolumeSlider, mp_OptionsMuteToggle, mp_BackButton };

	SetMenuActive(m_UIOptionMenu, false);
}

void DemoScene::SetMenuActive(std::vector<UI*>& ui, bool active)
{
	for (auto& element : ui)
	{
		if (element != nullptr)
			element->SetActive(active);
	}
}

void DemoScene::OpenMenu(std::vector<UI*>& menu)
{
	if (!m_MenuStack.empty())
		SetMenuActive(*m_MenuStack.back(), false);

	SetMenuActive(menu, true);
	m_MenuStack.push_back(&menu);
}

void DemoScene::CloseCurrentMenu()
{
	if (m_MenuStack.empty())
		return;

	SetMenuActive(*m_MenuStack.back(), false);
	m_MenuStack.pop_back();

	if (!m_MenuStack.empty())
		SetMenuActive(*m_MenuStack.back(), true);
}

void DemoScene::HandleEscape()
{
	Clock* pClock = GameManager::GetInstance().GetTime();

	if (m_MenuStack.empty())
	{ 
		OpenMenu(m_UIPauseMenu);
		pClock->SetTimeScale(0.0);
	}
	else if (m_MenuStack.back() == &m_UIPauseMenu)
	{ 
		CloseCurrentMenu();
		pClock->SetTimeScale(1.0);
	}
	else
	{ 
		CloseCurrentMenu();
	}
}

void DemoScene::UpdateHUD()
{
	if (mp_Player == nullptr || mp_HealthBar == nullptr)
		return;

	mp_HealthBar->SetValue(static_cast<float32>(mp_Player->GetCurrentLife()), true);
}

void DemoScene::OnUpdate(Clock& time)
{
	Scene::OnUpdate(time);

	constexpr float ZOOM_ADDITION = 0.1f;

	m_Smooth.Update(static_cast<float>(time.GetDeltaTime()));

	UpdateHUD();

	if (InputManager::GetInstance().IsScrollingUp())
	{
		GetCurrentCamera()->SetZoom(m_Zoom + ZOOM_ADDITION);
		m_Zoom = GetCurrentCamera()->GetZoom();
	}

	if (InputManager::GetInstance().IsScrollingDown())
	{
		GetCurrentCamera()->SetZoom(m_Zoom - ZOOM_ADDITION);
		m_Zoom = GetCurrentCamera()->GetZoom();
	}

	if (InputManager::GetInstance().IsDown('O'))
		SceneManager::GetInstance().SetCurrentSceneWithTag("Sample", true); 
	
	if (InputManager::GetInstance().IsDown(Escape))
	{
		HandleEscape();
	}
}