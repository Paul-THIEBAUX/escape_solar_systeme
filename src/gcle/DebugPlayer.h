#pragma once
#include "Entity.h"
#include "Core/InputManager.h"


class DebugPlayer : public Entity {

public: 
	void OnUpdate() override;
	void OnInitialize() override;
	void OnCollisionEnter(Entity* collidedWith) override;
	void OnCollision(Entity* collidedWith) override;
	void OnCollisionExit(Entity* collidedWith) override;

private: 
	bool IsGrounded = false;
};

