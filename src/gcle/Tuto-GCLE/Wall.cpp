#include "Wall.h"
#include "Tag.h"

void Demo::Wall::OnInitialize()
{
	SetTag(Tag::Obstacle);
	SetRigidBody(true);
	SetStatic(true);
	CreateCollider(gcle::Shapes::Rectangle, true, { { 0.0f, 0.0f }, 0.0f, { 1.0f, 1.0f } });
}
