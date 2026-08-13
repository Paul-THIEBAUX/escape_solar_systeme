#pragma once
#include "Scene.h"
#include "SceneManager.h"
#include "GameManager.h"

template <typename S>
Scene* SceneManager::CreateScene(const std::string& tag) {
    static_assert(std::is_base_of<Scene, S>::value, "S must be derived from Scene");
     
    m_SceneFactories[tag] = []() -> Scene* { return GCLE_NEW S(); };

    if (m_Scenes[tag] == nullptr) {
        m_Scenes[tag] = m_SceneFactories[tag]();
        Scene* scene = m_Scenes[tag];
        scene->m_Tag = tag;

        std::string previousSceneTag = m_CurrentSceneTag;
        m_CurrentSceneTag = tag;

        scene->OnInitialize();

        if (previousSceneTag == "")
        {
            m_CurrentSceneTag = previousSceneTag;
            SetCurrentSceneWithTag(tag);
        }
        else
        {
            m_CurrentSceneTag = previousSceneTag;
        }

        return m_Scenes[tag];
    }
    std::cerr << "Scene " << tag << "already exists" << std::endl;
    return m_Scenes[tag];
}