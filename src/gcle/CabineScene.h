#pragma once
#include "Player.h"
#include "Engine\Scene.h"

class CabineScene : public Scene
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

	std::vector<Entity*> CabineEntities;

	bool debug = false;
	bool action = false;
};


