#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "GameManager.h"
#include "PhysicsManager.h"
#include "AssetEngine.h"

#include "include.h"

#include "SampleScene.h"
#include "CorridorScene.h"
#include "ReactorScene.h"
#include "LaboScene.h"
#include "CuisineScene.h"
#include "StorageScene.h"
#include "CabineScene.h"

#include "MainScene.h"
#include "SceneManager.h"
#include "Profiler.h"

#undef main

#include <windows.h>

#ifdef _DEBUG
#include <cstdio>
#include <iostream>

void CreateDebugConsole()
{
    AllocConsole();

    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
    freopen_s(&fp, "CONIN$", "r", stdin);

    std::ios::sync_with_stdio(true);

    SetConsoleTitleA("GCLE Debug Console");
}
#endif

void CreateScenes()
{
    SceneManager::GetInstance().CreateScene<CorridorScene>("CorridorScene");
    SceneManager::GetInstance().CreateScene<ReactorScene>("ReactorScene");
    SceneManager::GetInstance().CreateScene<LaboScene>("LaboScene");
    SceneManager::GetInstance().CreateScene<CuisineScene>("CuisineScene");
    SceneManager::GetInstance().CreateScene<StorageScene>("StorageScene");
    SceneManager::GetInstance().CreateScene<CabineScene>("CabineScene");
    SceneManager::GetInstance().CreateScene<MainScene>("MainScene");
}

void Destroy()
{
    GameManager::DestroyInstance();     
    SceneManager::DestroyInstance();
    PhysicsManager::DestroyInstance();  
    RessourceManager::DestroyInstance();
    AssetEngine::DestroyInstance();
    InputManager::DestroyInstance();
} 

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
#ifdef _DEBUG
    CreateDebugConsole();
#endif 

    //_CrtSetBreakAlloc(157);

    GameManager& gm = GameManager::GetInstance();


    gm.Init(1680, 900, 120);

    CreateScenes();
    SceneManager::GetInstance().SetCurrentSceneWithTag("MainScene", true);

    gm.Loop();

    gm.Close();

    Destroy();

    _CrtDumpMemoryLeaks();

    return 0;
}