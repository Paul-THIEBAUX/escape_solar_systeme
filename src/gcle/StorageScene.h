#pragma once
#include "Player.h"
#include "Engine\Scene.h"

class Door;
class InteractableObject;

class StorageScene : public Scene
{
public:

	void OnInitialize() override;
	void OnUpdate(Clock& time) override;

	void OnCollisionAB(MainPlayer* A, Entity* B);

private:

	Entity* pEntity = nullptr;
	Entity* otherEntity = nullptr;
	Camera* pSceneCamera = nullptr;
	MainPlayer* player = nullptr;
	Door* door = nullptr;

	std::vector<Entity*> StorageEntities;
	std::vector<InteractableObject*> pickAbleItem;
	std::vector<Door*> Doors;

	bool debug = false;
	bool action = false;
};
