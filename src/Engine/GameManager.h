#pragma once

#include <vector>
#include "Render/Window.h"
#include "include.h"
#include "Camera.h"

#include "RessourceManager.h"
#include "SceneManager.h"

#define FixedUpdateExecution 50

class Entity;
class Scene;

class GameManager
{
public:
	int16 m_fps = 120;

	static GameManager& GetInstance();
	static void DestroyInstance();

	float64 fixedUpdateDT = 1.f / FixedUpdateExecution; 
	float32 accDt = 0.f;
	float64 fpsDT = 1.f / m_fps; 

	GameManager() = default;
	~GameManager();


	bool Init(int32 windowWidth, int32 windowHeight, int16 FPS = 60);
	void Loop();
	void Close();

	Clock* GetTime();

	void SetWindowClearColor(Color color);

	void AddEntity(Entity* entity);
	void AddUI(UI* ui);

	void UpdateRigidBodies(float32 dt);

	void Quit();
	
	Window* GetWindow() ; 

	std::vector<Entity*> GetActiveEntities(const std::string& scene);
	std::vector<UI*> GetActiveUIs(const std::string& scene);

	std::vector<Camera*> GetCamerasInScene(const std::string& scene);
	void RemoveCamera(Camera* pCamera);

private:
	void UpdateEntitySystem();
	void UpdateUISystem();
	void DestroyEverything();

private:

	GameManager* mp_Instance = nullptr; 
	Window* mp_Window = nullptr;

	Color m_ClearColor = { 0, 0, 0, 255 };

	Clock m_Time;

	bool m_IsRunning = false;

	int32 m_WindW = 0, m_WindH = 0;

	std::vector <std::vector<Entity*>> m_Entities;
	std::vector <Entity*> m_EntitiesToDestroy;
	std::vector <Entity*> m_EntitiesToCreate;

	std::vector <std::vector<UI*>> m_UIs;
	std::vector <UI*> m_UIsToCreate;
	std::vector <UI*> m_UIsToDestroy;

	std::vector <Camera*> m_Camera; 
	
	int32 m_LoopTour = 0; 

	static GameManager* s_GameManagerInstance;


private:
	friend class Scene;
	friend class Window;

};