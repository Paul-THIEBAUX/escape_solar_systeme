#pragma once
#include "Player.h"
#include "..\src\Engine\Scene.h"


class MainScene : public Scene
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
	bool action = false;
	std::vector<Entity*> BedRoomEntities;
	bool debug = false;
};

