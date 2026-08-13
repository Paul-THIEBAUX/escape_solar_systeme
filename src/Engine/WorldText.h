#pragma once
#include "Entity.h"
#include "Render/Text.h"


class WorldText : public Entity{

	Text* m_Text = nullptr;
	friend class Scene;
	Vector2f m_RenderPosition = { 0, 0 };


public:
	void OnInitialize() override;
	void OnUpdate() override;

	Text* GetText();
	WorldText(): Entity() {};
	
	void SetRenderPosition(Vector2f pos);
};

