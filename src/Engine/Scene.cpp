#include "Scene.h"
#include "Render/Text.h"
#include "PhysicsManager.h"

#include <sstream>
#include <iomanip>
#include "WorldText.h"
#include "RessourceManager.h"
#include "InputManager.h"

void Scene::Draw(Window* pWindow) 
{
	m_NumberOfDraw = 0;
	for (auto& layer : GameManager::GetInstance().m_Entities) {
		for(Entity* e : layer){

			if (e->IsActive() == false)
				continue;

			if (pWindow->IsInsideWindow(e) && e->GetRenderShape() != nullptr && m_FrustrumCulling) 
			{
				if (e->IsActiveIn(m_Tag)) 
				{
					GameManager::GetInstance().GetWindow()->Draw(e->GetRenderShape());
					m_NumberOfDraw++;
				}
			}
			else if (e->GetRenderShape() != nullptr && m_FrustrumCulling == false)
			{
				if (e->IsActiveIn(m_Tag))
				{
					GameManager::GetInstance().GetWindow()->Draw(e->GetRenderShape());
					m_NumberOfDraw++;
				}
			}
			else if (e->IsWorldText()) 
			{
				if (e->IsActiveIn(m_Tag)) 
				{
					WorldText* text = static_cast<WorldText*>(e);
					GameManager::GetInstance().GetWindow()->DrawTextOnRenderer(text->GetText());
					m_NumberOfDraw++;
				}
			}
		}
	}

	for (auto& layer : GameManager::GetInstance().m_UIs) {
		for (UI* ui : layer) {

			if (ui->IsActive() == false)
				continue;

			if (pWindow->IsInsideWindow(ui) && ui->GetRenderShape() != nullptr && m_FrustrumCulling)
			{
				if (ui->IsActiveIn(m_Tag))
				{
					GameManager::GetInstance().GetWindow()->Draw(ui->GetRenderShape());
					m_NumberOfDraw++;
				}
			}
			else if (ui->GetRenderShape() != nullptr)
			{
				if (ui->IsActiveIn(m_Tag))
				{
					GameManager::GetInstance().GetWindow()->Draw(ui->GetRenderShape());
					m_NumberOfDraw++;
				}
			}
		}
	}

	for (Text* t : m_Texts)
	{
		if(t->IsActive())
		{
			GameManager::GetInstance().GetWindow()->DrawTextOnRenderer(t);
			m_NumberOfDraw++;
		}
	}
}

Scene::~Scene()
{
#ifdef _DEBUG
	for (Text* t : m_DebugTexts)
		delete t;
	m_DebugTexts.clear();

	for (Text* t : m_DebugInfoTexts)
		delete t;
	m_DebugInfoTexts.clear();
#endif // DEBUG


	for (Text* t : m_Texts)
		delete t;
	m_Texts.clear();


	mp_MainCamera = nullptr;
	mp_ActiveCamera = nullptr;
}

Text* Scene::CreateText(const std::string& text,Vector2f pos, int32 fontSize, byte r, byte g, byte b)
{
	Font* font = RessourceManager::GetInstance().GetFont("Hack-Regular");
	if (font == nullptr)
	{
		GCLE_WARN << "Couldn't find the default font" << ENDL;
		return nullptr;
	}

	Text* new_text = GCLE_NEW Text(font,text,pos, fontSize, r, g, b);
	m_Texts.push_back(new_text);
	return new_text;
}

void Scene::DestroyText(Text* pText)
{
	std::erase_if(m_Texts, [pText](Text* t) { return t == pText; });
	delete pText;
}

Camera* Scene::CreateCamera()
{
	Camera* pCamera = GCLE_NEW Camera();
	pCamera->Init(GameManager::GetInstance().GetWindow());

	GameManager::GetInstance().m_Camera.push_back(pCamera);

	if (mp_ActiveCamera != nullptr)
		mp_ActiveCamera->SetActive(false);

	pCamera->SetActive(true);
	mp_ActiveCamera = pCamera;

	return pCamera;
}

