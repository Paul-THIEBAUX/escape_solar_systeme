#pragma once
#include "Scene.h"
#include "GameManager.h"

template<typename T>
T* Scene::CreateEntity(gcle::Shapes shape)
{
	static_assert(std::is_base_of<Entity, T>::value, "T must be derived from Entity");

	T* new_entity = GCLE_NEW T();
	Entity* entity = new_entity;

	entity->m_ActiveScenes.push_back(m_Tag);

	entity->Initialize(shape);

	GameManager::GetInstance().AddEntity(new_entity);

	return new_entity;
}
template<typename T>
T* Scene::CreateEntity()
{
	static_assert(std::is_base_of<Entity, T>::value, "T must be derived from Entity");

	T* new_entity = GCLE_NEW T();
	Entity* entity = new_entity;

	entity->m_ActiveScenes.push_back(m_Tag);

	entity->Initialize();

	GameManager::GetInstance().AddEntity(new_entity);

	return new_entity;
}

template<typename T>
T* Scene::CreateUI(gcle::Shapes shape)
{
	static_assert(std::is_base_of<UI, T>::value, "T must be derived from UI");

	T* new_UI = GCLE_NEW T();
	UI* ui = new_UI;

	ui->m_ActiveScenes.push_back(m_Tag);

	ui->Initialize(shape);

	GameManager::GetInstance().AddUI(new_UI);

	return new_UI;
}
template<typename T>
T* Scene::CreateUI()
{
	static_assert(std::is_base_of<UI, T>::value, "T must be derived from UI");

	T* new_UI = GCLE_NEW T();
	UI* ui = new_UI;

	ui->m_ActiveScenes.push_back(m_Tag);

	ui->Initialize();

	GameManager::GetInstance().AddUI(new_UI);

	return new_UI;
}