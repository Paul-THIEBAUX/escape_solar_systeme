#pragma once
#include "Tuto-GCLE/Object.h"

namespace Demo
{
	class InteractableHeart : public Object
	{
	public: 
		void OnInitialize() override;
		void OnInteract(Entity* pEntity) override;
		void OnUpdate() override;

	private:
		int m_HealAmount = 10;
		bool m_WasShowingInteract = false;
	}; 
}
