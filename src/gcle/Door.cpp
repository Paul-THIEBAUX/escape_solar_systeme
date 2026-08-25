#include "Door.h"
#include "Tag.h"
#include "SceneManager.h"
#include "InputManager.h"

void Door::SetId(const std::string& id)
{
	sceneId = id;
}

void Door::Unlock()
{
	lock = false;
}

void Door::Lock()
{
	lock = true;
}

void Door::OnUpdate()
{
}

void Door::OnInitialize()
{
	SetTag(Tag::Obstacle);
	SetColor(Color::Red);
	SetRigidBody(true);
	SetStatic(true);
	SetLayer(1);
	CreateCollider(gcle::Shapes::Rectangle, true, gcle::ColliderDesc({ 0, 0 }, 0.0f, { 1, 1 }));
}


void Door::OnCollision(Entity* collidedWith)
{
}

void Door::CheckDoor(Entity* player, const Vector2f& pos)
{
	if (player == nullptr) 
		return;

	if (GetPosition().GetDistance(player->GetPosition()) > 200)
		return;

	if (IsInside(pos))
	{
		if (InputManager::GetInstance().IsDown(LeftButton))
			SceneManager::GetInstance().SetCurrentSceneWithTag(sceneId, true);
	}
}
