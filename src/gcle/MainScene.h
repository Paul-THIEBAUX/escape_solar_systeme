#pragma once
#include "Player.h"
#include "Engine\Scene.h"

class Door;
class InteractableObject;

class MainScene : public Scene
{
public:

	void OnInitialize() override;
	void OnUpdate(Clock& time) override;

	void OnCollisionAB(MainPlayer* A, Entity* B);
	void OnExit() override;

private:

	Entity* pEntity		 = nullptr;
	Entity* otherEntity  = nullptr;
	Camera* pSceneCamera = nullptr;
	MainPlayer* player	 = nullptr;
	Door* door = nullptr;

	std::vector<Entity*> BedRoomEntities;

	std::vector<InteractableObject*> pickAbleItem;

	bool debug  = false;
	bool action = false;
};

