#pragma once
#include <vector>
#include "Core/Utils.h"
#include "Engine/Entity.h"

class Cursor;

class Inventory : public Entity
{
private:

	std::vector<std::vector<Entity*>> inventory;

	int current_line = 0;
	int current = 0;

	int cursorPos = -700;

	Cursor* cursor = nullptr;

public:

	Entity* GetCurrentItem();

	void AddEntity(Entity* entity);
	void SetCursor(Cursor* new_cursor);

	void Next() { current = Clamp(current + 1, 0, static_cast<int>(inventory.size()) - 1); };
	void Previous() { current = Clamp(current - 1, 0, static_cast<int>(inventory.size()) - 1); };
	void NextLine();
	void PreviousLine();


	void OnInitialize() override;
	void OnUpdate() override;
};