#include <iostream>
#include "Scene.h" 
#include "Shape.h"
#include "Event.h"
#include "Entity.h"
#include "GameManager.h"
#include "SceneManager.h"
#include "PhysicsManager.h"
#include "Core/InputManager.h" 
#include "Engine/AssetEngine.h"

#include <algorithm>

#include <timeapi.h>
#pragma comment(lib, "winmm.lib")

GameManager* GameManager::s_GameManagerInstance = nullptr;

GameManager& GameManager::GetInstance()
{
	if (s_GameManagerInstance == nullptr)
		s_GameManagerInstance = GCLE_NEW GameManager();
	return *s_GameManagerInstance;
}

void GameManager::DestroyInstance()
{
	delete s_GameManagerInstance;
	s_GameManagerInstance = nullptr;
}

GameManager::~GameManager()
{
	UpdateEntitySystem();
	DestroyEverything();
}

bool GameManager::Init(int32 windowWidth, int32 windowHeight, int16 FPS)
{
#ifndef NDEBUG
	GCLE::DebuggerDesc desc{};
	desc.output = GCLE::DebuggerOutput::CONSOLE | GCLE::DebuggerOutput::LOGS;
	GCLE::Debugger::Init(&desc);
#endif

	srand(static_cast<int32>(m_Time.GetTime()));

	m_fps = FPS;
	fpsDT = 1.f / m_fps;

	m_WindW = windowWidth;
	m_WindH = windowHeight;

	uint32 windowFlags = SDL_WINDOW_FLAGS::WINDOW_RESIZABLE | SDL_WINDOW_FLAGS::WINDOW_SHOWN;
	uint32 renderFlags = SDL_RENDERER_FLAGS::RENDERER_ACCELERATED;


	mp_Window = GCLE_NEW Window("gcle", m_WindW, m_WindH, windowFlags, renderFlags, SDL_WINDOW_POSITION::WINDOWPOS_UNDEFINED, SDL_WINDOW_POSITION::WINDOWPOS_UNDEFINED);

	if (!mp_Window)
	{
		std::cout << "Window Initialization";
		return false;
	}
	RessourceManager& rm = RessourceManager::GetInstance();
	RessourceManager::GetInstance().Init(mp_Window);

	for (int i = 0; i < 32; i++)
	{
		m_Entities.push_back({});
		m_UIs.push_back({});
	}

	return true;
}

void GameManager::Loop()
{
	m_IsRunning = true;

	timeBeginPeriod(1);
	while (m_IsRunning == true)
	{
		// Set window in fullscreen
		if (InputManager::GetInstance().IsDown(F11))
		{
			mp_Window->ToggleFullscreen();
		}


		PROFILER_START("Colliders", "Colliders Update");
		int32 exec = 0;
		accDt += static_cast<float32>(m_Time.GetDeltaTime());


		PROFILER_START("SceneU", "Scene Update");
		SceneManager::GetInstance().UpdateCurrentScene(m_Time);
		PROFILER_END("SceneU");
		
		PROFILER_START("Input", "Input Update");
		InputManager::GetInstance().Update();
		PROFILER_END("Input");

		while (accDt >= fixedUpdateDT)
		{
			accDt -= static_cast<float32>(fixedUpdateDT);
			if (m_LoopTour < 1)
				m_LoopTour++;
			else
			{
				UpdateRigidBodies(static_cast<float32>(fixedUpdateDT));
				PhysicsManager::GetInstance().Update(static_cast<float32>(fixedUpdateDT));
			}
			exec += 1;
		}
		PROFILER_END("Colliders");

		PROFILER_START("Entity", "Entity Creation / Deletion");
		UpdateEntitySystem();
		UpdateUISystem();
		PROFILER_END("Entity");
		 
		PROFILER_START("SceneD", "Scene Draw");
		for (auto& cam : m_Camera)
		{
			if (cam->IsActive() && cam->IsActiveIn(SceneManager::GetInstance().GetCurrentSceneTag()))
				cam->Update(m_Time, m_Entities);
		}

		mp_Window->ClearWindowWithColor(m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, m_ClearColor.a);
		mp_Window->Clear();

		SceneManager::GetInstance().DrawCurrentScene(mp_Window);

		SceneManager::GetInstance().DrawCurrentSceneDebug(mp_Window);

		mp_Window->Present();
		PROFILER_END("SceneD");

		if (Event::WindowEvent())
		{
			m_IsRunning = false;
		}


		float64 rawDT = m_Time.GetRawDT();
		if (rawDT < fpsDT) {
			float64 timeToSleep = fpsDT - rawDT;
			m_Time.SmartSleep(timeToSleep);
		}


		//PROFILER_START("time", "Timer Update");
		m_Time.Update();
		//PROFILER_END("time"); 

		// GCLE_INFO << "FPS : " << m_Time.GetFramePerSecond() << ENDL;
		// PROFILER_END("Update");
		// system("CLS");
	}
	timeEndPeriod(1);
	m_IsRunning = false;
}


