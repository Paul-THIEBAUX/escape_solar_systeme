#include "InteractableObject.h"
#include "InputManager.h"


void InteractableObject::OnInitialize()
{
	AddActiveScene("MainScene");
	AddActiveScene("CorridorScene");
	AddActiveScene("ReactorScene");
	AddActiveScene("LaboScene");
	AddActiveScene("CuisineScene");
	AddActiveScene("StorageScene");
	AddActiveScene("CabineScene");
}

void InteractableObject::OnUpdate()
{
	InputManager& IM = InputManager::GetInstance();

	if (IsInside({ static_cast<float>(IM.GetMouseRelativePosition().x), static_cast<float>(IM.GetMouseRelativePosition().y) }))
	{
		if (InputManager::GetInstance().IsDown(LeftButton))
			pick = true;
	}
}
