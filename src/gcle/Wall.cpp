#include "Wall.h"
#include "Tag.h"

void Wall::OnInitialize()
{
	SetTag(Tag::Obstacle);
	SetRigidBody(true);
	SetStatic(true);
	SetLayer(1);
	CreateCollider(gcle::Shapes::Rectangle, true, gcle::ColliderDesc({ 0, 0 }, 0.0f, { 1, 1 }));
}