void Scene::AddDrawnTexture(const std::string& textureName){
	if (std::find(m_ActiveTextures.begin(), m_ActiveTextures.end(), textureName) != m_ActiveTextures.end()) {
		return;
	}
	m_ActiveTextures.push_back(textureName);
}

void Scene::AddActiveCamera(Camera* pCam)
{
	if (std::find(m_ActiveCamera.begin(), m_ActiveCamera.end(), pCam) != m_ActiveCamera.end()) {
		return;
	}
	m_ActiveCamera.push_back(pCam);
}

bool Scene::isDrawn(const std::string& tag){
	return (std::find(m_ActiveTextures.begin(), m_ActiveTextures.end(), tag) != m_ActiveTextures.end());
}

void Scene::Update(Clock& time) const 
{ 
	for (auto& layer : GameManager::GetInstance().m_Entities) {
		for(Entity* e : layer)
		{
			if (e->IsActiveIn(m_Tag))
				e->Update(static_cast<float32>(time.GetDeltaTime()));
		}
	} 
	for (auto& layer : GameManager::GetInstance().m_UIs) {
		for(UI* ui : layer)
		{
			if (ui->IsActiveIn(m_Tag))
				ui->Update(static_cast<float32>(time.GetDeltaTime()));
		}
	}  
}

#ifdef _DEBUG

void Scene::SetDebug()
{
	m_Debug = true;
}

void Scene::SetSelectedEntity(Entity* pEntity)
{
	mp_SelectedEntity = pEntity;
}

Text* Scene::CreateDebugText(const std::string& text, Vector2f pos, int32 fontSize, byte r, byte g, byte b)
{
	Font* font = RessourceManager::GetInstance().GetFont("Hack-Regular");
	if (font == nullptr)
	{
		GCLE_WARN << "Couldn't find the default font" << ENDL;
		return nullptr;
	}

	Text* new_text = GCLE_NEW Text(font, text, pos, fontSize, r, g, b);
	m_DebugTexts.push_back(new_text);
	return new_text;
}

void Scene::DestroyDebugText(Text* pText)
{
	std::erase_if(m_DebugTexts, [pText](Text* t) { return t == pText; });
	delete pText;
}

Text* Scene::CreateDebugInfoText(const std::string& text, Vector2f pos, int32 fontSize, byte r, byte g, byte b)
{
	Font* font = RessourceManager::GetInstance().GetFont("Hack-Regular");
	if (font == nullptr)
	{
		GCLE_WARN << "Couldn't find the default font" << ENDL;
		return nullptr;
	}

	Text* new_text = GCLE_NEW Text(font, text, pos, fontSize, r, g, b);
	m_DebugInfoTexts.push_back(new_text);
	return new_text;
}

void Scene::DestroyDebugInfoText(Text* pText)
{
	std::erase_if(m_DebugInfoTexts, [pText](Text* t) { return t == pText; });
	delete pText;
}

void Scene::DrawDebug(Window* pWindow)
{
	Vector2f camPos = mp_ActiveCamera != nullptr ? mp_ActiveCamera->GetPosition() : Vector2f{ 0.f, 0.f };
	float32 zoom = mp_ActiveCamera != nullptr ? mp_ActiveCamera->GetZoom() : 1.0f;

	if (m_IsVisualDebugActive)
	{
		for (auto& layer : GameManager::GetInstance().m_Entities)
		{
			for (Entity* e : layer)
			{
				if (!e->IsActiveIn(m_Tag))
					continue;

				for (auto& col : e->GetColliders())
				{
					if (col->IsActive())
					{
						GameManager::GetInstance().GetWindow()->ClearWindowWithColor(0, 255, 0, 255);
						GameManager::GetInstance().GetWindow()->DrawDebug(col->GetShape(), camPos, zoom);
					}
				}
			}
		}
	}

	if (mp_SelectedEntity != nullptr && m_DebugPerf && mp_SelectedEntity->IsActiveIn(m_Tag))
	{
		GameManager::GetInstance().GetWindow()->ClearWindowWithColor(255, 255, 0, 255);
		GameManager::GetInstance().GetWindow()->DrawDebug(mp_SelectedEntity->GetRenderShape(), camPos, zoom);

		for (auto& col : mp_SelectedEntity->GetColliders())
		{
			GameManager::GetInstance().GetWindow()->ClearWindowWithColor(0, 255, 0, 255);
			GameManager::GetInstance().GetWindow()->DrawDebug(col->GetShape(), camPos, zoom);
		}
	}

	if (m_Debug && mp_SelectedEntity != nullptr)
	{
		for (Text* t : m_DebugInfoTexts)
			GameManager::GetInstance().GetWindow()->DrawTextOnRenderer(t);
	}

	if (m_Debug && m_DebugPerf)
	{
		for (Text* t : m_DebugTexts)
			GameManager::GetInstance().GetWindow()->DrawTextOnRenderer(t);
	}
}


