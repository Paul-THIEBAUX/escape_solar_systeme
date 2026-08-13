#pragma once
#include "Engine/Scene.h"

class SampleScene : public Scene
{
public:
	
	Text* test = nullptr;

	void OnInitialize() override;
	void OnUpdate(Clock& time) override;

	private:
		Entity* pEntity = nullptr;
		Entity* otherEntity = nullptr;

		Camera* pSceneCamera = nullptr;
};

