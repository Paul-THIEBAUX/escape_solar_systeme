#pragma once
#include "Player.h"
#include "Engine\Scene.h"

class Door;
class InteractableObject;

class ReactorScene : public Scene
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

	std::vector<Entity*> ReactorEntities;
	std::vector<InteractableObject*> pickAbleItem;

	bool debug = false;
	bool action = false;
};