void Scene::SetGizmoVisibility()
{
	m_IsVisualDebugActive = !m_IsVisualDebugActive;
}

void Scene::EntityInfoVisibility(const int32 debugConstant)
{
	if (InputManager::GetInstance().IsDown(MiddleButton))
	{
		bool selected = false;
		for (auto& layer : GameManager::GetInstance().m_Entities)
		{
			for (Entity* e : layer)
			{
				if (e->IsInside(mp_ActiveCamera->GetScreenMousePosition()))
				{
					mp_SelectedEntity = e;
					mp_SelectedEntity->m_IsHighlighted = true;
					selected = true;
					break;
				}
			}
		}

		if (selected == false)
		{
			if (mp_SelectedEntity != nullptr)
			{
				mp_SelectedEntity->m_IsHighlighted = false;
				mp_SelectedEntity = nullptr;
			}
		}
	}

	if (mp_SelectedEntity != nullptr && m_UpdateDebug >= debugConstant)
	{ 
		Vector2f pos = mp_SelectedEntity->GetPosition();
		Degrees rot = mp_SelectedEntity->GetRotation();
		Vector2f scale = mp_SelectedEntity->GetScale();


		mp_PosX->SetText(std::to_string(pos.x));
		mp_PosY->SetText(std::to_string(pos.y));

		mp_RotZ->SetText(std::to_string(rot));

		mp_ScaleX->SetText(std::to_string(scale.x));
		mp_ScaleY->SetText(std::to_string(scale.y));


		m_UpdateDebug = 0;
	}
}

void Scene::DebugSetEntityInfo()
{
	if (mp_SelectedEntity != nullptr)
	{
		if (InputManager::GetInstance().IsDown(RightButton))
		{
			mp_SelectedEntity->SetPosition(
				mp_ActiveCamera->GetMouseScreenToWorldPosition().x,
				mp_ActiveCamera->GetMouseScreenToWorldPosition().y
			);
		}

		if (InputManager::GetInstance().IsHeld(LeftArrow))
		{
			mp_SelectedEntity->Rotate(-0.1f);
		}

		if (InputManager::GetInstance().IsHeld(RightArrow))
		{
			mp_SelectedEntity->Rotate(0.1f);
		}

		if (InputManager::GetInstance().IsHeld(UpArrow))
		{
			mp_SelectedEntity->ScaleBy({ 1.01f, 1.01f });
		}

		if (InputManager::GetInstance().IsHeld(DownArrow))
		{
			mp_SelectedEntity->ScaleBy({ 0.99f, 0.99f });
		}
	}
}

void Scene::SetDebugInfo() const
{
	if (m_DebugPerf && m_UpdateDebug >= 4)
	{
		auto FormatMs = +[](float value)
			{
				std::ostringstream oss;
				oss << std::fixed << std::setprecision(2) << value;
				return oss.str();
			};

		mp_CollidersP->SetText(FormatMs(PROFILER_GET("Colliders")) + "ms");
		mp_EntityP->SetText(FormatMs(PROFILER_GET("Entity")) + "ms");
		mp_InputP->SetText(FormatMs(PROFILER_GET("Input")) + "ms");
		mp_UpdateP->SetText(FormatMs(PROFILER_GET("SceneU")) + "ms");
		mp_DrawP->SetText(FormatMs(PROFILER_GET("SceneD")) + "ms");

		mp_NumberDrawP->SetText(std::to_string(m_NumberOfDraw));
	}
}

