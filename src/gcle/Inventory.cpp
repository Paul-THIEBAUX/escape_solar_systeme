#include "Inventory.h"

Entity* Inventory::GetCurrentItem()
{
	if (inventory.empty() || current >= inventory.size())
		return nullptr;

	return inventory[current];
}