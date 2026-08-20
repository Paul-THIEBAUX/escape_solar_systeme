#pragma once
#include "Engine/Entity.h"


class InteractableObject : public Entity
{
private:

	bool pick = false;

public:

	void OnInitialize() override;
	void OnUpdate() override;
	bool HasBeenPick() { return pick; };

};