#endif // _DEBUG

void Scene::OnInitialize()
{ 
#ifdef _DEBUG 

	SetDebug();
	mp_MainCamera = CreateCamera();
	mp_MainCamera->SetActive(true);
	mp_ActiveCamera = mp_MainCamera;

	mp_Position				= CreateDebugInfoText("Pos: "		, { 0,		0	},		25,		255, 225, 255);
	mp_PosX					= CreateDebugInfoText(""			, { 110,	0	},		25,		255, 225, 255);
	mp_PosY					= CreateDebugInfoText(""			, { 400,	0	},		25,		255, 225, 255);

	mp_Rotation				= CreateDebugInfoText("Rot: "		, { 0,		75  },		25,		255, 225, 255);
	mp_RotZ					= CreateDebugInfoText(""			, { 150,	75  },		25,		255, 225, 255);

	mp_Scale				= CreateDebugInfoText("Scale: "		, { 0,		150 },		25,		255, 225, 255);
	mp_ScaleX				= CreateDebugInfoText(""			, { 150,	150 },		25,		255, 225, 255);
	mp_ScaleY				= CreateDebugInfoText(""			, { 400,	150 },		25,		255, 225, 255);

	mp_Frame				= CreateText("FPS"					, { 1750,	0	},		25,		255, 225, 255);
	mp_FPS					= CreateText(""						, { 1830,	0	},		25,		255, 225, 255);
																
	mp_Colliders			= CreateDebugText("Colliders: "		, { 0,		1000 },		25,		255, 225, 255);
	mp_CollidersP			= CreateDebugText(""				, { 250,	1000 },		25,		255, 225, 255);
																
	mp_Entity				= CreateDebugText("Entity C/D: "	, { 0,		1030 },		25,		255, 225, 255);
	mp_EntityP				= CreateDebugText(""				, { 250,	1030 },		25,		255, 225, 255);
																
	mp_Input				= CreateDebugText("InputManager: "	, { 420,	1000 },		25,		255, 225, 255);
	mp_InputP				= CreateDebugText(""				, { 740,	1000 },		25,		255, 225, 255);
																
	mp_Update				= CreateDebugText("Update: "		, { 420,	1030 },		25,		255, 225, 255);
	mp_UpdateP				= CreateDebugText(""				, { 740,	1030 },		25,		255, 225, 255);
																
	mp_Draw 				= CreateDebugText("Draw: "			, { 900,	1000 },		25,		255, 225, 255);
	mp_DrawP				= CreateDebugText(""				, { 1100,	1000 },		25,		255, 225, 255);

	mp_NumberDraw			= CreateDebugText("Number To Draw: ", {900,		1030},			25,		255, 255, 255);
	mp_NumberDrawP			= CreateDebugText(""				, {1200,	1030},			25,		255, 255, 255);
	
	mp_QuadTree				= CreateDebugText("QuadTree"		, { 1300,	1000 },		25,		0  , 225, 0);
	mp_FrustrumCulling      = CreateDebugText("FrustrumCulling"	, { 1300,	1030 },		25,		0  , 225, 0);

#endif // _DEBUG
}

