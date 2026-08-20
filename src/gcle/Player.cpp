#include "Player.h"
#include "Inventory.h"
#include "Cursor.h"

void MainPlayer::InitInventory(Inventory* inv)
{
	inventory = inv;
	inventory->SetPosition(GetPosition().x, GetPosition().y + 400);
}

void MainPlayer::InitCursor(Cursor* cursor)
{
	inventory->SetCursor(cursor);
}

void MainPlayer::OnInitialize()
{
	SetTexture("E_Joueur");
	SetScale({ 1, 2 });

	AddActiveScene("MainScene");
	AddActiveScene("CorridorScene");
	AddActiveScene("ReactorScene");
	AddActiveScene("LaboScene");
	AddActiveScene("CuisineScene");
	AddActiveScene("StorageScene");
	AddActiveScene("CabineScene");
}

void MainPlayer::OnUpdate()
{
	inventory->SetPosition(GetPosition().x, GetPosition().y + 400);
}

void MainPlayer::AddEntity(Entity* entity)
{
	inventory->AddEntity(entity);
}
