#include "SceneManager.h"
#include "Scene.h"


SceneManager* SceneManager::s_Instance = nullptr;

SceneManager& SceneManager::GetInstance() {
    if (s_Instance == nullptr)
        s_Instance = GCLE_NEW SceneManager();
    return *s_Instance;
}

void SceneManager::DestroyInstance()
{
    delete s_Instance;
    s_Instance = nullptr;
}

SceneManager::SceneManager()
{ 
}

SceneManager::~SceneManager()
{
    DeleteAllScenes();
}

Scene* SceneManager::GetCurrentScene() {
    return m_Scenes[m_CurrentSceneTag];
}

std::string& SceneManager::GetCurrentSceneTag() {
    return m_CurrentSceneTag;
}


Scene* SceneManager::GetPreviousScene()
{
    return m_Scenes[m_PreviousSceneTag];
}

std::string& SceneManager::GetPreviousSceneTag() {
    return m_PreviousSceneTag;

}

Scene* SceneManager::GetSceneWithTag(const std::string& tag) {
    return m_Scenes[tag];
}

void SceneManager::SetCurrentSceneWithTag(const std::string& tag, bool pause) 
{
    if (tag == m_CurrentSceneTag)
        return; 

    if (!m_SceneFactories.contains(tag)) {
        std::cerr << "tag : " << tag << " doesn't exist " << std::endl;
        return;
    }

    bool hadCurrentScene = (m_CurrentSceneTag != "");
     
    auto it = m_Scenes.find(tag);
    if (it == m_Scenes.end() || it->second == nullptr)
    {
        InstantiateSceneFromFactory(tag);
    }
    else if (it->second->IsPaused())
    {
        it->second->OnResume();
        it->second->m_IsPaused = false;
    }
     
    if (hadCurrentScene)
        LoadUnloadActiveTextures(tag);
     
    if (hadCurrentScene)
    {
        Scene* outgoingScene = m_Scenes[m_CurrentSceneTag];
        m_PreviousSceneTag = m_CurrentSceneTag;

        if (pause)
        {
            outgoingScene->OnPause();
            outgoingScene->m_IsPaused = true;
        }
        else
        {
            DestroyScene(m_PreviousSceneTag);
        }
    }

    m_CurrentSceneTag = tag;

    SetCurrentCamera();
}

void SceneManager::SetCurrentSceneToPreviousScene(bool pause) {
    if (m_PreviousSceneTag == "") {
        std::cerr << "there is no previous scene " << std::endl;
        return;
    }
    SetCurrentSceneWithTag(m_PreviousSceneTag, pause);
}

Scene* SceneManager::InstantiateSceneFromFactory(const std::string& tag)
{
    Scene* scene = m_SceneFactories[tag]();
    scene->m_Tag = tag;
    m_Scenes[tag] = scene;

    std::string previousSceneTag = m_CurrentSceneTag;
    m_CurrentSceneTag = tag; 
    scene->OnInitialize();
    m_CurrentSceneTag = previousSceneTag;

    return scene;
}

void SceneManager::DestroyScene(const std::string& tag)
{
    auto it = m_Scenes.find(tag);
    if (it == m_Scenes.end() || it->second == nullptr)
        return;

    Scene* scene = it->second;

    scene->OnExit();

    DestroySceneEntities(tag);
    DestroySceneCameras(tag);

    delete scene;
    m_Scenes.erase(it);
}

void SceneManager::DestroySceneEntities(const std::string& tag)
{
    std::vector<Entity*> entities = GameManager::GetInstance().GetActiveEntities(tag);
    for (Entity* e : entities)
    {
        e->RemoveActiveScene(tag);
        if (!e->HasActiveScenes()) 
            e->Destroy(); 
    }
}

void SceneManager::DestroySceneCameras(const std::string& tag)
{
    std::vector<Camera*> cameras = GameManager::GetInstance().GetCamerasInScene(tag);
    for (Camera* cam : cameras)
    {
        cam->RemoveActiveScene(tag);
        if (!cam->HasActiveScenes())
            GameManager::GetInstance().RemoveCamera(cam);
    }
}

void SceneManager::DeleteScene(const std::string& tag) {
    if (m_Scenes[tag] != nullptr) {
        Scene* scene = m_Scenes[tag];

        scene->OnExit();
        DestroySceneEntities(tag);
        DestroySceneCameras(tag);

        m_Scenes.erase(tag);

        if (m_CurrentSceneTag == tag) m_CurrentSceneTag = "";
        if (m_PreviousSceneTag == tag) m_PreviousSceneTag = "";

        delete scene;
        return;
    }
    std::cerr << "Scene " << tag << "doesn't exist" << std::endl;
}

void SceneManager::SetCurrentCamera()
{
    Scene* scene = m_Scenes[m_CurrentSceneTag];
     
    if (scene->mp_ActiveCamera != nullptr)
    {
        scene->SwitchCamera(scene->mp_ActiveCamera);
    }
    else if (scene->m_ActiveCamera.size() > 0)
    {
        scene->SwitchCamera(scene->m_ActiveCamera[0]);
    }
}


void SceneManager::UpdateCurrentScene(Clock& time) {
    if (m_CurrentSceneTag != "") {
        //GCLE_INFO << m_CurrentSceneTag << ENDL;

        Scene* scene = m_Scenes[m_CurrentSceneTag];
        scene->Update(time);
        scene->OnUpdate(time);
    }
}

void SceneManager::DrawCurrentScene(Window* pWindow) {
    if (m_CurrentSceneTag != "") {
        m_Scenes[m_CurrentSceneTag]->Draw(pWindow);
    }
}

void SceneManager::DrawCurrentSceneDebug(Window* pWindow)
{
#ifdef _DEBUG
    if (m_CurrentSceneTag != "") {
        m_Scenes[m_CurrentSceneTag]->DrawDebug(pWindow);
    }
#endif // _DEBUG
}

void SceneManager::DeleteAllScenes()
{
    for (auto& scene : m_Scenes)
    {
        delete scene.second;
        scene.second = nullptr;
    }
    m_Scenes.clear();
    m_CurrentSceneTag = "";
    m_PreviousSceneTag = "";
}

void SceneManager::LoadUnloadActiveTextures(const std::string& newSceneId){
    Scene* currentScene = m_Scenes[m_CurrentSceneTag];
    Scene* newScene = m_Scenes[newSceneId];
    std::vector<std::string> m_activatedTextures;

    for (auto& texture : currentScene->m_ActiveTextures) {
        if (std::find(newScene->m_ActiveTextures.begin(), newScene->m_ActiveTextures.end(), texture) != newScene->m_ActiveTextures.end()) {
            m_activatedTextures.push_back(texture); 
            continue;
        }
        else {
            RessourceManager::GetInstance().m_surfaceMap[texture].UnloadTexture();
            GCLE_INFO << "unload " << texture << ENDL;
        }
    }
    for (auto& texture : newScene->m_ActiveTextures) {
        if (std::find(m_activatedTextures.begin(),m_activatedTextures.end(), texture) == m_activatedTextures.end()) {
            std::string path = "../../assets/textures/" + texture + ".png";
            RessourceManager::GetInstance().LoadSurface(GameManager::GetInstance().GetWindow(), path, texture);
            m_activatedTextures.push_back(texture);
            GCLE_INFO << "load " << texture << ENDL;
        }
    }
}