void GameManager::Close()
{
	// Just to be safe

	SceneManager::GetInstance();
	AssetEngine::GetInstance();
	PhysicsManager::GetInstance();
	InputManager::GetInstance();
	RessourceManager::GetInstance();


	SceneManager::GetInstance().DeleteAllScenes();
	RessourceManager::GetInstance().DeleteAll();
	DestroyEverything();

	PROFILER_CLEAR();

	delete mp_Window; 
}

Clock* GameManager::GetTime()
{
	return &m_Time;
}

void GameManager::SetWindowClearColor(Color color)
{
	m_ClearColor = color;
}


void GameManager::UpdateRigidBodies(float32 dt)
{
	for (auto& entity : GetActiveEntities(SceneManager::GetInstance().m_CurrentSceneTag))
	{
		if (entity->IsRigidBody())
			entity->GetRigidBody()->Update(dt);
	}
}

void GameManager::Quit()
{
	m_IsRunning = false;
}

std::vector<Entity*> GameManager::GetActiveEntities(const std::string& scene)
{
	std::vector<Entity*> results;
	for (auto& layer : m_Entities) {
		for (auto e : layer) {
			if (e->IsActiveIn(scene))
				results.push_back(e);
		}
	}
	return results;
}

std::vector<UI*> GameManager::GetActiveUIs(const std::string& scene)
{
	std::vector<UI*> results;
	for (auto& layer : m_UIs) {
		for (auto e : layer) {
			if (e->IsActiveIn(scene))
				results.push_back(e);
		}
	}
	return results;
}

std::vector<Camera*> GameManager::GetCamerasInScene(const std::string& scene)
{
	std::vector<Camera*> results;
	for (auto cam : m_Camera)
		if (cam->IsActiveIn(scene))
			results.push_back(cam);
	return results;
}

void GameManager::RemoveCamera(Camera* pCamera)
{
	if (pCamera == nullptr)
		return;

	auto it = std::find(m_Camera.begin(), m_Camera.end(), pCamera);
	if (it != m_Camera.end())
		m_Camera.erase(it);

	delete pCamera;
}

void GameManager::UpdateEntitySystem()
{
	for (int i = 0; i < m_Entities.size(); i++)
	{
		for (auto it = m_Entities[i].begin(); it != m_Entities[i].end(); )
		{
			Entity* entity = *it;

			if (entity->ToDestroy())
			{
				m_EntitiesToDestroy.push_back(entity);
				it = m_Entities[i].erase(it);
			}
			else if (entity->GetLayer() != i)
			{
				m_EntitiesToCreate.push_back(entity);
				it = m_Entities[i].erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	for (auto it = m_EntitiesToDestroy.begin(); it != m_EntitiesToDestroy.end(); ++it)
	{
		delete* it;
	}

	m_EntitiesToDestroy.clear();

	for (auto it = m_EntitiesToCreate.begin(); it != m_EntitiesToCreate.end(); ++it)
	{
		Entity* e = *it;
		m_Entities[e->GetLayer()].push_back(*it);
	}

	m_EntitiesToCreate.clear();
}

void GameManager::UpdateUISystem()
{
	for (int i = 0; i < m_UIs.size(); i++)
	{
		for (auto it = m_UIs[i].begin(); it != m_UIs[i].end(); )
		{
			UI* ui = *it;

			if (ui->ToDestroy())
			{
				m_UIsToDestroy.push_back(ui);
				it = m_UIs[i].erase(it);
			}
			else if (ui->GetLayer() != i)        
			{
				m_UIsToCreate.push_back(ui);     
				it = m_UIs[i].erase(it);         
			}
			else
			{
				++it;
			}
		}
	}

	for (auto it = m_UIsToDestroy.begin(); it != m_UIsToDestroy.end(); ++it)
	{
		delete* it;
	}

	m_UIsToDestroy.clear();

	for (auto it = m_UIsToCreate.begin(); it != m_UIsToCreate.end(); ++it)
	{
		UI* ui = *it;
		m_UIs[ui->GetLayer()].push_back(*it);
	}

	m_UIsToCreate.clear();
}

void GameManager::DestroyEverything()
{
	for (auto& layer : m_Entities)
	{
		for (auto& entity : layer)
			delete entity;
		layer.clear();
	}
	m_Entities.clear();
	for (auto& layer : m_UIs)
	{
		for (auto& ui : layer)
			delete ui;
		layer.clear();
	}
	m_UIs.clear();

	for (auto& cam : m_Camera)
		delete cam;
	m_Camera.clear();
}

void GameManager::AddEntity(Entity* pEntity) { 
	m_EntitiesToCreate.push_back(pEntity); 
}

void GameManager::AddUI(UI* ui)
{
	m_UIsToCreate.push_back(ui);
}

Window* GameManager::GetWindow() { 
	return mp_Window; 
}

