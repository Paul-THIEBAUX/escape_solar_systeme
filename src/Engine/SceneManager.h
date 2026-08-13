#pragma once

#include <unordered_map>
#include <functional>
#include <iostream>
#include <string>
#include "include.h"
#include <vector>

class Scene;
class Window;
class Clock;


class SceneManager { 
public:

	SceneManager();
	~SceneManager();

	static SceneManager& GetInstance();
	static void DestroyInstance();

	Scene* GetCurrentScene();
	Scene* GetPreviousScene();
	Scene* GetSceneWithTag(const std::string& tag);
	std::string& GetCurrentSceneTag();
	std::string& GetPreviousSceneTag();

	void SetCurrentSceneWithTag(const std::string& tag, bool pause = false); 
	void SetCurrentSceneToPreviousScene(bool pause = false);                 

	void DeleteAllScenes();

	template <typename S>
	Scene* CreateScene(const std::string& tag);
	void DeleteScene(const std::string& tag);

private:
	void SetCurrentCamera();
	 
	Scene* InstantiateSceneFromFactory(const std::string& tag);
	void DestroyScene(const std::string& tag);
	void DestroySceneEntities(const std::string& tag);
	void DestroySceneCameras(const std::string& tag);

private:
	void UpdateCurrentScene(Clock& time);
	void DrawCurrentScene(Window* pWindow);
	void DrawCurrentSceneDebug(Window* pWindow);
	void LoadUnloadActiveTextures(const std::string& newScene);

private: 
	std::unordered_map<std::string, Scene*> m_Scenes;
	std::unordered_map<std::string, std::function<Scene* ()>> m_SceneFactories;
	std::string m_CurrentSceneTag = "";
	std::string m_PreviousSceneTag = "";

private:
	static SceneManager* s_Instance;

private:
	friend class GameManager;
};

#include "SceneManager.inl"