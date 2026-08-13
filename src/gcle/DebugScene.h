#pragma once
#include "Engine/Scene.h"

class DebugScene : public Scene
{
public:
	void OnInitialize() override;
	void OnUpdate(Clock& time) override;

private:
	Entity* pEntity = nullptr;
	Entity* otherEntity = nullptr;
	Camera* pSceneCamera = nullptr;

};

