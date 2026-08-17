#pragma once
#include "Engine/Entity.h"

class Inventory;

class MainPlayer : public Entity
{
private:

	Inventory* inventory = nullptr;

public:

	void InitInventory(Inventory* inv);

	void OnInitialize() override;
	void OnUpdate() override;
};

