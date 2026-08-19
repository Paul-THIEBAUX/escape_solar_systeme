#include "Player.h"
#include "Inventory.h"

void MainPlayer::InitInventory(Inventory* inv)
{
	inventory = inv;
	inventory->SetPosition(GetPosition().x, GetPosition().y + 400);
}

void MainPlayer::InitCursor(Entity* cursor)
{
	inventory->SetCursor(cursor);
}

void MainPlayer::OnInitialize()
{
}

void MainPlayer::OnUpdate()
{
	inventory->SetPosition(GetPosition().x, GetPosition().y + 400);
}

void MainPlayer::AddEntity(Entity* entity)
{
	inventory->AddEntity(entity);
}
