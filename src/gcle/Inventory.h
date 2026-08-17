#pragma once
#include <vector>
#include "Core/Utils.h"
#include "Engine/Entity.h"

class Inventory : public Entity
{
private:

	std::vector<Entity*> inventory;

	int current = 0;
	bool init = false;

public:

	Entity* GetCurrentItem();

	void AddEntity(Entity* entity) { inventory.push_back(entity); };
	void Next() { current = Clamp(current + 1, 0, static_cast<int>(inventory.size()) - 1); };
	void Previous() { current = Clamp(current - 1, 0, static_cast<int>(inventory.size()) - 1); };

	void OnInitialize() override;

//	void DysplayInventory();
};