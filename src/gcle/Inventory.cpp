#include "Inventory.h"

Entity* Inventory::GetCurrentItem()
{
	if (inventory.empty() || current >= inventory.size())
		return nullptr;

	return inventory[current];
}

void Inventory::OnInitialize()
{
	SetColor(Color(100,100,100,255));
	SetRigidBody(false);
	SetScale({ 16, 1.75f });
	SetLayer(2);
}

//void Inventory::DysplayInventory()
//{
//	if (!init)
//	{
//		init = true;
//	}
//}