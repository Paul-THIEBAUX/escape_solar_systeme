#pragma once
#include <vector>
#include "Core/Utils.h"

class Entity;

class Inventory
{
private:

	std::vector<Entity*> inventory;

	int current = 0;

public:

	Entity* GetCurrentItem();

	void AddEntity(Entity* entity) { inventory.push_back(entity); };
	void Next() { current = Clamp(current + 1, 0, static_cast<int>(inventory.size()) - 1); };
	void Previous() { current = Clamp(current - 1, 0, static_cast<int>(inventory.size()) - 1); };
};