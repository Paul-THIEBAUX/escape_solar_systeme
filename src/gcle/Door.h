#pragma once
#include "Engine/Entity.h"

class Door : public Entity
{
private:

	std::string sceneId;

	bool lock = true;

public:

	void SetId(const std::string& id);
	void Unlock();
	void Lock();

	void OnUpdate() override;
	void OnInitialize() override;
	void OnCollision(Entity* collidedWith) override;

	void CheckDoor(Entity* player, const Vector2f& pos);
};

