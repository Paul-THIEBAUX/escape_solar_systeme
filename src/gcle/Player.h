#pragma once
#include "Engine/Entity.h"

class Inventory;
class Cursor;

class MainPlayer : public Entity
{
private:

	Inventory* inventory = nullptr;

public:

	void InitInventory(Inventory* inv);
	void InitCursor(Cursor* cursor);

	void OnInitialize() override;
	void OnUpdate() override;

	void AddEntity(Entity* entity);
};

