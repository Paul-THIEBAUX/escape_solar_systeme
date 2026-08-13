#pragma once

#include "Engine/Entity.h"

namespace Demo
{
	class Object : public Entity
	{
	public:

		void Interact(Entity* pEntity);
		bool CanBeInteractWith() const;
		void SetInteract(bool interact);
		bool IsPlayerInRange() const;

	protected:
		virtual void OnInteract(Entity* pEntity) {};

		void OnTriggerEnter(Entity* pOther) override;
		void OnTriggerExit(Entity* pOther) override;

	protected:
		bool m_CanInteract = true;

	private:
		bool m_PlayerInRange = false;
	};
}
