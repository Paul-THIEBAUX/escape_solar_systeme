#pragma once
#include "Engine/Entity.h"

class Inventory;

class MainPlayer : public Entity
{
private:

	Inventory* inventory = nullptr;

public:

	void InitInventory(Inventory* inv);
	void InitCursor(Entity* cursor);

	void OnInitialize() override;
	void OnUpdate() override;

	void AddEntity(Entity* entity);
};

