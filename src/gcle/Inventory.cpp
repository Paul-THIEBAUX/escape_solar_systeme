#include "Inventory.h"
#include "InputManager.h"
#include "Cursor.h"

Entity* Inventory::GetCurrentItem()
{
	if (inventory.empty() || current >= inventory[current_line].size())
		return nullptr;

	return inventory[current_line][current];
}

void Inventory::AddEntity(Entity* entity)
{
	int ligne = -1;
	if (inventory[0].size() < 8)
	{
		inventory[0].push_back(entity);
		ligne = 0;
	}
	else if (inventory[1].size() < 8)
	{
		inventory[1].push_back(entity);
		ligne = 1;
	}
	else if(inventory[2].size() < 8)
	{
		inventory[2].push_back(entity);
		ligne = 2;
	}
 else 
  return;

	if (ligne == current_line)
		entity->SetLayer(4);
	else
		entity->SetLayer(0);

	entity->SetScale({ 1.4f, 1.4f });

	int pos = -700;
	for (Entity* e : inventory[ligne])
	{
		e->SetPosition(GetPosition().x + pos, GetPosition().y);
		pos += 200;
	}
}

void Inventory::SetCursor(Cursor* new_cursor)
{
	cursor = new_cursor;
	cursor->SetTexture("cursor");
	cursor->SetRigidBody(false);
	cursor->SetScale({ 1.5f,1.5f });
	cursor->SetLayer(3);
	cursor->SetPosition(GetPosition().x - 700, GetPosition().y);
}

void Inventory::NextLine()
{
	for (Entity* e : inventory[current_line])
		e->SetLayer(0);

	current_line = Clamp(current_line - 1, 0, 2);

	for (Entity* e : inventory[current_line])
		e->SetLayer(4);
	
	current = 0;
	cursorPos = -700;
}

void Inventory::PreviousLine()
{
	for (Entity* e : inventory[current_line])
		e->SetLayer(0);

	current_line = Clamp(current_line + 1, 0, 2);

	for (Entity* e : inventory[current_line])
		e->SetLayer(4);

	current = 0;
	cursorPos = -700;
}

void Inventory::OnInitialize()
{
	SetColor(Color(100,100,100,255));
	SetRigidBody(false);
	SetScale({ 16, 1.75f });
	SetLayer(2);

	AddActiveScene("MainScene");
	AddActiveScene("CorridorScene");
	AddActiveScene("ReactorScene");
	AddActiveScene("LaboScene");
	AddActiveScene("CuisineScene");
	AddActiveScene("StorageScene");
	AddActiveScene("CabineScene");

	inventory.push_back(std::vector<Entity*>());
	inventory.push_back(std::vector<Entity*>());
	inventory.push_back(std::vector<Entity*>());
}
void Inventory::OnUpdate()
{
	InputManager& IM = InputManager::GetInstance();
	
	cursor->SetPosition(GetPosition().x + cursorPos, GetPosition().y);

	if (IM.IsDown(LeftArrow))
	{
		Previous();
		cursorPos = Clamp(cursorPos -= 200, -700, 700);
	}
	if (IM.IsDown(RightArrow))
	{
		Next();
		cursorPos = Clamp(cursorPos += 200, -700, 700);
	}

	if (IM.IsDown(UpArrow))
		PreviousLine();
	if (IM.IsDown(DownArrow))
		NextLine();

	int pos = -700;
	for (Entity* e : inventory[current_line])
	{
		e->SetPosition(GetPosition().x + pos, GetPosition().y);
		pos += 200;
	}
}