#pragma once
#include <vector>
#include <iostream>
#include "Entity.h"
#include "UI.h"
#include "Core/InputManager.h"
#include "Button.h"
#include "Toggle.h"
#include "Slider.h"
#include "Scrollbar.h"
#include "Panel.h"
#include "Image.h"

//class Entity;
class Font;
class Text;
class Camera;
class GameManager;
class Window;
class WorldText;

class Scene
{ 
public:

	Text* CreateText(const std::string& text, Vector2f pos, int32 fontSize, byte r = 255, byte g = 255, byte b = 255);
	void DestroyText(Text* pText);

	template<typename T>
	T* CreateEntity(gcle::Shapes shape);

	template<typename T>
	T* CreateEntity();
	
	template<typename T>
	T* CreateUI(gcle::Shapes shape);

	template<typename T>
	T* CreateUI();

	Button* CreateButton(gcle::Shapes shape, std::string text, const std::string& background = "background-button");
	Toggle* CreateToggle(gcle::Shapes shape, const std::string& checkImage, const std::string& unCheckImage);
	Slider* CreateSlider(gcle::Shapes shape, SliderOrientation orientation = SliderOrientation::Horizontal);
	Scrollbar* CreateScrollbar(gcle::Shapes shape, float32 handleSizeRatio = 0.2f);
	Panel* CreatePanel(gcle::Shapes shape, Color color = Color::White);
	Image* CreateImage(gcle::Shapes shape, const std::string& textureId);

	Entity* CreateWorldText(const std::string& text, int32 fontSize, const std::string& fontId = "Hack-Regular", byte r = 255, byte g = 255, byte b = 255, byte a = 255);
	 
	Camera* CreateCamera();
	 
	void AddDrawnTexture(const std::string&);
	void AddActiveCamera(Camera* pCam);
	bool isDrawn(const std::string& tag);
	 
	Camera* GetCurrentCamera();

	bool IsPaused() const;

protected: 
	Scene() = default;
		
	virtual ~Scene();

	virtual void OnInitialize();
	virtual void OnUpdate(Clock& time);
	virtual void OnExit();
	virtual void OnPause();
	virtual void OnResume();

	Camera* GetMainCamera();
	void SwitchCamera(Camera* pCamera);

private: 
	void Update(Clock& time) const;
	void Draw(Window* pWindow);


#ifdef _DEBUG

public:
	void SetDebug();
	void SetSelectedEntity(Entity* pEntity);

private:
	Text*	CreateDebugText(const std::string& text, Vector2f pos, int32 fontSize, byte r = 255, byte g = 255, byte b = 255);
	void	DestroyDebugText(Text* pText);
	Text*	CreateDebugInfoText(const std::string& text, Vector2f pos, int32 fontSize, byte r = 255, byte g = 255, byte b = 255);
	void	DestroyDebugInfoText(Text* pText);
	void	DrawDebug(Window* pWindow);
	void	SetGizmoVisibility();

	void	EntityInfoVisibility(const int32 debugConstant);
	void	DebugSetEntityInfo();

	void	SetDebugInfo() const;

#endif // _DEBUG

protected:
	std::string m_Tag;
	Camera* mp_MainCamera   = nullptr;
	Camera* mp_ActiveCamera = nullptr;

private:

	std::vector<Text*> m_Texts;
	std::vector<std::string> m_ActiveTextures;
	std::vector<Camera*> m_ActiveCamera;

	float32 m_Test = 0; 

	bool	m_FrustrumCulling = true;
	uint64	m_NumberOfDraw = 0;

	bool m_IsPaused = false;

#ifdef _DEBUG
private:
	// DEBUG
	bool	m_Debug = false;
	int32	m_UpdateDebug = 0;
	bool	m_DebugPerf = false;
	bool	m_IsVisualDebugActive = false;

	std::vector<Text*>	m_DebugTexts;
	std::vector<Text*>	m_DebugInfoTexts;
	Entity*				mp_SelectedEntity = nullptr;

	Text* mp_Position			= nullptr;
	Text* mp_PosX				= nullptr;
	Text* mp_PosY				= nullptr; 

	Text* mp_Rotation			= nullptr;
	Text* mp_RotZ				= nullptr; 

	Text* mp_Scale				= nullptr;
	Text* mp_ScaleX				= nullptr;
	Text* mp_ScaleY				= nullptr; 

	Text* mp_Frame				= nullptr;
	Text* mp_FPS				= nullptr;

	Text*  mp_Colliders			= nullptr;
	Text*  mp_CollidersP		= nullptr;

	Text*  mp_Entity			= nullptr;
	Text*  mp_EntityP			= nullptr;

	Text*  mp_Input				= nullptr; 
	Text*  mp_InputP			= nullptr;

	Text*  mp_Update			= nullptr;
	Text*  mp_UpdateP			= nullptr;

	Text*  mp_Draw 				= nullptr;
	Text*  mp_DrawP				= nullptr;

	Text*  mp_QuadTree			= nullptr;
	Text*  mp_FrustrumCulling	= nullptr;

	Text*  mp_NumberDraw		= nullptr;
	Text*  mp_NumberDrawP	= nullptr;
#endif // !_DEBUG

private: 
	friend class GameManager;
	friend class SceneManager;
};

#include "Scene.inl"