void Scene::OnUpdate(Clock& time)
{ 
#ifdef _DEBUG
	 
	constexpr int32 DEBUG_UPDATE = 5; 
	
	m_UpdateDebug++; 

	if (InputManager::GetInstance().IsDown(F1))
		m_DebugPerf = !m_DebugPerf;

	// QuadTree debug
	if (InputManager::GetInstance().IsDown(F2) && m_DebugPerf)
	{
		bool isActive = PhysicsManager::GetInstance().IsQuadTreeActive();

		if (isActive)
		{
			mp_QuadTree->SetColor(255, 0, 0, 255);
			PhysicsManager::GetInstance().SetActivateQuadTree(!isActive);
		}
		else
		{
			mp_QuadTree->SetColor(0, 255, 0, 255);
			PhysicsManager::GetInstance().SetActivateQuadTree(!isActive);
		} 
	}
	
	// Frustrum Culling
	if (InputManager::GetInstance().IsDown(F3) && m_DebugPerf)
	{ 
		if (m_FrustrumCulling)
		{
			m_FrustrumCulling = false;
			mp_FrustrumCulling->SetColor(255, 0, 0, 255); 
		}
		else
		{
			m_FrustrumCulling = true;
			mp_FrustrumCulling->SetColor(0, 255, 0, 255);
		} 
	}

	// Gizmo Colliders
	if (InputManager::GetInstance().IsDown(F4) && m_DebugPerf)
	{
		SetGizmoVisibility();
	}

	if (m_UpdateDebug >= 1)
	{
		mp_FPS->SetText(std::to_string(time.GetFramePerSecond() + 1));
	}

	if (m_DebugPerf)
		SetDebugInfo();
	
	EntityInfoVisibility(DEBUG_UPDATE);
	DebugSetEntityInfo(); 
#endif // _DEBUG
}

void Scene::OnExit()
{

}

void Scene::OnPause()
{
}

void Scene::OnResume()
{
}

bool Scene::IsPaused() const
{
	return m_IsPaused;
}

Camera* Scene::GetMainCamera()
{
	return mp_MainCamera;
}

Camera* Scene::GetCurrentCamera()
{
	return mp_ActiveCamera;
}

void Scene::SwitchCamera(Camera* pCamera)
{
	for (auto& cam : m_ActiveCamera)
	{
		cam->SetActive(false);

		if (cam->GetId() == pCamera->GetId())
		{
			cam->SetActive(true);
			mp_ActiveCamera = cam;
		}
	}
}

Button* Scene::CreateButton(gcle::Shapes shape, std::string text, const std::string& background)
{
	Button* button = CreateUI<Button>(shape);  
	button->SetTexture(background);

	button->SetTextObject(CreateText(text, button->GetRenderShape()->GetPosition(), 25));
	return button;
}

Toggle* Scene::CreateToggle(gcle::Shapes shape, const std::string& checkImage, const std::string& unCheckImage)
{
	Toggle* pToggle = CreateUI<Toggle>(shape);
	pToggle->SetOnCheckedTexture(checkImage);
	pToggle->SetOnUncheckedTexture(unCheckImage);
	pToggle->SetOn(false, true);
	return pToggle;
}

Slider* Scene::CreateSlider(gcle::Shapes shape, SliderOrientation orientation)
{
	Slider* pSlider = CreateUI<Slider>(shape);
	pSlider->SetOrientation(orientation);
	return pSlider;
}

Scrollbar* Scene::CreateScrollbar(gcle::Shapes shape, float32 handleSizeRatio)
{
	Scrollbar* pScrollbar = CreateUI<Scrollbar>(shape);
	pScrollbar->SetHandleSizeRatio(handleSizeRatio);
	return pScrollbar;
}

Panel* Scene::CreatePanel(gcle::Shapes shape, Color color)
{
	Panel* pPanel = CreateUI<Panel>(shape);
	pPanel->SetBackgroundColor(color);
	return pPanel;
}

Image* Scene::CreateImage(gcle::Shapes shape, const std::string& textureId)
{
	Image* pImage = CreateUI<Image>(shape);
	if (!textureId.empty())
		pImage->SetSprite(textureId);
	return pImage;
}

Entity* Scene::CreateWorldText( const std::string& text, int32 fontSize, const std::string& fontId, byte r, byte g, byte b, byte a)
{
	WorldText* worldText = CreateEntity<WorldText>();
	Font* font = RessourceManager::GetInstance().GetFont(fontId);
	if (font == nullptr)
	{
		GCLE_WARN << "Couldn't find the font " << fontId <<  ENDL;
		return nullptr;
	}

	Text* textObject = GCLE_NEW Text(font, text, worldText->GetPosition(), fontSize, r, g, b, a);
	worldText->m_Text = textObject;
	Entity* entity = worldText;

	return entity;
}